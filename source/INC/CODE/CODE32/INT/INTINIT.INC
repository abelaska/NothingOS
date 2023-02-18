;#################################################################################
;# Jednotka pro nastaveni spravnych addres dannym prerusenim
;#################################################################################
SetInterrupt Macro Interrupt,Sel,Ofs,Flag
    Mov     Word ptr Es:[(Interrupt Shl 3).IDT_struc_Selector],Sel
    Mov     EAX,Offset Ofs
    Mov     Es:[(Interrupt shl 3).IDT_struc_Offset_lo],Ax
    Shr     EAX,16
    Mov     Es:[(Interrupt shl 3).IDT_struc_Offset_Hi],Ax
    Mov     Es:[(Interrupt shl 3).IDT_struc_Flags],Flag
EndM
;#################################################################################
IDT_Initializing PROC
        Push    ES
        Push    EAX

        Mov     Ax,IDT_Descriptor
        Mov     Es,Ax
;nastaveni GP
        SetInterrupt 000H CS DE_Fault_Routine,Int_atr1
        SetInterrupt 007H CS NM_Fault_Routine,Int_atr1
        SetInterrupt 008H CS DF_Fault_Routine,Int_atr1
        SetInterrupt 009H CS CO_Fault_Routine,Int_atr1
        SetInterrupt 00AH CS TS_Fault_Routine,Int_atr1
        SetInterrupt 00BH CS NP_Fault_Routine,Int_atr1
        SetInterrupt 00CH CS SS_Fault_Routine,Int_atr1
        SetInterrupt 00DH CS GP_Fault_Routine,Int_atr1
        SetInterrupt 00EH CS MM_Page_Fault,Int_Atr1
        SetInterrupt 0B1H CS Main_Kernel_Interrupt,Int_atr2
;nastaveni irqcek
        SetInterrupt 0F0H CS IntReturn,Int_atr1
        SetInterrupt 0F1H CS IntReturn,Int_atr1
        SetInterrupt 0F2H CS IntReturn,Int_atr1
        SetInterrupt 0F3H CS IntReturn,Int_atr1
        SetInterrupt 0F4H CS IntReturn,Int_atr1
        SetInterrupt 0F5H CS IntReturn,Int_atr1
        SetInterrupt 0F6H CS IntReturn,Int_atr1
        SetInterrupt 0F7H CS IntReturn,Int_atr1
        SetInterrupt 0F8H CS IntReturn,Int_atr1
        SetInterrupt 0F9H CS IntReturn,Int_atr1
        SetInterrupt 0FAH CS IntReturn,Int_atr1
        SetInterrupt 0FBh CS IntReturn,Int_atr1
        SetInterrupt 0FCH CS IntReturn,Int_atr1
        SetInterrupt 0FDH CS IntReturn,Int_atr1
        SetInterrupt 0FEH CS IntReturn,Int_atr1
        SetInterrupt 0FFH CS IntReturn,Int_atr1
        Pop   EAX
        Pop   ES
        Ret
IDT_Initializing ENDP
