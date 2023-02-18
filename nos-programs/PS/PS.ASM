.486P

Code Segment Public Use32 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;
CodeStart:
EBF_Signature           DB  'EBF',0            ;signatura
EBF_Header_Size         DD  offset HeadEnd     ;velikost hlavicky
EBF_Name                DB  'ls                              ' ;vnitrni jmeno souboru - 32 znaku
EBF_Data_Size           DD  Data_Segment_Size  ;velikost data segmentu ve 4k (zarovnani nahoru)
EBF_Code_Header_Size    DD  Code_Segment_Size  ;velikost hlavicky a code segmentu ve 4k (zarovnani nahoru)
EBF_Code_Entry_Point    DD  Offset Enter_ptr   ;vstupni bod programu
EBF_Flags               DD  1                  ;flagy
EBF_Stack_Size          DD  16                 ;velikost zasobniku
EBF_LibsNum             DD  2                  ;pocet knihoven
;knihovny
DB           'taskman         '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (TSK_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
DB           'VFS             '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (VFS_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
HeadEnd :
EBF_CRC                 DD 0                   ;CRC hlavicky
;####################################################################################
;# Vstupni bod NOSHe
;####################################################################################
Enter_ptr :
;Zjistime VFS indody standardniho vstupu, vystupu a erroru
   Mov    EAX,00000002h  ;# 00000002 - TASK_Get_Current_Task_Num
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    EAX,00000012h  ;# 00000012 - TASK_Get_VFS_STDOut_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Out],EAX
   Mov    EAX,00000014h  ;# 00000014 - TASK_Get_VFS_STDError_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Error],EAX
;####################################################################################################
   Mov    Edi, offset prog_name
   call   Write_Pchar
   
   Mov    EAX, 23h
   Call   Fword ptr DS:[TSK_Call_Gate]
   Jc     @PS_Error
   Mov    EDI, Offset Family_Info
   Call   Write_Pchar
   Mov    ECX,512
   Xor    EDX,EDX
@PS_Writting :
   Mov    EAX,Dword ptr ES:[EDX.TSK_Info_ID]
   Or     EAX,EAX
   Jnz    @PS_Writting_Cont
@PS_Writing_Cont2 :
   Add    EDX,TSK_Info_Size
   Dec    ECX
   Jnz    @PS_Writting
   Jmp    @PS_Exit
@PS_Writting_Cont :
   Call   Write_Dec
   Mov    EDI,Offset Go_UID
   Call   Write_Pchar
   Mov    EAX,Dword ptr ES:[EDX.TSK_Info_UID_GID]
   Movzx  EAX,AX
   Call   Write_Dec
   Mov    EDI,Offset Go_GID
   Call   Write_Pchar
   Mov    EAX,Dword ptr ES:[EDX.TSK_Info_UID_GID]
   Shr    EAX,16
   Call   Write_Dec
   Mov    EDI,Offset Go_Parent
   Call   Write_Pchar
   Mov    EAX,Dword ptr ES:[EDX.TSK_Info_ParentTask]
   Or     EAX,EAX
   Jnz    @Skip_001
   Mov    Edi,Offset Name_kernel
   Call   Write_Pchar
   Jmp    @Skip_003
@Skip_001 :
   Cmp    EAX,0FFFFFFFFh
   Jne    @Skip_002
   Mov    EDI,Offset Name_none
   Call   Write_Pchar
   Jmp    @Skip_003
@Skip_002 :
   Call   Write_Dec
@Skip_003 :
   Mov    EDI,Offset Go_child
   Call   Write_Pchar
   Mov    EAX,Dword ptr ES:[EDX.TSK_Info_ChildTask]
   Or     EAX,EAX
   Jnz    @Skip_004
   Mov    EDI,Offset Name_none
   Call   Write_Pchar
   Jmp    @Skip_005
@Skip_004 :
   Call   Write_Dec
@Skip_005 :
   Mov    EDI,Offset Go_Priority
   Call   Write_Pchar
   Movzx  EAX,word ptr ES:[EDX.TSK_Info_Priority]
   Call   Write_Dec
   Mov    EDI,Offset Go_Status
   Call   Write_Pchar
   Mov    AX,word ptr ES:[EDX.TSK_Info_Status]
   Cmp    AX,_TSK_Status_RUN
   Jne    @PS_Write_Run_n
   Mov    EDI,Offset Status_Run
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Write_Run_n :
   Cmp    AX,_TSK_Status_Stop
   Jne    @PS_Write_Stop_n
   Mov    EDI,Offset Status_Stop
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Write_Stop_n :
   Cmp    AX,_TSK_Status_Sleep
   Jne    @PS_Write_Sleep_n
   Mov    EDI,Offset Status_Sleep
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Write_Sleep_n:
   Cmp    AX,_TSK_Forever_Run
   Jne    @PS_Write_Forever_n
   Mov    EDI,Offset Status_Forever
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Write_Forever_n :
   Cmp    AX,_TSK_Zoombie
   Jne    @PS_Write_Zoombie_n
   Mov    EDI,Offset Status_Zoombie
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Write_Zoombie_n:
   Mov    EDI,Offset Status_unknown
   Call   Write_Pchar
   Jmp    @PS_Writing_Cont2
@PS_Error :
   Mov    EDI,Offset Error
   Call   Write_Pchar
@PS_Exit :
   Mov    EAX,1
   Call   Fword ptr DS:[TSK_Call_Gate]
;####################################################################################################
;# Vstup : ES : EDI - retezec pchar na vypis
;#         ES : Data Segment
;####################################################################################################
Write_Pchar PROC
       PUSHAD
       PUSH   ES
        PUSH   DS
        POP    ES
        PUSH   EDI
        Mov    ECX, ES
        LSL    ECX,ECX
        Sub    ECX,EDI
        PUSH   ECX
         Mov    Al,0
         Repnz  Scasb
        POP    EDX
        Sub    EDX, ECX
        Mov    ECX, EDX
        Dec    ECX
       POP    EDI
        Mov    EDX, Dword ptr DS:[VFS_Standard_Out]
        Xor    EBX, EBX
        Xor    EAX, EAX
        Call   fword ptr DS:[VFS_Call_Gate]
       POP   ES
       POPAD
       Ret
Write_Pchar ENDP
;####################################################################################
;# vypise na obrazovku cislo EAX (dec)
;####################################################################################
Write_Dec PROC
       PUSHAD
       PUSH  ES
          Mov   EBX,DS
          Mov   ES,EBX
          Mov   EBX, 10
          Mov   EDI,Offset PREVOD_Output
Pocitej_Dec:
         PUSH EAX
          dec   ebx
          xor   edx, edx
          div   dword ptr DS:[DEC_Numbers+EBX*4]
          mov   esi, offset Numbers_Tab
          add   esi, eax
          xchg  eax, ecx
          lodsb
          stosb
          xchg  eax, ecx
          xor   edx, edx
          mul   dword ptr DS:[DEC_Numbers+EBX*4]
          inc   ebx
          mov   ecx, eax
         POP EAX
          sub eax, ecx
          dec ebx
          jnz Pocitej_Dec

          Mov    EDI,Offset PREVOD_Output
          Mov    Byte ptr DS:[EDI+10],0
          Mov    ECX,9
Write_Dec_Look_For_End:
         Cmp    Byte ptr DS:[EDI],'0'
         Jne    Write_Dec_Look_For_End_Found
         Inc    EDI
         Dec    ECX
         Jnz    Write_Dec_Look_For_End
Write_Dec_Look_For_End_Found:
         Call   Write_PCHAR
       POP  ES
       POPAD
       Ret
Write_Dec ENDP
;####################################################################################
ALIGN 4096
CSize             equ ($ - CodeStart)
Code_Segment_Size equ (CSize Shr 12)
CodeEnd:
Code Ends
;####################################################################################
;# Data segment
;####################################################################################
Include ./Data.inc
End CodeStart


