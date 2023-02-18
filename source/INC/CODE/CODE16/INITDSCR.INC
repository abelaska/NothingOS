;#################################################################################
;# Jednotka pro Inicializaci descriptoru
;#################################################################################
Descriptor_Initialization Proc
        mov     ax, GDT_Base shr 4
        mov     es, ax
;vynulovani pameti, ktera bude pouzivana jako GDT
        xor     edi, edi
        xor     eax, eax
        mov     ecx, 4000h
        cld
        rep     stosd
; Zinicializuje zakladni descriptory

        ;data
        mov     eax, Data_Base
        mov     word ptr ES:[Data_Descriptor+0], Data_Pages-1
        mov     word ptr ES:[Data_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[Data_Descriptor+4], al
        mov     byte ptr ES:[Data_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[Data_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[Data_Descriptor+7], ah

        ;gdt
        mov     eax, GDT_Base
        mov     word ptr ES:[GDT_Descriptor+0], GDT_Pages-1
        mov     word ptr ES:[GDT_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[GDT_Descriptor+4], al
        mov     byte ptr ES:[GDT_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[GDT_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[GDT_Descriptor+7], ah

        ;code
        mov     eax, Code_Base
        mov     word ptr ES:[Code_Descriptor+0], Code_Pages-1
        mov     word ptr ES:[Code_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[Code_Descriptor+4], al
        mov     byte ptr ES:[Code_Descriptor+5], Code_Atr1
        mov     byte ptr ES:[Code_Descriptor+6], Code_Atr2
        mov     byte ptr ES:[Code_Descriptor+7], ah

        ;stack
        mov     eax, Stack_Base
        mov     word ptr ES:[Stack_Descriptor+0], Stack_Pages-1
        mov     word ptr ES:[Stack_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[Stack_Descriptor+4], al
        mov     byte ptr ES:[Stack_Descriptor+5], Stack_Atr1
        mov     byte ptr ES:[Stack_Descriptor+6], Stack_Atr2
        mov     byte ptr ES:[Stack_Descriptor+7], ah

        ;idt
        mov     eax, IDT_Base
        mov     word ptr ES:[IDT_Descriptor+0], IDT_Pages-1
        mov     word ptr ES:[IDT_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[IDT_Descriptor+4], al
        mov     byte ptr ES:[IDT_Descriptor+5], IDT_Atr1
        mov     byte ptr ES:[IDT_Descriptor+6], IDT_Atr2
        mov     byte ptr ES:[IDT_Descriptor+7], ah

        ; textova videopamet
        mov     eax, 0B8000h
        mov     word ptr ES:[B8000_Descriptor+0], ((0C0000h-0B8000h)/4096)
        mov     word ptr ES:[B8000_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[B8000_Descriptor+4], al
        mov     byte ptr ES:[B8000_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[B8000_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[B8000_Descriptor+7], ah

        ; Graficka VideoPamet
        mov     eax, 0A0000h
        mov     word ptr ES:[A0000_Descriptor+0], ((0B0000h-0A0000h)/4096)
        mov     word ptr ES:[A0000_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[A0000_Descriptor+4], al
        mov     byte ptr ES:[A0000_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[A0000_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[A0000_Descriptor+7], ah

        ; BIOS area
        mov     eax, 0A0000h
        mov     word ptr ES:[BIOS_AREA_Descriptor+0], ((0FFFFFh-0A0000h)/4096)
        mov     word ptr ES:[BIOS_AREA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[BIOS_AREA_Descriptor+4], al
        mov     byte ptr ES:[BIOS_AREA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[BIOS_AREA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[BIOS_AREA_Descriptor+7], ah

        ;ide data
        mov     eax, offset IDE_DATA
        mov     word ptr ES:[IDE_DATA_Descriptor+0], IDE_DATA_SIZE+1
        mov     word ptr ES:[IDE_DATA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[IDE_DATA_Descriptor+4], al
        mov     byte ptr ES:[IDE_DATA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[IDE_DATA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[IDE_DATA_Descriptor+7], ah

        ;exec data
        mov     eax, offset EXEC_DATA
        mov     word ptr ES:[EXEC_DATA_Descriptor+0], EXEC_DATA_SIZE+1
        mov     word ptr ES:[EXEC_DATA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[EXEC_DATA_Descriptor+4], al
        mov     byte ptr ES:[EXEC_DATA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[EXEC_DATA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[EXEC_DATA_Descriptor+7], ah

        ;CONS data
        mov     eax, offset Console_Driver_DATA
        mov     word ptr ES:[CONS_DATA_Descriptor+0], Console_Driver_DATA_SIZE+1
        mov     word ptr ES:[CONS_DATA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[CONS_DATA_Descriptor+4], al
        mov     byte ptr ES:[CONS_DATA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[CONS_DATA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[CONS_DATA_Descriptor+7], ah


        ;KEYB data
        mov     eax, offset KEYB_DATA
        mov     word ptr ES:[KEYB_DATA_Descriptor+0], KEYB_DATA_SIZE+1
        mov     word ptr ES:[KEYB_DATA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[KEYB_DATA_Descriptor+4], al
        mov     byte ptr ES:[KEYB_DATA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[KEYB_DATA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[KEYB_DATA_Descriptor+7], ah

        ;EXT2 data
        mov     eax, offset EXT2_Driver_DATA
        mov     word ptr ES:[EXT2_DATA_Descriptor+0], EXT2_Driver_DATA_SIZE+1
        mov     word ptr ES:[EXT2_DATA_Descriptor+2], ax
        shr     eax, 16
        mov     byte ptr ES:[EXT2_DATA_Descriptor+4], al
        mov     byte ptr ES:[EXT2_DATA_Descriptor+5], Data_Atr1
        mov     byte ptr ES:[EXT2_DATA_Descriptor+6], Data_Atr2
        mov     byte ptr ES:[EXT2_DATA_Descriptor+7], ah


        Ret
Descriptor_Initialization ENDP