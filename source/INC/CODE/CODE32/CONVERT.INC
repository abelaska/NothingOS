;#################################################################################
;# Jednotka pro konverzi cisel => retezec
;#################################################################################

;######################################################
;# Konverze BYToveho cisla na BIN string              #
;######################################################
; Vstup: AL = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
Bin2Str_DB Proc
 push   ds es ebx edx esi edi
  mov   edi, offset X2Str_Buffer
  mov   esi, edi
  mov   bx, data_descriptor
  mov   es, bx
  mov   ds, bx
  cld
  mov   esi, edi
  xor   ah, ah
  xchg  al, ah
  mov   cx, 8
next:
  rcl   ah, 1
  jc    Jedna
  mov   al, '0'
  stosb
  jmp   decremente
Jedna:
  mov   al, '1'
  stosb
decremente:
  dec   cx
  jnz   next
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 8
 pop    edi esi edx ebx es ds
  ret
Bin2Str_DB Endp

;######################################################
;# Konverze DWORDoveho cisla na BIN string            #
;######################################################
; Vstup: EAX = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
Bin2Str_DD Proc
 PUSH ES
 PUSH EDI
 PUSH EAX
 PUSH EBX
  push ax
   mov ax, data_descriptor
   mov ds, ax
  pop  ax
  cld
  mov   cx, ds
  mov   es, cx
  mov   edi, offset X2Str_Buffer
  mov   esi, edi
  mov   ebx, eax
  mov   cl, 16
  shr   eax, cl
  call  Konvert_Bin_Byte_AH
  xchg  ah, al
  call  Konvert_Bin_Byte_AH
  xchg  bx, ax
  call  Konvert_Bin_Byte_AH
  xchg  ah, al
  call  Konvert_Bin_Byte_AH
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 8*4
 POP EBX
 POP EAX
 POP EDI
 POP ES
  ret
Konvert_Bin_Byte_AH:
 PUSH AX
  mov   ch, 8
next1:
  rcl   ah, 1
  jc    Jedna1
  mov   al, '0'
  stosb
  jmp   decremente1
Jedna1:
  mov   al, '1'
  stosb
decremente1:
  dec   ch
  jnz   next1
 POP AX
  ret
Bin2Str_DD Endp

;######################################################
;# Konverze BYToveho cisla na HEX string              #
;######################################################
; Vstup: AL = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
Hex2Str_DB Proc
 PUSH ES
 PUSH EDI
 PUSH EAX
 PUSH EBX
  push ax
   mov ax, data_descriptor
   mov ds, ax
  pop  ax
  cld
  mov   bx, ds
  mov   es, bx
  mov   edi, offset X2Str_Buffer
 PUSH EDI
  xchg  bl, al
  xor   eax, eax
  xchg  bl, al
  mov   esi, offset Numbers_Tab
 PUSH AX
  and   al, 11110000b
  shr   al, 4
  add   esi, eax
  lodsb
  stosb
 POP AX
  mov   esi, offset Numbers_Tab
  and   al, 00001111b
  add   esi, eax
  lodsb
  stosb
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 2
 POP ESI
 POP EBX
 POP EAX
 POP EDI
 POP ES
  ret
Hex2Str_DB Endp

;######################################################
;# Konverze DWORDoveho cisla na HEX string              #
;######################################################
; Vstup: EAX = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
Hex2Str_DD Proc
 PUSH ES
 PUSH EDI
 PUSH EAX
 PUSH EBX
  push ax
   mov ax, data_descriptor
   mov ds, ax
  pop  ax
  mov   bx, ds
  mov   es, bx
  mov   edi, offset X2Str_Buffer
 PUSH EDI
  cld
  mov   ebx, eax
  mov   cl, 16
  shr   eax, cl
  xchg  al, ah
  call  Konvert_Hex_Byte_AL
  xchg  al, ah
  call  Konvert_Hex_Byte_AL
  xchg  ax, bx
  xchg  al, ah
  call  Konvert_Hex_Byte_AL
  xchg  al, ah
  call  Konvert_Hex_Byte_AL
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 8
 POP ESI
 POP EBX
 POP EAX
 POP EDI
 POP ES
  ret
