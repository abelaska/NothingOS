Return_Kernel_Data_Segment                  equ     00000000h
Get_DRV_Gate_Adr                            equ     00000001h
MemMan_ReturnDescriptorBaseAddress          equ     00000002h
MemMan_SetDescriptorBaseAddress             equ     00000003h
Memory_Manager_Data2Code_Descr              equ     00000004h
Memory_Manager_Change2System_Descr          equ     00000005h
Memory_Manager_ChangeDPL_Descr              equ     00000006h
Memory_Manager_InvertRWBit_Descr            equ     00000007h
Memory_Manager_Data2Stack_Descr             equ     00000008h
Memory_Manager_Dispose_Descriptor           equ     00000009h
Memory_Manager_Create_Alias                 equ     0000000Ah
Memory_Manager_Create_CallGate              equ     0000000Bh
Create_Descriptor_GDT_Baze                  equ     0000000Ch
Memory_Manager_Allocate_Memory_TSS_LDT      equ     0000000Dh
MemMan_Create_CR3                           equ     0000000Eh
FMPI_Map_File_To_Mem                        equ     0000000Fh
FPIT_PAGE_Set_Page_Implicit                 equ     00000010h
FPIT_PAGE_Set_FysPage_NotUsed               equ     00000011h
FPIT_PAGE_Set_FysPage_Used                  equ     00000012h
FPIT_PAGE_Inc_PageAge                       equ     00000013h
FPIT_PAGE_Set_Page_PTR                      equ     00000014h
FPIT_PAGE_Set_Page_CR3                      equ     00000015h
PAGE_SetPageAS_NotUsed                      equ     00000016h
PAGE_Get_Free_Page                          equ     00000017h
PAGE_SetPagesAVL_Type                       equ     00000018h
PAGE_Find_Free_Subdirectory_FysLin          equ     00000019h
PAGE_Set_Page_As_Free                       equ     0000001Ah
SwapDeamon_Over_Critical_Limit              equ     0000001Bh
SwapDeamon_Swap_Page                        equ     0000001Ch
SwapDeamon_Restore_Page                     equ     0000001Dh
SwapDeamon_Swap_FileMaped_Page              equ     0000001Eh
SwapDeamon_Restore_FileMaped_Page           equ     0000001Fh
TaskMan_Create_Task_Structure               equ     00000020h
TaskMan_InitializeTSS                       equ     00000021h
Task_Return_Task_Info_Descriptor            equ     00000022h
Task_Return_Layout_Descriptor               equ     00000023h
Task_Return_TaskInfo_Rec_Offset             equ     00000024h
TaskMan_ReLayout                            equ     00000025h
TSKMM_Inicialize_LDT                        equ     00000026h
TSKMM_CreateDescriptor_LDT                  equ     00000027h
TSKMM_Memory_Manager_Data2Code_Descr_LDT    equ     00000028h
TSKMM_Memory_Manager_ChangeDPL_Descr_LDT    equ     00000029h
TSKMM_Memory_Manager_Data2Stack_Descr_LDT   equ     0000002Ah
Set_Int_Routine                             equ     0000002Bh
Get_Int_Routine                             equ     0000002Ch
Get_Partition_INFO_Selector                 equ     0000002Dh

