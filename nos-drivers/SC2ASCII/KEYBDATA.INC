;##########################################################################
;# Soubor s prevodnimi tabulkami Scan codu na ascii
;##########################################################################
;# Version : 1.2-2-1
;##########################################################################


;##########################################################################
;# Definovani tabulek prevodu ScanCode -> ASCII
;##########################################################################
;# Tabulka1 prevodu ScanCode -> ASCII
;# Obsahuje mala pismena, prvni znaky, numericka klavesnica
;##########################################################################
Keyboard_ScanCode2ascii_Table1 :
;--ASCII---|----ScanCode
  dw 001bh      ;01 - ESC
  dw 0031h      ;02 - 1
  dw 0032h      ;03 - 2
  dw 0033h      ;04 - 3
  dw 0034h      ;05 - 4
  dw 0035h      ;06 - 5
  dw 0036h      ;07 - 6
  dw 0037h      ;08 - 7
  dw 0038h      ;09 - 8
  dw 0039h      ;0a - 9
  dw 0030h      ;0b - 0
  dw 002dh      ;0c - -
  dw 003dh      ;0d - =
  dw 0008h      ;0e - BackSpace
  dw 0009h      ;0f - Tabulator
  dw 0071h      ;10 - q
  dw 0077h      ;11 - w
  dw 0065h      ;12 - e
  dw 0072h      ;13 - r
  dw 0074h      ;14 - t
  dw 0079h      ;15 - y
  dw 0075h      ;16 - u
  dw 0069h      ;17 - i
  dw 006fh      ;18 - o
  dw 0070h      ;19 - p
  dw 005bh      ;1a - [
  dw 005dh      ;1b - ]
  dw 000ah      ;1c - ENTER Velky
  dw 0102h      ;1d - Left Control
  dw 0061h      ;1e - a
  dw 0073h      ;1f - s
  dw 0064h      ;20 - d
  dw 0066h      ;21 - f
  dw 0067h      ;22 - g
  dw 0068h      ;23 - h
  dw 006ah      ;24 - j
  dw 006bh      ;25 - k
  dw 006ch      ;26 - l
  dw 003bh      ;27 - ;
  dw 0027h      ;28 - '
  dw 0060h      ;29 - `
  dw 0103h      ;2a - Left Shift
  dw 005ch      ;2b - \
  dw 007ah      ;2c - z
  dw 0078h      ;2d - x
  dw 0063h      ;2e - c
  dw 0076h      ;2f - v
  dw 0062h      ;30 - b
  dw 006eh      ;31 - n
  dw 006dh      ;32 - m
  dw 002ch      ;33 - ,
  dw 002eh      ;34 - .
  dw 002fh      ;35 - /
  dw 0106h      ;36 - right shift
  dw 002ah      ;37 - seda hvezdicka
  dw 0101h      ;38 - left ALT
  dw 0020h      ;39 - Space
  dw 0109h      ;3a - CapsLock
  dw 0111h      ;3b - F1
  dw 0112h      ;3c - F2
  dw 0113h      ;3d - F3
  dw 0114h      ;3e - F4
  dw 0115h      ;3f - F5
  dw 0116h      ;40 - F6
  dw 0117h      ;41 - F7
  dw 0118h      ;42 - F8
  dw 0119h      ;43 - F9
  dw 011Ah      ;44 - F10
  dw 0108h      ;45 - NumLock
  dw 010ah      ;46 - ScroolLock
  dw 0037h      ;47 - 7
  dw 0038h      ;48 - 8
  dw 0039h      ;49 - 9
  dw 002dh      ;4a - -
  dw 0034h      ;4b - 4
  dw 0035h      ;4c - 5
  dw 0036h      ;4d - 6
  dw 002bh      ;4e - +
  dw 0031h      ;4f - 1
  dw 0032h      ;50 - 2
  dw 0033h      ;51 - 3
  dw 0030h      ;52 - 0
  dw 002eh      ;53 - .
  dw 0131h      ;54 - ALT + PrintScreen
  dw 0000h      ;55 - ???
  dw 0000h      ;56 - ???
  dw 011bh      ;57 - F11
  dw 011ch      ;58 - F12
;##########################################################################
;# Tabulka2 prevodu ScanCode -> ASCII
;# Obsahuje velka pismena, prvni znaky, numericka klavesnica
;##########################################################################
Keyboard_ScanCode2ascii_Table2 :
;--ASCII---|----ScanCode
  dw 001bh      ;01 - ESC
  dw 0031h      ;02 - 1
  dw 0032h      ;03 - 2
  dw 0033h      ;04 - 3
  dw 0034h      ;05 - 4
  dw 0035h      ;06 - 5
  dw 0036h      ;07 - 6
  dw 0037h      ;08 - 7
  dw 0038h      ;09 - 8
  dw 0039h      ;0a - 9
  dw 0030h      ;0b - 0
  dw 002dh      ;0c - -
  dw 003dh      ;0d - =
  dw 0008h      ;0e - BackSpace
  dw 0009h      ;0f - Tabulator
  dw 0051h      ;10 - Q
  dw 0057h      ;11 - W
  dw 0045h      ;12 - E
  dw 0052h      ;13 - R
  dw 0054h      ;14 - T
  dw 0059h      ;15 - Y
  dw 0055h      ;16 - U
  dw 0049h      ;17 - I
  dw 004fh      ;18 - O
  dw 0050h      ;19 - P
  dw 005bh      ;1a - [
  dw 005dh      ;1b - ]
  dw 000ah      ;1c - ENTER Velky
  dw 0102h      ;1d - Left Control
  dw 0041h      ;1e - A
  dw 0053h      ;1f - S
  dw 0044h      ;20 - D
  dw 0046h      ;21 - F
  dw 0047h      ;22 - G
  dw 0048h      ;23 - H
  dw 004ah      ;24 - J
  dw 004bh      ;25 - K
  dw 004ch      ;26 - L
  dw 003bh      ;27 - ;
  dw 0027h      ;28 - '
  dw 0060h      ;29 - `
  dw 0103h      ;2a - Left Shift
  dw 005ch      ;2b - \
  dw 005ah      ;2c - Z
  dw 0058h      ;2d - X
  dw 0043h      ;2e - C
  dw 0056h      ;2f - V
  dw 0042h      ;30 - B
  dw 004eh      ;31 - N
  dw 004dh      ;32 - M
  dw 002ch      ;33 - ,
  dw 002eh      ;34 - .
  dw 002fh      ;35 - /
  dw 0106h      ;36 - right shift
  dw 002ah      ;37 - seda hvezdicka
  dw 0101h      ;38 - left ALT
  dw 0020h      ;39 - Space
  dw 0109h      ;3a - CapsLock
  dw 0111h      ;3b - F1
  dw 0112h      ;3c - F2
  dw 0113h      ;3d - F3
  dw 0114h      ;3e - F4
  dw 0115h      ;3f - F5
  dw 0116h      ;40 - F6
  dw 0117h      ;41 - F7
  dw 0118h      ;42 - F8
  dw 0119h      ;43 - F9
  dw 011Ah      ;44 - F10
  dw 0108h      ;45 - NumLock
  dw 010ah      ;46 - ScroolLock
  dw 0037h      ;47 - 7
  dw 0038h      ;48 - 8
  dw 0039h      ;49 - 9
  dw 002dh      ;4a - -
  dw 0034h      ;4b - 4
  dw 0035h      ;4c - 5
  dw 0036h      ;4d - 6
  dw 002bh      ;4e - +
  dw 0031h      ;4f - 1
  dw 0032h      ;50 - 2
  dw 0033h      ;51 - 3
  dw 0030h      ;52 - 0
  dw 002eh      ;53 - .
  dw 0131h      ;54 - ALT + PrintScreen
  dw 0000h      ;55 - ???
  dw 0000h      ;56 - ???
  dw 011bh      ;57 - F11
  dw 011ch      ;58 - F12
