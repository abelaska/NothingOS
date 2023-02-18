Uses    Dos,disk,other,ata;
Var     I,U : Word;
        A : pointer;
        buf,buffer : pointer;
        Flag : Byte;

Const   Base : word = $1f0;
        Magic_Number = $EF53;
        cs_ip = $16;
        kernel_path = '/kernel/kernel';

Type tSuper_Block = Record
         s_inodes_count : longint;                                      { Inodes count }
         s_blocks_count : longint;                                      { Blocks count }
         s_r_blocks_count : longint;                                    { Reserved blocks count }
         s_free_blocks_count : longint;                                 { Free blocks count }
         s_free_inodes_count : longint;                                 { Free inodes count }
         s_first_data_block : longint;                                  { First Data Block }
         s_log_block_size : longint;                                    { Block size }
         s_log_frag_size : longint;                                     { Fragment size +/- hodnoty}
         s_blocks_per_group : longint;                                  { # Blocks per group }
         s_frags_per_group : longint;                                   { # Fragments per group }
         s_inodes_per_group : longint;                                  { # Inodes per group }
         s_mtime : longint;                                             { Mount time }
         s_wtime : longint;                                             { Write time }
         s_mnt_count : word;                                            { Mount count }
         s_max_mnt_count : word;                                        { Maximal mount count +/- hodnoty}
         s_magic : word;                                                { Magic signature }
         s_state : word;                                                { File system state }
         s_errors : word;                                               { Behaviour when detecting errors }
         s_pad : word;
         s_lastcheck : longint;                                         { time of last check }
         s_checkinterval : longint;                                     { max. time between checks }
         s_creator_os : longint;                                                { OS }
         s_rev_level : longint;                                         { Revision level }
         s_def_resuid : word;                                           { Default uid for reserved blocks }
         s_def_resgid : word;                                           { Default gid for reserved blocks }
         s_reserved : array[1..235] of longint;                                 { Padding to the end of the}
     end;

{ Group Descriptor Structure }
Type tGroup = record
         bg_block_bitmap : longint;                                     { Blocks bitmap block }
         bg_inode_bitmap : longint;                                     { Inodes bitmap block }
         bg_inode_table : longint;                                      { Inodes table block }
         bg_free_blocks_count : word;                                   { Free blocks count }
         bg_free_inodes_count : word;                                   { Free inodes count }
         bg_used_dirs_count : word;                                     { Directories count }
         bg_pad : word;
         bg_reserved : array[1..3] of longint;
       end;

{ Inode Structure }
Type tInode = Record
         i_mode : word;                                         { File mode }
         i_uid : word;                                                  { Owner Uid }
         i_size : longint;                                              { Size in bytes }
         i_atime : longint;                                             { Access time }
         i_ctime : longint;                                             { Creation time }
         i_mtime : longint;                                             { Modification time }
         i_dtime : longint;                                             { Deletion Time }
         i_gid : word;                                                  { Group Id }
         i_links_count : word;                                          { Links count }
         i_blocks : longint;                                            { Blocks count }
         i_flags : longint;                                             { File flags }
         i_reserved1 : longint;                                         { Reserved 1 }
         i_block : array[0..14] of longint;                                             { Pointers to blocks }
         i_version : longint;                                           { File version (for NFS) }
         i_file_acl : longint;                                          { File ACL }
         i_dir_acl : longint;                                           { Directory ACL }
         i_faddr : longint;                                             { Fragment address }
         i_frag : byte;                                                 { Fragment number }
         i_fsize : byte;                                                { Fragment size }
         i_pad1 : word;
         i_reserved2 : array [1..2] of longint;                                 { Reserved 2 }
    end;

{ Directory Structure }
Type tDir = Record
      inode_num : longint;
      rec_len : word;
      name_len : word;
      name : array[0..255] of char;                                             { between 0 and 256 chars }
    end;

Var SB : tSuper_Block;
    l0,l1,l2,l3 : longint;
    w0,w1,w2,w3 : word;
    b0,b1,b2,b3 : byte;
    bo0,bo1,bo2,bo3 : boolean;

    Zacatek_part,block_size : longint;

    num_groups : word;
    gt : array[0..31] of tGroup;
    temp_inode : tInode;

procedure Write_Super_Block;
begin
writeln('Super blok:');
with SB do begin
writeln('Inodes : ',         s_inodes_count );
writeln('blocks : ',         s_blocks_count     );
writeln('reserved blocks :',         s_r_blocks_count   );
writeln('free blocks : ',         s_free_blocks_count);
writeln('free inodes : ',         s_free_inodes_count);
writeln('first data block : ',         s_first_data_block );
writeln('log block size : ',         s_log_block_size   );
writeln('log frag size : ',         s_log_frag_size    );
writeln('blocks per group : ',         s_blocks_per_group );
writeln('frags per group : ',         s_frags_per_group  );
writeln('inodes per group : ',         s_inodes_per_group );
writeln('mount time : ',         s_mtime            );
writeln('write time : ',         s_wtime            );
writeln('mount count : ',         s_mnt_count        );
writeln('max mount : ',         s_max_mnt_count    );
writeln('magic : ',         s_magic            );
writeln('state : ',         s_state            );
writeln('errors : ',         s_errors           );
writeln('last check : ',         s_lastcheck        );
writeln('check interval : ',         s_checkinterval    );
writeln('create OS : ',         s_creator_os);
writeln('rev level : ',         s_rev_level );
writeln('def. reserved uid : ',         s_def_resuid);
writeln('def. reserved gid : ',         s_def_resgid);
end;
end;

procedure Write_Group(gr:word);
begin
if gr > num_groups then exit;
writeln('Group ',gr,':');
with gt[gr] do begin
  writeln('block bitmap : ', bg_block_bitmap);
  writeln('Inode bitmap : ',bg_inode_bitmap);
  writeln('Inode table : ',bg_inode_table);
  writeln('free blocks : ',bg_free_blocks_count);
  writeln('free inodes : ',bg_free_inodes_count);
  writeln('used dirs : ',bg_used_dirs_count);
end;
end;

procedure Write_Inode(ind:tInode);
begin
with ind do begin
         writeln('Mode : ',i_mode);
         writeln('UID : ',i_uid );
         writeln('SIZE : ',i_size);
         writeln('Access time : ',i_atime);
         writeln('Create time : ',i_ctime);
         writeln('Modify time : ',i_mtime);
         writeln('Delete time : ',i_dtime);
         writeln('GID : ',i_gid  );
         writeln('Links : ',i_links_count);
         writeln('Blocks : ',i_blocks );
         writeln('Flags : ',i_flags  );
         writeln('Version : ',i_version);
         writeln('File ACL : ',i_file_acl);
         writeln('Directory ACL : ',i_dir_acl );
         writeln('Fragment address : ',i_faddr   );
         writeln('Fragments : ',i_frag    );
         writeln('Fragment size : ',i_fsize   );
         writeln('Used Inodes : ');
         write  ('  ');
         for i:=0 to 11 do begin write(i_block[i]); if i<11 then write(','); end;
         writeln;
         Writeln('Indirect block : ',i_block[12]);
         Writeln('Double-Indirect block : ',i_block[13]);
         Writeln('Triple-Indirect block : ',i_block[14]);
end;
end;

procedure Write_Dir(dir:tDir);
var i : byte;
begin
with dir do begin
      writeln('Inode num : ',inode_num);
      {writeln('Record length : ',rec_len);
      writeln('name len : ',name_len );}
      write('name : "');
      for i:=0 to lo(name_len)-1 do write( name[i] );
      writeln('"');
end;
end;

Procedure _Write(kam:pointer; odkud : longint);
var x,y,z: longint;
    head : byte;
    sec : byte;
    cyl : word;
begin
 x:=Zacatek_part+(odkud div 512);

 cyl:=x div (63*32);
 head:=(x div 63) mod 32;
 sec:=(x mod 63)+1;
 writeln(cyl,' ',head,' ',sec); readkey;
 cyl:=(cyl shl 6)+sec;
 asm
  les bx, [kam]
  mov dl, 80h
  mov dh, [head]
  mov cx, [cyl]
  mov ax, 0301h
  int 13h
 end;
end;

Procedure _Read(kam:pointer; odkud : longint; kolik : longint);
var x,y: longint;
begin
 x:=(odkud mod 512)+kolik;
 y:=x div 512;
 if (x mod 512)>0 then inc(y);
 x:=odkud div 512;
 HD_Read(Zacatek_part+x,y,buffer);
 move(pointer(longint(buffer)+(odkud mod 512))^, kam^, kolik);
end;

function Load_Inode(inode_no : longint; var inode : tInode) : integer;
var loc,loc2 : longint;
begin
 if(inode_no < 1) or (inode_no > sb.s_inodes_count) then begin
  Load_Inode:=0;
 end;

 loc:= gt[ (inode_no-1) div sb.s_inodes_per_group ].bg_inode_table*block_size +
       ((inode_no-1) mod sb.s_inodes_per_group) * sizeof(tInode);

 _Read(@Inode, loc, sizeof(tInode));
end;

function Read_Inode(i:tInode;data : pointer) : longint;
var x: longint;
    loaded_size : longint;
begin
 x:=12*Block_Size;
 loaded_size:=0;
 if i.i_size>x then begin
  {bude se pouzivat minimalne indirect block}
  inc(x, (Block_Size div 4)*Block_size);
  if i.i_size>x then begin
   {bude se pouzivat double-indirect block}
   inc(x, (Block_Size div 4)*(Block_Size div 4));
   if i.i_size>x then begin
    {bude se pouzivat triple-indirect block}
    inc(x, (Block_Size div 4)*(Block_Size div 4)*(Block_Size div 4));
    if i.i_size>x then begin
    end else begin
     {size<=16843020Kb (16448.26MB)}
    end;
   end else begin
    {size<=65804Kb (64.26MB)}
   end;
  end else begin
   {size<=268Kb}
  end;
 end else begin
  {size<=12Kb}
  l0:=i.i_size;
  if l0>=Block_Size then l1:=Block_Size else l1:=l0;
  b1:=i.i_size div block_size;
  if i.i_size mod block_size>0 then inc(b1);

  for b0:=0 to b1-1 do begin
   if l0<1 then exit;
   _read( pointer(longint(data)+i.i_size-l0), i.i_block[b0]*Block_Size, l1);
   dec(l0, l1);
   if l0>Block_Size then l1:=Block_Size else l1:=l0;
  end;
  loaded_size:=i.i_size;
 end;
 Read_inode:=loaded_size;
end;

var _path : string;

function Search_Dir(i : tInode; path : string) : longint;
var dir : tDir;
    s,ss: string;
begin
 Search_Dir:=0;
 if(i.i_mode and $A000)=$A000 then {je symlink?} begin
  ss[0]:=#63; move(i.i_block[0], ss[1], 64);
  for b0:=1 to 64 do if ss[b0]=#0 then break; ss[0]:=chr(b0-1);
  _path:=ss+'/'+_path;
  Search_Dir:=-1;
  exit;
 end;

 if(i.i_mode and $4000)<>$4000 then exit; {je adresar?}

  fillchar(buffer^, 65535, 0);
  read_inode(i, buffer);

 l0:=0;
 repeat
  if(l0>=i.i_size) then break;
  move( pointer(longint(buffer)+l0)^, dir, sizeof(tDir));
  {write_dir(dir);}
  s:='';
  for b0:=0 to lo(dir.name_len)-1 do s:=s+dir.name[b0];
  if s=path then begin Search_Dir:=dir.inode_num; exit; end;
  inc(l0, dir.rec_len);
 until 0=1;
end;

Function Eat_Path(path :string) : longint;
var s,find: string;
    hledam_soubor : boolean;
    Inode_num : longint;
begin
 _path:=path;
 Eat_Path:=0;
 Inode_Num:=2;

 if path='/' then begin Eat_Path:=2; exit; end;

repeat
 if inode_num=-1 then inode_num:=2;
 Load_Inode(Inode_Num, temp_Inode);

 s:=_path;
 for b0:=1 to length(_path) do if _path[b0]='/' then break;

 _path:='';
 if b0<length(s) then
 for b0:=b0+1 to length(s) do _path:=_path+s[b0];

 find:='';
 for b0:=1 to length(_path) do if _path[b0]<>'/' then find:=find+_path[b0] else break;
 if b0=length(_path) then hledam_soubor:=true else  hledam_soubor:=false;
 if find='' then exit;

 inode_num:= Search_Dir(temp_inode, find);

 if inode_num=0 then exit;
 if hledam_soubor then begin
    if inode_num>0 then begin eat_path:=inode_num; exit; end;
 end;
until 0=1;
end;

var OldExitProc: pointer;

function hex(cislo:byte) : string;
const hexx : array[0..$f] of char = '0123456789ABCDEF';
var s : string;
begin
 hex:='0x'+hexx[cislo shr 4]+hexx[cislo and $F];
end;

const MaxKernelBuffers = 8;

var pozice_kernelu,kernel_inode : longint;
    i_kernel : tInode;

    kernel_size : longint;
    p : pointer;
    head_size,head_end : word;
    _cs, _ip : word;
    kernel_pages : word;
    kernel_jmp : longint;
    f: file;
    done : boolean;

procedure write_nolo;
const  _base_               = 11;
       _pozice_kernelu_LBA_ = 13;
       _delka_kernelu_      = 17; {HI = pocet opakovani-1 ; LO = kolik sektoru ma nacist}
       _base                : word = $1F0;
       _pozice_kernelu_LBA  : longint = 0;
       _delka_kernelu       : word = 0; {HI = pocet opakovani-1 ; LO = kolik sektoru ma nacist}

var boot_img : array[0..511] of byte;
    data_img : array[0..127] of longint;
    blocks,data_len : word;
    l0 : longint;
    p : pointer;
begin
  fillchar(boot_img, 512, 0);
  fillchar(data_img, 512, 0);
  assign(f, 'nolo.com'); reset(f, 1);
   fillchar(boot_img, 512, 0);
   blockread(f, boot_img, 512, data_len);
  close(f);

  boot_img[$0B]:=lo(_base);
  boot_img[$0C]:=hi(_base);
  blocks:=(i_kernel.i_size-1+Block_Size) div block_Size; {blocks count}
  boot_img[$0D]:=lo(blocks);
  boot_img[$0E]:=hi(blocks);
  w0:=Block_Size div 512;
  boot_img[$0F]:=lo(w0);
  boot_img[$10]:=hi(w0);

  l0:=Zacatek_part+1;
  meml[seg(boot_img):ofs(boot_img)+$11]:=l0;
  {writeln(_base,' ',_pozice_kernelu_LBA,' ',_delka_kernelu);}

 boot_img[510]:=$55;
 boot_img[511]:=$AA;

 boot_img[cs_ip]:=lo(_ip);
 boot_img[cs_ip+1]:=hi(_ip);
 boot_img[cs_ip+2]:=lo(_cs);
 boot_img[cs_ip+3]:=hi(_cs);

 if paramstr(1)='debug' then begin
  assign(f, 'img'); rewrite(f, 1); blockwrite(f, boot_img, 512); close(f);
 end;

 dec(l0);
 w2:=0;
 if blocks>12 then w1:=11 else w1:=blocks;
 dec(blocks, w1+1);
 for w0:=0 to w1 do begin data_img[w2]:=l0+(i_kernel.i_block[w0]*block_size div 512); inc(w2); end;
 if blocks>0 then begin
  if blocks>(128-w2) then blocks:=128-w2;
  _read(@data_img[w2],(i_kernel.i_block[12]*block_size),blocks*4);
  for w0:=w2 to w2+blocks-1 do data_img[w0]:=l0+(data_img[w0]*block_size div 512)
 end;
 if paramstr(1)='debug' then begin
  assign(f, 'data_img'); rewrite(f, 1); blockwrite(f, data_img, 512); close(f);
 end;

 HD_Write(Zacatek_part, 1, @boot_img);
 HD_Write(Zacatek_part+1, 1, @data_img);
end;

Begin
 Asm Mov ax,3; Int 10h End;
 done:=false;

 Get_Write_Partition(true);

 writeln('þ Byla vybrana ',write_partition,'. partition => sys code = ',hex(partitiony[write_partition].syskod));

 getmem(buffer, 65535);
 getmem(buf, 65535);
 fillchar(buf^, 65535, 0);
 fillchar(buffer^, 65535, 0);

 Zacatek_part:=partitiony[write_partition].relativ_partition_pozice;
 block_size:=1024 shl 0;

 _Read(@SB,Block_Size*1,sizeof(tSuper_Block));
 if SB.s_Magic<>Magic_Number then begin writeln('FS neni EXT2!!'); halt; end;

 block_size:=1024 shl sb.s_log_block_size;

 num_groups:= (sb.s_blocks_count - sb.s_first_data_block + sb.s_blocks_per_group - 1 ) div sb.s_blocks_per_group;

 _Read(@gt, (SB.s_first_data_block+1)*Block_Size, num_groups*sizeof(tGroup));

 kernel_inode:=Eat_Path(kernel_path);
 if(kernel_inode=0) then begin
  writeln('þ! Nebyl nalezen soubor "'+kernel_path+'" !'); halt; end;
 writeln('þ Byl nalezen soubor "'+kernel_path+'" ... (Inode=',kernel_inode,')');

 load_inode(kernel_inode, i_kernel);

 assign(f, 'kernel.exe'); {$I-} reset(f, 1); {$I+}
 if ioresult=0 then begin
  writeln('þ Byl nalezen soubor "kernel.exe" ...');
  seek(f, $14); blockread(f, _ip, 2); blockread(f, _cs, 2);
  close(f);
  Write_nolo;
  done:=true;
 end else writeln('þ! Soubor "kernel.exe" nebyl nalezen...');

 if done then
  writeln('þ Nolo bylo uspesne zapsano ...') else
  writeln('þ! Nolo nebylo zapsano ...');

End.
