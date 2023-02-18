Data Segment
Data_Segment_Start   equ $
Include ./../../include/vfs.inc
Include ./../../include/taskman.inc
Include ./../../include/memory.inc
Include ./../../include/ext2.inc

test_path1 db '/',0
test_path2 db '/lib',0
test_path3 db '/etc',0
test_path4 db '/kernel',0
test_path5 db '/lib/.',0

ls_intro  db 'ls [path]\n',0
no_memory db 'Not enought memory\n',0
bad_path  db 'Bad path\n',0

PREVOD_Output       db '0123456789ABCDEF'
;#### Texts
New_Line_txt        db '\n',0
space_txt           db ' ',0

param_size          dd 0
command_line_sel    dw 0

;#### Call Gates na knihovny
Mem_Call_Gate dd 0
              dw 0
TSK_Call_Gate dd 0
              dw 0
VFS_Call_Gate dd 0
              dw 0
;#### VFS inody standarnich vstupu - vystupu
VFS_Standard_In     dd 0
VFS_Standard_Out    dd 0
VFS_Standard_Error  dd 0
;####################################################################################
DEC_Numbers      dd 0000000001
                 dd 0000000010
                 dd 0000000100
                 dd 0000001000
                 dd 0000010000
                 dd 0000100000
                 dd 0001000000
                 dd 0010000000
                 dd 0100000000
                 dd 1000000000
Numbers_Tab      db '0123456789ABCDEF'
Data_Segment_Size    equ (($-Data_Segment_Start)Shr 12)+1
Data Ends