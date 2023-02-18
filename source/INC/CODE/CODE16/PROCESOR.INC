;#################################################################################
;# Reutinky pro detekci procesoru
;#################################################################################

;###################################################################################################
; Detekce procesoru
; Vystup: AL  = 0 - 8086
;               1 - 80286
;               2 - 80386
;               3 - 80486
Detekuj_Procesor Proc
    pushf                ; push original FLAGS
    pop     ax           ; get original FLAGS
    mov     cx, ax       ; save original FLAGS
    and     ax, 0fffh    ; clear bits 12-15 in FLAGS
    push    ax           ; save new FLAGS value on stack
    popf                 ; replace current FLAGS value
    pushf                ; get new FLAGS
    pop     ax           ; store new FLAGS in AX
    and     ax, 0f000h   ; if bits 12-15 are set, then
    cmp     ax, 0f000h   ;   processor is an 8086/8088
    mov     byte ptr ds:[Processor_Type], 0 ; turn on 8086/8088 flag
    je      end_cpu_type ; jump if processor is 8086/8088
    or      cx, 0f000h   ; try to set bits 12-15
    push    cx           ; save new FLAGS value on stack
    popf                 ; replace current FLAGS value
    pushf                ; get new FLAGS
    pop     ax           ; store new FLAGS in AX
    and     ax, 0f000h   ; if bits 12-15 are clear
    mov    byte ptr ds:[Processor_Type], 2 ; processor=80286, turn on 80286 flag
    jz      end_cpu_type ; if no bits set, processor is 80286
    pushfd               ; push original EFLAGS
    pop     eax          ; get original EFLAGS
    mov     ecx, eax     ; save original EFLAGS
    xor     eax, 40000h  ; flip AC bit in EFLAGS
    push    eax          ; save new EFLAGS value on stack
    popfd                ; replace current EFLAGS value
    pushfd               ; get new EFLAGS
    pop     eax          ; store new EFLAGS in EAX
    xor     eax, ecx     ; can't toggle AC bit, processor=80386
    mov    byte ptr ds:[Processor_Type], 3 ; turn on 80386 processor flag
    jz      end_cpu_type ; jump if 80386 processor
    push    ecx
    popfd                ; restore AC bit in EFLAGS first

    mov     byte ptr ds:[Processor_Type], 4    ; turn on 80486 processor flag
    mov     eax, ecx        ; get original EFLAGS
    xor     eax, 200000h    ; flip ID bit in EFLAGS
    push    eax             ; save new EFLAGS value on stack
    popfd                   ; replace current EFLAGS value
    pushfd                  ; get new EFLAGS
    pop     eax             ; store new EFLAGS in EAX
    xor     eax, ecx        ; can't toggle ID bit,
    je      end_cpu_type    ; processor=80486

    mov      eax, 0          ; set up for CPUID instruction
    db       0fh,0a2h   ;CPU_ID

    mov      dword ptr DS:[_vendor_id+0], ebx
    mov      dword ptr DS:[_vendor_id+4], edx
    mov      dword ptr DS:[_vendor_id+8], ecx
    mov      byte ptr DS:[_intel_CPU], 1   ; indicate an Intel processor
    cmp      eax, 1                        ; make sure 1 is valid input for CPUID
    jl       end_cpu_type                  ; if not, jump to end

    mov      eax, 1
    db       0fh,0a2h   ;CPU_ID

    shr     eax, 8          ; isolate family
    and     eax, 0fh
    mov     byte ptr ds:[Processor_Type], al   ; set _cpu_type with family
end_cpu_type :
    ret
Detekuj_Procesor Endp
;###################################################################################################
;#
;###################################################################################################
Detekuj_Rychlost Proc
   PUSH   ES
    xor    edx,edx
    mov    es,edx
    mov    word ptr es:[8*4+2],cs
    mov    dx,offset Detekce_Rychlosti_interrupt
    mov    word ptr es:[8*4],dx
    cmp    al, 4
    ja     @detekuj_pent
@detekuj_normal :
    xor     edx,edx
    xor     ebx,ebx
    mov    al,36h
    out    43h,al
    mov    ax,59659
    out    40h,al
    xchg   ah,al
    out    40h,al
   STI
    Xor    cl,cl
@Detekuj_normal_loop :
    inc    edx
    adc    ebx,0
    test   cl,0ffh
    jz     short @Detekuj_normal_loop
   CLI
    Mov    eax,edx
    xor    edx,edx
    mov    ecx,20
    mul    ecx
    Mov    Dword ptr DS:[Processor_Speed_Low],EAX
    Mov    Dword ptr DS:[Processor_Speed_Hi],EDX
    Mov    EAX,EBX
    Mov    Ecx,20*6
    Mul    ECX
    Add    Dword ptr DS:[Processor_Speed_Hi],EAX
    jmp    @Detekuj_rychlost_End
@detekuj_pent :
    mov    al,36h
    out    43h,al
    mov    ax,59659
    out    40h,al
    xchg   ah,al
    out    40h,al
   STI
    db     0fh,031h ;RDTSC
    PUSH   EAX
    PUSH   EDX
    Xor    Cl,Cl
@Rychlost_Loop :
    Test   Cl,0FFh
    Jz     @Rychlost_Loop
   CLI
    DB     0fh,031h
    POP   ECX
    SUB   EDX,ECX
    POP   ECX
    Sub   EAX,ECX
   PUSH  EDX
    mov   ECX,20
    Mul   ECX
    Mov   Dword ptr DS:[Processor_Speed_Low],EAX
    Mov   Dword ptr DS:[Processor_Speed_Hi],EDX
   POP   EAX
    Mov  ECX,20
    Mul  ECX
    Add  Dword ptr DS:[Processor_Speed_Hi],EAX
@Detekuj_rychlost_End :
    Mov   EAX,Dword ptr DS:[Processor_Speed_Low]
    Mov   EDX,Dword ptr DS:[Processor_Speed_Hi]
    Mov   ECX,1000*1000
    Div   ECX
    Mov   Dword ptr DS:[Processor_Speed],EAX
  POP   ES
   RET
Detekuj_Rychlost ENDP
;###################################################################################################
Detekce_Rychlosti_interrupt:
    mov    al,20h
    out    0a0h,al
    out    20h,al
    mov    cl,1
    iret
;###################################################################################################
; Vypise informace o typu pouzivaneho procesoru
Detekce_Procesoru Proc
       Call Detekuj_Procesor
       mov  al, byte ptr ds:[Processor_Type]
       cmp  al, 3
       jl  short Detekce_Procesoru_false
       call Detekuj_rychlost
       ret
Detekce_Procesoru_false:
       mov  eax, Er_bad_procesor
       call Fatal_error_16
Detekce_Procesoru Endp
