// Connect.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "Connect.h"
#include "WriteCardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HWND	gConnectReaderHwnd	=	NULL;	//连接的窗体的句柄
BOOL    nThreadFlag			=	FALSE;







//////////////////////////////////////////////////////////////////////////
//用途:		取得系统的COM口
//函数名:	VHGetSystemPorts
//功能:		
//输入参数:	CComboBox 指针
//输出参数:	
//返回值:	无
//备注:		
//////////////////////////////////////////////////////////////////////////
void _stdcall VHGetSystemPorts(CComboBox *tempCombBox)
{    
    CRegKey RegKey;  
    int nCount = 0;       
    CString tempCStr;
	long i;
	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	
	i=RegKey.Open(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),KEY_READ );
	
	
	//打开HKEY_CURRENT_USER\"Software"
	//i	=	RegOpenKeyEx(HKEY_CURRENT_USER, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_WRITE|KEY_READ,
	//	&hSoftKey);
	
	
    if(i == ERROR_SUCCESS)    
    {    
        while(true)
        {    
            char ValueName[_MAX_PATH];
            unsigned char ValueData[_MAX_PATH];
            DWORD nValueSize = _MAX_PATH;
            DWORD nDataSize = _MAX_PATH;
            DWORD nType;
			
            if(::RegEnumValue(HKEY(RegKey), nCount, ValueName, &nValueSize, NULL, &nType, ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS)    
            {    
                break;
            }
			if(strlen((char*)ValueData))
			{
				tempCStr.Format(_T("%s"),ValueData);
				tempCombBox->AddString(tempCStr);
			}
			nCount++;
        }
    }    
    
	RegKey.Close();
}


//按照特定的排序，例如将COM1,COM11,COM2,COM23,COM15排成如下：
//1:COM1,COM2,COM11,COM15,COM23,或许有人排成下面这：
//2:COM1,COM11,COM15,COM2,COM23,
//注意本处是第一种
int zzcmp(const void *a, const void *b)
{
	int ia	=	0;
	int ib	=	0;
	char *x	=	(char *)((char*)a+3);
	char *y	=	(char *)((char*)b+3);
	
	ia	=	atoi(x);
	ib	=	atoi(y);
	//Asc2Bcd((char*)&ia, x, 2);
	//Asc2Bcd((char*)&ib, y, 2);
    //return strcmp(x, y);
	
	return ia >= ib ? 1 : -1;
}

//////////////////////////////////////////////////////////////////////////
//用途:		取得系统的COM口
//函数名:	VH75GetSystemPorts
//功能:		
//输入参数:	char ** tempCombBox为字符串二级指针
//输出参数:	
//返回值:	返回个数
//备注:		
//////////////////////////////////////////////////////////////////////////
int _stdcall VH75GetSystemPorts(char **tempCombBox)
{    
    //CRegKey RegKey;  
    int nCount = 0;
	long i;
	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	char chCCom[MAX_PATH][16]	=	{0};
	int k =0;
	
	//i=RegKey.Open(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),KEY_READ );
	
	
	//打开HKEY_LOCAL_MACHINE\\DEVICEMAP\\SERIALCOMM"
	i	=	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hSoftKey);
	
    if(i == ERROR_SUCCESS)    
    {    
        while(TRUE)    
        {    
            char ValueName[_MAX_PATH];
            unsigned char ValueData[_MAX_PATH];
            DWORD nValueSize = _MAX_PATH;
            DWORD nDataSize = _MAX_PATH;
            DWORD nType;
			
            if(RegEnumValue(hSoftKey, nCount, ValueName, &nValueSize, NULL, &nType, ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS)    
            {    
                break;    
            }
			if(strlen((char*)ValueData))
			{
				//tempCStr.Format(_T("%s"),ValueData);
				//tempCombBox->AddString(tempCStr);
				strcpy((char*)&chCCom[k], (char*)ValueData);
				k++;
			}
			nCount++;
        }
		
		qsort(chCCom,k,16*sizeof(char),zzcmp);
		if ( NULL != tempCombBox )
		{
			for ( i = 0; i < k; i++)
			{
				strcpy((char*)tempCombBox[i], (char*)&chCCom[i]);
			}
		}
		
		
    }
	
	RegCloseKey(hSoftKey);
	
	return k;
}


