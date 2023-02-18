;#############################################################################
;# Fce internich knihoven
;#############################################################################

;#############################################################################
;# Fce knihovny VFS
;#############################################################################
VL_VFS_Fce :
VL_VFS_Num_Of_Fce_Start Equ $
    DD Offset VFS_Write_vl
    DD Offset VFS_Read_vl
    DD Offset VFS_Open_vl
    DD Offset VFS_Close_vl
    DD Offset VFS_Map_File_vl
    DD Offset VFS_Sync_vl
    DD Offset VFS_MkNod_vl
    DD Offset VFS_Trace_Path_vl
    DD Offset VFS_Dir_vl
    DD Offset VFS_Mount_Device_vl
    DD Offset VFS_UMount_Device_vl
    DD Offset VFS_Mount_Device_Core_vl
    DD Offset VFS_Find_Mounted_Inode_vl
    DD Offset VFS_Get_Inode_Mount_device_vl
    DD Offset VFS_Check_Open_Mode_vl
    DD Offset VFS_Check_Inode_vl
    DD Offset VFS_Check_Inode_In_VFS_Inode_vl
    DD Offset VFS_Is_Inode_File_vl
    DD Offset VFS_Is_Inode_Directory_vl
    DD Offset VFS_Is_Inode_BLK_Device_vl
VL_VFS_Num_Of_Fce Equ (($ - VL_VFS_Num_Of_Fce_Start) Shr 2)-1
;#############################################################################
;# Fce knihovny Libs
;#############################################################################
VL_Libs_Fce :
VL_Libs_Num_Of_Fce_Start Equ $
    DD Offset LIB_TestLIBExist
    DD Offset LIB_Get_Lib_Version
    DD Offset LIB_Get_CallGate
VL_Libs_Num_Of_Fce Equ (($ - VL_Libs_Num_Of_Fce_Start) Shr 2)-1
;#############################################################################
;# Fce knihovny TaskMan
;#############################################################################
VL_Task_Fce :
VL_Task_Num_Of_Fce_Start Equ $
    DD Offset Exec_Restricted
    DD Offset TASK_Kill_Me
    DD Offset TASK_Get_Current_Task_Num
    DD Offset TASK_Get_Task_UID_GID
    DD Offset TASK_Get_Task_UID_GID_Eff
    DD Offset TASK_Get_Task_UID_GID_FS
    DD Offset TASK_Get_Task_Priority
    DD Offset TASK_Get_Task_Status
    DD Offset TASK_Get_Task_Parent
    DD Offset TASK_Get_Task_Child
    DD Offset TASK_Get_Task_OlderBrother
    DD Offset TASK_Get_Task_YoungerBrother
    DD Offset TASK_Get_Tasks_Count
    DD Offset TASK_Get_Max_Tasks
    DD Offset TASK_Get_Max_Zoombies
    DD Offset TASK_Get_Time_Switch_Const
    DD Offset TASK_Switch_Now
    DD Offset TASK_Get_Time_Used
    DD Offset TASK_Get_VFS_STDOut_Inode
    DD Offset TASK_Get_VFS_STDIn_Inode
    DD Offset TASK_Get_VFS_STDError_Inode
    DD Offset TASK_Get_VFS_Home_Inode
    DD Offset TASK_Get_VFS_Temp_Inode
    DD Offset TASK_Get_My_Parameters_Length
    DD Offset TASK_Get_My_Parameters
    DD Offset TASK_Get_Task_Name
    DD Offset TASK_Get_VFS_Inode
    DD Offset TASK_Sleep_Me
    DD Offset EXEC_Ret_Libs_Name
    DD Offset EXEC_Ret_Libs_Version
    DD Offset TASK_Set_My_Current_Dir
    DD Offset TASK_Get_VFS_HOME
    DD Offset TASK_Get_VFS_Temp
    DD Offset TASK_Get_Current_Dir
    DD Offset TASK_Get_My_Current_Dir
    DD Offset TASK_Get_Processes_Info
    DD Offset TASK_Get_Parameters_Length_vl ;(Booh Only)
    DD Offset TASK_Get_Parameters_vl        ;(Booh Only)
    DD Offset TASK_Switch_Disable_vl        ;(Booh Only)
    DD Offset TASK_Switch_Enable_vl         ;(Booh Only)
    DD Offset TASK_Switch_Enable_X_vl       ;(Booh Only)
    DD Offset TASK_Set_Task_UID_GID_vl      ;(Booh Only)
    DD Offset TASK_Set_Task_UID_GID_eff_vl  ;(Booh Only)
    DD Offset TASK_Set_Task_UID_GID_FS_vl   ;(Booh Only)
    DD Offset TASK_Set_Task_Priority_vl     ;(Booh Only)
    DD Offset TASK_Set_Task_Status_vl       ;(Booh Only)
    DD Offset TASK_Kill_Task_vl             ;(Booh All, of family tasks only)
    DD Offset EXEC_CPL0_vl                  ;(Booh Only)
    DD Offset TASK_Set_Time_Switch_Const_vl ;(Booh Only)
    DD Offset TASK_Set_Max_Zoombies_vl      ;(Booh Only)
    DD Offset TASK_Sleep_Task_vl            ;(Booh Only)
    DD Offset Task_Wakeup_Task_vl           ;(Booh Only)
    DD Offset TASK_Set_VFS_HOME_vl          ;(Booh Only)
    DD Offset TASK_Set_VFS_Temp_Vl          ;(Booh Only)
    DD Offset TASK_Set_Current_Dir_vl       ;(Booh Only)
