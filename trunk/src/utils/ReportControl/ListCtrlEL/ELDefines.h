#pragma once


//
//=============================================================================
// �汾��Ϣ
//=============================================================================
//
#define _EL_VERSION_                    MAKELONG(1, 0)

#define _EL_VERSION_MAJOR               0

#define _EL_VERSION_MINOR               1

#define _EL_VERSION_REVISION            1



// ����
#ifdef _DEBUG
#define EL_DEBUGLOG  NKDbgPrintfW
#else
#define EL_DEBUGLOG(...)
#endif

#define EL_STRING2(x) #x
#define EL_STRING(x) EL_STRING2(x)


// Ĭ�Ͽ���˫�������
#ifndef EL_DOUBLEBUFFER
#define EL_DOUBLEBUFFER
#endif



//
//=============================================================================
// �����������
//=============================================================================
//
class CListCtrlEL;
class CHeaderCtrlEL;
class CELColumn;
class CELRecord;

class CELRecordItem;
class CELRecordItemText;
class CELRecordItemNumber;
class CELRecordItemEditOptions;
class CELRecordItemConstraints;
class CELRecordItemConstraint;
class CELInplaceButtons;

class CELInplaceControl;
class CELInplaceEdit;
class CELInplaceList;
class CELInplaceButton;



//
//=============================================================================
// ��������
//=============================================================================
//
#define ELSUPER             CListCtrl
#define ELHDRSUPER          CHeaderCtrl

// ��������ӳ��
#define ELVK_ESC            VK_ESCAPE
#define ELVK_OK             VK_RETURN

const COLORREF  EL_COLOR_DEFAULT = (COLORREF)-1;

// item��������ʱ���������׵ļ��
const int  EL_ITEMDRAW_MARGIN = 5;


//
//=============================================================================
// ֪ͨ��Ϣ����
//=============================================================================
//


//
//-----------------------------------------------------------------------------
// ELNM_ITEM_CHECKED
//  ֪ͨ��������һ��item��ĸ�ѡ��ѡ��/ȡ��
//  ����Ĳ���ELNM_RECORDITEM_ARGS
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_CHECKED   (NM_FIRST-53)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_VALUECHANGED
//  ֪ͨ��������һ��item����ַ���ֵ�����˸ı�
//  ����Ĳ���ELNM_RECORDITEM_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_VALUECHANGED  (NM_FIRST-57)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_INPLACEBUTTONDOWN
//  ֪ͨ������item��һ�����ð�ť�����
//  ����Ĳ���ELNM_INPLACEBUTTON_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_INPLACEBUTTONDOWN  (NM_FIRST-58)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_AFTERLISTDROPDOWN
//  ֪ͨ������item��һ������LIST�����, 
//  �����ڿ��Դ���һЩ������Ĵ���, ��������Լ�����Ŀ, ����ѡ��ĳ����
//  ����Ĳ���ELNM_INPLACELIST_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_AFTERLISTDROPDOWN  (NM_FIRST-59)
#define ELNM_ITEM_ENDLISTDROPDOWN    (NM_FIRST-60)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_CONSTRAINTCHANGED
//  ֪ͨ������item��һ������LIST�ѡ��, �����ڿ���֪���ĸ��ѡ����
//  ����Ĳ���ELNM_INPLACELIST_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_CONSTRAINTCHANGED  (NM_FIRST-62)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_BEGINEDIT
//  ֪ͨ������item��һ������Edit�Ѿ�����, �û�׼���༭(������List, List��ELNM_ITEM_AFTERLISTDROPDOWN��Ϣ)
//  ��һ��inplace-edit����, ׼������inplace�ؼ�(ֻEdit, ������List)
//  �������ELNM_RECORDITEM_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_BEGINEDIT (NM_FIRST-65)
#define ELNM_ITEM_ENDEDIT   (NM_FIRST-66)
//
//-----------------------------------------------------------------------------
// ELNM_ITEM_EDIT_CANCELED
//  ֪ͨ������item��edit�ؼ���ȡ���༭
//  ����Ĳ���ELNM_RECORDITEM_ARGS
//
// !!!ע��: �ڸ���Ϣ�в��ܽ���ҳ���л�(�ؼ�����), ��Itemɾ������ bug2013.06.19
//-----------------------------------------------------------------------------
//
#define ELNM_ITEM_EDIT_CANCELED  (NM_FIRST-72)
//
//-----------------------------------------------------------------------------
// ELNM_OKCLICK
//  ֪ͨ������OK����������, ��������ڴ����˸���Ϣ, ��Ӧ�÷���TRUE
//  ����Ĳ���NULL
//
// �ڸ���Ϣ�п��Խ���ҳ���л�(���ؼ�����),��Itemɾ�����κβ���
//-----------------------------------------------------------------------------
//
#define ELNM_OKCLICK       (NM_FIRST-83)


//
//=============================================================================
// WM_COMMAND������Ϣ
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// EL_COMMAND_INPLACEEDIT_BACKSPACE
//  ��inplace-edit����ʱ, ��Ӧ���˵�������ID: ɾ��һ���ַ�
//  ע��: ��ID��Ҫ���������ܶ����IDһ��, ����Ӧ���޸�
//-----------------------------------------------------------------------------
//
#define EL_COMMAND_INPLACEEDIT_BACKSPACE  32799


//
//=============================================================================
// ö��
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ���뷽ʽ
//-----------------------------------------------------------------------------
//
enum ELAlignment
{
    EL_LEFT                 = DT_LEFT,
    EL_CENTER               = DT_CENTER,
    EL_RIGHT                = DT_RIGHT
};

//
//-----------------------------------------------------------------------------
// inplace��ť����
//-----------------------------------------------------------------------------
//
enum ELInplaceButtonType 
{
    EL_BTNTYPE_EXPANDBUTTON = 100,
    EL_BTNTYPE_COMBOBUTTON  = 101
};
