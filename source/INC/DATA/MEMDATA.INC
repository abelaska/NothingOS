;###########################################################################
;# Datove Struktury MemoryManageru
;###########################################################################

;###########################################################################
;# Zakladni prommenne
;###########################################################################
RAM_Size                 DD 0     ;##### Velikost RAM v 4k
RAM_Size_MB              DD 0     ;##### Velikost RAM v MB
Beginning_Of_Free_Space  DD 0     ;##### Konec kernelu - nepocita se Page_Struc
Pages_Base               DD 0     ;##### Bazova adresa Pages
;###########################################################################
;# Konstanty popisujici rozvrstveni virtualni pameti tasku
;###########################################################################
VMem_FysLin_Area              equ 0
VMem_FysLin_Are_Size          equ 1024*1024*1024
VMem_Shared_Area              equ VMem_FysLin_Area+VMem_FysLin_Are_Size
VMem_Shared_Area_Size         equ ((1024*1024*1024)*2)
VMem_Private_Area             equ VMem_Shared_Area+VMem_Shared_Area_Size
VMem_Private_Area_Size        equ 1024*1024*1024
;###########################################################################
Memory_Allocate_Mask_Structure struc
       Alocate_Mask_Start_Mask DB 0
       Alocate_Mask_Array_len  DD 0
       Alocate_Mask_End_Mask   DB 0
       Alocate_Mask_Reserved   DW 0
Memory_Allocate_Mask_Structure ends
;###########################################################################
Task_Mem_Alloc_Strategy Struc
       TMA_Start equ $
       TMA_Block_Start     dd 0
       TMA_Block_Size      dd 0
       TMA_Selector        dw 0
       TMA_Size Equ ($-TMA_Start)
Task_Mem_Alloc_Strategy Ends
;###########################################################################
Tsk_Mem_Layout_Descriptor Equ 4
;###########################################################################
Memory_Allocate_Mask_8less DD 9 Dup (0)
Memory_Allocate_Masks00 Memory_Allocate_Mask_Structure <11111111b,00000000H,00000000b>
Memory_Allocate_Masks01 Memory_Allocate_Mask_Structure <01111111b,00000000H,00000000b>
Memory_Allocate_Masks02 Memory_Allocate_Mask_Structure <00111111b,00000000H,00000000b>
Memory_Allocate_Masks03 Memory_Allocate_Mask_Structure <00011111b,00000000H,00000000b>
Memory_Allocate_Masks04 Memory_Allocate_Mask_Structure <00001111b,00000000H,00000000b>
Memory_Allocate_Masks05 Memory_Allocate_Mask_Structure <00000111b,00000000H,00000000b>
Memory_Allocate_Masks06 Memory_Allocate_Mask_Structure <00000011b,00000000H,00000000b>
Memory_Allocate_Masks07 Memory_Allocate_Mask_Structure <00000001b,00000000H,00000000b>