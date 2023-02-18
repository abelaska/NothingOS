Data Segment
Data_Segment_Start   equ $
;###########################################

parm_size dd 0

;###########################################
prog_name db 'mknod v1.0, compiled ',??DATE,' at ',??TIME,'\n',0
help_txt  db 'mknod [path] [device type] [type number]\n'
          db '       device type = c - char device\n'
          db '                   = b - block device\n'
          db '                   = f - fifo\n'
          db '                   = s - socket\n',0

new_line db '\n',0

device_t db 'Device ',0
created_t db ' was created\n',0
wasnt_created_t db ' was not created\n',0
error2 db 'Not enought memory\n',0
error3 db 'Unknown device type\n',0
error4 db 'Unknown path\n',0
error5 db 'Non supported device type.\n',0
;###########################################
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
Data_Segment_Size    equ (($-Data_Segment_Start)Shr 12)+1
Data Ends