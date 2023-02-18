Data Segment
Data_Segment_Start   equ $
;#### Promenne
Command_Line_Sel  dw 0 ; selector na buffer, kde je ulozena prikazova radka
Read_Keyboard_sel dw 0 ; selectro pro ulozeni vystupu ze std. vstupu "klavesnice"
Extended_char     db 0 ; urcuje, je-li znak nasledujici po 0
LIBS_Output       db '0123456789ABCDEF'
LIBS_Output_Ver   db '0123456789ABCDEF'
LIBS_Ver          dd 0
;#### Texts
First_run_wellcome              db 'nosh v1.0, compiled ',??DATE,' at ',??TIME,0
Welcome_txt                     db '\n#',0
Error_Allocation                db 'Allocation memory error',0
Error_Exec_Error_File_Not_Found db '\ncommand not found',0
Error_Exec_Error_Memory         db '\nnot enought memory to run program',0
Error_Exec_Error_Not_Supported  db '\nfile format is not supported',0
Error_Exec_Error_File_Corrupted db '\nfile structure is corrupted',0
Error_Exec_Unknown_error        db '\nkernel returned unknown error code - you should use new version of NOSH',0
Error_Exec_Error_Lib_No_Exist   db '\nlibrary "',0
Error_Exec_Error_Lib_No_Exist2  db '" using by program not found',0
Error_Exec_Error_Lib_Ver        db '" have not enought version, program needs version ',0
Error_Exec_Error_Lib_Ver_2      db ' or higher',0
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
;####################################################################################
Data_Segment_Size    equ (($-Data_Segment_Start)Shr 12)+1
Data Ends