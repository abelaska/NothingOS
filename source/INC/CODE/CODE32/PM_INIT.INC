;###########################################################################
;# Jednotka, ktera obsahuje zakladni inicializacni proceduru kernelu
;###########################################################################
;# Pozor na prehozeni jednotlivych procedur.... to by bylo hodne spatne...
;###########################################################################
Kernel_Initialization PROC
        Mov   Ax,ES
        Mov   FS,AX
        Mov   GS,AX
        ; zjisteni velikosti pameti
        Call  Memory_Size_Detection
        ; Inicializace strankovani
        Call  Page_Initializing
        ; Inicializace obsluhy preruseni
        Call  IDT_Initializing
        ; Inicializace radice preruseni
        Call  IRQ_Initializing
        Sti
        ; nacte standartni nastaveni palety
        Call  Pallete_Init
        ; Inicializace konzoli
        Call  Consoles_Init
        ; inicializace FPU - musi byt pred TaskManem, else error
        Call  FPU_Initializing
        ; Inicializace TaskMana
        Call  TSK_Initializing
        ; Inicializace cekacich smycek
        Call  Wait_Loop_Initializing
        ; inicializace prostredi pro nacteni file systemu
        call  FS_Init
        ; Inicializace systemu pro praci s FS
        Call  DRV_main_Init
        ;Inicializace Swap Deamona
        Call SwapDeamon_Initializing
        ; inicializujeme virtual file system
        call  VFS_Init
        ; namontujeme root
        call  VFS_Mount_Root
        ; Inicializace Knihoven
        Call  LIB_Initializing
        ; Nahraje a inicializuje drivery
        call  DRV_Load_Drivers
        ; spustime init
        Call  Execute_INIT
Kernel_Initialization ENDP


Nothing_Welcome Proc
       PUSH DS
       Mov  EAX,Data_Descriptor
       Mov  DS,EAX
        Print Nothing_Init_txt
        Mov   Al,Byte ptr DS:[Processor_Type]
        Add   Al,'0'
        Mov   Byte ptr DS:[CompInfoText2],Al
        Print CompInfoText1
        Print CompInfoText3
       POP  DS
      RET
Nothing_Welcome ENDP
