;############################################################################
;# Rutina pro nacteni driveru
;############################################################################

; Nacteme driver
; Vstup:  ES:EDI - cesta k driveru
; Vystup: -
DRV_Load_Driver proc
      pushad
      push   ds es fs gs
      push   es
      pop    ds
       mov   esi, edi
       ; pokud neni na zacatku cesty k souboru "/", tak se na to vyser
       cmp   byte ptr ds:[esi], '/'
       jnz   DRV_Load_Driver_End

       mov   eax, 1
       Call  VFS_Open
       jc    DRV_Load_Driver_End
       call  VFS_Is_Inode_File
       jc    DRV_Load_Driver_close
       xor   ebx, ebx
       mov   ecx, 0FFFFFFFFh
       call  VFS_Map_File
       jc    DRV_Load_Driver_close
      push   eax
       mov   ds, edx
       ; DS:0 - namapovany soubor driveru
       ; ECX - delka souboru

       ; precteme signaturu
       mov   eax, dword ptr ds:[drv_id]
       cmp   eax, DRV_Signature
       jne   DRV_Load_Driver_load_end
       ; najdeme volnou polozku pro nacitany driver
       call  DRV_find_free
       jc    DRV_Load_Driver_load_end
       ; ES:EDI - ukazatel na volnou polozku device
      push   es
      pop    fs
       mov   ebx, edi
       ; zjistime, jestli uz neni nacteny driver se stejnym typovym cislem
       movzx eax, word ptr ds:[drv_Device_Type]
       Call  DRV_Find
       jc    short DRV_Load_Driver_load_2
       jmp   DRV_Load_Driver_load_end
DRV_Load_Driver_load_2:
       ; FS:EBX - ukazatel na volnou polozku device
       ; vynulujeme semafor
       mov   word ptr fs:[ebx+Device_Semafor], 0
       ; zjistime pocet fci v driveru
       movzx eax, word ptr ds:[drv_Func_Count]
       mov   word ptr fs:[ebx+Device_Func_Count], ax
       ; typ driveru, tohle je jen pro pripad erroru
       mov   word ptr fs:[ebx+Device_Type], 0
       ; nacteme data segment driveru
       movzx eax, word ptr ds:[drv_Data_Size]
       call  Alloc
       jc    DRV_Load_Driver_load_end
       mov   es, eax
       xor   edi, edi
       ; ES:EDI - buffer pro ulozeni data segmentu driveru
       ; ulozime selector na data segment
       mov   word ptr fs:[ebx+Device_Sel_Data], es
       ; prekopirujeme data segment
       movzx ecx, word ptr ds:[drv_Data_Size]
       mov   esi, dword ptr ds:[drv_Start_Data]
       cld
       rep   movsb
       mov   edx, es
       ; EDX - selector na datasegment driveru
       movzx eax, word ptr ds:[drv_Code_Size]
       call  Alloc
       jnc   short DRV_Load_Driver_load_1
DRV_Load_Driver_load_10:
       ; odalokujeme data segment
       mov   ecx, data_descriptor
       mov   eax, es
       mov   es, ecx
       call  DeAlloc
       jmp   DRV_Load_Driver_load_end
DRV_Load_Driver_load_1:
       mov   es, eax
       xor   edi, edi
       ; ES:EDI - buffer pro ulozeni data segmentu driveru
       ; ulozime selector na code segment
       mov   word ptr fs:[ebx+Device_Sel_Code], es
       ; prekopirujeme code segment
       movzx ecx, word ptr ds:[drv_Code_Size]
       mov   esi, dword ptr ds:[drv_Start_Code]
       cld
       rep   movsb
       ; ulozime do code segmentu selector na data segment driveru
       mov   word ptr es:[0], dx
       ; prevedeme code segment z datoveho descriptoru na codovy
       mov   edx, es
       call  Memory_Manager_Data2Code_Descr
       ; typ driveru
       movzx eax, word ptr ds:[drv_Device_Type]
       mov   word ptr fs:[ebx+Device_Type], ax
       ; zvysime pocet nactenych driveru v systemu
       mov   eax, data_descriptor
       mov   es, eax
       inc   dword ptr es:[drivers_count]
DRV_Load_Driver_load_end:
      pop    eax
DRV_Load_Driver_close:
       call  VFS_Close
DRV_Load_Driver_End:
      pop    gs fs es ds
      popad
       ret
DRV_Load_Driver endp
