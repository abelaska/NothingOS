;########################################################################
;# Data driveru Consoles
;########################################################################

Console_Driver_DATA :
Console_Driver_Data_Start :
;################# standardni fce
   DD      Offset Console_Driver_INIT     ;00h
   DD      Offset Console_Driver_DONE     ;01h
   DD      Offset Console_Driver_READ     ;02h
   DD      Offset Console_Driver_WRITE    ;03h
   DD      Offset UnImplement   ;04h
   DD      Offset UnImplement   ;05h
   DD      Offset UnImplement   ;06h
   DD      Offset UnImplement   ;07h
   DD      Offset UnImplement   ;08h
   DD      Offset UnImplement   ;09h
   DD      Offset UnImplement   ;0Ah
   DD      Offset UnImplement   ;0Bh
   DD      Offset UnImplement   ;0Ch
   DD      Offset UnImplement   ;0Dh
   DD      Offset UnImplement   ;0Eh
   DD      Offset UnImplement   ;0Fh
;################# rozsirene fce
   DD      Offset Console_Driver_Scroll                 ;10h
   DD      Offset Console_Driver_ScrollDelta            ;11h
   DD      Offset Console_Driver_Clear                  ;12h
   DD      Offset Console_Driver_Show_Cursor            ;13h
   DD      Offset Console_Driver_Hide_Cursor            ;14h
   DD      Offset Console_Driver_GotoXY                 ;15h
   DD      Offset Console_Driver_GetXY                  ;16h
   DD      Offset Console_Driver_AddLine                ;17h
   DD      Offset Console_Driver_Switch                 ;18h
   DD      Offset Console_Driver_exist_Console          ;19h
   DD      Offset Console_Driver_GetActive_Console      ;1Ah
   DD      Offset Console_Driver_SetAttrib              ;1Bh
   DD      Offset Console_Driver_Get_Char_Redir_Addr    ;1Ch
   DD      Offset Console_Driver_Set_Char_Redir_Addr    ;1Dh
Console_Driver_Fce_Count :
;##############################################################################
Console_Driver_Library_Name    DB      'nConsoles Driver 1.3 ',0
Horizontal_Tab                 DD  12
Vertical_Tab                   DD  3
;##############################################################################
Control_Seq_Start              Equ '\'
Control_Seq_Size               equ 64
;##############################################################################
Consoles_Control_Seguence      DB (Max_Console_Count*Control_Seq_Size) Dup (0)
Consoles_Cursor_Saved          DD (Max_Console_Count*2)  Dup (0)
Consoles_Control_Escape        DB (Max_Console_Count)    Dup (0)
;##############################################################################
Colors_Redirection_30   DB  00h,0Ch,0Ah,0Eh,09h,0Dh,0Bh,0Fh
Colors_Redirection_40   DB  00h,40h,20h,60h,10h,50h,30h,70h
Blink_Col               DB  080h
Dim_Col                 DB  0F3h
Reset_Col               DB  000h
;##############################################################################
Char_Redirection_Descriptor DW 0
Consoles_Initializing_OK    DB 0
;##############################################################################
CCS_ANSI_EndChars      db 'ABCDfHRnsuJKLM@Pypmhlae`dFEZXST'
CCS_ANSI_EndChars_Num  Equ ($ - Offset CCS_ANSI_EndChars)
;##############################################################################
CCS_ANSI_FCE :
    DD    Offset  CCS_ANSI_Cursor_Up                    ;A
    DD    Offset  CCS_ANSI_Cursor_Down                  ;B
    DD    Offset  CCS_ANSI_Cursor_Forward               ;C
    DD    Offset  CCS_ANSI_Cursor_Backward              ;D
    DD    Offset  CCS_ANSI_Cursor_position              ;f
    DD    Offset  CCS_ANSI_Cursor_position              ;H
    DD    Offset  CCS_ANSI_Cursor_position_Report       ;R
    DD    Offset  CCS_ANSI_Device_status_report         ;n
    DD    Offset  CCS_ANSI_Save_cursor_position         ;s
    DD    Offset  CCS_ANSI_Restore_cursor_position      ;u
    DD    Offset  CCS_ANSI_Erase_display                ;J
    DD    Offset  CCS_ANSI_Erase_to_eol                 ;K
    DD    Offset  CCS_ANSI_Inserts_blank_lines          ;L
    DD    Offset  CCS_ANSI_Deletes_lines                ;M
    DD    Offset  CCS_ANSI_Inserts_blank_chars          ;@
    DD    Offset  CCS_ANSI_Erase_Characters             ;P
    DD    Offset  CCS_ANSI_Output_char_translate        ;y
    DD    Offset  CCS_ANSI_Keyboard_Key_Reassignment    ;p
    DD    Offset  CCS_ANSI_Set_Graphics_Rendition       ;m
    DD    Offset  CCS_ANSI_Set_mode                     ;h
    DD    Offset  CCS_ANSI_Reset_Mode                   ;l
    DD    Offset  CCS_ANSI_Cursor_Forward               ;a
    DD    Offset  CCS_ANSI_Cursor_Down                  ;e
    DD    Offset  CCS_ANSI_Move_Cursor_2CharPos         ;`
    DD    Offset  CCS_ANSI_Move_Cursor_2Line            ;d
    DD    Offset  CCS_ANSI_Cursor_Up_StartLine          ;F
    DD    Offset  CCS_ANSI_Cursor_Down_StartLine        ;E
    DD    Offset  CCS_ANSI_Cursor_Back_Tabs             ;Z
    DD    Offset  CCS_ANSI_Erase_Characters             ;X
    DD    Offset  CCS_ANSI_Scroll_Up                    ;S
    DD    Offset  CCS_ANSI_Scroll_Down                  ;T


;##############################################################################
Console_Driver_Data_End :
Console_Driver_DATA_SIZE equ (((Console_Driver_DATA_END - Console_Driver_DATA_Start - 1) shr 12) +1)