/////////////////////////////////////////////////////////////////////////////
// CConnect property page

IMPLEMENT_DYNCREATE(CConnect, CPropertyPage)

CConnect::CConnect() : CPropertyPage(CConnect::IDD)
{
	//{{AFX_DATA_INIT(CConnect)
		// NOTE: the ClassWizard will add member initialization here
	m_ConnectMode = 1;//0-rs232, 1-USB
	m_LanguageIndex	=	0;//0-English,1-chinese
	//}}AFX_DATA_INIT
}

CConnect::~CConnect()
{
}

void CConnect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnect)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Radio(pDX, IDC_RADIO_ConnectRS232, m_ConnectMode);
	DDX_Radio(pDX, IDC_RADIO_CONEnglish, m_LanguageIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnect, CPropertyPage)
	//{{AFX_MSG_MAP(CConnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_CONEnglish, OnRADIOCONEnglish)
	ON_BN_CLICKED(IDC_RADIO_CONChinese, OnRADIOCONChinese)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTDIS, OnButtonConnectdis)
	ON_BN_CLICKED(IDC_RADIO_ConnectRS232, OnRADIOConnectRS232)
	ON_BN_CLICKED(IDC_RADIO_ConnectUSB, OnRADIOCONUsb)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CONNECTDATACUSTOM, OnConnectdatacustom)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnect message handlers

void CConnect::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_B0001"));//读写器连接
	
	GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText(theApp.g_LoadString("IDWQF_B0002"));//连接读写器
	GetDlgItem(IDC_STATIC_ConnectMode)->SetWindowText(theApp.g_LoadString("IDWQF_B0004"));//连接模式
	GetDlgItem(IDC_RADIO_ConnectRS232)->SetWindowText(theApp.g_LoadString("IDWQF_B0005"));//RS232
	GetDlgItem(IDC_BUTTON_CONNECTDIS)->SetWindowText(theApp.g_LoadString("IDWQF_B0003"));//断开连接
	GetDlgItem(IDC_STATIC_CONLanguage)->SetWindowText(theApp.g_LoadString("IDWQF_B0007"));//语言
	GetDlgItem(IDC_RADIO_CONEnglish)->SetWindowText(theApp.g_LoadString("IDWQF_B0008"));//英文
	GetDlgItem(IDC_RADIO_CONChinese)->SetWindowText(theApp.g_LoadString("IDWQF_B0009"));//简体中文
	
	
}

void CConnect::UpdateStatus()//控制使能与不使能
{
	UpdateData(TRUE);
	
 	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!ifConnectReader);
 	GetDlgItem(IDC_BUTTON_CONNECTDIS)->EnableWindow(ifConnectReader);

	GetDlgItem(IDC_RADIO_ConnectRS232)->EnableWindow(!ifConnectReader);
	GetDlgItem(IDC_COMBO1)->EnableWindow(!ifConnectReader);
	
 	
 	if ( 1 == m_ConnectMode )
 	{
 		//如果USB选中
 		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
 	}
	
	UpdateData(FALSE);
}

BOOL CConnect::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	//////////////////////////////////////////////////////////////////////////
	//UpdateStatus();//控制使能与不使能
	::PostMessage(gConnectReaderHwnd,WM_CONNECTDATACUSTOM,2,0);	//convert language

	return CPropertyPage::OnSetActive();
}

BOOL CConnect::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

