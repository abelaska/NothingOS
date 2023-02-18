;####################################################
;# Kernel New Generation :))))
;####################################################
;# Version 1.2-1-1
;####################################################
.486P
;####################################################
; Datovy segment
;####################################################
Data segment public Para
Assume DS:Nothing;
Start_Data:
             Include _ds_inc.Inc
nonalign_Data_end:
Align 4096
End_Data:
Data Ends

;####################################################
; 16 bitovy zavadec jadra
;####################################################
Code Segment Public Use16 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;SS:Nothing;
;###############################
Start_Code16:
        cli
        ;vypneme motor disketove mechaniky :) ked uz to BIOS tak zmrdal...
        Mov     DX,3F2H
        Mov     Al,0Dh
        Out     Dx,Al
        Mov     Al,0Fh
        Out     Dx,Al
        Mov     DX,372H
        Mov     Al,0Dh
        Out     Dx,Al
        Mov     Al,0Fh
        Out     Dx,Al

        mov     ax, Data_Base shr 4
        mov     ds, ax

        ;nastaveni zasobniku
        mov     ax, 9000h ;Stack_Base shr 4
        mov     ss, ax
        xor     esp, esp
        dec     esp
        dec     esp

        call    Init_80_30_Text
        call    clrscr_16
;        call    Detekce_Procesoru

        ;vynulovani pameti, ktera bude pouzivana jako IDT
        mov     eax, IDT_Base shr 4
        mov     es, ax
        xor     edi, edi
        xor     eax, eax
        mov     ecx, 400h
        cld
        rep     stosd
        ;vynulovani pameti, ktera bude pouzivana jako MemTab
        Call    A20_Enable_Gate_0001
        Test    Al,0FFh
        Jz      @Kernel_Initializing_Continue
@Kernel_Initializing_A20_Error :
        Mov     EAX,er_KER+0001h
        Call    Fatal_Error_16
        ;olala, nastala chyba pri inicializaci A20 brany... haltujeme...
@Kernel_Initializing_Continue :
        Call    Descriptor_Initialization
        ;nacteni GDT
        LGDT    pword ptr DS:[TEMP_pointer]
        ;nacteni IDT
        Mov     Word ptr  Ds:[TEMP_Pointer],256*8
        Mov     Dword ptr DS:[TEMP_Pointer+2],IDT_Base
        LIDT    pword ptr DS:[TEMP_pointer]
        ; vyprazdni frontu instrukci
        jmp     ResetQ1
ResetQ1:; nastavime PE bit v Cr0 (Protected mode Enabled)
        smsw    ax
        or      ax,1
        lmsw    ax
        jmp     ResetQ2
ResetQ2:
        ;nastaveni hodnot segmentovych registru pro praci v protektu
        mov     ax, Data_Descriptor
        mov     ds, ax
        mov     ax, B8000_Descriptor
        mov     es, ax
        mov     ax, Stack_descriptor
        mov     ss,ax

        jmp     fword ptr DS:[JMP2Protect]
End_Code16:
;######### 16-ti bitove Includy
        Include _cs_16.inc
Code Ends
;####################################################
;####################################################
;# Samotne 32 bitove jadro systemu
;####################################################
;####################################################
code32 segment use32
assume  cs:code32
;######### 32-ti bitove Includy
SCode32 :
        Include _cs_32.inc
Start_Code32:
        Call Kernel_Initialization

jmp $


test :
 mov ax,b8000_descriptor
 mov es,ax
 mov ax,data_descriptor
 mov ds,ax
 mov ecx,100000
loop $
 mov ax,word ptr es:[1620]
 mov ah,02
 inc al
 mov word ptr es:[1620],ax
 mov eax, dword ptr ds:[system_timer]
 call hex2str_dd
 mov edi,1600+160+160
 call write_text
 jmp test



test2 :
 mov ax,b8000_descriptor
 mov es,ax
 mov ax,data_descriptor
 mov ds,ax
 mov ecx,2000000
loop $
 mov ax,word ptr es:[1680]
 mov ah,04
 inc al
 mov word ptr es:[1680],ax
 jmp test2

        Call  DRV_Done
        jmp $

End_Code32:
Code32 ends

;####################################################
; Globalni kodove Konstanty
;####################################################
Data_Size     equ (End_Data-Start_Data)
Real_Data_size equ (nonalign_Data_end-start_data)
Code16_Size   equ (End_Code16-Start_Code16)
Code32_Size   equ (End_Code32-SCode32)
Code_Size     equ (Code16_Size+Code32_Size) ; 32 bitovy code segment je pred 16 bitovym code segmentem, proto se jejich delka scita
Delka_kernelu equ (Data_Size+Code_Size)

end Start_Code16
