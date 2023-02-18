;###########################################################################################
;# Obsluha preruseni sekundarniho IDE radice
;###########################################################################################
Secondary_HardDisk_IRQ_Routine :
       Pushfd
       Push  EDX
       Push  EAX
       Push  ES
       Mov   DX,Data_descriptor
       Mov   ES,DX
       Mov   Dx,177h
       In    Al,DX
       Mov   Byte ptr ES:[IDE_Flags],Al
       Mov   Al,20h
       Out   0A0H,Al
       Out   020h,Al
       Pop   ES
       Pop   EAX
       Pop   EDX
       Popfd
       Iret