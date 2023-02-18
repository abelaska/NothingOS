;#################################################################################
;# Inicializace textoveho modu v rozliseni 80x30
;#################################################################################

Init_80_30_Text Proc
                mov dx,3C4h ; sequencer adress register
                mov al,0    ; reset
                out dx,al
                mov al,1    ; synchronn¡ smaz n¡ sequenceru
                inc dx
                out dx,al   ; zachovan¡ obsahu orazovky
                dec dx      ; sequencer adress register
                mov dx,3D4h
                mov al,17h  ; mode control
                out dx,al
                inc dx      ; mode control register
                in  al,dx
                and al,7Fh
                out dx,al
                dec dx      ; sequencer adress register
                mov al,11h  ; light pen low
                out dx,al
                inc dx      ; light pen low register
                in  al,dx
                and al,7Fh
                out dx,al
                mov dx,3CCh
                in  al,dx   ; al <- nastaven¡ miscellaneous output registeru
                or  al,0C0h ; zmˆna © dkov n¡ 400 (80x25) na 480 (80x30)
                mov dx,3C2h ; miscellaneous output register
                out dx,al
                mov dx,3D4h
                mov al,6    ; vertical character display
                out dx,al
                mov al,0Bh
                inc dx      ; vertical character display register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,7    ; vertical synchronisation position
                out dx,al
                mov al,3Eh
                inc dx      ; vertical synchronisation position register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,9    ; max raster address
                out dx,al
                mov al,4Fh
                inc dx      ; max raster address register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,10h  ; light pen high
                out dx,al
                mov al,0EAh
                inc dx      ; light pen high register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,11h  ; light pen low
                out dx,al
                mov al,8Ch
                inc dx      ; light pen low register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,12h  ; vertical end scan line
                out dx,al
                mov al,0DFh
                inc dx      ; vertical end scan line register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,15h  ; vertical start blanking
                out dx,al
                mov al,0E7h
                inc dx      ; vertical start blanking register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,16h  ; vertical end blanking
                out dx,al
                mov al,4
                inc dx      ; vertical end blanking register
                out dx,al
                dec dx      ; sequencer adress register
                mov al,11h  ; light pen low
                out dx,al
                inc dx      ; light pen low register
                in  al,dx
                or  al,80h
                out dx,al
                dec dx      ; sequencer adress register
                mov al,17h  ; mode control
                out dx,al
                inc dx      ; mode control register
                in  al,dx
                or  al,80h
                out dx,al
                mov dx,3C4h ; sequencer adress register
                mov al,0    ; reset sequencer
                out dx,al
                mov al,3
                inc dx      ; reset sequencer register
                out dx,al
                ret
Init_80_30_Text Endp