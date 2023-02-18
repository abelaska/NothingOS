Unit disk;

interface
Const OSLO_SYS_CODE = $B1;

type tpartition = record
      boot_flag : byte;
      begin_hlava : byte;
      begin_sector_cylinder : word;
      syskod : byte;
      end_hlava : byte;
      end_sector_cylinder : word;
      relativ_partition_pozice : longint;
      delka : longint;
     end;

    tbootsektor = record
     code : array[0..445] of byte;
     partition : array[0..3] of tpartition;
     ID : word;
    end;

var buf : array[0..511] of byte;
    partitiony : array[0..31] of tpartition;
    main_boot : tbootsektor;
    Label_poz, FS_Poz : word;
    Write_Partition, boots : byte;
    partitionu : byte;

Procedure Inc_CHS(var b:byte;var w:word; kolik:longint);
function ReadSectors_CHS(bb:byte;ww:word) : boolean;
function writeSektors_CHS(bb:byte;ww:word; p:pointer; sektoru:byte) : boolean;
Procedure Find(bb:byte; ww:word; vypisovat : boolean);
Procedure Get_Write_Partition(automat:boolean);

implementation
uses other;

Procedure Inc_CHS(var b:byte;var w:word; kolik:longint);
var i : longint;
begin
if kolik<1 then exit;
for i:=1 to kolik do
if (w and $3f)>62 then begin
 w:=w and not($3f);
 inc(w);
 if b>254 then begin
  b:=0;
  inc(w, $40); {cylinder up}
 end else inc(b);
end else inc(w);
end;

function writeSektors_CHS(bb:byte;ww:word; p:pointer; sektoru:byte) : boolean; assembler;
asm
  les bx, [p]
  mov dl, 80h
  mov dh, [bb]
  mov cx, [ww]
  mov ah, 03h
  mov al, [sektoru]
  int 13h
  xchg al, ah
  neg al
end;

function ReadSectors_CHS(bb:byte;ww:word) : boolean; assembler;
asm
   push ds
   pop es
   lea bx, [buf]
   mov dl, 80h
   mov dh, [bb]
   mov cx, [ww]
   mov ax, 0201h
   int 13h
   xchg al, ah
   neg al
end;

Procedure Find(bb:byte; ww:word; vypisovat : boolean);
var xx,w,ii : word;
    b : byte;
    boot: tbootsektor;

procedure wrt(st:string);
var iii:byte;
begin
  if vypisovat then if boots>1 then for iii:=1 to boots-1 do write(' ');
  if(ii=0)and(boots=1) then begin
   if vypisovat then writeln(partitionu:3,'. | '+st);
   partitiony[partitionu]:=boot.partition[xx];
   inc(partitionu);
  end else if vypisovat then writeln('   X | ',st);
end;

Procedure WriteInfo;
var st : string;
    c : byte;
begin
   st:='';
   for c:=3 to 3+7 do st:=st+chr(buf[c]);
   st:=st+'      | '; for c:=label_poz to label_poz+10 do
   if (chr(buf[c]) in ['a'..'z','A'..'Z','0'..'9']) then st:=st+chr(buf[c]) else st:=st+' ';
   st:=st+'      | '; for c:=fs_poz to fs_poz+7 do st:=st+chr(buf[c]);
   wrt(st);
end;

begin
  inc(boots);
  readsectors_CHS(bb,ww);
  move(buf, boot, 512);

 for xx:=0 to 3 do begin
   {writeln(boot.partition[xx].syskod);}
   label_poz:=$2b; fs_poz:=$36;

  w:=boot.partition[xx].begin_sector_cylinder;
  b:=boot.partition[xx].begin_hlava;
  readsectors_CHS(b,w);

  ii:=0;
  case boot.partition[xx].syskod of
   1  : WriteInfo; {FAT12}
   2,3: wrt('Xenix         | ---              | ---'); {Extended partition}
   4  : WriteInfo; {FAT16 <32MB}
   5  : wrt('Ext  5        | ---              | ---'); {Extended partition}
   6  : WriteInfo; {FAT16 >32MB}
   11 : begin label_poz:=$47; fs_poz:=$52; WriteInfo; end; {FAT32}
   12 : begin label_poz:=$47; fs_poz:=$52; WriteInfo; end; {FAT32}
   15 : begin
         wrt('Ext 15        | ---              | ---'); {Extended partition}
         ii:=11;
         find(b,w,vypisovat);
         dec(boots);
        end;
   30,22 : wrt('Unformated    | ---              | ---'); {Unformated}
   $B1: WriteInfo;  {Nothing - Not-FS}
   $82: wrt('Linux Swap    | ---              | ---'); {Linux SWAP}
   $83: wrt('Linux Native  | ---              | Ext2'); {Linux Native - Ext2}
  else
   ii:=11;
   wrt('???           | ???              | ???'); {Extended partition}
  end;
 end;
end;

Procedure Get_Write_Partition(automat:boolean);
var b0 : byte;
    bo0 : boolean;
    ch : char;
begin
 if not automat then begin
  writeln('  n. |     OEM       |      Label       | File system');
  writeln('------------------------------------------------------');
 end;

 boots:=0;
 partitionu:=0;
 find(0,1,not(automat));

 {zjisti, jestli jiz neni nejaka partitiona pouzivana oslem}
 bo0:=false;
 for b0:=0 to partitionu-1 do
  if partitiony[b0].syskod=OSLO_SYS_CODE then begin bo0:=true; break; end;
  if not bo0 then begin
     writeln(#13#10'Nebyla nalezena Nothing Partition !');
     automat:=false;
     bo0:=false;
  end;

 if not automat then writeln;
 ch:=#0;
 if(bo0) then begin Write_Partition:=b0; ch:=chr(ord('0')+b0);
  if automat then exit; end;
repeat
  if (ch in ['n','N']) then ch:=#0;
  write('Vyber partition(0..',partitionu-1,'): ');
  if ch<>#0 then write(ch);
  while not(ch in ['0'..chr(ord('0')+partitionu-1),#27]) do ch:=readkey;
  writeln;
  if ch=#27 then begin writeln('Exiting ...'); halt; end;
   write_partition:=ord(ch)-ord('0');

   write('Pouzit partition cislo ',write_partition,' ? (A/n/Esc)');
   repeat ch:=readkey; until ch in ['A','n','N',#27]; writeln;
until not(ch in ['n','N']);
   if ch<>'A' then begin writeln('Exiting ...'); halt; end;
   writeln;

end;

end.