Konvert_Hex_Byte_AL:
 PUSH EAX
 PUSH BX
  xchg  bl, al
  xor   eax, eax
  xchg  bl, al
 POP BX
  mov   esi, offset Numbers_Tab
 PUSH AX
  and   al, 11110000b
  shr   al, 4
  add   esi, eax
  lodsb
  stosb
 POP AX
  mov   esi, offset Numbers_Tab
  and   al, 00001111b
  add   esi, eax
  lodsb
  stosb
 POP EAX
  ret
Hex2Str_DD Endp

;######################################################
;# Konverze BYToveho cisla na DEC string              #
;######################################################
; Vstup: AL = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
DEC2Str_DB Proc
 PUSH ES
 PUSH EDI
 PUSH EAX
 PUSH EBX
  push ax
   mov ax, data_descriptor
   mov ds, ax
  pop  ax
  mov   bx, ds
  mov   es, bx
  mov   edi, offset X2Str_Buffer+2
  xchg  al, bl
  xor   eax, eax
  xchg  al, bl
  aam
  xor   bx, bx
  xchg  ah, bh
  mov   esi, offset Numbers_Tab
  add   esi, eax
  cld
  lodsb
  std
  stosb
  xchg  al, bh
  aam
  xor   bx, bx
  xchg  ah, bh
  mov   esi, offset Numbers_Tab
  add   esi, eax
  cld
  lodsb
  std
  stosb
  xchg  al, bh
  aam
  xor   bx, bx
  xchg  ah, bh
  mov   esi, offset Numbers_Tab
  add   esi, eax
  cld
  lodsb
  std
  stosb
  cld
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 3
  ; ukazatel na zacatek dat
  mov   esi, offset X2Str_Buffer
 POP EBX
 POP EAX
 POP EDI
 POP ES
  ret
Dec2Str_DB Endp

;######################################################
;# Konverze DWORDoveho cisla na DEC string            #
;######################################################
; Vstup: EAX = Cislo pro prevedeni
;######################################################
; Vystup: DS:ESI = Adresa na buffer s vysledkem
;         ECX    = Delka vysledneho cisla
;######################################################
DEC2Str_DD Proc
 PUSH ES
 PUSH EDI
 PUSH EAX
 PUSH EBX
 PUSH EDX
  push ax
   mov ax, data_descriptor
   mov ds, ax
  pop  ax
  mov   bx, ds
  mov   es, bx
  mov   edi, offset X2Str_Buffer
 PUSH EDI
  xor   ebx, ebx
  mov   bl, 10
  xor   edx, edx
  cld
Pocitej_Dec:
 PUSH EAX
  dec   ebx
  div   dword ptr DS:[DEC_Numbers+EBX*4]
  mov   esi, offset Numbers_Tab
  add   esi, eax
  xchg  eax, ecx
  lodsb
  stosb
  xchg  eax, ecx
  xor   edx, edx
  mul   dword ptr DS:[DEC_Numbers+EBX*4]
  inc   ebx
  mov   ecx, eax
 POP EAX
  sub eax, ecx
  dec ebx
  jnz Pocitej_Dec
  ; vrati delku vysledneho retezce
  xor   ecx, ecx
  mov   cl, 10
 POP ESI
 POP EDX
 POP EBX
 POP EAX
 POP EDI
 POP ES
  ret
Dec2Str_DD Endp


DEC2Str_DD_WithoutNulls Proc
  PUSH  EAX
   Call DEC2Str_DD
@DEC2Str_DD_WithoutNullsTestnig_Zeros :
   Lodsb
   Cmp  Al,'0'
   Jnz  @DEC2Str_DD_WithoutNullsExit_Loop
   Loop @DEC2Str_DD_WithoutNullsTestnig_Zeros
   Inc  ECX
@DEC2Str_DD_WithoutNullsExit_Loop :
   Dec   ESI
  POP   EAX
  ret
Dec2Str_DD_WithoutNulls Endp

;-----------------------------------------------
; 1.1-1-1 = vsechny funkce testovany pro 32 bitove kodovani a funkcni