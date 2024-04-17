#if !defined(AFX_DLGTESTR_H__38FD6CFA_AEF1_4244_92D0_BA2B064A41E5__INCLUDED_)
#define AFX_DLGTESTR_H__38FD6CFA_AEF1_4244_92D0_BA2B064A41E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTestR.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgTestR dialog

class DlgTestR : public CPropertyPage
{
	DECLARE_DYNCREATE(DlgTestR)

// Construction
public:
	DlgTestR();
	~DlgTestR();

// Dialog Data
	//{{AFX_DATA(DlgTestR)
	enum { IDD = IDD_DIALOG_TEST };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DlgTestR)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	//自动模式下读线程函数
	static DWORD WINAPI ReadThreadFuncTag(LPVOID pParam);
	HANDLE hReadThread;

protected:
	// Generated message map functions
	//{{AFX_MSG(DlgTestR)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTESTR_H__38FD6CFA_AEF1_4244_92D0_BA2B064A41E5__INCLUDED_)
