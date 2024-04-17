#if !defined(AFX_READISO6C_H__10F1E3FA_E676_4E4D_857C_496B77F664E3__INCLUDED_)
#define AFX_READISO6C_H__10F1E3FA_E676_4E4D_857C_496B77F664E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReadISO6C.h : header file
//

#define  WM_READISO6C   WM_USER+102

/////////////////////////////////////////////////////////////////////////////
// CReadISO6C dialog

class CReadISO6C : public CPropertyPage
{
	DECLARE_DYNCREATE(CReadISO6C)

// Construction
public:
	CReadISO6C();
	virtual ~CReadISO6C();

// Dialog Data
	//{{AFX_DATA(CReadISO6C)
	enum { IDD = IDD_DIALOG_READISO6C };
		// NOTE - ClassWizard will add data members here.
	int		m_EasState;
	CString	m_AAccessPassword;
	BOOL	m_EasAlarm;
	CStatic	m_APic;
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CReadISO6C)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString stEpc;
	CString stUsr;
	CString stTid;
	CString stPass;

	CListCtrl m_ListShowData;
	int m_Time;
	int nStop;

	CStatic m_Dis;
	BOOL m_CheckTID;
	BOOL m_CheckPass;
	BOOL m_CheckUser;
	UINT m_TIDB;
	UINT m_PassB;
	UINT m_UserB;
	UINT m_TIDL;
	UINT m_PassL;
	UINT m_UserL;
	CStatic m_Out;

	HANDLE m_ScanExitThreadEvent;
	HANDLE m_ReadEpcHandle;
	void OnInitUIText();//界面和文本更新
	void UpdateStatus();//控制使能与不使能
	static void WINAPI CallBack_MyTimeParam(HWND   hWnd,UINT   nMsg,UINT   nIDEvent,DWORD   dwTime);

	void OnEasAlarmTest();
	CString ReportError(USHORT res);

protected:
	// Generated message map functions
	//{{AFX_MSG(CReadISO6C)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonStop();
	afx_msg LRESULT ReadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonDelone();
	afx_msg void OnBnClickedButtonSave2();
	afx_msg void OnBnClickedButton5SetAlarm6C();
	afx_msg void OnBnClickedCheck7EasAlarm6C();
	//}}AFX_MSG
	LRESULT On6cReaddatamsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READISO6C_H__10F1E3FA_E676_4E4D_857C_496B77F664E3__INCLUDED_)
