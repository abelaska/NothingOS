;############################################################################
;# Datove structury a promenne TaskMana
;############################################################################

;################################################################################
;# zakladni konstanty
;################################################################################
Max_Task                   equ 512  ; pocet tasku
Start_Max_Zoombies         equ 1    ; maximalni pocet mrtvych tasku, ktere lze vsak stale obnovit - po startu systemu
Max_Priority               equ 32   ; maximalni priorita
Task_Descriptors_Start_Pos equ (8192 - (Max_Task*2)) ;offset v GDT, kde zacinaji tasky
;################################################################################
;# Descriptory taskmana
;################################################################################
TaskMan_TSK_Info_Descriptor           dw Data_descriptor ;obsahuje info struktury vsech tasku
TaskMan_TSK_TSK_Layout_Descriptor     dw Data_descriptor ;obsahuje rozlozeni prepinani tasku
TaskMan_CR3s_Descriptor               dw Data_descriptor ;obsahuje seznam vsech CR3 existujicich
                                                         ;v systemu
TaskMan_CR3_Semafor                   db 0
;################################################################################
;#  dulezite promenne
;################################################################################
Task_Switching_Disabled   db 00h   ; promena, ktera neni-li nulova, nelze prepnout task
TSK_Time_Switch_Const     dw 5966  ; konstanta casovace => 200Hz

Current_Layout_Pos        dd 0     ; pozice v rozlozeni tasku, ktera je prave zpracovavana
Current_Layout_Length     dd 0     ; pocet zaznamu v layout
Current_Task_Num          dd 0     ; pocet tasku, momentalne existujicich
Max_Zoombies              dd 0     ; Pocet zombii

Last_Task                 dd 0     ; cislo posledne zavadeneho tasku
Current_Task_TSS_Sel      dw 0     ; ukazatel na TSS alias soucastneho tasku
Current_Task              dd 0     ; Cislo prave provadeneho tasku
TaskMan_TSS_Alias_Sel     dw 0     ; alias ma TSS taskmana
;################################################################################
;# Typy systemovy descriptoru
;################################################################################
TSS_Descriptor_Type_Free equ 00001001b
TSS_Descriptor_Type_Busy equ 00001011b
LDT_Descriptor_Type      equ 00000010b
TSS_Size     equ 1+(TSS_Struc_Size shr 12)    ;kolik 4k stranek je potreba na ulozeni TSS ?
;################################################################################
;# Status tasku
;################################################################################
_TSK_Status_RUN             equ  01h     ;Proces bezi normalne
_TSK_Status_SLEEP           equ  02h     ;Proces spi, ceka na probuzeni vnejsi udalosti
_TSK_Status_STOP            equ  03h     ;Proces je zastaven
_TSK_Forever_Run            equ  04h     ;Proces, ktery nelze zabit
_TSK_Zoombie                equ  0FFFFh  ;Proces se chce nechat zabit :)
;################################################################################
;# Priority tasku
;################################################################################
High_Priority             equ  16
Normal_Priority           equ  07
Low_Priority              equ  01
RealTime_High_Priority    equ  16+High_Priority
RealTime_Normal_Priority  equ  16+Normal_Priority
RealTime_Low_Priority     equ  16+Low_Priority
;################################################################################
;# Struktura rozlozeni pageu ve virtualni pameti tasku
;# cisla jsou pocet podadresaru obsazenych jednotlivymi bloky
;################################################################################
_4M                          equ  (4*1024*1024)
_Virtual2FysMapedMem_Start   equ  0
_Vistual2FysMapedMem_Size    equ  (1024*1024*1024)/_4M   ;(1G)
_DynamicMem_Start            equ  (_Virtual2FysMapedMem_Start+_Vistual2FysMapedMem_Size)
_DynamicMem_Size             equ  (2*(1024*1024*1024/_4M)) ;(2G)
_StackPrgMem_Start           equ  (_DynamicMem_Start+_DynamicMem_Size)
_StackPrgMem_Size            equ  (512*1024*1024)/_4M ;(512M)
_StackCPL0Mem_Start          equ  (_StackPrgMem_Start+_StackPrgMem_Size)
_StackCPL0Mem_Size           equ  (4*1024*1024)/_4M; (4M)
_FreeMem_Start               equ  (_StackCPL0Mem_Start+_StackCPL0Mem_Size)
;################################################################################
;# Struktura UID a GID
;################################################################################
;# Lo(DD) -> UID
;# HI(DD) -> GID
;################################################################################
;# Struktura informacni tabulky tasku
;################################################################################
TSK_Info_Struc Struc
TSK_Info_start equ $
          TSK_Info_ID             dd 0        ;cislo tasku
          TSK_Info_ParentTask     dd 0        ;cislo rodicovskeho tasku
          TSK_Info_ChildTask      dd 0        ;cislo ditetovskeho tasku
          TSK_Info_OlderBrother   dd 0        ;cislo tasku starsiho tasku
          TSK_Info_YoungerBrother dd 0        ;cislo tasku mladsiho bratra
          TSK_Info_Priority       dw 0        ;priorita
          TSK_Info_Status         dw 0        ;status
          TSK_Info_UID_GID        dd 0        ;UID a GID tasku
          TSK_Info_UID_GID_Eff    dd 0        ;efektivni UID a GID
          TSK_Info_UID_GID_fs     dd 0        ;UID a GID pro filesystem
          TSK_Info_UID_GID_save   dd 0        ;Ulozene UID a GID
          TSK_Info_Variable       dd 0        ;promenna ma ruzna vyuziti
          TSK_Info_Selektor       dw 0        ;ukazatel na TSS tasku
