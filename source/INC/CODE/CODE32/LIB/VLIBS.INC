;#################################################################################
;# Jednotka pro includovani Virtualnich knihoven :)
;#################################################################################
;# Version : 1.2-1-1
;#################################################################################


;##################################################################################
;# Pocet Virtualnich knihoven - musi odpovidat poctu virtualnich knihoven, tj.
;# poctu includu a inicializaci
;##################################################################################
LIB_Virtual_Libs EQU 5
;##################################################################################
;# Procedura nastavuje virtualni knihovny
;##################################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;##################################################################################
LIB_Virtual_LIBs_Initializing PROC
        Push    DS ES FS GS EAX EBX
        Mov     Ax,GDT_Descriptor
        Mov     ES,AX
        Mov     EBX,(First_Avaible_Descriptor)-8
@LIB_Virtual_LIBs_Looking_For_Descriptor :
        Add     EBX,8
        Mov     EAX,Dword ptr ES:[EBX]
        OR      EAX,EAX
        Jnz     @LIB_Virtual_LIBs_Looking_For_Descriptor
        Mov     EAX,Dword ptr ES:[EBX+4]
        OR      EAX,EAX
        Jnz     @LIB_Virtual_LIBs_Looking_For_Descriptor
        Mov     EAX,Dword ptr ES:[CODE_Descriptor]
        Mov     Dword ptr ES:[EBX],EAX
        Mov     EAX,Dword ptr ES:[CODE_Descriptor+4]
        Mov     Dword ptr ES:[EBX+4],EAX
        Mov     Byte ptr ES:[EBX+5],Data_Atr1
        Mov     DS,BX
        Call    LIB_Virtual_LIBs_Update
        Xor     EAX,EAX
        Mov     Dword ptr ES:[EBX],EAX
        Mov     Dword ptr ES:[EBX+4],EAX
        Pop     EBX EAX GS FS ES DS
        Ret
LIB_Virtual_LIBs_Initializing ENDP
;##################################################################################
;# Procedura, ktera uklada informace o 1 Virtualni knihovne v tabulkach knihoven
;##################################################################################
;# Vstup  : DS  = Datovy alias codoveho segmentu kernelu :)
;#          FS  = Libraries name descriptor
;#          GS  = Libraries info descriptor
;#          EDI = Cislo knihovny
;#          ESI = Ukazatel na zacatek virtualni knihovny
;#          Dl  = DPL call gate
;#          Dh  = CNT call gate
;# Nici   : Vsechno krome EDI
;##################################################################################
Virtual_Lib_LoadData Proc
         Push  EDI
         Push  EDI
          ;Ulozeni spravneho jmena jednotky do segmentu Librarires_Names_Info
          PUSH  ESI
          Push  FS
          Pop   ES
          Shl   EDI,4
          Add   ESI,LIB_F_Name
          Mov   ECX,4
          Rep   Movsd
          ;Vytvoreni call gate a ulozeni do segmentu Libraries_Info
          POP   ESI
          Add   ESI,LIB_F_CallGate
          Mov   BX,CS         ; Vstupni bod pro cpl3 = CS : EDI
          Mov   EDI,ESI
          Xor   AL,AL         ; alokovat v GDT
          Call  Memory_Manager_Create_CallGate
         Pop   EDI
         PUSH  EDX
          Mov   EAX,LMS_Struc_Size
          Mul   EDI
          Mov   EDI,EAX
         POP   EDX
         Mov   Ax,CS
         Mov   Word ptr  GS:[EDI.LIB_M_Version],01h
         Mov   Word ptr  GS:[EDI.LIB_M_CodeSel],AX
         Mov   Word ptr  GS:[EDI.LIB_M_UseCnt],0
         Mov   Word ptr  GS:[EDI.LIB_M_CallGate],DX
         Mov   Word ptr  GS:[EDI.LIB_M_Flags],Virtual_Lib_Flags
         Pop   EDI
         Ret
Virtual_Lib_LoadData ENDP
;##################################################################################
;# Procedura nastavuje informace o Virtualnich knihovnach v tabulkach knihoven
;##################################################################################
;# Vstup  : DS = Datovy alias codoveho segmentu kernelu :)
;#          FS = Libraries name descriptor
;#          GS = Libraries info descriptor
;# Vystup : None
;# Nici   : None
;##################################################################################
LIB_Virtual_LIBs_Update PROC
            Pushad
            Push  ES
            ;nastevani virtualni knihovny cislo 0
            Xor   EDI,EDI                 ;knihovna cislo 0
            Mov   ESI,Offset vl_libraries ;zacatek knihovny
            Xor   EDX,EDX
            Mov   Dl,3
            Call  Virtual_Lib_LoadData
            ;nastaveni virtualni knihovny cislo 1
            Mov   ESI,Offset vl_Mem       ;zacatek knihovny
            Inc   EDI                     ;knihovna cislo 1
            Xor   EDX,EDX
            Mov   Dl,3
            Call  Virtual_Lib_LoadData
            ;nastevani virtualni knihovny cislo 2
            Inc   EDI                     ;knihovna cislo 2
            Mov   ESI,Offset vl_task      ;zacatek knihovny
            Xor   EDX,EDX
            Mov   Dl,3
            Call  Virtual_Lib_LoadData
            ;nastevani virtualni knihovny cislo 3
            Inc   EDI                     ;knihovna cislo 3
            Mov   ESI,Offset vl_VFS       ;zacatek knihovny
            Xor   EDX,EDX
            Mov   Dl,3
            Call  Virtual_Lib_LoadData
            ;nastevani virtualni knihovny cislo 4
            Inc   EDI                     ;knihovna cislo 4
            Mov   ESI,Offset vl_system    ;zacatek knihovny
            Xor   EDX,EDX
            Call  Virtual_Lib_LoadData
            Pop ES
            Popad
            Ret
LIB_Virtual_LIBs_Update ENDP
;##################################################################################
;# Include part
;##################################################################################
;# Sem se includuje kazda hlavicka virtualni knihovny
;##################################################################################
 Include .\..\inc\code\code32\LIB\virtual\VL_mem.inc
 Include .\..\inc\code\code32\LIB\virtual\VL_task.inc
 Include .\..\inc\code\code32\LIB\virtual\VL_sys.inc
 Include .\..\inc\code\code32\LIB\virtual\VL_libs.inc
 Include .\..\inc\code\code32\LIB\virtual\VL_VFS.inc