BOOL CConnect::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_STATIC_ConnectMode)->ShowWindow(SW_HIDE);//
	GetDlgItem(IDC_RADIO_ConnectUSB)->ShowWindow(SW_HIDE);//USB
	GetDlgItem(IDC_RADIO_ConnectRS232)->ShowWindow(SW_HIDE);//RS232

	//如果要要界面上显示语言，则不要编译下面,如果不选择语言，则编译下面的
	#if 0
	GetDlgItem(IDC_STATIC_CONLanguage)->ShowWindow(SW_HIDE);//
	GetDlgItem(IDC_RADIO_CONEnglish)->ShowWindow(SW_HIDE);//
	GetDlgItem(IDC_RADIO_CONChinese)->ShowWindow(SW_HIDE);//
	#endif

	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);//
	GetDlgItem(IDC_BUTTON_CONNECTDIS)->EnableWindow(FALSE);//

	//为了调试用，工程师在这么弄的，客户拿到代码后，只要计算名不是mashui/maquan,则只显示USB口
	//是否是我的电脑名为"mashui",是则为真,否则为假
	if ( g_ThsFlagMe )
	{
		GetDlgItem(IDC_STATIC_ConnectMode)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_ConnectRS232)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_ConnectUSB)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_CONRS232)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SerialPort)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMBO1)->ShowWindow(SW_SHOW);


		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_PROGRESS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PROGRESS1)->ShowWindow(SW_SHOW);


	}
	else
	{
		GetDlgItem(IDC_STATIC_ConnectMode)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_ConnectRS232)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_ConnectUSB)->ShowWindow(SW_HIDE);
		
		GetDlgItem(IDC_STATIC_CONRS232)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SerialPort)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO1)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PROGRESS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROGRESS1)->ShowWindow(SW_HIDE);

	}


	//	CComboBox *m_SerialPort	=	(CComboBox*)GetDlgItem(IDC_COMBO1);
	// 	m_SerialPort->AddString("COM1");
	// 	m_SerialPort->AddString("COM2");
	// 	m_SerialPort->AddString("COM3");
	// 	m_SerialPort->AddString("COM4");
	// 	m_SerialPort->AddString("COM5");
	// 	m_SerialPort->AddString("COM6");
	// 	m_SerialPort->AddString("COM7");
	// 	m_SerialPort->AddString("COM8");
	// 	m_SerialPort->AddString("COM9");
	// 	m_SerialPort->AddString("COM10");
	// 	m_SerialPort->AddString("COM11");
	// 	m_SerialPort->AddString("COM12");
	//////////////////////////////////////////////////////////////////////////
	//取系统有的COM口
	CComboBox *m_SerialPort	=	(CComboBox*)GetDlgItem(IDC_COMBO1);
	//m_SerialPort->AddString("COM1");
	//VHGetSystemPorts(m_SerialPort);
	
	int		chi		=	0;
	char **chComm	=	NULL;
	//如何动态申请二维数组
	chComm = (char**)malloc(sizeof(char*) * MAX_PATH);
	for (chi = 0; chi < MAX_PATH; chi++)
	{
		chComm[chi]	=	(char*)malloc(sizeof(char) * 16);
	}
				
	short nCurSelected	=	(short)VH75GetSystemPorts(chComm);
	for (chi = 0; chi < nCurSelected; chi++)
	{
		m_SerialPort->AddString(chComm[chi]);
		
	}
	if ( nCurSelected > 0 )
	{
		m_SerialPort->SetCurSel(0);
	}
	
	
	for (chi = 0; chi < MAX_PATH; chi++)
	{
		free(chComm[chi]);
		chComm[chi]	=	NULL;
	}
	free(chComm);
	chComm	=	NULL;


	#if 1
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	gConnectReaderHwnd	=	this->GetSafeHwnd();	//连接的窗体的句柄
	
	
	ConnectMode		=	1;
	m_LanguageIndex	=	theApp.iLanguage;
	UpdateData(FALSE);
	
	
	//////////////////////////////////////////////////////////////////////////
	UpdateStatus();//控制使能与不使能
	
	//主界面
	Wq_WriteAppLogFile(__FILE__, __LINE__, "Connect...\r\n" );
	//::PostMessage(gMainHwnd,WM_LISTUSERDATA,2,0);	//界面和文本更新
	//连接界面
	
	#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnect::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	//关了时间
	::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP+1);
	
}

