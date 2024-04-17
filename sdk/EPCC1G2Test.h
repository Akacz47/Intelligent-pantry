#if !defined(AFX_EPCC1G2TEST_H__71E6047D_7FA5_4C09_A25C_E464CDC85F2B__INCLUDED_)
#define AFX_EPCC1G2TEST_H__71E6047D_7FA5_4C09_A25C_E464CDC85F2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EPCC1G2Test.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EPCC1G2Test dialog

class EPCC1G2Test : public CPropertyPage
{
	DECLARE_DYNCREATE(EPCC1G2Test)
// Construction
public:
	EPCC1G2Test();
	~EPCC1G2Test();

	void UpdateStatus();
	CString ReportError(USHORT res);

	BYTE m_antenna_sel;
	unsigned int Read_times;
	int mem,ptr,len,EPC_Word,timer_interval,Detect;
	unsigned char mask[512],IDTemp[512];
	BOOL OpenPassword;
	BYTE AccessPassword[4];
	BYTE gAntenna[4];//add by mqs 20130710
	int iModAnt;	//选中的个数


	void OnInitUIText();//界面和文本更新

//	MFECToolTip m_toolTip;

// Dialog Data
	//{{AFX_DATA(EPCC1G2Test)
	enum { IDD = IDD_EPCC1G2_DIALOG };
	CComboBox	m_PTagID;
	CStatic	m_APic;
	CComboBox	m_ATagID;
	CComboBox	m_LBTagID;
	CComboBox	m_KTagID;
	CComboBox	m_LTagID;
	CListBox	m_ListData;
	CComboBox	m_TagID;
	CComboBox	m_Interval;
	CListCtrl	m_ListID;
	BOOL	m_Antenna1;
	BOOL	m_Antenna2;
	BOOL	m_Antenna3;
	BOOL	m_Antenna4;
	int		m_mem;
	int		m_LAddress;
	int		m_LLen;
	CString	m_LData;
	BOOL	m_ListTagID;
	int		m_RWAddress;
	int		m_RWLen;
	BOOL	m_ReadDataBlock;
	int		m_Condition;
	int		m_Password;
	CString	m_LAccessPassword;
	CString	m_RWAccessPassword;
	int		m_Condition2;
	int		m_RWmem;
	int		m_Lmem;
	CString	m_Data;
	CString	m_KillPassword;
	int		m_LBAddress;
	CString	m_LBAccessPassword;
	int		m_EasState;
	CString	m_AAccessPassword;
	BOOL	m_EasAlarm;
	CString	m_RProtectPassword;
	CString	m_KPassword;
	BOOL	m_Kword;
	BOOL	m_DetectTag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(EPCC1G2Test)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(EPCC1G2Test)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCHECK5ListEpcofTag6C();
	afx_msg void OnCHECK6ReadD6C();
	afx_msg void OnBUTTON4WrtieD6C();
	afx_msg void OnButton1();
	afx_msg void OnRadio19();
	afx_msg void OnRadio20();
	afx_msg void OnRadio21();
	afx_msg void OnRadio22();
	afx_msg void OnBUTTON2KillTag6C();
	afx_msg void OnButton3();
	afx_msg void OnButton5();
	afx_msg void OnCheck7();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnCheck9();
	afx_msg void OnCHECK10DetectTag6C();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg LRESULT OnEpc6ctrandatamsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EPCC1G2TEST_H__71E6047D_7FA5_4C09_A25C_E464CDC85F2B__INCLUDED_)
