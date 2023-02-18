IDE_TABLE Struc
   Conntroler_Base dw ?                    ;Base Controlleru 1F0 - primary, 170 secundary
   Master_Slave    db ?                    ;Disc - 0=master, 1=slave
   LBA_CHS         db 0                    ;addresovani 1=LBA,0=CHS
   DMA_Support     db 0                    ;podpora DMA ?

   LBA_Max         dd 0                    ;maximalni mozna LBA addresa -> je-li LBA_CHS = 1

   CHS_Cyl         dw 0                    ;hodnoty default CHS, je-li LBA_CHS = 0
   CHS_Heads       dw 0
   CHS_Sectors     dw 0

   DiskSize        dw 0                    ;velikost disku v bytech
   ATA_ATAPI       db 0                    ;1 = ATA kompatibilni, 2 = ATAPI
   Drive_Name      db 'none', 36 dup (' ') ;nazev zarizeni
   Serial          db 'none', 16 dup (' ') ;seriove cislo zarizeni
   UnFormated      db 512 dup (0)          ;neformatovany vystup
IDE_TABLE Ends


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