void CConnect::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	
	//连接读写器
	UpdateData(TRUE);
	
	apiReturn res	=	_no_scanner;
	int i = 0;
	char port[MAX_PATH]={0},IPAddr[MAX_PATH]={0};
	CComboBox *m_SerialPort	=	(CComboBox*)GetDlgItem(IDC_COMBO1);
	m_SerialPort->GetLBText(m_SerialPort->GetCurSel(),IPAddr);
	sprintf(port,"\\\\.\\%s",IPAddr);

	//输入参数:	lOpt 为何种方式通信(0--TCP/IP, 1--RS232,2--RS485, 3--USB)
	long lopt = 0;

	Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnect1\r\n" );
#if 1
	switch ( m_ConnectMode )
	{
		case 0://RS232
			lopt	=	1;//RS232
			Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnectaaa\r\n" );
			res=ConnectScanner(&m_hScanner,port,0);
			Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnectbbb\r\n" );
			break;
		case 1://USB
			lopt	=	3;//USB
			Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnectxxx\r\n" );
			res=VH_ConnectScannerUsb(&m_hScanner);
			Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnectyyy\r\n" );
			break;
		default:
			Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnectzzz\r\n" );
	}
	ConnectMode = lopt;
	Wq_WriteAppLogFile(__FILE__, __LINE__, "OnButtonConnect2\r\n" );

	if (res==_OK)
	{
		//将通信方式存起来, 后来改的可以不用
		//VH_SetCommunicationOpt(lopt, (void*)&m_hScanner);
		//Sleep(1000);
// 		for (i = 0; i < 3; i++)
// 		{
// 			//先停止自动
// 			res=AutoMode(m_hScanner, 0x00);
// 			if ( _OK == res )
// 			{
// 				break;
// 			}
// 		}
// 		if (res!=_OK)
// 		{
// 			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
// 			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
// 			OnButtonConnectdis();
// 			return;
// 		}
		

		for (i = 0; i < 3; i++)
		{
			//读版本
			res=VH_GetVersion(m_hScanner,&m_HardVersion,&m_SoftVersion);
			if ( _OK == res )
			{
				break;
			}
		}
		if (res!=_OK)
		{
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
			OnButtonConnectdis();
			return;
		}

		for (i = 0; i < 3; i++)
		{
			//读基本参数
			res=ReadBasicParam(m_hScanner,gBasicParam);
			if ( _OK == res )
			{
				break;
			}
		}
		if (res!=_OK)
		{
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
			OnButtonConnectdis();
			return;
		}

		for (i = 0; i < 3; i++)
		{
			//国家频率
			res=GetFrequencyRange(m_hScanner, &gFre);
			if ( _OK == res )
			{
				break;
			}
		}
		if (res!=_OK)
		{
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
			OnButtonConnectdis();
			return;
		}

		for (i = 0; i < 3; i++)
		{
			//读auto参数
			res=ReadAutoParam(m_hScanner,gAutoParam);
			if ( _OK == res )
			{
				break;
			}
		}
		if (res!=_OK)
		{
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
			OnButtonConnectdis();
			return;
		}

		for (i = 0; i < 3; i++)
		{
			//读ID
			res=GetReaderID(m_hScanner,gReaderID);
			if ( _OK == res )
			{
				break;
			}
		}
		if (res!=_OK)
		{
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
			OnButtonConnectdis();
			return;
		}


		//为了调试用，一般不会发生。当串口调试用
		if ( g_ThsFlagMe )
		{
			
		}
		else
		{
			for (i = 0; i < 3; i++)
			{
				//读仿真参数
				res=ReadSimParam(m_hScanner,&Param);
				if ( _OK == res )
				{
					break;
				}
			}
			if (res!=_OK)
			{
				//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
				OnButtonConnectdis();
				return;
			}
		}

		ifConnectReader=1;
		NewConnect=1;

		//连上了，相关变灰
		UpdateStatus();//控制使能与不使能

		//MessageBox("Connect Reader Success!", "info",MB_ICONINFORMATION);
		MessageBox(theApp.g_LoadString("IDWQF_B0013"), theApp.g_LoadString("IDWQF_B0014"), MB_ICONINFORMATION);

		CWriteCardDlg * pMainDlg=(CWriteCardDlg *)pMain;
		if ( NULL != pMainDlg )
		{
			pMainDlg->m_Sheet.SetActivePage(1);
		}
		
		
	}
	else
	{
		//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
		OnButtonConnectdis();
		
	}

