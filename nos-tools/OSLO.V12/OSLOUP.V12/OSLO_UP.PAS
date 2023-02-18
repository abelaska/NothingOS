label ER1;

type tpart = record
      name : string[10];
      part : string[4];
      pozice : longint;
     end;

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

    pbootsektor = ^tbootsektor;
    tbootsektor = record
     code : array[0..445] of byte;
     part : array[0..3] of tpartition;
     ID : word;
    end;

const oslo_cfg = 'oslo.cfg';
      default : string = '';
      default_num : byte = 0;
      time : string = '';
      time_num : byte = 0;
      dest : string = '';
      dest_num : byte = 0;
      maxparts = 9;
      parts : byte = 0;
      menus : byte = 0;

var f : text;
    s : string;
    i : integer;
    w0,w1,w2 : word;
    b0,b1,b2 : byte;
    bo0,bo1 : boolean;
    ff : file;

    part : array[0..8] of tpart;
    boot : pbootsektor;
    newboot : array[0..511] of byte;

function ismenu(ss:string) : boolean;
var a0,a1 : byte;
    bb0 : boolean;
begin
 for a0:=1 to length(s) do if s[a0]<>' ' then break;
 a1:=a0; bb0:=true;
 for a0:=1 to length(ss) do
  if s[a0+a1-1]<>ss[a0] then begin bb0:=false; break; end;
 if s='' then bb0:=false;
 ismenu:=bb0;
end;

function hodnota : string;
var ss : string;
    a0,a1 : byte;
begin
 for a0:=1 to length(s) do if s[a0]='=' then break;
 if a0<length(s) then inc(a0);
 for a0:=a0 to length(s) do if s[a0]<>' ' then break;
 ss:='';
 for a0:=a0 to length(s) do if s[a0]<>' ' then ss:=ss+s[a0] else break;
 hodnota:=ss;
end;

var p :pointer;

begin
 asm mov ax,3; int 10h end;

 for b0:=0 to maxparts-1 do fillchar(part[b0].name[1], 10, ord(' '));

 assign(f, oslo_cfg); reset(f);
 bo1:=false;
 while not eof(f) do begin
  repeat readln(f, s); until (s<>'')or(eof(f));
  bo0:=true;
  for b0:=1 to length(s) do if s[b0]='#' then begin bo0:=false; break; end;
  if bo0 then begin
   for b0:=1 to length(s) do s[b0]:=upcase(s[b0]);
    if ismenu('DESTINATION') then dest:=hodnota;
    if ismenu('DEFAULT') then default:=hodnota;
    if ismenu('TIME') then time:=hodnota;
    if ismenu('PARTITION') then begin
       part[parts].part:=hodnota;
       inc(parts);
    end;
   if ismenu('LABEL') then part[parts-1].name:=hodnota;
  end;
 end;
 close(f);

 val(time,time_num,i);
 for b0:=0 to parts-1 do if default=part[b0].name then begin default_num:=b0; break; end;

 {precte boot sector}
asm
 mov dx, 0080h
 mov cx, 1
 mov ax, 0201h
 lea bx, newboot
 push ds
 pop es
 int 13h
end;
 p:=@newboot;
 boot:=p;

 for b0:=0 to parts-1 do begin
  s:='';
  for b1:=1 to 2 do s:=s+part[b0].part[b1];
  b2:=ord(part[b0].part[4])-ord('1');
  if (s='HD')and(part[b0].part[3]='A')and(b2 in [0..3]) then begin
    part[b0].pozice:=boot^.part[b2].relativ_partition_pozice;
  end else begin writeln('U menu ',part[b0].name,': ',part[b0].part,' partition je nedostupna.'); halt; end;
 end;

 assign(ff, 'oslo.com'); reset(ff, 1);
  w0:=filesize(ff);
  if w0>446 then begin writeln('Soubor OSLO.COM je vetsi nez 446 B !'); halt; end;
  blockread(ff, newboot, w0);
 close(ff);

{ for b0:=0 to parts-1 do
  writeln(part[b0].name,' ',part[b0].part,' ',part[b0].pozice );}

  newboot[w0-9*(10+4)-3]:=parts;
  newboot[w0-9*(10+4)-2]:=time_num;
  newboot[w0-9*(10+4)-1]:=default_num;

 for b0:=0 to parts-1 do begin
  move(part[b0].name[1], newboot[w0-9*10+b0*10], 10);
  move(part[b0].pozice, newboot[w0-9*(10+4)+b0*4], 4);
  write('þ Added ',part[b0].name:10);
  if b0=default_num then write(' *');
  writeln;
 end;

 assign(ff, 'img'); rewrite(ff, 1);
  blockwrite(ff, newboot, 512);
 close(ff);

 s:=dest[1]+dest[2];
 if s='HD' then begin
    b1:=ord(dest[3])-ord('A');
    if b1 in [0..15] then b0:=$80+b1 else goto ER1;
 end else
 if s='FD' then begin
    b1:=ord(dest[3])-ord('0');
    if b1 in [0..15] then b0:=b1 else goto ER1;
 end else begin
ER1:
  writeln('þ! Nezname zarizeni ',dest,'.');
  halt;
 end;

 write('þ Zapisuji MBR ...',#13);

{zapise boot sector}
asm
 mov  dl, [b0]
 mov  dh, 00h
 mov  cx, 1
 mov  ax, 0301h
 lea  bx, newboot
 push ds
 pop es
 int  13h
 jnc @ok
 mov [bo0], 0
 jmp @ende
@ok:
 mov [bo0], 1
@ende:
end;

 if bo0 then write('þ MBR uspesne zapsan') else
 write('þ! Chyba pri zapisu boot sektoru');
 writeln(' na ',dest,'.');
end.