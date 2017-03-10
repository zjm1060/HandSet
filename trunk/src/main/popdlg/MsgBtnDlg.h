#pragma once

#include "src/main/resource.h"
#include "src/main/control/statusdisp.h"
#include "src/main/control/CountDownWnd.h"
#include "src/main/control/buttonex.h"


typedef DWORD (WINAPI *MsgTask)(WPARAM wParam, LPARAM lParam, void* arg);

class CRNaviMenu;
class CNaviToolBar;

// CMsgBtnDlg dialog

class CMsgBtnDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgBtnDlg)

public:
	CMsgBtnDlg(UINT nMsgID, UINT nBitmapID = 0, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgBtnDlg();

// Dialog Data
	enum { IDD = IDD_POPUP_SHOW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnMenuF5();
    afx_msg void OnMenuF6();
    afx_msg void OnClickBtnOk();
    afx_msg void OnClickBtnCancel();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


	afx_msg void OnClickBtnNum2();
protected:

    virtual void OnCancel() {}
    virtual void OnOK() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:

    /**
     *      ������Ϣ����
     */
    void _SetMsg(CString csMsg)
    {
        m_csMsg = csMsg;
    }

    /**
     *      ������ϢͼƬ
     */
    void _SetBitmap(HBITMAP hBitmap)
    {
        std::vector<HBITMAP> v;
        v.push_back(hBitmap);
        m_ctlPic._InsertStatus(1, v);
    }

    /**
     *      ���ö��ͼƬ
     */
    void _SetBitmap(std::vector<HBITMAP>& v)
    {
        m_ctlPic._InsertStatus(1, v);
    }

    /**
     *      ��ȡ�ؼ�
     */
    CStatusDisp* _GetCtl()
    {
        return &m_ctlPic;
    }

    /**
     *      ���������С
     */
    void _SetRect(CRect rc)
    {
        m_rcClient = rc;
    }

    /**
     *      ����ͼƬ����
     */
    void _SetPicRC(CRect rc)
    {
        m_rcPic = rc;
    }

    /**
     *      ������������
     */
    void _SetTextRC(CRect rc)
    {
        m_rcText = rc;
    }

    /**
     *      ���ò˵�
     */
    void _SetMenu(UINT menu)
    {
        m_nMenuID = menu;
    }

    /**
     *      �����Ƿ���ʾͼƬ
     */
    void _SetBitmapDisp(bool show)
    {
        m_bBitmap = show;
    }

    /**
     *      ���ò˵�ID��Χ
     */
    void _SetCommandRange(UINT begin, UINT end, MsgTask func, void* data = NULL)
    {
        m_nBegin    = begin;
        m_nEnd      = end;
        m_pFunc     = func;
        m_pData     = data;
    }

    /**
     *      ���õ���ʱ�ؼ�
     */
    inline void _EnableCountDown()
    {
        m_bEnableCountDown = true;
    }

    inline CCountDownWnd* _GetCountDown()
    {
        return &m_ctlCountDown;
    }

    /**
     *      �ֶ���ԭ�˵�
     */
    inline void _ResetMenu();
	/**
     *     ���ò˵�����
     */
	void _ResetF5Text(CString text);
	/**
     *     ���ò˵�����
     */
	void _ResetF6Text(CString text);

private:

    UINT                m_nMenuID;          ///< �˵�ID

    UINT                m_nBegin;
    UINT                m_nEnd;
    MsgTask             m_pFunc;
    void*               m_pData;

    bool                m_bBitmap;          ///< �Ƿ񴴽�ͼƬ�ؼ�
    bool                m_bEnableCountDown; ///< ���õ���ʱ�ؼ�

    CRect               m_rcClient;         ///< �ͻ�����С
    CRect               m_rcPic;            ///< ͼƬ����
    CRect               m_rcText;           ///< ��������

    CString             m_csMsg;            ///< ��Ϣ����

    CStatusDisp         m_ctlPic;           ///< ͼƬ�ؼ�
    CCountDownWnd       m_ctlCountDown;     ///< ����ʱ�ؼ�

    CNaviToolBar*       m_pNaviBar;         ///< �˵�����
    CRNaviMenu*         m_pRNaviMenu;       ///< �˵���Դ��������

	CButtonEx			m_btnNum2;
	CButtonEx			m_btnCancel;

	CString				m_F5Text;
	CString				m_F6Text;
};


inline void CMsgBtnDlg::_ResetF5Text(CString text)
{
	m_F5Text = text;
}

inline void CMsgBtnDlg::_ResetF6Text(CString text)
{
	m_F6Text = text;
}