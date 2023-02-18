;####################################################################################
;# Hlavni Include soubor pro 32bitovy kod kernelu
;####################################################################################
      Include   .\..\inc\code\code32\macros.inc
      Include   .\..\inc\code\code32\pm_init.inc
      Include   .\..\inc\code\code32\IDE\ide.inc
      Include   .\..\inc\code\code32\EXEC\EXEC.inc
      Include   .\..\inc\code\code32\EXEC\execdrv.inc
      Include   .\..\inc\code\code32\INT\int.inc
      Include   .\..\inc\code\code32\pic.inc
      Include   .\..\inc\code\code32\MEM\mem.inc
      Include   .\..\inc\code\code32\Paging\paging.inc
      Include   .\..\inc\code\code32\waits.inc
      Include   .\..\inc\code\code32\LIB\lib.inc
      Include   .\..\inc\code\code32\DRV\DRV.inc
      Include   .\..\inc\code\code32\FS\ext2\ext2.inc
      Include   .\..\inc\code\code32\FS\ext2\ext2drv.inc
      Include   .\..\inc\code\code32\FS\vfs.inc
      Include   .\..\inc\code\code32\FS\fs.inc
      Include   .\..\inc\code\code32\FS\device.inc
      Include   .\..\inc\code\code32\TSS\tss.inc
      Include   .\..\inc\code\code32\panic.inc
      Include   .\..\inc\code\code32\tools.inc
      Include   .\..\inc\code\code32\pal.inc
      Include   .\..\inc\code\code32\FPU\Fpu.inc
      Include   .\..\inc\code\code32\swapd\swap.inc
      Include   .\..\inc\code\code32\init\init.inc
      Include   .\..\inc\code\code32\cmos.inc
      Include   .\..\inc\code\code32\convert.inc
      Include   .\..\inc\code\code32\filetool.inc
      Include   .\..\inc\code\code32\Keyb\keyb.inc
      Include   .\..\inc\code\code32\CONSOLE\console.inc
;#### Start Smazat
      Include   .\..\inc\code\code32\t00ls-32.inc
;#### End Smazat

;####################################################################################
;# Adds
;####################################################################################
;# T2 = Waits.inc
;# T3 = pm_init.inc
;# T4 = lib.inc