;###########################################################################
;# Rutiny pro praci se soubory
;###########################################################################

; nacita radky ze souboru a vola externi proceduru pro jejich vyhodnoceni
; Vstup:  DS:ESI - cesta k souboru, ze ktereho se budou nacitat nazvy souboru
;         EBP    - offsetova hodnota procedury, ktera se bude volat s kazdym
;                  nalezenym nazvem souboru
;                - tato procedura ma vstup:
;                  = ES:EDI - ukazatel na nazev souboru, zakonceno na 0
;                - vystup: nesmi menit zadny registr
; Vystup: CF = 0 - OK
;         CF = 1 - Chyba pri praci se vstupnim souborem
Load_Filenames_From_File  Proc
       mov   eax, data_descriptor
       mov   ds, eax
       mov   esi, offset drv_conf_file
       mov   eax, 1
       Call  VFS_Open
       jc    Load_Filenames_From_File_False
       call  VFS_Is_Inode_File
       jc    Load_Filenames_From_File_False_close_conf
       xor   ebx, ebx
       mov   ecx, 0FFFFFFFFh
       call  VFS_Map_File
       jc    Load_Filenames_From_File_False_close_conf
      push   eax
       mov   ds, edx
       xor   esi, esi
       ; DS:ESI - namapovany drivers_conf
       ; ECX - velikost souboru
       ; alokujeme temp buffer
       mov   eax, 4096
       call  Alloc
      pushfd
       mov   ebx, eax
      popfd
      pop    eax
       jc    Load_Filenames_From_File_False_close_conf_dealloc
      push   eax
       mov   es, ebx
Load_Filenames_From_File_Loop:
       xor   edi, edi
       ; ES:EDI - temp buffer
       ; vymazeme temp buffer
      push   ecx
       mov   ecx, 1024
       xor   eax, eax
       cld
       rep   stosd
      pop    ecx
       xor   edi, edi
Load_Filenames_From_File_load_filename:
       lodsb
       or    al, al
       jz    short Load_Filenames_From_File_filename_found
       cmp   al , 0ah
       jz    Load_Filenames_From_File_filename_found
       cmp   edi, 4094
       jg    short Load_Filenames_From_File_Loop_end
       stosb
       dec   ecx
       jnz   Load_Filenames_From_File_load_filename
Load_Filenames_From_File_filename_found:
      push   ecx
       ; volame externi proceduru
      push   edi
       xor   edi, edi
       call  ebp
      pop    edi
      pop    ecx
       or    ecx, ecx
       jz    short Load_Filenames_From_File_Loop_end
       dec   ecx
       jnz   Load_Filenames_From_File_Loop
Load_Filenames_From_File_Loop_end:
       ; odalokujeme temp buffer
       mov   eax, es
       mov   ebx, data_descriptor
       mov   es, ebx
       call  DeAlloc
      pop    eax
       ; zavreme drivers.conf
       call  VFS_Close
Load_Filenames_From_File_True:
       clc   ;CF=0
       jmp   short Load_Filenames_From_File_End
Load_Filenames_From_File_False_close_conf_dealloc:
       call  VFS_Close
       stc   ;CF=1
       jmp   short Load_Filenames_From_File_End
Load_Filenames_From_File_False_close_conf:
       call  VFS_Close
       stc   ;CF=1
       jmp   short Load_Filenames_From_File_End
Load_Filenames_From_File_False:
       stc   ;CF=1
Load_Filenames_From_File_End:
       ret
Load_Filenames_From_File Endp