#endif
	UpdateData(FALSE);

	return;

}

void CConnect::OnButtonConnectdis() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	switch ( m_ConnectMode )
	{
	case 0://RS232
		//断开读写器
		DisconnectScanner(m_hScanner);
		break;
	case 1:
	case 3://USB
		//断开读写器
		VH_DisconnectScannerUsb(m_hScanner);
		break;
		
	}
	
	m_hScanner	=	0;
	ifConnectReader=0;
	NewConnect=0;
	m_HardVersion=0;
	m_SoftVersion=0;

	//连上了，相关变灰
	UpdateStatus();//控制使能与不使能

	UpdateData(FALSE);
	return;
}


void CConnect::OnRADIOCONEnglish() 
{
	// TODO: Add your control notification handler code here
	BOOL bFlag = FALSE;
	theApp.iLanguage	=	0;
	
	if ( theApp.iLanguage == 0 )	//英语
	{
		
		theApp.MakeResource(theApp.iLanguage, _T(""), &g_lpBuf);
		bFlag	=	TRUE;
	}
	else if ( theApp.iLanguage == 1 ) //中文
	{
		
		theApp.MakeResource(theApp.iLanguage, _T(""), &g_lpBuf);
		bFlag	=	TRUE;
	}
	
	if ( bFlag )
	{
		//当前主界面
		::PostMessage(gMainHwnd,WM_LISTUSERDATA,2,0);	//界面和文本更新
		//连接界面
		::PostMessage(gConnectReaderHwnd,WM_CONNECTDATACUSTOM,2,0);	//convert language
		//参数页
		//::PostMessage(gReaderParamHwnd,WM_READERPARMDATAMSG,2,0);	//convert language
		//蓝牙页
		//::PostMessage(gBlueToothHwnd,WM_READERBLUETOOTHMSG,2,0);	//convert language
		//数据下载页
		//::PostMessage(gDataDownHwnd,WM_DOWNLOADDATAMSG,2,0);	//convert language
		
		
	}
	
	return;

}

void CConnect::OnRADIOCONChinese() 
{
	// TODO: Add your control notification handler code here
	BOOL bFlag = FALSE;
	theApp.iLanguage	=	1;
	
	if ( theApp.iLanguage == 0 )	//英语
	{

		theApp.MakeResource(theApp.iLanguage, _T(""), &g_lpBuf);
		bFlag	=	TRUE;
	}
	else if ( theApp.iLanguage == 1 ) //中文
	{

		theApp.MakeResource(theApp.iLanguage, _T(""), &g_lpBuf);
		bFlag	=	TRUE;
	}
	
	if ( bFlag )
	{
		//当前主界面
		::PostMessage(gMainHwnd,WM_LISTUSERDATA,2,0);	//界面和文本更新
		//连接界面
		::PostMessage(gConnectReaderHwnd,WM_CONNECTDATACUSTOM,2,0);	//convert language
		//参数页
		//::PostMessage(gReaderParamHwnd,WM_READERPARMDATAMSG,2,0);	//convert language
		//蓝牙页
		//::PostMessage(gBlueToothHwnd,WM_READERBLUETOOTHMSG,2,0);	//convert language
		//数据下载页
		//::PostMessage(gDataDownHwnd,WM_DOWNLOADDATAMSG,2,0);	//convert language
		
	}
	
	return;

}

void CConnect::OnRADIOConnectRS232()
{
	UpdateStatus();//控制使能与不使能
}

void CConnect::OnRADIOCONUsb()
{
	UpdateStatus();//控制使能与不使能
}