;##########################################################################
;# Tabulka3 prevodu ScanCode -> ASCII
;# Obsahuje velka pismena, druhe znaky, numericka klavesnica
;##########################################################################
Keyboard_ScanCode2ascii_Table3 :
;--ASCII---|----ScanCode
  dw 001bh      ;01 - ESC
  dw 0021h      ;02 - !
  dw 0040h      ;03 - @
  dw 0023h      ;04 - #
  dw 0024h      ;05 - $
  dw 0025h      ;06 - %
  dw 005eh      ;07 - ^
  dw 0026h      ;08 - &
  dw 002ah      ;09 - *
  dw 0028h      ;0a - (
  dw 0029h      ;0b - )
  dw 005fh      ;0c - _
  dw 002bh      ;0d - +
  dw 0008h      ;0e - BackSpace
  dw 0009h      ;0f - Tabulator
  dw 0051h      ;10 - Q
  dw 0057h      ;11 - W
  dw 0045h      ;12 - E
  dw 0052h      ;13 - R
  dw 0054h      ;14 - T
  dw 0059h      ;15 - Y
  dw 0055h      ;16 - U
  dw 0049h      ;17 - I
  dw 004fh      ;18 - O
  dw 0050h      ;19 - P
  dw 007bh      ;1a - {
  dw 007dh      ;1b - }
  dw 000ah      ;1c - ENTER Velky
  dw 0102h      ;1d - Left Control
  dw 0041h      ;1e - A
  dw 0053h      ;1f - S
  dw 0044h      ;20 - D
  dw 0046h      ;21 - F
  dw 0047h      ;22 - G
  dw 0048h      ;23 - H
  dw 004ah      ;24 - J
  dw 004bh      ;25 - K
  dw 004ch      ;26 - L
  dw 003ah      ;27 - :
  dw 0022h      ;28 - "
  dw 007eh      ;29 - ~
  dw 0103h      ;2a - Left Shift
  dw 007ch      ;2b - |
  dw 005ah      ;2c - Z
  dw 0058h      ;2d - X
  dw 0043h      ;2e - C
  dw 0056h      ;2f - V
  dw 0042h      ;30 - B
  dw 004eh      ;31 - N
  dw 004dh      ;32 - M
  dw 003ch      ;33 - <
  dw 003eh      ;34 - >
  dw 003fh      ;35 - ?
  dw 0106h      ;36 - right shift
  dw 002ah      ;37 - seda hvezdicka
  dw 0101h      ;38 - left ALT
  dw 0020h      ;39 - Space
  dw 0109h      ;3a - CapsLock
  dw 0111h      ;3b - F1
  dw 0112h      ;3c - F2
  dw 0113h      ;3d - F3
  dw 0114h      ;3e - F4
  dw 0115h      ;3f - F5
  dw 0116h      ;40 - F6
  dw 0117h      ;41 - F7
  dw 0118h      ;42 - F8
  dw 0119h      ;43 - F9
  dw 011Ah      ;44 - F10
  dw 0108h      ;45 - NumLock
  dw 010ah      ;46 - ScroolLock
  dw 0037h      ;47 - 7
  dw 0038h      ;48 - 8
  dw 0039h      ;49 - 9
  dw 002dh      ;4a - -
  dw 0034h      ;4b - 4
  dw 0035h      ;4c - 5
  dw 0036h      ;4d - 6
  dw 002bh      ;4e - +
  dw 0031h      ;4f - 1
  dw 0032h      ;50 - 2
  dw 0033h      ;51 - 3
  dw 0030h      ;52 - 0
  dw 002eh      ;53 - .
  dw 0131h      ;54 - ALT + PrintScreen
  dw 0000h      ;55 - ???
  dw 0000h      ;56 - ???
  dw 011bh      ;57 - F11
  dw 011ch      ;58 - F12