VL_Task_Num_Of_Fce Equ (($ - VL_Task_Num_Of_Fce_Start) Shr 2)-1
;#############################################################################
;# Fce knihovny Memory
;#############################################################################
VL_Mem_Fce :
VL_Mem_Num_Of_Fce_Start Equ $
    DD Offset TSKMM_Alloc_B
    DD Offset TSKMM_Dealloc
    DD Offset Alloc_shared_memory               ;(Booh Only)
    DD Offset Alloc_fyslin_memory               ;(Booh Only)
    DD Offset Alloc_taskman_memory              ;(Booh Only)
    DD Offset DeAlloc_vl                        ;(Booh Only)
    DD Offset Memory_Manager_Create_CallGate_vl ;(Booh Only)
VL_Mem_Num_Of_Fce Equ (($ - VL_Mem_Num_Of_Fce_Start) Shr 2)-1
;#############################################################################
VL_System_Fce :
VL_System_Num_Of_Fce_Start Equ $
    DD Offset Return_Kernel_Data_Segment
    DD Offset Get_DRV_Gate_Adr
    DD Offset MemMan_ReturnDescriptorBaseAddress
    DD Offset MemMan_SetDescriptorBaseAddress
    DD Offset Memory_Manager_Data2Code_Descr
    DD Offset Memory_Manager_Change2System_Descr_vl
    DD Offset Memory_Manager_ChangeDPL_Descr_vl
    DD Offset Memory_Manager_InvertRWBit_Descr
    DD Offset Memory_Manager_Data2Stack_Descr
    DD Offset Memory_Manager_Dispose_Descriptor
    DD Offset Memory_Manager_Create_Alias
    DD Offset Memory_Manager_Create_CallGate_vl
    DD Offset Create_Descriptor_GDT_Baze_vl
    DD Offset Memory_Manager_Allocate_Memory_TSS_LDT
    DD Offset MemMan_Create_CR3
    DD Offset FMPI_Map_File_To_Mem
    DD Offset FPIT_PAGE_Set_Page_Implicit_vl
    DD Offset FPIT_PAGE_Set_FysPage_NotUsed_vl
    DD Offset FPIT_PAGE_Set_FysPage_Used_vl
    DD Offset FPIT_PAGE_Inc_PageAge_vl
    DD Offset FPIT_PAGE_Set_Page_PTR_vl
    DD Offset FPIT_PAGE_Set_Page_CR3_vl
    DD Offset PAGE_SetPageAS_NotUsed
    DD Offset PAGE_Get_Free_Page
    DD Offset PAGE_SetPagesAVL_Type
    DD Offset PAGE_Find_Free_Subdirectory_FysLin
    DD Offset PAGE_Set_Page_As_Free
    DD Offset SwapDeamon_Wake_Up_Monster
    DD Offset SwapDeamon_Swap_Page
    DD Offset SwapDeamon_Restore_Page
    DD Offset SwapDeamon_Swap_FileMaped_Page
    DD Offset SwapDeamon_Restore_FileMaped_Page
    DD Offset TaskMan_Create_Task_Structure
    DD Offset TaskMan_InitializeTSS
    DD Offset Task_Return_Task_Info_Descriptor
    DD Offset Task_Return_Layout_Descriptor
    DD Offset Task_Return_TaskInfo_Rec_Offset
    DD Offset TaskMan_ReLayout
    DD Offset TSKMM_Inicialize_LDT
    DD Offset TSKMM_CreateDescriptor_LDT_vl
    DD Offset TSKMM_Memory_Manager_Data2Code_Descr_LDT
    DD Offset TSKMM_Memory_Manager_ChangeDPL_Descr_LDT_vl
    DD Offset TSKMM_Memory_Manager_Data2Stack_Descr_LDT
    DD Offset Set_Int_Routine_vl
    DD Offset Get_Int_Routine
    DD Offset Get_Partition_INFO_Selector
VL_System_Num_Of_Fce Equ (($ - VL_System_Num_Of_Fce_Start) Shr 2)-1