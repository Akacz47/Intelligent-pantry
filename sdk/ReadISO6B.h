#if !defined(AFX_READISO6B_H__55486510_8A83_4E23_8C05_B9D60B2F79B6__INCLUDED_)
#define AFX_READISO6B_H__55486510_8A83_4E23_8C05_B9D60B2F79B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReadISO6B.h : header file
//

#define  WM_READISO6B   WM_USER+112

/////////////////////////////////////////////////////////////////////////////
// CReadISO6B dialog

class CReadISO6B : public CPropertyPage
{
	DECLARE_DYNCREATE(CReadISO6B)

// Construction
public:
	CReadISO6B();
	virtual ~CReadISO6B();

// Dialog Data
	//{{AFX_DATA(CReadISO6B)
	enum { IDD = IDD_DIALOG_READISO6B };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CReadISO6B)
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
	
	void OnInitUIText();//界面和文本更新
	void UpdateStatus();//控制使能与不使能

protected:
	// Generated message map functions
	//{{AFX_MSG(CReadISO6B)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonStop();
	afx_msg LRESULT ReadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonDelone();
	afx_msg void OnBnClickedButtonSave2();
	//}}AFX_MSG
	LRESULT On6bReaddatamsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READISO6B_H__55486510_8A83_4E23_8C05_B9D60B2F79B6__INCLUDED_)