TSK_Info_Size equ ($ - TSK_Info_start)
TSK_Info_Struc ENDS
;################################################################################
;# Struktura tasku(TSS)
;################################################################################
TSK_Params_Max_Length             equ 2048
TSK_Params_End                    equ (Offset TSS_Parameters+TSK_Params_Max_Length)
Max_Signals                       equ 32

TSS_Struc Struc
TSS_Struc_Start equ $
           TSS_Link     dw 0
                        dw 0
           TSS_CPL0_ESP dd 0
           TSS_CPL0_SS  dw 0
                        dw 0
           TSS_CPL1_ESP dd 0
           TSS_CPL1_SS  dw 0
                        dw 0
           TSS_CPL2_ESP dd 0
           TSS_CPL2_SS  dw 0
                        dw 0
           TSS_CR3      dd 0
           TSS_EIP      dd 0
           TSS_EEFLAGS  dd 0
           TSS_EAX      dd 0
           TSS_ECX      dd 0
           TSS_EDX      dd 0
           TSS_EBX      dd 0
           TSS_ESP      dd 0
           TSS_EBP      dd 0
           TSS_ESI      dd 0
           TSS_EDI      dd 0
           TSS_ES       dw 0
                        dw 0
           TSS_CS       dw 0
                        dw 0
           TSS_SS       dw 0
                        dw 0
           TSS_DS       dw 0
                        dw 0
           TSS_FS       dw 0
                        dw 0
           TSS_GS       dw 0
                        dw 0
           TSS_LDT      dw 0
                        dw 0
           TSS_Trap     dw 0 ; pokud se sem da 1, bude se tento task po jeho spusteni krokovat
           TSS_UnFrm    dw 0 ; pri vytvoreni TSS doplnit !!!!
          TSS_Formated            equ $
          TSS_Create_Time         Date_Time <> ; datum a cas vytvoreni procesu
          TSS_Coprocesor_Register dd 7 dup(0)    ; registr koprocesoru
          TSS_STx                 db 8*10 dup(0) ; registry koprocesoru
          TSS_Sel_Data_Libs       dw Max_Libs dup(0) ; selectory na taskem pouzivane knihovny
          TSS_Libs_Count          dd 0 ; kolik knihoven task pouziva
          TSS_VFS_Inode           dd 0 ; VFS inoda souboru, ktery je spusten
          TSS_Handle_STDOUT       dd 0 ; rukojet na otevreny standartni vystup
          TSS_Handle_STDIN        dd 0 ; rukojet na otevreny standartni vstup
          TSS_Handle_STDERR       dd 0 ; rukojet na otevreny standartni chybovy vystup
          TSS_Data_Memory         dd 0 ; kolik datove pameti ma task alokovane v LDT
          TSS_Stack_Memory        dd 0 ; jak velky zasobnik ma task
          TSS_Count_Open_File     dd 0 ; kolik souboru ma task otevrenych
          TSS_Time_Used           dd 0 ; kolik casovych jednotek task spotreboval
          TSS_Signal_Handle       dd Max_Signals dup(0) ; adresy rutin pro obsluhu signalu
          TSS_Home_VFS_Inode      dd 0 ; VFS inoda adresare, ze ktereho byl task spusten
          TSS_Temp_VFS_Inode      dd 0 ; VFS inoda adresare, ktery task pouziva jako pracovni
          TSS_Work_VFS_Inode      dd 0 ; VFS inoda prave adresare, ve kterem prave pracujeme
          TSS_Program_Name        db 32 dup (0)
          TSS_Parameters_Length   dd 0 ; delka parametru
          TSS_Parameters          db TSK_Params_Max_Length dup (0) ;parametry spusteni
 TSS_Formated_End        equ $
          TSS_Ports               dw 0FFFFh
 TSS_Struc_Size          equ ($ - TSS_Struc_Start)
TSS_Struc Ends


; Dulezite konstanty
 TSS_UnFormated_Offset   equ TSS_Formated_End
 TSS_Formated_Part_Size  equ TSS_Formated_End - TSS_Formated




