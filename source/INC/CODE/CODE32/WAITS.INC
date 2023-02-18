;###########################################################################
;# Soubor s casovymi smyckami Loop...
;###########################################################################

;###########################################################################
;# Procedura, ktera zjistuje efektivni rychlost compu
;###########################################################################
;# Vstup  : DS =  Datovy segment kernelu
;# Vystup : Upravuje hodnoty v promennych
;# Nici   : Nic
;###########################################################################
Wait_Loop_Initializing PROC
          Pushad
          Push   ES
          CLI
          Mov    Ax,IDT_Descriptor
          Mov    ES,Ax
          Mov    EDI,0F0h shl 3
          Push   Dword ptr ES:[EDI]
          Push   Dword ptr ES:[EDI+4]     ; uchovame puvodni rutinu preruseni...
          SetInterrupt  0F0h,Cs,Wait_Loop_Interrupt,INT_Atr1 ;nastavime si vlastni rutinku...
          STI
          Xor    ECX,ECX
          Mov    Al,34h
          Out    43h,Al
          Mov    Al,0a9h
          Out    40h,Al
          Mov    Al,4
          Out    40h,Al
          Loop   $
          CLI
          Mov    ECX,EDX
          Mov    EAX,EDX
          Mov    Dword ptr DS:[Loop_Wait_Delay_001ms],EDX
          Shl    EDX,3
          Shl    EAX,1
          Add    EDX,EAX
          Mov    Dword ptr DS:[Loop_Wait_Delay_010ms],EDX
          Mov    EAX,EDX
          Shl    EDX,3
          Shl    EAX,1
          Add    EDX,EAX
          Mov    Dword ptr DS:[Loop_Wait_Delay_100ms],EDX
          Mov    EAX,EDX
          Shl    EDX,3
          Shl    EAX,1
          Add    EDX,EAX
          Mov    Dword ptr DS:[Loop_Wait_Delay_001s],EDX
          Shr    ECX,4
          Inc    ECX
          Mov    Dword ptr DS:[Loop_Wait_Delay_062us],ECX
          Pop    Dword ptr ES:[EDI+4]
          Pop    Dword ptr ES:[EDI]
          STI
          Pop    ES
          Popad
          print wait_init_txt
          Ret
Wait_Loop_Initializing ENDP
;###########################################################################
;# Procedura, ktera pomoci loopu ceka 62us, plus minus autobus
;###########################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;###########################################################################
Wait_Loop_062us PROC
         Push  DS
         Push  ECX
         Mov   Cx,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,Dword ptr DS:[Loop_Wait_Delay_062us]
         Loop  $
         Pop   ECX
         Pop   DS
         Ret
Wait_Loop_062us ENDP
;###########################################################################
;# Procedura, ktera pomoci loopu ceka 1ms, plus minus autobus
;###########################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;###########################################################################
Wait_Loop_001ms PROC
         Push  DS
         Push  ECX
         Mov   Cx,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,Dword ptr DS:[Loop_Wait_Delay_001ms]
         Loop  $
         Pop   ECX
         Pop   DS
         Ret
Wait_Loop_001ms ENDP
;###########################################################################
;# Procedura, ktera pomoci loopu ceka 10ms, plus minus autobus
;###########################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;###########################################################################
Wait_Loop_010ms PROC
         Push  DS
         Push  ECX
         Mov   Cx,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,Dword ptr DS:[Loop_Wait_Delay_010ms]
         Loop  $
         Pop   ECX
         Pop   DS
         Ret
Wait_Loop_010ms ENDP
;###########################################################################
;# Procedura, ktera pomoci loopu ceka 100ms, plus minus autobus
;###########################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;###########################################################################
Wait_Loop_100ms PROC
         Push  DS
         Push  ECX
         Mov   Cx,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,Dword ptr DS:[Loop_Wait_Delay_100ms]
         Loop  $
         Pop   ECX
         Pop   DS
         Ret
Wait_Loop_100ms ENDP
;###########################################################################
;# Procedura, ktera pomoci loopu ceka 1s, plus minus autobus
;###########################################################################
;# Vstup  : None
;# Vystup : None
;# Nici   : None
;###########################################################################
Wait_Loop_001s PROC
         Push  DS
         Push  ECX
         Mov   Cx,Data_Descriptor
         Mov   DS,CX
         Mov   ECX,Dword ptr DS:[Loop_Wait_Delay_001s]
         Loop  $
         Pop   ECX
         Pop   DS
         Ret
Wait_Loop_001s ENDP
;###########################################################################
;# Pomocna interrupt procedura...
;###########################################################################
Wait_Loop_Interrupt PROC
          Mov    Al,34h
          Out    43h,Al
          Mov    Al,0a9h
          Out    40h,Al
          Mov    Al,4
          Out    40h,Al
          Mov    EDX,ECX
          Neg    EDX
          Or     EDX,EDX
          Jz     @Wait_Loop_Interrupt_Skip
          Mov    ECX,1
@Wait_Loop_Interrupt_Skip :
          Mov    Al,20h
          Out    20h,Al
          Iret
Wait_Loop_Interrupt ENDP