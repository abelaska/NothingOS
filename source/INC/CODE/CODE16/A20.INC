;###########################################################################################
;# Procedury pro zapnuti A20 brany
;###########################################################################################

;###########################################################################################
;# Prvni zpusob - pres porty
;#      Vstup  : Neni
;#      Vystup : Al = 0  - OK
;#               Al = FF - tak to je spatne.... :( musime pouzit jinou proceduru na enablovani gatu... :(((
;#      Nici   : AX,CX
;###########################################################################################
A20_Enable_Gate_0001 PROC
                Mov     CX,10
@A20_Clear_Input_Buffer :
                In      Al,64h
                In      Al,60h
                Dec     CX
                Jnz     @A20_Clear_Input_Buffer
                Call    @Sync8042
                mov     AL, 0D1h
                out     64h, AL
                call    @Sync8042
                jnz     @A20_Error

                mov     AL, 0DFh
                out     60h, AL
                call    @Sync8042
                jnz     @A20_error

                mov     AL, 0FFh
                out     64h, AL
                call    @Sync8042
                jnz     @A20_error
                Xor     Al,Al
                Ret
@A20_Error :    Mov     Al,0FFh
                Ret

@Sync8042 :
                Mov     ECX,0FFFFh
@S8InSync:      in      AL, 64h
                And     AL, 2
                Dec     ECX
                Jnz     @S8InSync
                ret
A20_Enable_Gate_0001 ENDP
