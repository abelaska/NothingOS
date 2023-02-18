; version 1.4
.386

Code Segment Public Use16 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;

Start:
  jmp   main
Starting           db 'NOLO 1.4'
startint_len       equ ($-Starting)
base               dw 01f0h ;+11
blocks_count       dw 0
Block_size         dw 0 ; v sektorech
Address_sector_LBA dd 0
wr_offs            equ (startint_len+1) shl 1

skok_kernel:
  ;skoci na kernel
  db    0eah
  dw    0 ; offset - IP
  dw    0 ; segment - CS

main:
  cli
  xor   di, di
  mov   ds, di
  mov   ax, 09000h
  mov   es, ax
  mov   si, 07c00h
  mov   cx, offset konec
  cld
  rep   movsb

 ; skoci na rutinu nacteni kernelu
 db 0EAh
 dw offset nacti_kernel
 dw 09000h

nacti_kernel:
  cli
  mov   ax, 08000h
  mov   ss, ax
  xor   ax, ax
  mov   sp, ax

  mov   ax, 3
  int   10h

 push   cs
 pop    ds
 push   0b800h
 pop    es
  mov   si, offset starting
  mov   ah, 0fh
  xor   di, di
  mov   cx, startint_len
napis:
  lodsb
  stosw
  dec   cx
  jnz   short napis

            mov    bp, di
            inc    bp
            inc    bp

            push   cs
            pop    es
             mov   di, offset konec
             mov   bx, offset Address_sector_LBA
             xor   cx, cx
             inc   cx
            push   di
             call  Read_disk
            pop    bx

             ;nacteni kernelu
             mov   cx, word ptr ds:[blocks_count]
             xor   di, di
             mov   es, di
load_blocks:
           push    bx cx
             mov   cx, word ptr ds:[block_size]
             call  Read_disk
; zjistime, jestli se ma nacist dalsi block
           pop     cx bx
             add   bx, 4
             dec   cx
             jnz   short load_blocks
             jmp   short skok_kernel

; vstup: CX - kolik sektoru precist
;        bx - ofs ukazatel na pozici, kde je ulozena pocatecni LBA adresa
Read_disk:
             mov   dx, word ptr ds:[BASE]
             Mov   Ax, word ptr ds:[bx]

             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al

             mov   ax, word ptr ds:[bx+2]

             Inc   Dl
             Out   Dx,Al

             Inc   Dl
             And   Ah,0Fh
             Mov   Al,11100000b  ;disk = master
             Or    Al,Ah
             Out   Dx,Al

             Sub   Dl,4
             Mov   Al,cl
             Out   Dx,Al

             Add   Dl,5
             Mov   Al,020h
             And   Ch,001h
             Or    Al,Ch
             Out   Dx,Al

Wait0003 :   In    Al,Dx
             Test  Al,80h
             Jnz   short Wait0003
             Test  Al,08
             Jz    short ATA_Error

             Sub   Dl,07

             Mov   Ah,cl
Wait0007 :
             Mov   CX,128
             Rep   Insd
             Add   Dl,7
Wait0008 :   In    Al,Dx
             Test  Al,80h
             Jnz   short Wait0008
             Test  Al,01h
             Jnz   short ATA_Error
             Sub   Dx,7
             Dec   Ah
             Jnz   short Wait0007
             jmp   short read_ok
ATA_Error :
  mov   al, 'E'
  jmp   short napis_result
Read_ok:
  mov   al, '.'
Napis_result:
 push   es di
  ; napise tecku
  push  0B800h
  pop   es
  mov   di, bp
  mov   ah, 0fh
  stosw
  mov   bp, di
 pop    di es
  ret
konec:
; dd pocatecni pozice cteni bloku

Code Ends
end Start

; 1.3-1-2 = boot loader ne nakopiruje do pameti od adresy 0F000H:0 pak na sebe
;           skoci a nacte kernel do pameti
; 1.3-1-3 = funkcni verze, doufam ...
; 1.4     = predelana funkce pro nacitani kernelu