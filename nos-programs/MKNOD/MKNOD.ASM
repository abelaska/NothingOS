.486P

Code Segment Public Use32 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;

Include ./../../include/vfs.inc
Include ./../../include/taskman.inc
Include ./../../include/memory.inc
Include ./../../include/ext2.inc

CodeStart:
EBF_Signature           DB  'EBF',0            ;signatura
EBF_Header_Size         DD  offset HeadEnd     ;velikost hlavicky
EBF_Name                DB  'mknod                           ' ;vnitrni jmeno souboru - 32 znaku
EBF_Data_Size           DD  Data_Segment_Size  ;velikost data segmentu ve 4k (zarovnani nahoru)
EBF_Code_Header_Size    DD  Code_Segment_Size  ;velikost hlavicky a code segmentu ve 4k (zarovnani nahoru)
EBF_Code_Entry_Point    DD  Offset Enter_ptr   ;vstupni bod programu
EBF_Flags               DD  1                  ;flagy
EBF_Stack_Size          DD  16                 ;velikost zasobniku
EBF_LibsNum             DD  3                  ;pocet knihoven
;knihovny
DB           'memory          '                ;Jmeno knihovny
DW           00001h                            ;Minimalni verze knihovny
DD           offset (MEM_Call_Gate+4)          ;Offset v DataSegmentu, kam se ulozi Call Gate
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
   Mov    EAX, TASK_Get_Current_Task_Num
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    EAX, TASK_Get_VFS_STDOut_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Out],EAX
   Mov    EAX, TASK_Get_VFS_STDError_Inode
   Call   Fword ptr DS:[TSK_Call_Gate]
   Mov    Dword ptr DS:[VFS_Standard_Error],EAX
;####################################################################################################
   mov    edi, offset prog_name
   call   Write_Pchar

   mov    eax, TASK_Get_My_Parameters_Length
   Call   Fword ptr DS:[TSK_Call_Gate]
   or     eax, eax
   jz     show_help

   mov    dword ptr ds:[parm_size], eax

   Mov    EDX, EAX
   mov    eax, TSKMM_Alloc_B
   Call   Fword ptr DS:[Mem_Call_Gate]
   jc     err2
   mov    es, edx
   xor    edi, edi

   mov    eax, TASK_Get_My_Parameters
   Call   Fword ptr DS:[TSK_Call_Gate]
   ; ES:EDI - parametry

   mov    ecx, dword ptr ds:[parm_size]
   mov    al, ' '
   xor    ebx, ebx
   xor    edi, edi
   xor    edx, edx
get_parm_count:
  push   edi
   mov    edx, edi
   repnz  scasb
   sub    edx, edi
   neg    edx
   cmp    byte ptr es:[edi], ' '
   jz     short get_parm_count_0
   dec    edx
   inc    ebx
get_parm_count_0:
   or     ecx, ecx
   jnz    get_parm_count_1
   inc    edx
   jmp    short get_parm_count_2
get_parm_count_1:
   dec    edi
   mov    byte ptr es:[edi], 0
   inc    edi
get_parm_count_2:
  push    edx
   or     ecx, ecx
   jnz    short get_parm_count
   ; EBX - pocet parametru

   cmp    ebx, 3
   jnz    show_help

   pop   edx ; velikost
   pop   edi ; pozice
   mov   ebx,16
   call  Str2Num
   jc    show_help

   pop   ecx
   pop   edi
   cmp   ecx, 1
   jnz   err3

   call  Upcase

   mov   ecx, EXT2_S_IFCHR
   cmp   byte ptr es:[edi], 'C'
   jz    mknod_next0
   mov   ecx, EXT2_S_IFBLK
   cmp   byte ptr es:[edi], 'B'
   jz    mknod_next0
   mov   ecx, EXT2_S_IFIFO
   cmp   byte ptr es:[edi], 'F'
   jz    err5
   mov   ecx, EXT2_S_IFSOCK
   cmp   byte ptr es:[edi], 'S'
   jz    err5
mknod_next0:
   or    ecx, 4+20h+100h+80h
   ; ECX - mod inody
   pop   edx
   pop   edi
   cmp   byte ptr es:[edi], '/'
   jnz   err4
   mov   esi, edi
   add   edi, edx
mknod_next1:
   cmp   byte ptr es:[edi], '/'
   jz    short mknod_next2
   dec   edi
   jnz   short mknod_next1
   jmp   err4
