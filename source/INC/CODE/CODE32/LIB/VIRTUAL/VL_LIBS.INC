;#############################################################################
;# Virtualni jednotka Libraries
;#############################################################################

;#############################################################################
;# Zakladni knihovna, call gate na ni lze kdykoliv ziskat
;#  fci EBX = 00000001  int 0b1h
;#############################################################################
;# Fce      - name
;#############################################################################
;# 00000000 - LIB_TestLIBExist
;# 00000001 - LIB_Get_Lib_Version
;# 00000002 - LIB_Get_CallGate
;#############################################################################

VL_Libraries:
;################ Emulovana structura souboru knihovny
DB   'LIB',0                 ; Signatura
DW   0001h                   ; Version
DB   'libraries       '      ; Library Name
DW   0000h                   ; Unit Number  - not used in Virtual +18
DD   00000000h               ; CodeSeg size - not used in Virtual +20
Jmp VL_Lib_EnterPoint_3  ;+???
;##################################################################################
;# Vstupni bod knihovny
;##################################################################################
VL_Lib_EnterPoint_3 :
          Cmp    EAX,VL_Libs_Num_Of_Fce
          Ja     Short @VL_Lib_End
        PUSH     DS
          PUSH   DATA_Descriptor
          Pop    DS
          Mov    EAX,Dword ptr DS:[VL_Libs_Fce+EAX*4]
        POP      DS
          PUSH   EAX
          Call   Dword ptr SS:[ESP]
          Jc     @VL_LIB_End_C
          Add    ESP,4
          CLC
          Jmp     @VL_LIB_End
   @VL_LIB_End_C :
          ADD     ESP,4
          STC
   @VL_Lib_End :
        RetF
