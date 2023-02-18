Unit ATA;
Interface
 Procedure HD_Read(LBA:Longint;Num : Byte;Buffer : Pointer);
 Procedure HD_Write(LBA:Longint;Num : Byte;Buffer : Pointer);
 Procedure HD_SetInterrupt;
 Procedure HD_RestoreInterrupt;
 procedure ATA_WriteSectors(adr : longint;sektoru, opak : byte;A:pointer);
 Procedure ATA_ReadSectors(adr : longint;sektoru, opak : byte;a:pointer);


Implementation
Var flag : byte;
    ppp : pointer;
    I : Word;
    radic1,radic2 : byte;

Procedure ATA_ReadSectors(adr : longint;sektoru, opak : byte;a:pointer);assembler;
asm

mov dx,1f0h

mov byte ptr ds:[flag],0
Mov AX,Word ptr [adr]
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
Mov AX,Word ptr [adr+2]
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             Mov   Al,11100000b
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,sektoru
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,020h
             Mov   CH,opak
             And   Ch,001h
             Or    Al,Ch
             Out   Dx,Al
             Sub   Dl,7

{             Add   DX,206h
@@wt:        In    Al,DX
             Test  Al,80h
             Jnz   @@wt
             Sub   DX,206h
             Test  Al,1
             Jnz   @ATA_Error}

             Mov   AX,word ptr [a+2]
             Mov   ES,AX
             Mov   DI,word ptr [a]
             Mov   Ah,sektoru

@@IDE_Wait0007:
             Mov   Al,Byte ptr DS:[Flag]
             Test  Al,0FFh
             Jz    @@IDE_Wait0007
             Mov   Byte ptr DS:[Flag],0
             Mov   CX,256
             Rep   Insw
             Dec   Ah
             Jnz   @@IDE_Wait0007
@ATA_Error:
End;




procedure ATA_WriteSectors(adr : longint;sektoru, opak : byte;A:pointer);assembler;
Asm
push ds
mov dx,1f0h

mov byte ptr ds:[flag],0
Mov AX,Word ptr [adr]
             Add   Dl,3
             Out   Dx,Al
             Xchg  Al,Ah
             Inc   Dl
             Out   Dx,Al
Mov AX,Word ptr [adr+2]
             Inc   Dl
             Out   Dx,Al
             Inc   Dl
             And   Ah,0Fh
             In    Al,Dx
             Or    Al,Ah
             Out   Dx,Al
             Sub   Dl,4
             Mov   Al,sektoru
             Out   Dx,Al
             Add   Dl,5
             Mov   Al,030h
             Mov   CH,opak
             And   Ch,01h
             Or    Al,Ch
             Mov   Ah,Cl
             Out   Dx,Al
             Sub   Dl,7

             mov   ax,ds
             mov   es,ax
             Mov   AX,word ptr [a+2]
             Mov   DS,AX
             Mov   SI,word ptr [a]
             Mov   Ah,sektoru

             Add   DX,206h
@@wt:        In    Al,DX
             Test  Al,80h
             Jnz   @@wt
             Sub   DX,206h
             Test  Al,1
             Jnz   @ATA_Error

@@IDE_Wait000A:
             Mov   CX,256
             Rep   Outsw
@here :      mov   al, byte ptr es:[flag]
             test  al, 0FFh
             jz    @here
             test  al, 1
             jnz   @ATA_Error
             mov   byte ptr es:[flag], 0
             Dec   Ah
             Jnz   @@IDE_Wait000A
@ATA_Error:
 pop ds
End;



procedure inter;interrupt;assembler;
asm
 push  ds
 push  ax
 push  dx
 mov   dx, seg @data
 mov   ds, dx
 mov   dx, 3f6h
 mov   al, 8
 out   dx, al
 mov   dx,1f7h
 in    al,dx
 mov   byte ptr DS:[flag],al
 pop   dx
 mov   al,20h
 out   0a0h,al
 out   20h,al
 pop   ax
 pop   ds
end;


Procedure HD_SetInterrupt;Assembler;
Asm
  cli
  xor ax,ax
  mov es,ax
  mov bx,76h*4
  mov ax,word ptr es:[bx]
  mov word ptr [ppp],ax
  mov ax,word ptr es:[bx+2]
  mov word ptr [ppp+2],ax
  mov ax,offset inter;
  mov word ptr es:[bx],ax
  mov ax,seg(inter)
  mov word ptr es:[bx+2],ax

  in al, 21h
  mov [radic1], al
  mov al, 0FBh
  out 21h, al

  in al, 0a1h
  mov [radic2], al
  mov al, 0BFh
  out 0a1h, al

  sti
End;




Procedure HD_RestoreInterrupt;Assembler;
asm
  cli
  xor ax,ax
  mov es,ax
  mov bx,76h*4
  mov ax,word ptr [ppp]
  mov word ptr es:[bx],ax
  mov ax,word ptr [ppp+2]
  mov word ptr es:[bx+2],ax

  mov al, [radic1]
  out 21h, al
  mov al, [radic2]
  out 0a1h, al

  sti
end;


Procedure HD_Read(LBA:Longint;Num : Byte;Buffer : Pointer);
Begin
 HD_SetInterrupt;
 ATA_ReadSectors(LBA,Num,1,Buffer);
 HD_RestoreInterrupt;
End;


Procedure HD_Write(LBA:Longint;Num : Byte;Buffer : Pointer);
Begin
 HD_SetInterrupt;
 ATA_WriteSectors(LBA,Num,1,Buffer);
 HD_RestoreInterrupt;
End;





End.
