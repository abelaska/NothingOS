; version 1.0-1-1
; max 446B
.386

Code Segment Public Use16 Para 'Code'
Assume CS:Code;DS:Nothing;ES:Nothing;

Start:
  cli
  xor   di, di
  mov   ss, di
  mov   si, 07c00h
  mov   sp, si
 push   di
 push   si

  mov   ds, di
  mov   ax, 09000h
  mov   es, ax
  mov   cx, offset konec
  cld
  rep   movsb

 ; skoci na rutinu nacteni kernelu
  db    0eah
  dw    offset Start_MBR
  dw    9000h

Start_MBR:
  mov   ax, cs
  mov   ds, ax

  mov   ax, 03h
  int   10h

  push  0b800h
  pop   es
  mov   si, offset texts
  xor   di, di
  xor   cx, cx
stt:
  cmp   cl, byte ptr ds:[Default_menu]
  jnz   short stt1
  mov   ah, 0fh
  jmp   short stt2
stt1:
  mov   ah, 07h
stt2:

  mov   al, 'F'
  stosw
  mov   al, cl
  add   al, '1'
  stosw
  mov   al, ' '
  stosw
  stosw

  push  cx
  mov   cl, 10
st1:
  lodsb
  stosw
  loop  st1
  pop   cx

  add   di, (80-14)*2
  inc   cx
  cmp   cl, byte ptr ds:[Boot_Menus_Count]
  jnz   stt

  mov   ah, 0fh
  xor   edx, edx
  mov   dl, byte ptr ds:[time_max]
 push   dx
 push   di
  mov   al, '.'
tecky:
  stosw
  dec   dl
  jnz   short tecky

 pop    di
 push   di
  add   di, 2*80*2
  mov   si, offset text
  mov   cl, text_lenght
text_wr:
  lodsb
  stosw
  loop  text_wr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; cekani
 pop    di
 pop    dx
  shl   dx, 1
  add   di, dx
  mov   fs, cx ;0
  shl   dx, 3
  mov   ecx, dword ptr fs:[46ch]
  add   edx, ecx
main_lop_2:
  mov   ebx, dword ptr fs:[46ch]
  cmp   ebx, ecx
  jl    next
  add   ecx, 16
  mov   al, 20h
  stosw
  sub   di, 4
next:
  cmp   edx, ebx
  jg    short dal_main
boot_def:
  xor   bx, bx
  mov   bl, byte ptr ds:[Default_menu]
  jmp   short akce
dal_main:
  mov   ah, 1
  int   16h
  jz    main_lop_2
  xor  ax, ax
  int  16h
  xchg al, ah
  cmp  al, 59
  jl   boot_def ;main_lop
  cmp  al, 67
  jg   boot_def ;main_lop
  xor  bx, bx
  mov  bl, al
  sub  bl, 59
  ; bl = 0..9
akce:

  mov  ax, 3
  int  10h
             ;nacteni kernelu
            push bx
             shl   bx, 2
             mov   dx, 1f0h
             Mov   Ax, word ptr ds:[positions+bx]
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
             Mov   Ax, word ptr ds:[positions+bx+2]
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
            pop bx
             Mov   Al, 11100000b
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             xor   ax, ax
             inc   ax
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,020h
             Out   Dx,Al
Wait0003 :   In    Al,Dx
             Test  Al,80h
             Jnz   Wait0003
             ;;
            pop   di
            pop   es
            push  es
            push  di
             ;;
             Sub   Dl,07
Wait0007 :   Mov   CX,256
             Rep   Insw
ATA_Error :
; skoci na nahrany boot program
 retf
Code_End:


text db 'OSLO v1.3'
text_lenght equ ($ - offset text)
Boot_Menus_Count db 3 ;1..9
time_max         db 5 ;sekundy
Default_menu     db 0
Positions dd 63 ;0
          dd 0 ;1
          dd 1632960 ;2
          dd 0 ;3
          dd 0 ;4
          dd 0 ;5
          dd 0 ;6
          dd 0 ;7
          dd 0 ;8
Texts     db 'DOS       ' ;0
          db 'LINUX     ' ;1
          db 'NOTHING   ' ;2
          db '          ' ;3
          db '          ' ;4
          db '          ' ;5
          db '          ' ;6
          db '          ' ;7
          db '          ' ;8

Konec: ;celkova velikost musi byt 512 B

Code Ends
end Start