mknod_next2:
   mov   byte ptr es:[edi], 0
   inc   edi

  push    edi es
  push    ds
  pop     es
   mov    edi, offset device_t
   call   Write_Pchar
  pop     es edi

   ; ES:ESI - zacatek cesty k adresari, kde se ma inoda ulozit
   ; ES:EDI - nazev souboru
  push    ds esi edi
  push    ds
  pop     fs
  push    es
  pop     ds
   ; EDX - typove cislo
   mov   edx, eax
   mov    eax, VFS_MkNod
   Call   Fword ptr FS:[VFS_Call_Gate]
  pop     esi edi ds
  pushfd
   dec    esi
   mov    byte ptr es:[esi], '/'
   inc    esi
  popfd
   jc     err1
   ; succesfull
   call   Write_Pchar
  push    ds
  pop     es
   mov    edi, offset created_t
   call   Write_Pchar
   jmp    mknod_exit

show_help:
  push    ds
  pop     es
   mov    edi, offset help_txt
   call   Write_Pchar
   jmp    mknod_exit
err5:
  push    ds
  pop     es
   mov    edi, offset error5
   call   Write_Pchar
   jmp    mknod_exit
err4:
  push    ds
  pop     es
   mov    edi, offset error4
   call   Write_Pchar
   jmp    mknod_exit
err3:
  push    ds
  pop     es
   mov    edi, offset error3
   call   Write_Pchar
   jmp    mknod_exit
err2:
  push    ds
  pop     es
   mov    edi, offset error2
   call   Write_Pchar
   jmp    mknod_exit
err1:
   call   Write_Pchar
  push    ds
  pop     es
   mov    edi, offset wasnt_created_t
   call   Write_Pchar
mknod_exit:
   Mov    EAX, TASK_Kill_Me
   Call   Fword ptr DS:[TSK_Call_Gate]

; upcasene string
; Vstup: ES:EDI - retezec
;        ECX - delka retezce
; Vystup: -
UpCase Proc
       push  ecx edi
UpCase_loop:
        cmp  byte ptr es:[edi], 'a'
        jl   short UpCase_loop_next
        cmp  byte ptr es:[edi], 'z'
        jg   short UpCase_loop_next
        sub  byte ptr es:[edi], 'a'
        add  byte ptr es:[edi], 'A'
UpCase_loop_next:
        inc  edi
        dec  ecx
        jnz  short UpCase_loop
       pop   edi ecx
        ret
UpCase Endp


;####################################################################################################
;# Vstup : ES : EDI - retezec pchar na vypis
;#         ES : Data Segment
;####################################################################################################
Write_Pchar PROC
       PUSHAD
       PUSH   ES
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
;# Procedura navraci dekadicke cislo, ktere je ulozeno v retezci
;####################################################################################
;# Vstup  : ES:[EDI] - zacatek retezce
;#          EDX      - kolik znaku ma cislo
;#          EBX      - zaklad soustavy, do ktere prevadime
;# Vystup : If C then Error Else EAX - cislo
;####################################################################################
Str2Num PROC
       PUSH ECX EDX EBP EBX EDI
         STC
         Movzx EBX,Bl
         Cmp   Bl,10
         Jbe   @Dec_Skip_1
         Mov   Bh,7
  @Dec_Skip_1 :
         Or    EDX,EDX
         Jz    @Dec_Return_NumCount_End
         Mov   ECX, EDX
         Xor   EBP, EBP
         Mov   EAX, 1
         Dec   EDI
  @Dec_Return_NumCount :
         PUSH  EAX
          Movzx EDX, Byte ptr ES:[EDI+ECX]
          Sub   Dl,'0'
          Js    @Dec_Return_NumCount_Error
          Cmp   Dl,Bl
          Jna   @Dec_Return_NumCount_OK
          Or    Bh,Bh
          Jz    @Dec_Return_NumCount_Error
          Sub   Dl,Bh
          Cmp   Dl,Bl
          Ja    @Dec_Return_NumCount_Error
  @Dec_Return_NumCount_OK :
          Mul   EDX
          Add   EBP, EAX
         POP   EAX
          Movzx EDX,BL
          Mul   EDX
          Dec   ECX
          Jnz   @Dec_Return_NumCount
          Mov   EAX, EBP
          CLC
  @Dec_Return_NumCount_End :
          POP  EDI EBX EBP EDX ECX
        RET
  @Dec_Return_NumCount_Error :
         POP    EAX
         Xor    EAX,EAX
         STC
         Jmp    Short @Dec_Return_NumCount_End
Str2Num ENDP
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


