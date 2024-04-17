// WriteCardDlg.h : header file
//

#if !defined(AFX_WRITECARDDLG_H__522CE0A9_6D3A_4820_9D42_71F83EB71D5A__INCLUDED_)
#define AFX_WRITECARDDLG_H__522CE0A9_6D3A_4820_9D42_71F83EB71D5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Connect.h"
#include "Param.h"
#include "WriteISO6C.h"
#include "ReadISO6C.h"
#include "WriteISO6B.h"
#include "ReadISO6B.h"
#include "DlgTestR.h"
#include "EPCC1G2Test.h"
/////////////////////////////////////////////////////////////////////////////
// CWriteCardDlg dialog

class CWriteCardDlg : public CDialog
{
// Construction
public:
	CWriteCardDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWriteCardDlg)
	enum { IDD = IDD_WRITECARD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWriteCardDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CPropertySheet m_Sheet;
	
	
	CConnect    m_ConnectPage;
	CParam		m_Param;
	CWriteISO6C m_WriteISO6CPage;
	CReadISO6C  m_ReadISO6CPage;
	CWriteISO6B m_WriteISO6BPage;
	CReadISO6B  m_ReadISO6BPage;
	DlgTestR	m_DlgTestPage;
	EPCC1G2Test	m_EPCC1G2TestPage;


	void OnInitUIText();//界面和文本更新

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWriteCardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnListuserdata(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRITECARDDLG_H__522CE0A9_6D3A_4820_9D42_71F83EB71D5A__INCLUDED_)
