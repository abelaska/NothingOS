;############################################################################
;# Datove structury a promenne pro praci s CMOS
;############################################################################

; Mapa pameti CMOS
CMOS_RTC_Second equ 0
CMOS_RTC_Minute equ 2
CMOS_RTC_Hour   equ 4

CMOS_RTC_Alarm_Second equ 1
CMOS_RTC_Alarm_Minute equ 3
CMOS_RTC_Alarm_Hour   equ 5

CMOS_RTC_Day_in_Week   equ 6
CMOS_RTC_Day     equ 7
CMOS_RTC_Month   equ 8
CMOS_RTC_Year    equ 9


CMOS_RTC_Status_A   equ 0Ah
CMOS_RTC_Status_B   equ 0Bh
CMOS_RTC_Status_C   equ 0Ch
CMOS_RTC_Status_D   equ 0Dh

CMOS_POST_Status      equ 0Eh
CMOS_SHUTDOWN_Status  equ 0Fh
CMOS_Floppy_Disk_Type equ 10H

CMOS_Reserved1   equ 11h

CMOS_Hard_Disk_Type equ 12H

CMOS_Reserved2   equ 13h

; na zbytek ... nechce se mi to tu ted vsechno psat
