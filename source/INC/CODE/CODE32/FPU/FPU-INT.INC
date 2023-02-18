;################################################################################
;# FPU interrupt Handlers
;################################################################################
;# v budoucnu dodelat obsluhu Device_Not_Avaible a FPU error...

FPU_Device_Not_Avaible :
      Mov   EAX,CR0
      And   Al,11110011b
      Mov   CR0,EAX
     Iret



