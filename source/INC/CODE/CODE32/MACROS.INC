;################################################################################################
; Zjisti, jestli je vzdaleny ukazatel nulovy
; Vstup:  DS:ESI - ukazatel na ukazatel :)
; Vystup: CF = 0 - neni nulovy
;         CF = 1 - je nulovy
;################################################################################################
Is_Far_Pointer_Null Proc
        ; selector (nesmi byt 0)
        cmp  word ptr ds:[esi+FP_Selector], 0
        jz   short Is_Far_Pointer_Null_is
        clc
        ret
Is_Far_Pointer_Null_is:
        stc
        ret
Is_Far_Pointer_Null Endp
;################################################################################################
;# Makro pro pouzivani procedury kPrint
;################################################################################################
Print Macro zdroj
       push  es edi eax
        mov  ax, data_descriptor
        mov  es, ax
        mov  edi, offset zdroj
        call kPrint
       pop   eax edi es
endm
;################################################################################################
;# Macro pro vytvoreni noveho tempovaciho descriptoru v GDT
;# Vystup : EDX - descriptor, if not C
;################################################################################################
Create_Temp_Descriptor MACRO Baze
   PUSH  EAX
    Mov   EAX, Baze
    Call  Create_Descriptor_GDT_Baze
   POP   EAX
endm
;################################################################################################
Dispose_Temp_Descriptor MACRO desc
  PUSH edx eax
   Mov   edx,desc
   Mov   eax, data_descriptor
   Mov   desc,eax
   Call  Memory_Manager_Dispose_Descriptor
  POP  eax edx
endm
;################################################################################################
Remap_Temp_Descriptor MACRO desc,newbaze
  PUSH  EDX EAX EDI
   PUSH  desc
    Push  newbaze
    mov   edx, desc
    mov   eax, data_descriptor
    mov   desc, eax
    pop   edi
    Call  MemMan_SetDescriptorBaseAddress
   POP   desc
  POP   EDI EAX EDX
endm
;################################################################################################
;# Macro pro vypis znaku a na obrazovku
;################################################################################################
v macro a
pushad
push es ds
pushfd
  push b8000_descriptor
  pop  es
  mov  ah,0Eh
  mov  al,a
  mov  word ptr es:[1620],ax
  call waitkey
;  mov ecx,0200000h
;  loop $
popfd
pop  ds es
popad
endm
;################################################################################################
;# Macro pro vypis registru v hexa na obrazovku
;################################################################################################
wr_cons macro a
pushad
push ds es fs gs
pushfd
  mov  eax,a
  push data_descriptor
  pop  ds
  mov  dword ptr ds:[num_txt_num], 0
  mov  dword ptr ds:[num_txt_num], eax
  print num_txt
popfd
pop  gs fs es ds
popad
endm

wr_cons_nowait macro a
pushad
push ds es fs gs
pushfd
  push data_descriptor
  pop  ds
  mov  dword ptr ds:[num_txt_num], 0
  mov  dword ptr ds:[num_txt_num], a
  print num_txt
popfd
pop  gs fs es ds
popad
endm
;################################################################################################
;# Macro pro vypis registru v hexa na obrazovku
;################################################################################################
wr_reg macro a
pushad
push ds es fs gs
pushfd
  mov  eax, a
  call hex2str_dd
  mov  edi, 24*160
  call write_text
  mov ecx,0F000000h
  loop $
popfd
pop  gs fs es ds
popad
endm

wr_reg_nowait macro a,pos
pushad
push ds es fs gs
pushfd
  mov  eax, a
  call hex2str_dd
  mov  edi, pos
  call write_text
popfd
pop  gs fs es ds
popad
endm


wr_reg_pos macro a,pos
pushad
push ds es fs gs
pushfd
  mov  eax, a
  call hex2str_dd
  mov  edi, pos
  call write_text
  call waitkey
popfd
pop  gs fs es ds
popad


endm

