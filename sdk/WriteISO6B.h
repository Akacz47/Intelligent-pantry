#if !defined(AFX_WRITEISO6B_H__B59D1AD6_A435_4718_8E07_34B353BCF8BD__INCLUDED_)
#define AFX_WRITEISO6B_H__B59D1AD6_A435_4718_8E07_34B353BCF8BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WriteISO6B.h : header file
//

#define  WM_MSG_W6B   WM_USER+201

struct InData_st_W6B
{
	int     nLog;
	CString stNo;
	CString stEPC;
	CString stUse;
	CString stTid;
	CString stPass;
};

struct StartNo_st_W6B
{
	int    nOne;
	int    nBegin;
	int    nCount;
	int    nNumber; 
	int    nTotal;
};

/////////////////////////////////////////////////////////////////////////////
// CWriteISO6B dialog

class CWriteISO6B : public CPropertyPage
{
	DECLARE_DYNCREATE(CWriteISO6B)

// Construction
public:
	CWriteISO6B();
	virtual ~CWriteISO6B();

// Dialog Data
	//{{AFX_DATA(CWriteISO6B)
	enum { IDD = IDD_PROPPAGE_WRITEISO6B };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWriteISO6B)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString stFileName;
	CString strEpc;
	int     nStatus;
	int     nStop;
	int     nWord;
	
	BYTE AccessPassword[32];
	int		iFlagProtectOpt;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了)
	
	StartNo_st_W6B stStartNo;
	InData_st_W6B stInData;
	
	CListCtrl m_ListInData;
	UINT m_BeginData;
	UINT m_Count;
	UINT m_Sum;
	CString m_DisStr;
	int m_Word;
	
	
	BOOL m_SetEPC;
	BOOL m_SetUse;
	BOOL m_SetPass;
	CListCtrl m_ListOutData;
	UINT m_Time;
	CStatic m_MsgImg;
	CStatic m_MsgDis;
	CStatic m_MsgOut;
	int m_Total;
	int m_Falsh;
	int m_Mod;
	int m_True;
	int m_False;
	
	CString m_End;
	CString m_Befro;
	BOOL m_CBefor;
	BOOL m_CEnd;
	
	CStatic m_MsgIn;
	int m_WriteMode;
	
	void OnInitUIText();//界面和文本更新
	void UpdateStatus();//控制使能与不使能

protected:
	// Generated message map functions
	//{{AFX_MSG(CWriteISO6B)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonData();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnCbnSelendokComboWord();
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonMove();
	afx_msg void OnCbnKillfocusComboWord();
	afx_msg LRESULT WriteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonDelout();
	afx_msg void OnBnClickedRadioEpc();
	afx_msg void OnBnClickedRadioUser();
	afx_msg void OnBnClickedRadioPass();
	afx_msg void OnBnClickedButtonIndata();
	afx_msg void OnBnClickedCheckEpc();
	afx_msg void OnBnClickedCheckUse();
	afx_msg void OnBnClickedCheckPass();
	afx_msg void OnBnClickedButtonSuccess();
	afx_msg void OnBnClickedButtonError();
	afx_msg void OnBnClickedCheckLockepcPass();
	afx_msg void OnBnClickedCheckLockepcNeverwriteable();
	//}}AFX_MSG
	afx_msg LRESULT On6bWritedatamsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITEISO6B_H__B59D1AD6_A435_4718_8E07_34B353BCF8BD__INCLUDED_)