//////////////////////////////////////////////////////////////////////////
//用途:		本模块的消息静态成员函数,回调函数
//函数名:	CallBack_MyTimeListDisp
//功能:		
//输入参数:	hWnd--handle of CWnd that called SetTimer
//			nMsg--WM_TIMER
//			nIDEvent--timer identification
//			dwTime--system time 
//输出参数:	无
//返回值:	无
//备注:TIME_IDENTIFIER_LISTDISP, INTERVALTIME_LISTDISP
//////////////////////////////////////////////////////////////////////////
void WINAPI CConnect::CallBack_MyTimeListDisp(HWND   hWnd,UINT   nMsg,UINT   nIDEvent,DWORD   dwTime)
{
	//Debug_Printf("r:%s\r\n","hello");
	if ( TIME_IDENTIFIER_LISTDISP == nIDEvent )
	{
		::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 0, 0);
	}
	else if ( (TIME_IDENTIFIER_LISTDISP+1) == nIDEvent )
	{
		::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 5, 0);
	}
}

//WM_CONNECTDATACUSTOM
afx_msg LRESULT CConnect::OnConnectdatacustom(WPARAM wParam, LPARAM lParam)
{
	int iwParam	=	(int)wParam;
	int ilParam	=	(int)lParam;
	
	if ( 0 == iwParam )
	{
		if ( 0 == ilParam )
		{
			apiReturn res	=	_no_scanner;

			if ( iCount > 0 )
			{
				res	=	VH_GetVersion(m_hScanner,&m_HardVersion,&m_SoftVersion);
				if ( _OK == res )
				{
					//连上了
					iCount = 0;
					::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP);
					//关掉进度框
					#if defined (ENABLE_PROGRESSDISPTIP)
					::PostMessage(gProgressHwnd, WM_PROGRESS, 2, 0);
					::PostMessage(gProgressHwnd, WM_PROGRESS, 3, 0);
					#endif

					ifConnectReader=1;
					NewConnect=1;
					
					//连上了，相关变灰
					UpdateStatus();//控制使能与不使能
					
					//MessageBox("Connect Reader Success!", "info",MB_ICONINFORMATION);
					MessageBox(theApp.g_LoadString("IDWQF_B0013"), theApp.g_LoadString("IDWQF_B0014"), MB_ICONINFORMATION);
					
					CWriteCardDlg * pMainDlg=(CWriteCardDlg *)pMain;
					if ( NULL != pMainDlg )
					{
						pMainDlg->m_Sheet.SetActivePage(1);
					}

				}

				#if defined (ENABLE_PROGRESSDISPTIP)
				::PostMessage(gProgressHwnd, WM_PROGRESS, 1, iCount);
				#endif

				iCount--;
			}
			else
			{

				::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP);
				

				#if defined (ENABLE_PROGRESSDISPTIP)
				::PostMessage(gProgressHwnd, WM_PROGRESS, 1, 0);
				#endif

				Sleep(500);
				::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 0, 1);
			}
		}
		else if ( 1 == ilParam )
		{

			Sleep(500);
			#if defined (ENABLE_PROGRESSDISPTIP)
			::PostMessage(gProgressHwnd, WM_PROGRESS, 2, 0);
			::PostMessage(gProgressHwnd, WM_PROGRESS, 3, 0);
			#endif

			//提示没有连接上
			::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 3, 0);

		}
		else if ( 2 == ilParam )
		{
			#if defined (ENABLE_PROGRESSDISPTIP)
			//进度提示的最大值
			::PostMessage(gProgressHwnd, WM_GRESSINIT, icnt, iNum);
			::PostMessage(gProgressHwnd, WM_PROGRESS, 0, iCount);
			#endif
			
			//关了时间
			::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP);
			//开启时间
			::SetTimer(gConnectReaderHwnd,TIME_IDENTIFIER_LISTDISP, INTERVALTIME_LISTDISP, CallBack_MyTimeListDisp);
		}
		else if ( 3 == ilParam )
		{
			nThreadFlag	=	FALSE;
			DWORD dwTmpTheaedID = 0;
			hDispThread	=	CreateThread(NULL,0,ThreadListDispPro,this,0, &dwTmpTheaedID);
			if( hDispThread != NULL )
			{		
				nThreadFlag	=	TRUE;
				icnt	=	0;
				iNum	=	100;
				iCount	=	100;
				//先屏掉此按钮，防止再点
				#if defined (ENABLE_PROGRESSDISPTIP)
				//进度提示的最大值
				::PostMessage(gProgressHwnd, WM_GRESSINIT, icnt, iNum);
				::PostMessage(gProgressHwnd, WM_PROGRESS, 0, iCount);
				#endif
			}
			else
			{
				MessageBox(_T("Create Thread Error!"), _T("Notice"),MB_ICONERROR);
				
			}

		}
	}
	else if ( 1 == iwParam )
	{
		if ( 0 == ilParam )	//非模态弹提示进度框
		{
			
		}
		else if ( 1 == ilParam )	//模态弹提示进度框
		{
			//启动进度条
			#if defined (ENABLE_PROGRESSDISPTIP)
			VH73BProgress dlg;
			dlg.DoModal();
			#endif
		}

	}
	else if ( 2 == iwParam )
	{
		if ( 0 == ilParam )
		{
			OnInitUIText();	//更新语言
		}
		
	}
	else if ( 3 == iwParam )
	{
		if ( 0 == ilParam )
		{
			//提示连接不上
			OnButtonConnectdis();
			//MessageBox("Connect Reader Fail!","Warring",MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_B0011"), theApp.g_LoadString("IDWQF_B0012"), MB_OK | MB_ICONERROR);
		}
	}
	else if ( 4 == iwParam )
	{
		if ( 0 == ilParam )
		{
			UpdateStatus();//控制使能与不使能
		}
		else
		{
			OnButtonConnectdis();// 关闭
		}
	}
	else if ( 5 == iwParam )
	{
		if ( 0 == ilParam )
		{
			//int iPos = ((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->GetPos();
			itPos = 80000;
			if ( 80000 < itPos )
			{
				//关了时间
				::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP+1);
				return 1;
			}
			
			((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetPos(itPos);
			
			CString strTemp;
			int nLower,nUpper;
			float fUpper=0.0;
			((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->GetRange(nLower,nUpper);
			fUpper=(float)nUpper;
			nLower=int((itPos/fUpper)*100);
			strTemp.Format("正在读取数据中请稍候...(%d%s)",nLower,"%");
			
			GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(strTemp);
	

		}
		else
		{
			
		}
	}
	
	return 0;
}


//进度提示
DWORD WINAPI CConnect::ThreadListDispPro(LPVOID pParam)
{
	CConnect* m_pStandard;
	
	m_pStandard=(CConnect*)pParam;
	
	//开始查询
	::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 0, 2);
	
	//启动进度条
	#if defined (ENABLE_PROGRESSDISPTIP)
	::SendMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 1, 1);
	#endif
	
	
	return 0;
	
}



void CConnect::OnButton2() 
{
	// TODO: Add your control notification handler code here
#if 0
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetRange32(0, 40000);
	//((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetStep(100);
	itPos = 0;
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetPos(itPos);

	//关了时间
	::KillTimer(gConnectReaderHwnd, TIME_IDENTIFIER_LISTDISP+1);
	//开启时间
	::SetTimer(gConnectReaderHwnd,TIME_IDENTIFIER_LISTDISP+1, 5, CallBack_MyTimeListDisp);
#else
	HWND hwnd = ::FindWindow(NULL, "软件注册");
	HWND hwnd_edit1 = NULL;
	hwnd_edit1 = ::GetDlgItem(hwnd, 0x00300F0E);
	hwnd_edit1 = ::FindWindowEx(hwnd, NULL, "", "");
	//SetWindowText("1234");
	//hwnd_edit1 = ::FindWindowEx(hwnd, 0, "WindowsForms10.EDIT.app.0.1ca0192_r24_ad1", NULL);//WindowsForms10.EDIT.app.0.1ca0192_r24_ad1
	::SendMessage(hwnd_edit1,WM_SETTEXT ,0 , (LPARAM)"abcd");


#endif
}
