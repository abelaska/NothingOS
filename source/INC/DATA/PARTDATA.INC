;############################################################################
;# Datove structury a promenne pro praci s partitionama
;############################################################################

Partition Struc
          Boot_Flag             db 0
          Begin_Head            db 0
          Begin_Sector_Cylinder dw 0
          Sys_Code              db 0
          End_Head              db 0
          End_Sector_Cylinder   dw 0
          Relative_Beginning    dd 0
          Partition_Size        dd 0
Partition Ends

Nothing_Root_Code         equ 0b1h
Swap_Part_code            equ 082h
Max_partition_count       equ 64  ; na jednom disku muze byt maximalne 64
                                  ; partitionu
Max_total_partition_Count equ max_partition_count*4
;jsou 4 disky, takze dohromady zname 256 partitionu

Partition_Info_Size  equ 10h
root_partition_num    dd 0
swap_partition_num    dd 0
Sel_Partitions    dw 0

;############################################################
;# Sel_Partitions_Struc
;############################################################
;# (Max_Partition_Count - 1)*0 => (Max_Partition_Count - 1)*1 = partitiony pro HDB
;# (Max_Partition_Count - 1)*1 => (Max_Partition_Count - 1)*2 = partitiony pro HDB
;# (Max_Partition_Count - 1)*2 => (Max_Partition_Count - 1)*3 = partitiony pro HDC
;# (Max_Partition_Count - 1)*3 => (Max_Partition_Count - 1)*4 = partitiony pro HDD
;############################################################