;##########################################################################
;# Tabulka4 prevodu ScanCode -> ASCII
;# Obsahuje mala pismena, druhe znaky, numericka klavesnica
;##########################################################################
Keyboard_ScanCode2ascii_Table4 :
;--ASCII---|----ScanCode
  dw 001bh      ;01 - ESC
  dw 0021h      ;02 - !
  dw 0040h      ;03 - @
  dw 0023h      ;04 - #
  dw 0024h      ;05 - $
  dw 0025h      ;06 - %
  dw 005eh      ;07 - ^
  dw 0026h      ;08 - &
  dw 002ah      ;09 - *
  dw 0028h      ;0a - (
  dw 0029h      ;0b - )
  dw 005fh      ;0c - _
  dw 002bh      ;0d - +
  dw 0008h      ;0e - BackSpace
  dw 0009h      ;0f - Tabulator
  dw 0071h      ;10 - q
  dw 0077h      ;11 - w
  dw 0065h      ;12 - e
  dw 0072h      ;13 - r
  dw 0074h      ;14 - t
  dw 0079h      ;15 - y
  dw 0075h      ;16 - u
  dw 0069h      ;17 - i
  dw 006fh      ;18 - o
  dw 0070h      ;19 - p
  dw 007bh      ;1a - {
  dw 007dh      ;1b - }
  dw 000ah      ;1c - ENTER Velky
  dw 0102h      ;1d - Left Control
  dw 0061h      ;1e - a
  dw 0073h      ;1f - s
  dw 0064h      ;20 - d
  dw 0066h      ;21 - f
  dw 0067h      ;22 - g
  dw 0068h      ;23 - h
  dw 006ah      ;24 - j
  dw 006bh      ;25 - k
  dw 006ch      ;26 - l
  dw 003ah      ;27 - :
  dw 0022h      ;28 - "
  dw 007eh      ;29 - ~
  dw 0103h      ;2a - Left Shift
  dw 007ch      ;2b - |
  dw 007ah      ;2c - z
  dw 0078h      ;2d - x
  dw 0063h      ;2e - c
  dw 0076h      ;2f - v
  dw 0062h      ;30 - b
  dw 006eh      ;31 - n
  dw 006dh      ;32 - m
  dw 003ch      ;33 - <
  dw 003eh      ;34 - >
  dw 003fh      ;35 - ?
  dw 0106h      ;36 - right shift
  dw 002ah      ;37 - seda hvezdicka
  dw 0101h      ;38 - left ALT
  dw 0020h      ;39 - Space
  dw 0109h      ;3a - CapsLock
  dw 0111h      ;3b - F1
  dw 0112h      ;3c - F2
  dw 0113h      ;3d - F3
  dw 0114h      ;3e - F4
  dw 0115h      ;3f - F5
  dw 0116h      ;40 - F6
  dw 0117h      ;41 - F7
  dw 0118h      ;42 - F8
  dw 0119h      ;43 - F9
  dw 011Ah      ;44 - F10
  dw 0108h      ;45 - NumLock
  dw 010ah      ;46 - ScroolLock
  dw 0037h      ;47 - 7
  dw 0038h      ;48 - 8
  dw 0039h      ;49 - 9
  dw 002dh      ;4a - -
  dw 0034h      ;4b - 4
  dw 0035h      ;4c - 5
  dw 0036h      ;4d - 6
  dw 002bh      ;4e - +
  dw 0031h      ;4f - 1
  dw 0032h      ;50 - 2
  dw 0033h      ;51 - 3
  dw 0030h      ;52 - 0
  dw 002eh      ;53 - .
  dw 0131h      ;54 - ALT + PrintScreen
  dw 0000h      ;55 - ???
  dw 0000h      ;56 - ???
  dw 011bh      ;57 - F11
  dw 011ch      ;58 - F12
