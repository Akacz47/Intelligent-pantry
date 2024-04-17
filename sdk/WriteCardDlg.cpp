// WriteCardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "WriteCardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
HWND	gMainHwnd	=	NULL;////主界面页子


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteCardDlg dialog

CWriteCardDlg::CWriteCardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWriteCardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWriteCardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWriteCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWriteCardDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWriteCardDlg, CDialog)
	//{{AFX_MSG_MAP(CWriteCardDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LISTUSERDATA, OnListuserdata)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteCardDlg message handlers
void CWriteCardDlg::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_A0001"));//主界面的标题
	
	//GetDlgItem(IDC_BUTTON_CLEAR)->SetWindowText(theApp.g_LoadString("IDWQE_A0003"));//清空
	
	
#if 1
	CString strText;
	int iIndex = 0;
	CTabCtrl *pTab = m_Sheet.GetTabControl();
	
	TC_ITEM ti;
	ti.mask = TCIF_TEXT;
	strText	=	theApp.g_LoadString("IDWQF_B0001");//读写器连接测试
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));
	
	
	strText	=	theApp.g_LoadString("IDWQF_C0001");//读写器参数
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));
	
	
	strText	=	theApp.g_LoadString("IDWQF_D0001");//蓝牙参数
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));
	
	strText	=	theApp.g_LoadString("IDWQF_E0001");//数据下载
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));

	strText	=	theApp.g_LoadString("IDWQF_F0001");//数据下载
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));

	strText	=	theApp.g_LoadString("IDWQF_G0001");//数据下载
	ti.pszText = strText.GetBuffer(MAX_PATH);
	strText.ReleaseBuffer();
	VERIFY (pTab->SetItem (iIndex++, &ti));

#endif
	
	
}

BOOL CWriteCardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	gMainHwnd	=	this->GetSafeHwnd();


	m_Sheet.AddPage(&m_ConnectPage);
	m_Sheet.AddPage(&m_Param);
	m_Sheet.AddPage(&m_WriteISO6CPage);
	m_Sheet.AddPage(&m_ReadISO6CPage);
	m_Sheet.AddPage(&m_WriteISO6BPage);
	m_Sheet.AddPage(&m_ReadISO6BPage);
	m_Sheet.AddPage(&m_EPCC1G2TestPage);
	//m_Sheet.AddPage(&m_DlgTestPage);
	
	m_Sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	
	//ShowWindow(SW_SHOWNORMAL);
	m_Sheet.SetActivePage(0);
	m_ConnectPage.pMain=this;


	::PostMessage(gMainHwnd,WM_LISTUSERDATA,2,0);	//界面和文本更新


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWriteCardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWriteCardDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWriteCardDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CWriteCardDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}

//用户自定义消息 WM_LISTUSERDATA
afx_msg LRESULT CWriteCardDlg::OnListuserdata(WPARAM wParam, LPARAM lParam)
{
	int iwParam	=	(int)wParam;
	int ilParam	=	(int)lParam;
	
	if ( 2 == iwParam )
	{
		OnInitUIText();	//更新语言
	}
	else if ( 3 == iwParam )
	{
		int iSeet = (int)ilParam;
		
		m_Sheet.SetActivePage(iSeet);
		
	}
	
	return 0;
}

void CWriteCardDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if ( 0 != m_hScanner )
	{
		apiReturn res	=	_no_scanner;
		
		for (int i = 0; i < 3; i++)
		{
			res = SetReaderMode(m_hScanner, 0x00);
			if ( _OK == res )
			{
				break;
			}
		}
		
		
		////为何种方式通信(0--TCP/IP, 1--RS232,2--RS485, 3--USB)
		switch ( ConnectMode )
		{
		case 0://RS232
			//断开读写器
			DisconnectScanner(m_hScanner);
			break;
		case 3://USB
			//断开读写器
			VH_DisconnectScannerUsb(m_hScanner);
			break;
			
		}
		
	}
	
	
	
}
