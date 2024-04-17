#if !defined(AFX_CONNECT_H__EEC04AFA_1959_49D2_9CD2_2A48BC12EB88__INCLUDED_)
#define AFX_CONNECT_H__EEC04AFA_1959_49D2_9CD2_2A48BC12EB88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Connect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnect dialog

class CConnect : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnect)

// Construction
public:
	CConnect();
	~CConnect();

// Dialog Data
	//{{AFX_DATA(CConnect)
	enum { IDD = IDD_PROPPAGE_CONNECT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	int		m_ConnectMode;//0-rs232
	int		m_LanguageIndex;//0-English,1-chinese
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnect)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CWnd * pMain;

	void OnInitUIText();//界面和文本更新
	void UpdateStatus();//控制使能与不使能
	
	int icnt;
	int iNum;
	int iCount;
	
	static void WINAPI CallBack_MyTimeListDisp(HWND   hWnd,UINT   nMsg,UINT   nIDEvent,DWORD   dwTime);
	//进度提示
	static DWORD WINAPI ThreadListDispPro(LPVOID pParam);
	HANDLE hDispThread;

	int itPos;

protected:
	// Generated message map functions
	//{{AFX_MSG(CConnect)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRADIOCONEnglish();
	afx_msg void OnRADIOCONChinese();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonConnectdis();
	afx_msg void OnRADIOConnectRS232();
	afx_msg void OnRADIOCONUsb();
	afx_msg void OnButton2();
	//}}AFX_MSG
	afx_msg LRESULT OnConnectdatacustom(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECT_H__EEC04AFA_1959_49D2_9CD2_2A48BC12EB88__INCLUDED_)
