;####################################################################################
;# Jednotka obsahujici pomocne procedury pro jednoduche alokovani pameti
;####################################################################################

;####################################################################################
;# Alokuje pamet
;####################################################################################
;# Vstup:  EAX - pocet pozadovanych bajtu pameti
;# Vystup: EAX - selector na pamet
;#          0  = ERROR
;####################################################################################
Kalloc Proc
       push   edx
       push   ebx
        or    eax, eax
        jz    short Kalloc_error
        dec   eax
        shr   eax, 12
        inc   eax
        mov   edx, eax
        xor   eax, eax
        call  Memory_Manager_Allocate_Memory
        or    eax ,eax
        jz    short Kalloc_2
Kalloc_error:
        mov   eax, Er_nedostatek_pameti
        Call  Fatal_Error
Kalloc_2:
        mov   eax, edx
       pop    ebx
       pop    edx
        ret
Kalloc Endp

; odalokuje pamet
; Vstup : EAX - selector na odalokovavany descriptor
; Vystup: -
kDeAlloc Proc
    push  edx
     mov  edx, eax
     call Memory_Manager_DeAllocate_Memory
    pop   edx
     ret
kDeAlloc Endp

;####################################################################################
;# Alokuje pamet
;####################################################################################
;# Vstup:  EAX - pocet pozadovanych bajtu pameti
;# Vystup: CF = 0 - OK
;                 - EAX = selector na pamet
;          CF = 1 - Error
;####################################################################################
Alloc Proc
       push   edx ebx eax
        or    eax, eax
        jz    short alloc_error
        dec   eax
        shr   eax, 12
        inc   eax
        mov   edx, eax
        xor   eax, eax
        call  Memory_Manager_Allocate_Memory
        or    eax ,eax
        jnz   short alloc_error
       pop    eax
        mov   eax, edx
        clc   ;CF=0
        jmp   short alloc_end
alloc_error:
       pop    eax
        stc   ;CF=1
alloc_end:
       pop    ebx edx
        ret
alloc Endp

; odalokuje pamet
; Vstup : EAX - selector na odalokovavany descriptor
; Vystup: -
DeAlloc Proc
    push  edx
     mov  edx, eax
     call Memory_Manager_DeAllocate_Memory
    pop   edx
     ret
DeAlloc Endp