;##########################################################################
;# Tabulka4 prevodu ScanCode -> ASCII
;# Tabulka obsahuje provody scancodu 047h..053h na ASCII, neni-li
;# zmacknut NumLock
;##########################################################################
Keyboard_ScanCode2ascii_NonNumLock :
  dw 0120h      ;47 - Home
  dw 010bh      ;48 - sipka nahoru
  dw 0122h      ;49 - PageUp
  dw 002dh      ;4a - -
  dw 010dh      ;4b - sipka vlevo
  dw 010fh      ;4c - petka... v nenumlock stavu
  dw 010eh      ;4d - sipka vpravo
  dw 002bh      ;4e - +
  dw 0121h      ;4f - END
  dw 010ch      ;50 - sipka dolu
  dw 0123h      ;51 - PgDown
  dw 011eh      ;52 - INSERT
  dw 011fh      ;53 - DEL
;##########################################################################
;# Tabulka dvouznakovych ScanCode -> ASCII
;# Tabulka obsahuje provody 2-znakovych scancodu na ASCII
;##########################################################################
Keyboard_2Chars2ascii :
  dd 00000012h   ; pocet zaznamu v tabulce = 17
  dd 010438e0h   ; e0h 38h --> 0104h Right Alt
  dd 01051de0h   ; Right Ctrl
  dd 013237e0h   ; Shift+PrintScreen
  dd 012047e0h   ; Home
  dd 01214fe0h   ; END
  dd 010b48e0h   ; Sipka hore
  dd 000a1ce0h   ; ENTER sedy
  dd 010c50e0h   ; Sipka dolu
  dd 012249e0h   ; PageUp
  dd 011e52e0h   ; Insert
  dd 011f53e0h   ; Delete
  dd 010d4be0h   ; Sipka vlevo
  dd 012351e0h   ; PageDown
  dd 010e4de0h   ; sipka vpravo
  dd 002f35e0h   ; sede /
  dd 01402ae0h   ; Fake Left Shift - kurevska hnusna zpetna kompatibilita :(
  dd 014036e0h   ; Fake Right Shift -  --- || ----
  dd 0140aae0h   ; Fake Shift

Keyboard_6Chars2ascii :
  dd 0e1451de1h ; pause

 Keyboard_Internal_Buffer DB 16 Dup (0)
 TEMP_BUFFER_FLAGS        DW 0