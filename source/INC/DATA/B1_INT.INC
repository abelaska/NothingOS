;############################################################################
;# Datove structury a promenne pro praci se systemovym prerusenim
;############################################################################

INT_B1_FUNCTIONS:
    DD 000000001H
    DD Offset Return_Lib_CallGate
    DD 000000002H
    DD Offset Return_System_CallGate
    DD 000000003H
    DD Offset Wr_1
    DD 000000004H
    DD Offset Wr_2
    DD 000000005H
    DD Offset Wr_3
    DD 0FFFFFFFFH

task_1_priority dd 40 dup (0)
family_info db '\nID       Parent   Child    Older    Younger  Priority Status  Variable ',0
blablablabla db 'taskman         '
asdfasfasdfasd db 0
fasdfjas dd 0
sadkljfkldasjf db 0
tex   db 'check : ',0
tex2  db 'exec  : ',0