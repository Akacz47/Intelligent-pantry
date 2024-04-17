// WriteISO6C.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "WriteISO6C.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////综合查询界面定时器/////////////////////////////////////
//定义定时器的ID
//时间
#define					TIME_IDENTIFIER_WRITE6C		(500)			//时间ID号
#define					INTERVALTIME____WRITE6C		(1000)			//多长时间执行一次
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
HWND	g6CWriteHwnd	=	NULL;	//读写器6C写窗体句柄
DWORD   g6CWriteTheardID = 0;
BYTE gIDTemp[512]={0};
extern char *big_num_add(char *ans, char *first, char *second);


//////////////////////////////////////////////////////////////////////////
static void PopupMenu()
{
	CPoint point;
	GetCursorPos( & point );
	CMenu menuPopup;
	menuPopup.CreatePopupMenu();
	
	//menuPopup.AppendMenu( MF_STRING, 100, _T("Export"));
	menuPopup.AppendMenu( MF_STRING, 100, theApp.g_LoadString("IDWQF_D0040"));//导入数居
	menuPopup.AppendMenu( MF_STRING, 101, theApp.g_LoadString("IDWQF_D0124"));//导出数据
	
	//menuPopup.AppendMenu( MF_SEPARATOR );
	//menuPopup.AppendMenu( MF_STRING, 101, _T("Exit"));
	
	int nCmd = (int) menuPopup.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd() );
	switch( nCmd )
	{
	case	100:
		//AfxMessageBox(_T("退出"));
		//CMainFrame::OnEditComm ();
		//::SendMessage(FrameHwnd, WM_CLOSE, 0, 0);
		::PostMessage(g6CWriteHwnd,WM_6CWRITEDATAMSG,3,0);	//导入数居
		break;
		//case	101:
		//	::PostMessage(gEcpISO6C,WM_CLOSE,0,0);
		//	break;
	case	101:
		::PostMessage(g6CWriteHwnd,WM_6CWRITEDATAMSG,3,1);	//导出数据
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CWriteISO6C property page

IMPLEMENT_DYNCREATE(CWriteISO6C, CPropertyPage)

CWriteISO6C::CWriteISO6C() : CPropertyPage(CWriteISO6C::IDD)
	, m_BeginData(0)
	, m_Count(1)
	, m_Sum(1)
	, m_DisStr(_T(""))
	, m_Word(0)
	, m_SetEPC(FALSE)
	, m_SetUse(FALSE)
	, m_SetPass(FALSE)
	, m_Time(0)
	, m_Total(0)
	, m_Falsh(0)
	, m_Mod(0)
	, m_True(0)
	, m_False(0)
	, m_End(_T(""))
	, m_Befro(_T(""))
	, m_CBefor(FALSE)
	, m_CEnd(FALSE)
	, m_WriteMode(0)
{
	//{{AFX_DATA_INIT(CWriteISO6C)
	//}}AFX_DATA_INIT

	iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
	strcpy((char*)AccessPassword, "12341234");

	m_ScanExitThreadEvent		=	INVALID_HANDLE_VALUE;
	m_WriteEpcHandle			=	INVALID_HANDLE_VALUE;

	isAscii = false;
}

CWriteISO6C::~CWriteISO6C()
{
}

void CWriteISO6C::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWriteISO6C)
	DDX_Control(pDX, IDC_LIST_INDATA, m_ListInData);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_Count);
	DDX_Text(pDX, IDC_EDIT_SUM, m_Sum);
	DDX_Text(pDX, IDC_EDIT_XXXX, m_DisStr);
	DDX_CBIndex(pDX, IDC_COMBO_WORD, m_Word);
	DDX_Check(pDX, IDC_CHECK_EPC, m_SetEPC);
	DDX_Check(pDX, IDC_CHECK_USE, m_SetUse);
	DDX_Check(pDX, IDC_CHECK_PASS, m_SetPass);
	DDX_Control(pDX, IDC_LIST_OUTDATA, m_ListOutData);
	DDX_Text(pDX, IDC_EDIT_TIME, m_Time);
	DDV_MinMaxUInt(pDX, m_Time, 1, 1000);
	DDX_Control(pDX, IDC_STATIC_PIC, m_MsgImg);
	DDX_Control(pDX, IDC_STATIC_DIS, m_MsgDis);
	DDX_Control(pDX, IDC_STATIC_OUT, m_MsgOut);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_Total);
	DDX_Text(pDX, IDC_EDIT_FALSH, m_Falsh);
	DDX_Text(pDX, IDC_EDIT_MOD, m_Mod);
	DDX_Text(pDX, IDC_EDIT_TRUE, m_True);
	DDX_Text(pDX, IDC_EDIT_FALSE, m_False);
	DDX_Text(pDX, IDC_EDIT_END, m_End);
	DDX_Text(pDX, IDC_EDIT_BEFOR, m_Befro);
	DDX_Check(pDX, IDC_CHECK_BEFOR, m_CBefor);
	DDX_Check(pDX, IDC_CHECK_END, m_CEnd);
	DDX_Control(pDX, IDC_STATIC_IN, m_MsgIn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWriteISO6C, CPropertyPage)
	//{{AFX_MSG_MAP(CWriteISO6C)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_DATA, OnBnClickedButtonData)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_CBN_SELENDOK(IDC_COMBO_WORD, OnCbnSelendokComboWord)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnBnClickedButtonMove)
	ON_CBN_KILLFOCUS(IDC_COMBO_WORD, OnCbnKillfocusComboWord)
	ON_MESSAGE(WM_MSG,WriteMsg)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_DELOUT, OnBnClickedButtonDelout)
	ON_BN_CLICKED(IDC_RADIO_EPC, OnBnClickedRadioEpc)
	ON_BN_CLICKED(IDC_RADIO_USER, OnBnClickedRadioUser)
	ON_BN_CLICKED(IDC_RADIO_PASS, OnBnClickedRadioPass)
	ON_BN_CLICKED(IDC_BUTTON_INDATA, OnBnClickedButtonIndata)
	ON_BN_CLICKED(IDC_CHECK_EPC, OnBnClickedCheckEpc)
	ON_BN_CLICKED(IDC_CHECK_USE, OnBnClickedCheckUse)
	ON_BN_CLICKED(IDC_CHECK_PASS, OnBnClickedCheckPass)
	ON_BN_CLICKED(IDC_BUTTON_SUCCESS, OnBnClickedButtonSuccess)
	ON_BN_CLICKED(IDC_BUTTON_ERROR, OnBnClickedButtonError)
	ON_BN_CLICKED(IDC_CHECK_LOCKEPC_PASS, OnBnClickedCheckLockepcPass)
	ON_BN_CLICKED(IDC_CHECK_LOCKEPC_Neverwriteable, OnBnClickedCheckLockepcNeverwriteable)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_6CWRITEDATAMSG, On6cWritedatamsg)
	ON_BN_CLICKED(IDC_CHECK1, &CWriteISO6C::OnBnClickedCheck1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteISO6C message handlers
void CWriteISO6C::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_D0001"));//写ISO18000-6C类型电子标签
	
	
	GetDlgItem(IDC_STATIC_DATABIT)->SetWindowText(theApp.g_LoadString("IDWQF_D0002"));//数据位数(Word)
	GetDlgItem(IDC_STATIC_XUHAO)->SetWindowText(theApp.g_LoadString("IDWQF_D0003"));//起始序号
	GetDlgItem(IDC_STATIC_BUCHANG)->SetWindowText(theApp.g_LoadString("IDWQF_D0004"));//累加步长
	GetDlgItem(IDC_BUTTON_DATA)->SetWindowText(theApp.g_LoadString("IDWQF_D0005"));//按规则生成数据
	GetDlgItem(IDC_STATIC_MAKENUM)->SetWindowText(theApp.g_LoadString("IDWQF_D0006"));//生成数量
	GetDlgItem(IDC_STATIC_LISTD)->SetWindowText(theApp.g_LoadString("IDWQF_D0007"));//待写入数据列表
	GetDlgItem(IDC_STATIC_CODESET)->SetWindowText(theApp.g_LoadString("IDWQF_D0008"));//编码规则设置
	GetDlgItem(IDC_BUTTON_FILE)->SetWindowText(theApp.g_LoadString("IDWQF_D0009"));//导入数据生成规则
	GetDlgItem(IDC_STATIC_SAVEOPT)->SetWindowText(theApp.g_LoadString("IDWQF_D0010"));//记数器
	GetDlgItem(IDC_STATIC_NUMT)->SetWindowText(theApp.g_LoadString("IDWQF_D0011"));//数据总数
	GetDlgItem(IDC_STATIC_SHENGNUM)->SetWindowText(theApp.g_LoadString("IDWQF_D0012"));//剩余数
	GetDlgItem(IDC_STATIC_SUCCESSNUM)->SetWindowText(theApp.g_LoadString("IDWQF_D0013"));//成功数
	GetDlgItem(IDC_STATIC_FINISHNUM)->SetWindowText(theApp.g_LoadString("IDWQF_D0014"));//完成数
	GetDlgItem(IDC_STATIC_FAILNUM)->SetWindowText(theApp.g_LoadString("IDWQF_D0015"));//失败数
	GetDlgItem(IDC_STATIC_W6CEXAMPLE)->SetWindowText(theApp.g_LoadString("IDWQF_D0016"));//示例
	GetDlgItem(IDC_CHECK_BEFOR)->SetWindowText(theApp.g_LoadString("IDWQF_D0017"));//前缀
	GetDlgItem(IDC_CHECK_END)->SetWindowText(theApp.g_LoadString("IDWQF_D0018"));//后缀
	GetDlgItem(IDC_STATIC_BETT)->SetWindowText(theApp.g_LoadString("IDWQF_D0019"));//间隔时间:秒
	GetDlgItem(IDC_BUTTON_WRITE)->SetWindowText(theApp.g_LoadString("IDWQF_D0020"));//开始写卡
	GetDlgItem(IDC_BUTTON_STOP)->SetWindowText(theApp.g_LoadString("IDWQF_D0021"));//暂停写卡
	GetDlgItem(IDC_BUTTON_MOVE)->SetWindowText(theApp.g_LoadString("IDWQF_D0022"));//移除当前数据
	GetDlgItem(IDC_STATIC_WRITEOP)->SetWindowText(theApp.g_LoadString("IDWQF_D0023"));//写卡操作
	GetDlgItem(IDC_CHECK_USE)->SetWindowText(theApp.g_LoadString("IDWQF_D0024"));//用户区
	GetDlgItem(IDC_CHECK_PASS)->SetWindowText(theApp.g_LoadString("IDWQF_D0025"));//密码区
	GetDlgItem(IDC_STATIC_FINLIST)->SetWindowText(theApp.g_LoadString("IDWQF_D0026"));//完成数据列表
	GetDlgItem(IDC_BUTTON_SUCCESS)->SetWindowText(theApp.g_LoadString("IDWQF_D0027"));//生成成功数据TXT
	GetDlgItem(IDC_BUTTON_ERROR)->SetWindowText(theApp.g_LoadString("IDWQF_D0028"));//生成失败数据TXT
	GetDlgItem(IDC_BUTTON_DELOUT)->SetWindowText(theApp.g_LoadString("IDWQF_D0029"));//清空完成数据列表
	GetDlgItem(IDC_STATIC_DIS)->SetWindowText(theApp.g_LoadString("IDWQF_D0030"));//状态
	GetDlgItem(IDC_STATIC_OUT)->SetWindowText(theApp.g_LoadString("IDWQF_D0031"));//数据
	GetDlgItem(IDC_BUTTON_DELALL)->SetWindowText(theApp.g_LoadString("IDWQF_D0032"));//清空写入数据列表
	GetDlgItem(IDC_STATIC_WWriteDATA)->SetWindowText(theApp.g_LoadString("IDWQF_D0033"));//待写入数据
	GetDlgItem(IDC_STATIC_IN)->SetWindowText(theApp.g_LoadString("IDWQF_D0034"));//数据
	GetDlgItem(IDC_STATIC_WWriteSTAT)->SetWindowText(theApp.g_LoadString("IDWQF_D0035"));//写入状态
	GetDlgItem(IDC_STATIC_FWriteData)->SetWindowText(theApp.g_LoadString("IDWQF_D0036"));//写入数据
	GetDlgItem(IDC_STATIC_STATTIP)->SetWindowText(theApp.g_LoadString("IDWQF_D0037"));//状态提示
	GetDlgItem(IDC_RADIO_USER)->SetWindowText(theApp.g_LoadString("IDWQF_D0038"));//用户区
	GetDlgItem(IDC_RADIO_PASS)->SetWindowText(theApp.g_LoadString("IDWQF_D0039"));//密码区
	GetDlgItem(IDC_BUTTON_INDATA)->SetWindowText(theApp.g_LoadString("IDWQF_D0040"));//导入数据
	GetDlgItem(IDC_CHECK_LOCKEPC_PASS)->SetWindowText(theApp.g_LoadString("IDWQF_D0041"));//使用密码写
	GetDlgItem(IDC_STATIC_ProtectEPCA)->SetWindowText(theApp.g_LoadString("IDWQF_D0042"));//EPC保护
	GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable)->SetWindowText(theApp.g_LoadString("IDWQF_D0043"));//永远不可写
	GetDlgItem(IDC_STATIC_USERPASS)->SetWindowText(theApp.g_LoadString("IDWQF_D0044"));//密码:
	GetDlgItem(IDC_STATIC)->SetWindowText(theApp.g_LoadString("IDWQF_D0129"));//密码:
	GetDlgItem(IDC_STATIC_PASSFR)->SetWindowText(theApp.g_LoadString("IDWQF_D0045"));//密码
	GetDlgItem(IDC_CHECK_LOCKEPC_EAS_ALARM)->SetWindowText(theApp.g_LoadString("IDWQF_D0121"));//EAS报警
	GetDlgItem(IDC_CHECK1)->SetWindowText(theApp.g_LoadString("IDWQF_C0041"));
	
	int nS=m_ListInData.GetItemCount();
	if ( nS == 0 )
	{
		m_ListInData.DeleteColumn(0);
		m_ListInData.DeleteColumn(0);
		m_ListInData.DeleteColumn(0);
		m_ListInData.DeleteColumn(0);
		if(isAscii)
		{
			m_ListInData.DeleteColumn(0);
			switch(m_WriteMode)
			{
				case 0:
					m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0046"), LVCFMT_LEFT, 40 );
					m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0047"), LVCFMT_LEFT, 220 );	
					m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0048"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0049"), LVCFMT_LEFT, 120 );
				break;
				case 1:
					m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0046"), LVCFMT_LEFT, 40 );
					m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0047"), LVCFMT_LEFT, 220 );	
					m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0048"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0049"), LVCFMT_LEFT, 120 );
				break;
				case 2:
					m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0046"), LVCFMT_LEFT, 40 );
					m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0047"), LVCFMT_LEFT, 220 );	
					m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0048"), LVCFMT_LEFT, 220 );
					m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0049"), LVCFMT_LEFT, 120 );
				break;
			}
		}
		else
		{
			m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0046"), LVCFMT_LEFT, 40 );
			m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0047"), LVCFMT_LEFT, 220 );	
			m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0048"), LVCFMT_LEFT, 220 );
			m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0049"), LVCFMT_LEFT, 120 );
		}
		

	}

	nS=m_ListOutData.GetItemCount();
	if ( nS ==  0 )
	{
		m_ListOutData.DeleteColumn(0);
		m_ListOutData.DeleteColumn(0);
		m_ListOutData.DeleteColumn(0);
		m_ListOutData.DeleteColumn(0);
		m_ListOutData.DeleteColumn(0);
		if(isAscii)
		{
			m_ListOutData.DeleteColumn(0);
			switch(m_WriteMode)
			{
				case 0:
					m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
					m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0050"), LVCFMT_LEFT, 40 );
					m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0051"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0052"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0053"), LVCFMT_LEFT, 120 );
					m_ListOutData.InsertColumn( 5, theApp.g_LoadString("IDWQF_D0054"), LVCFMT_LEFT, 80 );
				break;
				case 1:
					m_ListOutData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
					m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0050"), LVCFMT_LEFT, 40 );
					m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0051"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0052"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0053"), LVCFMT_LEFT, 120 );
					m_ListOutData.InsertColumn( 5, theApp.g_LoadString("IDWQF_D0054"), LVCFMT_LEFT, 80 );
				break;
				case 2:
					m_ListOutData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
					m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0050"), LVCFMT_LEFT, 40 );
					m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0051"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0052"), LVCFMT_LEFT, 200 );
					m_ListOutData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0053"), LVCFMT_LEFT, 120 );
					m_ListOutData.InsertColumn( 5, theApp.g_LoadString("IDWQF_D0054"), LVCFMT_LEFT, 80 );
				break;
			}
		}
		else
		{
			m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0050"), LVCFMT_LEFT, 40 );
			m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0051"), LVCFMT_LEFT, 200 );
			m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0052"), LVCFMT_LEFT, 200 );
			m_ListOutData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0053"), LVCFMT_LEFT, 120 );
			m_ListOutData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0054"), LVCFMT_LEFT, 80 );
		}
		
	}
	
}

void CWriteISO6C::UpdateStatus()//控制使能与不使能
{
	UpdateData(TRUE);
	
	// 	GetDlgItem(IDC_RADIO_ConnectRS232)->EnableWindow(!ifConnectReader);
	// 	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!ifConnectReader);
	// 	GetDlgItem(IDC_COMBO1)->EnableWindow(!ifConnectReader);
	// 	GetDlgItem(IDC_BUTTON_CONNECTDIS)->EnableWindow(ifConnectReader);
	// 	
	// 	if ( 1 == m_ConnectMode )
	// 	{
	// 		//如果USB选中
	// 		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
	// 	}
	
	UpdateData(FALSE);
}

BOOL CWriteISO6C::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	g6CWriteHwnd	=	this->GetSafeHwnd();
	
	int i = 0;
	CString strTemp	=	_T("");
	DWORD dwStyle = m_ListInData.GetExtendedStyle(); 
	
    //选中某行使整行高度
    dwStyle |= LVS_EX_FULLROWSELECT;
    dwStyle |= LVS_EX_GRIDLINES;//网格线
	
    m_ListInData.SetExtendedStyle(dwStyle); //设置扩展风格
	
	
	
    //m_ListInData.SetBkColor(RGB(200, 200, 200)); //设置背景颜色
    //m_ListInData.SetTextBkColor(RGB(200, 200, 200)); //设置文本背景颜色
    //m_ListInData.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

	
    //插入列的标题
    //m_ListInData.InsertColumn( 0, _T("序号"), LVCFMT_LEFT, 40 );
    //m_ListInData.InsertColumn( 1, _T("EPC区"), LVCFMT_LEFT, 220 );
    //m_ListInData.InsertColumn( 2, _T("用户区"), LVCFMT_LEFT, 220 );
	//m_ListInData.InsertColumn( 3, _T("密码区"), LVCFMT_LEFT, 120 );
	m_ListInData.DeleteColumn(0);
	m_ListInData.DeleteColumn(0);
	m_ListInData.DeleteColumn(0);
	m_ListInData.DeleteColumn(0);
	m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0046"), LVCFMT_LEFT, 40 );
    m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0047"), LVCFMT_LEFT, 220 );
    m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0048"), LVCFMT_LEFT, 220 );
	m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0049"), LVCFMT_LEFT, 120 );
	
	
	dwStyle = m_ListOutData.GetExtendedStyle(); 
	
    //选中某行使整行高度
    dwStyle |= LVS_EX_FULLROWSELECT;
    dwStyle |= LVS_EX_GRIDLINES;//网格线
	
    m_ListOutData.SetExtendedStyle(dwStyle); //设置扩展风格
	
	
	
    //m_ListInData.SetBkColor(RGB(200, 200, 200)); //设置背景颜色
    //m_ListInData.SetTextBkColor(RGB(200, 200, 200)); //设置文本背景颜色
    //m_ListInData.SetTextColor(RGB(10, 10, 80)); //设置文本颜色
	
	
    //插入列的标题
    //m_ListOutData.InsertColumn( 0, _T("序号"), LVCFMT_LEFT, 40 );
    //m_ListOutData.InsertColumn( 1, _T("EPC区"), LVCFMT_LEFT, 200 );
    //m_ListOutData.InsertColumn( 2, _T("用户区"), LVCFMT_LEFT, 200 );
	//m_ListOutData.InsertColumn( 3, _T("密码区"), LVCFMT_LEFT, 120 );
	//m_ListOutData.InsertColumn( 4, _T("状态"), LVCFMT_LEFT, 80 );
	m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_D0050"), LVCFMT_LEFT, 40 );
    m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_D0051"), LVCFMT_LEFT, 200 );
    m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_D0052"), LVCFMT_LEFT, 200 );
	m_ListOutData.InsertColumn( 3, theApp.g_LoadString("IDWQF_D0053"), LVCFMT_LEFT, 120 );
	m_ListOutData.InsertColumn( 4, theApp.g_LoadString("IDWQF_D0054"), LVCFMT_LEFT, 80 );

	
	strEpc="0054100B";
	
	stInData.nLog=0; 
	
	m_SetEPC=TRUE;
	
	m_Time=1;
	
	nStop=0;
	
	nWord=0;
	
	((CButton *)GetDlgItem(IDC_RADIO_EPC))->SetCheck(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_USERPASS))->SetLimitText(8);//密码为8个字符
	((CEdit*)GetDlgItem(IDC_EDIT_USERPASS))->SetWindowText(_T("11112222"));//系统默认
	
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetLimitText(8);//密码为8个字符
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetWindowText(_T("00001111"));//系统默认

	//default
	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->EnableWindow(TRUE);
	
	UpdateData(FALSE);
	
	for (i = 0; i < 9; i++)
	{
		strTemp.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->AddString(strTemp);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->SetCurSel(0);


	//测试用
	//int nRow=m_ListOutData.InsertItem(0,_T("2"));
	//m_ListOutData.SetItemText(nRow, 1, _T("ABCD"));
	//m_ListOutData.SetItemText(nRow, 2, _T("ABCE"));
	//m_ListOutData.SetItemText(nRow, 3, _T("ABCF"));
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWriteISO6C::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CPropertyPage::OnCancel();
}

//WM_6CWRITEDATAMSG
LRESULT CWriteISO6C::On6cWritedatamsg(WPARAM wParam, LPARAM lParam)
{
	int iwParam	=	(int)wParam;
	int ilParam	=	(int)lParam;
	
	if ( 2 == iwParam )
	{
		OnInitUIText();	//更新语言
	}
	else if( 0 == iwParam )
	{
		
	}
	else if ( 3 == iwParam )
	{
		if ( 0 == ilParam )
		{
			// 导入
			OnBnClickedButtonIndata();

		}
		else if ( 1 == ilParam )
		{
			// 导出
			OnExportData();
		}
	}
	
	return 0;
}

BOOL CWriteISO6C::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

BOOL CWriteISO6C::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	//6C写页
	::PostMessage(g6CWriteHwnd,WM_6CWRITEDATAMSG,2,0);	//convert language

	return CPropertyPage::OnSetActive();
}

void CWriteISO6C::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}
#if 1
void CWriteISO6C::OnBnClickedButtonData()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CString strTemp;
	CString epcData;
	UINT i=0;
	CString tmp;
	int nBLen;
	int nELen;

	GetDlgItem(IDC_EDIT_BEGIN)->GetWindowText(strTemp);

    if(m_Count<=0)
	{
		//MessageBox(_T("累加步长不能小于等于0"),_T("警告"),MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_D0055"), theApp.g_LoadString("IDWQF_D0056"), MB_ICONWARNING);
		return;
	}

	if(m_Count<=0)
	{
		//MessageBox(_T("数据总数不能小于等于0"),_T("警告"),MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_D0057"), theApp.g_LoadString("IDWQF_D0058"), MB_ICONWARNING);
		return;
	}

	int j;

	j=m_WriteMode+1;

	if(nStatus)
	{
		CString str;
		int ib = 0, is = 0, ie = 0;
		if((m_CBefor)&&(!m_CEnd))
		{
			nBLen=m_Befro.GetLength(); 
			if((nBLen<(isAscii ? 2 : 4))||((nBLen%(isAscii ? 2 : 4))!=0)||(nBLen>=m_Word*(isAscii ? 2 : 4)))
			{
				//MessageBox(_T("前缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0059"), theApp.g_LoadString("IDWQF_D0060"), MB_ICONWARNING);
				return;
			}
			str= isAscii ? m_Befro : m_Befro.SpanIncluding(_T("0123456789ABCDEFabcdef"));
			
			int strLen = isAscii ? m_Befro.GetLength() : str.GetLength();

			if( strLen != nBLen)
			{
				//MessageBox(_T("前缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0061"), theApp.g_LoadString("IDWQF_D0062"), MB_ICONWARNING);
				return;
			}
			
			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			ib = m_Befro.GetLength();
			is = strTemp.GetLength();

			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*(isAscii ? 2 : 4))-nBLen)/(isAscii ? 2 : 4)))
				{
				case 1:
					//epcData.Format(_T("%s%04X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%2s"),m_Befro,strTemp);					
					else
						epcData.Format(_T("%s%04s"),m_Befro,strTemp);
					break;
				case 2:
					//epcData.Format(_T("%s%08X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%4s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%08s"),m_Befro,strTemp);
					break;
				case 3:
					//epcData.Format(_T("%s%012X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%6s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%012s"),m_Befro,strTemp);
					break;
				case 4:
					//epcData.Format(_T("%s%016X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%8s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%016s"),m_Befro,strTemp);
					break;
				case 5:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%10s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%020s"),m_Befro,strTemp);
					break;
				case 6:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%12s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%024s"),m_Befro,strTemp);
					break;
				case 7:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%s%14s"),m_Befro,strTemp);
					else
						epcData.Format(_T("%s%028s"),m_Befro,strTemp);
					break;
				default:
					//MessageBox(_T("前缀数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_D0063"), theApp.g_LoadString("IDWQF_D0064"), MB_ICONWARNING);
					return;
					break;
				}
				nB=nB+m_Count;
				char chixA[255] = {0};
				char chixB[255] = {0};
				char chixC[255] = {0};
				strcpy(chixA, strTemp);
				sprintf(chixB, "%x", m_Count);
				big_num_add(chixC, chixA, chixB);
				strTemp = chixC;

				if(!isAscii)
					epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 				

				if(isAscii)
				{
					byte str1[256] = { 0 };
					
					HexToStr(str1, (byte*)epcData.GetBuffer(), epcData.GetLength());
					if((epcData.GetLength() - ib - is) * 2 > 0)
					{
						for(int ii = 0; ii < (epcData.GetLength() - ib - is)*2; ii++)
							str1[ ib*2 + ii] = '0';
					}
					m_ListInData.SetItemText(nRow, j, (char*)str1);

					m_ListInData.SetItemText(nRow, j + 1, epcData);
					
				}
				else
				{
					m_ListInData.SetItemText(nRow, j, epcData);
				}
			}
		}

		if((m_CEnd)&&(!m_CBefor))
		{
			nELen=m_End.GetLength(); 
			if((nELen<(isAscii ? 2 : 4))||((nELen%(isAscii ? 2 : 4))!=0)||(nELen>=m_Word*(isAscii ? 2 : 4)))
			{
				//MessageBox(_T("后缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0065"), theApp.g_LoadString("IDWQF_D0066"), MB_ICONWARNING);
				return;
			}
			str= isAscii ? m_End : m_End.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=nELen)
			{
				//MessageBox(_T("后缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0067"), theApp.g_LoadString("IDWQF_D0068"), MB_ICONWARNING);
				return;
			}

			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			int is = strTemp.GetLength();
			int ie = m_End.GetLength();

			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*(isAscii ? 2 : 4))-nELen)/(isAscii ? 2 : 4)))
				{
				case 1:
					if(isAscii)
						epcData.Format(_T("%2s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%04s%s"),strTemp,m_End);
					break;
				case 2:
					//epcData.Format(_T("%s%08X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%4s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%08s%s"),strTemp,m_End);
					break;
				case 3:
					//epcData.Format(_T("%s%012X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%6s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%012s%s"),strTemp,m_End);
					break;
				case 4:
					//epcData.Format(_T("%s%016X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%8s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%016s%s"),strTemp,m_End);
					break;
				case 5:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%10s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%020s%s"),strTemp,m_End);
					break;
				case 6:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%12s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%024s%s"),strTemp,m_End);
					break;
				case 7:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					if(isAscii)
						epcData.Format(_T("%14s%s"),strTemp,m_End);
					else
						epcData.Format(_T("%028s%s"),strTemp,m_End);
					break;
				default:
					//MessageBox(_T("后数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_D0069"), theApp.g_LoadString("IDWQF_D0070"), MB_ICONWARNING);
					return;
					break;
				}
				nB=nB+m_Count;
				char chixA[255] = {0};
				char chixB[255] = {0};
				char chixC[255] = {0};
				strcpy(chixA, strTemp);
				sprintf(chixB, "%x", m_Count);
				big_num_add(chixC, chixA, chixB);
				strTemp = chixC;

				if(!isAscii)
					epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				if(isAscii)
				{
					byte str1[256] = { 0 };
					
					HexToStr(str1, (byte*)epcData.GetBuffer(), epcData.GetLength());
					if((epcData.GetLength() - ie - is) * 2 > 0)
					{
						for(int ii = 0; ii < (epcData.GetLength() - ie - is)*2; ii++)
							str1[ii] = '0';
					}
					m_ListInData.SetItemText(nRow, j, (char*)str1);

					m_ListInData.SetItemText(nRow, j + 1, epcData);
					
				}
				else
				{
					m_ListInData.SetItemText(nRow, j, epcData);
				}
			}
		}

		if(m_CEnd&&m_CBefor)
		{
			nBLen=m_Befro.GetLength(); 
			if((nBLen<(isAscii ? 2 : 4))||((nBLen%(isAscii ? 2 : 4))!=0)||(nBLen>=m_Word*(isAscii ? 2 : 4)))
			{
				//MessageBox(_T("前缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0071"), theApp.g_LoadString("IDWQF_D0072"), MB_ICONWARNING);
				return;
			}
			str=m_Befro.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if( !isAscii && str.GetLength()!=nBLen)
			{
				//MessageBox(_T("前缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0073"), theApp.g_LoadString("IDWQF_D0074"), MB_ICONWARNING);
				return;
			}

			nELen=m_End.GetLength(); 
			if((nELen<(isAscii ? 2 : 4))||((nELen%(isAscii ? 2 : 4))!=0)||(nELen>=m_Word*(isAscii ? 2 : 4)))
			{
				//MessageBox(_T("后缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0075"), theApp.g_LoadString("IDWQF_D0076"), MB_ICONWARNING);
				return;
			}
			str=m_End.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if( !isAscii && str.GetLength()!=nELen)
			{
				//MessageBox(_T("后缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0077"), theApp.g_LoadString("IDWQF_D0078"), MB_ICONWARNING);
				return;
			}

			if((nBLen+nELen)>=m_Word*(isAscii ? 2 : 4))
			{
				//MessageBox(_T("前缀和后缀数据长度错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_D0079"), theApp.g_LoadString("IDWQF_D0080"), MB_ICONWARNING);
				return;
			}

			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			int ib = m_Befro.GetLength();
			int is = strTemp.GetLength();
			int ie = m_End.GetLength();
			
 			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*(isAscii ? 2 : 4))-nBLen-nELen)/(isAscii ? 2 : 4)))
				{
				case 1:
					//epcData.Format(_T("%s%04X%s"),m_Befro,nB,m_End);					
					if(isAscii)
						epcData.Format(_T("%s%2s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%04s%s"),m_Befro,strTemp,m_End);
					break;
				case 2:
					//epcData.Format(_T("%s%08X%s"),m_Befro,nB,m_End);				
					if(isAscii)
						epcData.Format(_T("%s%4s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%08s%s"),m_Befro,strTemp,m_End);
					break;
				case 3:
					//epcData.Format(_T("%s%012X%s"),m_Befro,nB,m_End);					
					if(isAscii)
						epcData.Format(_T("%s%6s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%012s%s"),m_Befro,strTemp,m_End);
					break;
				case 4:
					//epcData.Format(_T("%s%016X%s"),m_Befro,nB,m_End);					
					if(isAscii)
						epcData.Format(_T("%s%8s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%016s%s"),m_Befro,strTemp,m_End);
					break;
				case 5:
					//epcData.Format(_T("%s%020X%s"),m_Befro,nB,m_End);
					if(isAscii)
						epcData.Format(_T("%s%10s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%020s%s"),m_Befro,strTemp,m_End);
					break;
				case 6:
					//epcData.Format(_T("%s%024X%s"),m_Befro,nB,m_End);					
					if(isAscii)
						epcData.Format(_T("%s%12s%s"),m_Befro,strTemp,m_End);
					else
						epcData.Format(_T("%s%024s%s"),m_Befro,strTemp,m_End);
					break;
				default:
					//MessageBox(_T("后数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_D0081"), theApp.g_LoadString("IDWQF_D0082"), MB_ICONWARNING);
					return;
					break;
				}
				nB=nB+m_Count;
				char chixA[255] = {0};
				char chixB[255] = {0};
				char chixC[255] = {0};
				strcpy(chixA, strTemp);
				sprintf(chixB, "%x", m_Count);
				big_num_add(chixC, chixA, chixB);
				strTemp = chixC;
				if(!isAscii)
					epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				if(isAscii)
				{
					byte str1[256] = { 0 };
					
					HexToStr(str1, (byte*)epcData.GetBuffer(), epcData.GetLength());
					if((epcData.GetLength() - ie - is - ib) * 2 > 0)
					{
						for(int ii = 0; ii < (epcData.GetLength() - ie - is - ib)*2; ii++)
							str1[ ib*2 + ii] = '0';
					}
					m_ListInData.SetItemText(nRow, j, (char*)str1);

					m_ListInData.SetItemText(nRow, j + 1, epcData);
					
				}
				else
				{
					m_ListInData.SetItemText(nRow, j, epcData);
				}
			}
		}

		if(!m_CEnd&&!m_CBefor)
		{
			m_ListInData.DeleteAllItems();

			int nB=m_BeginData;
			GetDlgItem(IDC_EDIT_BEGIN)->GetWindowText(strTemp);

			int is = strTemp.GetLength();

			for(i=0;i<m_Sum;i++)
			{
				switch(m_Word)
				{
				case 1:
					//epcData.Format(_T("%04X"),nB);
					if(isAscii)
						epcData.Format(_T("%2s"),strTemp);
					else
						epcData.Format(_T("%04s"),strTemp);
					break;
				case 2:
					//epcData.Format(_T("%08X"),nB);
					if(isAscii)
						epcData.Format(_T("%4s"),strTemp);
					else
						epcData.Format(_T("%08s"),strTemp);
					break;
				case 3:
					//epcData.Format(_T("%012X"),nB);
					if(isAscii)
						epcData.Format(_T("%6s"),strTemp);
					else
						epcData.Format(_T("%012s"),strTemp);
					break;
				case 4:
					//epcData.Format(_T("%016X"),nB);
					if(isAscii)
						epcData.Format(_T("%8s"),strTemp);
					else
						epcData.Format(_T("%016s"),strTemp);
					break;
				case 5:
					//epcData.Format(_T("%020X"),nB);
					if(isAscii)
						epcData.Format(_T("%10s"),strTemp);
					else
						epcData.Format(_T("%020s"),strTemp);
					break;
				case 6:
					//epcData.Format(_T("%024X"),nB);
					if(isAscii)
						epcData.Format(_T("%12s"),strTemp);
					else
						epcData.Format(_T("%024s"),strTemp);
					break;
				case 7:
					//epcData.Format(_T("%028X"),nB);
					if(isAscii)
						epcData.Format(_T("%14s"),strTemp);
					else
						epcData.Format(_T("%028s"),strTemp);
					break;
				default:
					//epcData.Format(_T("%032X"),nB);
					if(isAscii)
						epcData.Format(_T("%16s"),strTemp);
					else
						epcData.Format(_T("%032s"),strTemp);
					break;
				}
				nB=nB+m_Count;
				char chixA[255] = {0};
				char chixB[255] = {0};
				char chixC[255] = {0};
				strcpy(chixA, strTemp);
				sprintf(chixB, "%x", m_Count);
				big_num_add(chixC, chixA, chixB);
				strTemp = chixC;

				if(!isAscii)
					epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				if(isAscii)
				{
					byte str1[256] = { 0 };
					
					HexToStr(str1, (byte*)epcData.GetBuffer(), epcData.GetLength());
					if((epcData.GetLength() - is) * 2 > 0)
					{
						for(int ii = 0; ii < (epcData.GetLength() - is)*2; ii++)
							str1[ii] = '0';
					}
					m_ListInData.SetItemText(nRow, j, (char*)str1);

					m_ListInData.SetItemText(nRow, j + 1, epcData);
					
				}
				else
				{
					m_ListInData.SetItemText(nRow, j, epcData);
				}
			}
		}
	}
	else
	{
		//由配置文件生成数据列表
		int nB=stStartNo.nBegin;
		int nN=stStartNo.nNumber;
		int nO=stStartNo.nOne;

		m_ListInData.DeleteAllItems();

		for(i=0;i<(UINT)stStartNo.nTotal;i++)
		{
			epcData.Format(_T("%06d000000000000%06d"),nB,nN);
			nB=nB+stStartNo.nCount;
			nN=nN+stStartNo.nCount;
//			if((10000-nB)==0)
//			{
//				nB=0;
//				nO++;
//			}

			epcData.MakeUpper();

			int nS=m_ListInData.GetItemCount(); 

			tmp.Format(_T("%d"),nS+1);

			int nRow=m_ListInData.InsertItem(nS,tmp); 

			m_ListInData.SetItemText(nRow, 1, epcData);
		}

	/*	for(i=0;i<(UINT)stStartNo.nTotal;i++)
		{
			epcData.Format(_T("%04d%04X%s%08d"),nO,nB,strEpc,nN);
			nB=nB+stStartNo.nCount;
			nN=nN+stStartNo.nCount;
			if((10000-nB)==0)
			{
				nB=0;
				nO++;
			}

			epcData.MakeUpper();

			int nS=m_ListInData.GetItemCount(); 

			tmp.Format(_T("%d"),nS+1);

			int nRow=m_ListInData.InsertItem(nS,tmp); 

			m_ListInData.SetItemText(nRow, j, epcData);
		}*/
	}

	m_Total=m_Sum;
	m_Falsh=0;
	m_Mod=m_Sum;
	m_True=0;
	m_False=0;

	UpdateData(FALSE);
}

void CWriteISO6C::OnBnClickedButtonFile()
{
	// TODO: 在此添加控件通知处理程序代码

 	CFileDialog dlg(TRUE,NULL,NULL,OFN_EXPLORER ,_T("TXT files(*.TXT)|*.txt")); 
    if(dlg.DoModal()==IDOK)
	{
		stFileName=dlg.GetPathName();

		CFileFind tempFind; 
        BOOL IsFinded = tempFind.FindFile(stFileName);

		if(!IsFinded)
		{
			//AfxMessageBox(_T("数据文件选择错误!"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_D0083"));
			return;
		}

		stStartNo.nOne    =GetPrivateProfileInt(_T("Data"),_T("One"),-1,stFileName);
		stStartNo.nBegin  =GetPrivateProfileInt(_T("Data"),_T("Begin"),-1,stFileName);
		stStartNo.nCount  =GetPrivateProfileInt(_T("Data"),_T("Count"),-1,stFileName);
		stStartNo.nNumber =GetPrivateProfileInt(_T("Data"),_T("Number"),-1,stFileName);
		stStartNo.nTotal  =GetPrivateProfileInt(_T("Data"),_T("Total"),-1,stFileName);

		if((stStartNo.nCount<=0)||(stStartNo.nOne<0)||(stStartNo.nBegin<0)||(stStartNo.nNumber<0)||(stStartNo.nTotal<=0))
		{
			//AfxMessageBox(_T("数据文件选择错误!"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_D0084"));
			return;
		}

		CString tmp;
		m_BeginData=stStartNo.nBegin;
		m_Count=stStartNo.nCount;
		m_Sum=stStartNo.nTotal;
		tmp.Format(_T("%04d%04x%s%08d"),stStartNo.nOne,stStartNo.nBegin,strEpc,stStartNo.nNumber);
		tmp.MakeUpper(); 
		m_DisStr=tmp;
		m_Word=6;

		nStatus=0;

		GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_BEFOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BEFOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_BEGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SUM)->EnableWindow(FALSE);

		UpdateData(FALSE);

	}

}

void CWriteISO6C::OnCbnSelendokComboWord()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_BEFOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_BEFOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_BEGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_COUNT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SUM)->EnableWindow(TRUE);

	nStatus=1;

	UpdateData(TRUE);
}

//////////////////////////////////////////////////////////////////////////
//用途:		本模块的消息静态成员函数,回调函数
//函数名:	CWriteISO6C
//功能:		
//输入参数:	hWnd--handle of CWnd that called SetTimer
//			nMsg--WM_TIMER
//			nIDEvent--timer identification
//			dwTime--system time 
//输出参数:	无
//返回值:	无
//备注:TIME_IDENTIFIER_WRITE6C, INTERVALTIME____WRITE6C
//////////////////////////////////////////////////////////////////////////
void WINAPI CWriteISO6C::CallBack_MyTimeWrite6C(HWND   hWnd,UINT   nMsg,UINT   nIDEvent,DWORD   dwTime)
{
	//Debug_Printf("r:%s\r\n","hello");
	if ( INTERVALTIME____WRITE6C == nIDEvent )
	{
		//::PostMessage(gConnectReaderHwnd, WM_CONNECTDATACUSTOM, 0, 0);
	}
}

//WM_MSG
DWORD WINAPI ThreadEPC(LPVOID lpParameter)
{
	int t;
	CWriteISO6C *epcDlg = (CWriteISO6C *)lpParameter;

	apiReturn res	=	_no_scanner;
	int i,j,k,nCounter=0,ID_len=0,ID_len_temp=0;
	CString str,str_temp;
	BYTE temp[512];
	BYTE IDBuffer[30*256];

	BYTE mask[512],IDTemp[512];
	int mem,ptr,len,EPC_Word; 
	BYTE AccessPassword[32]={0};

	char p[3];

	BYTE lock	=	0;
	VH_WriteAppLogFile(__FILE__, __LINE__, "ThreadEPC\r\n");

	while(epcDlg->nStop)
	{
		if (WaitForSingleObject(epcDlg->m_ScanExitThreadEvent, 0) == WAIT_OBJECT_0)
		{
			epcDlg->nStop = FALSE;
			break;
		}
		int iCounter=epcDlg->m_ListInData.GetItemCount();
		if(iCounter==0)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG,0,0);//列表数据完成或连接断开
			epcDlg->nStop=0;
			break;
		}

		if(!ifConnectReader)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG,0,1);//读写器连接已断开断开，请重新连接
			epcDlg->nStop=0;
			break;
		}
		
		if(epcDlg->m_SetEPC) //读卡中数据检测是否与上次写入数据相同，避免重复写卡
		{
			//if(epcDlg->stInData.nLog),别人写的程序，这个地方为什么要用这个???
			if(1)
			{
				len=0;	//掩码长度LEN
				ptr=0;	//掩码起始地址addr
				mem=1;	//0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User
				ID_len=0;  
				j=0;
				memset(mask,0,512);
				
				while(epcDlg->nStop)
				{
					Sleep(30);
					switch(ConnectMode)
					{
					case 0:		
						break;
					case 1:
						//读EPC号,nCounter为读的个数，被读的数据放在IDBuffer中
						res=EPC1G2_ReadLabelID(m_hScanner,mem,ptr,len,mask,IDBuffer,&nCounter,RS485Address);	
						break;
					case 3://usb
						//读EPC号,nCounter为读的个数，被读的数据放在IDBuffer中
						res=EPC1G2_ReadLabelID(m_hScanner,mem,ptr,len,mask,IDBuffer,&nCounter,0);
						break;
					}
					if(res==_OK)
					{
						VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ReadLabelID:OK(%d)\r\n", nCounter);
						if(nCounter==1)
						{
							ID_len=(int)IDBuffer[0];  //IDBuffer[0]为Word总数;1word=2Byte

							memcpy(temp,&IDBuffer[1],ID_len*2);

							char chTemp[512]={0};
							Bcd2AscEx(chTemp, (char*)temp, ID_len*2*2);

							VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ReadLabelVl:(%s)\r\n", chTemp);

							//去掉相同ID号，然后写卡，modify by mqs 20121010
							if(memcmp(temp,gIDTemp,ID_len*2)==0)
							{
								//重复写卡则报警
								//关了时间
								//::KillTimer(g6CWriteHwnd, TIME_IDENTIFIER_WRITE6C);
								//开启时间
								//::SetTimer(g6CWriteHwnd, TIME_IDENTIFIER_WRITE6C, INTERVALTIME____WRITE6C, epcDlg->CallBack_MyTimeWrite6C);
								SendMessage(epcDlg->m_hWnd,WM_MSG,3,0);

								//用户自定义报警命令 add by martrin 20140916
								#if defined (ENABLECUSTOMBUZZER)
								//开始报警
								if ( 3 == ConnectMode )
								{
									BuzzerAlarm(m_hScanner, 1);
								}
								
								Sleep(1000);

								//停止报警
								if ( 3 == ConnectMode )
								{
									BuzzerAlarm(m_hScanner, 0);
								}
								#endif

							}
							else
							{
								SendMessage(epcDlg->m_hWnd,WM_MSG,101,0);//等待写卡，请放入卡:
								break;
							}
						}
						else
						{
							SendMessage(epcDlg->m_hWnd,WM_MSG,3,1);//有多张卡存在，请放入一张卡
							Sleep(1000);
						}
					}
					else
					{
						j++;
						t=res*1000+j;
						SendMessage(epcDlg->m_hWnd,WM_MSG,101,t);//等待写卡，请放入卡:t指数量
						Sleep(500);
					}
				}
			}// end of if(epcDlg->stInData.nLog)
			
			//////////////////////////////////////////////////////////////////////////
			//这部分判断准备写入的数据是否正确
			//取列表数据，并检查数据
			str_temp=epcDlg->m_ListInData.GetItemText(0,1); 

			str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=str_temp.GetLength())
			{
				SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入数据错误
				epcDlg->nStop=0;
				break;
			}
			//////////////////////////////////////////////////////////////////////////



			j=0;
			while(epcDlg->nStop) //写入数据的循环
			{
				if (WaitForSingleObject(epcDlg->m_ScanExitThreadEvent, 0) == WAIT_OBJECT_0)
				{
					epcDlg->nStop = FALSE;
					break;
				}
				memset(temp,0,512);
				memset(mask,0,512);
				memset(IDTemp,0,512);
				//设置写入的数据
				//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
				len=str.GetLength();
				memcpy(temp, str, len);

				len=str.GetLength()/4;
				k=0;
				for(i=0;i<(len*2);i++)
				{
					memset(p,0,3);
					memcpy(p,&temp[k],2);
					k=k+2;
					mask[i]=(BYTE)strtol(&p[0],NULL,16);
				}
				memcpy(IDTemp,mask,len*2);//IDTemp为准备写的数据
				memset(AccessPassword,0,4);

				Sleep(30);
				switch(ConnectMode)
				{
				case 0:
	//				res = Net_EPC1G2_WriteEPC(m_hSocket,len,&mask[0],AccessPassword);		
					break;
				case 1:
					res = EPC1G2_WriteEPC(m_hScanner,len,&mask[0],AccessPassword,RS485Address);		
					break;
				case 3://usb
					res = EPC1G2_WriteEPC(m_hScanner,len,&mask[0],AccessPassword,0);		
					break;
				}

				if(res==_OK) //写完后立即读取，以便检测写入数据是否正确
				{
					memcpy(gIDTemp, mask, 512);
					char chTemp[512]={0};
					Bcd2AscEx(chTemp, (char*)mask, len*2);
					
					VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_WriteEPC:OK(%s)\r\n", chTemp);

					len=0;	
					ptr=0;	
					mem=1;
					memset(mask,0,512);

					for (i = 0; i < 3; i++)
					{	
						Sleep(30);
						switch(ConnectMode)
						{
						case 0:
							break;
						case 1:
							res=EPC1G2_ReadLabelID(m_hScanner,mem,ptr,len,mask,IDBuffer,&nCounter,RS485Address);
							break;
						case 3://usb
							res=EPC1G2_ReadLabelID(m_hScanner,mem,ptr,len,mask,IDBuffer,&nCounter,0);
							break;
						}
						if ( _OK == res )
						{
							break;
						}
					}
					ID_len=(int)IDBuffer[0];  //IDBuffer[0]为Word总数;1word=2Byte

					memcpy(temp,&IDBuffer[1],ID_len*2);//temp为读到的数据，

					if(memcmp(temp,IDTemp,ID_len*2)==0)//与先前的IDTemp准备写的数据 作个 比较
					{
						//到这里来，说明写成功了
						char chTemp[512]={0};
						Bcd2AscEx(chTemp, (char*)temp, ID_len*2);

						VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ReadLabelIE:OK(%s)\r\n", chTemp);

						//////////////////////////////////////////////////////////////////////////
						if(epcDlg->m_SetUse)//写入用户区数据
						{
							//取列表数据，并检查数据
							str_temp=epcDlg->m_ListInData.GetItemText(0,2); 

							str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

							if(str.GetLength()!=str_temp.GetLength())
							{
								SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入数据错误
								epcDlg->nStop=0;
								break;
							}

							memset(temp,0,512);	memset(mask,0,512);	

							//设置写入的数据
							//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
							len=str.GetLength();
							memcpy(temp, str, len);

							len=str.GetLength()/4;
							k=0;
							for(i=0;i<(len*2);i++)
							{
								memset(p,0,3);
								memcpy(p,&temp[k],2);
								k=k+2;
								mask[i]=(BYTE)strtol(&p[0],NULL,16);
							}

							EPC_Word=ID_len;  mem=3; ptr=0; j=0;
							memset(AccessPassword,0,4);

							while(epcDlg->nStop)
							{
								Sleep(30);
								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
									break;
								case 3://usb
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,0);
									break;
								}

								if(res==_OK)
								{
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,103,t);//用户区数据写入失败，请放入卡:
									Sleep(1000);
								}
							}
						}

						if(epcDlg->m_SetPass)//写入密码区数据
						{
							//取列表数据，并检查数据
							str_temp=epcDlg->m_ListInData.GetItemText(0,3); 

							str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

							if(str.GetLength()!=str_temp.GetLength())
							{
								SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入数据错误
								epcDlg->nStop=0;
								break;
							}

							memset(temp,0,512);	memset(mask,0,512);	

							//设置写入的数据
							//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
							len=str.GetLength();
							memcpy(temp, str, len);

							len=str.GetLength()/4;
							k=0;
							for(i=0;i<(len*2);i++)
							{
								memset(p,0,3);
								memcpy(p,&temp[k],2);
								k=k+2;
								mask[i]=(BYTE)strtol(&p[0],NULL,16);
							}

							EPC_Word=ID_len;  mem=0; ptr=0; j=0;
							memset(AccessPassword,0,4);

							while(epcDlg->nStop)
							{
								Sleep(30);
								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
									break;
								case 3://usb
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,0);
									break;
								}

								if(res==_OK)
								{
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,104,t);//密码区数据写入失败，请放入卡:%d-%d
									Sleep(1000);
								}
							}
						}


						//////////////////////////////////////////////////////////////////////////
						//add by mqs 20130409 增加EPC保护功能:
						//1.使用密码写保护(将来有一天要还原，可以找回)
						//2.永远不可写，这种作法跟死了没啥区别，相当于挂了一样，只能以后换标签。
						//iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
						int iSuccess = 0 ;//成功
						if ( epcDlg->iFlagProtectOpt == 1 || 2 == epcDlg->iFlagProtectOpt )
						{
							//1---使用密码锁
							len=4;	//读取数据的长度
							ptr=0;	//
							mem=0;	//0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User
							EPC_Word	=	ID_len;//为EPC的长度1
							
							//////////////////////////////////////////////////////////////////////////
							//第一步先读出原密码区
							while(epcDlg->nStop)
							{
								
								memset(mask,0,512);
								Sleep(30);

								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									//EPC_Word 为EPC的长度
									//IDTemp 传入的EPC号
									//mem 0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User, 这里为0
									//ptr 读取数据的起始地址, len 读取数据的长度
									//读到的东东放在这里 mask, 
									//AccessPassword 原访问密码
									res=EPC1G2_ReadWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
									break;
								}

								if(res==_OK)
								{
									//读到了原密码
									char chTemp[512]={0};
									Bcd2AscEx(chTemp, (char*)mask, len*2);
									
									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ReadWordBlock:ReadPassword(%s)\r\n", chTemp);
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,105,t);//读原密码失败，请放入卡，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////


							//////////////////////////////////////////////////////////////////////////
							//第2步先写新密码，用界面上的密码值epcDlg->AccessPassword
							while(epcDlg->nStop)
							{
								len=4;	//读取数据的长度
								ptr=0;	//掩码起始地址addr
								mem=0;	//0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User
								EPC_Word	=	ID_len;//为EPC的长度
								
								memcpy(mask, epcDlg->AccessPassword, len);
								memcpy(mask+4, epcDlg->AccessPassword + 4, len);

								Sleep(30);

								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									//EPC_Word 为EPC的长度
									//IDTemp 传入的EPC号
									//mem 0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User, 这里为0
									//ptr 如果是密码=0, len =4
									//这里是传的密码值 mask, 
									//AccessPassword 原访问密码
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
									break;
								}

								if(res==_OK)
								{
									char chTemp[512]={0};
									Bcd2AscEx(chTemp, (char*)mask, len*2);

									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ReadWordBlock:New Password(%s)\r\n", chTemp);
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,106,t);//写新密码失败，请放入卡，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////


							//////////////////////////////////////////////////////////////////////////
							//第3步锁住EPC，用使用密码写来锁住EPC,注意传入的新密码
							while(epcDlg->nStop)
							{
								EPC_Word	=	ID_len;//为EPC的长度
								ptr=0;	//掩码起始地址addr
								mem=2;	//0-- Kill Password 1--Access Password 2-- EPC号 3-- TID标签ID号 4--用户区User

								//0--可写         1--永久可写
								//2—带密码写     3--永不可写
								//4--可读写       5--永久可读写
								//6--带密码读写   7--永不可读写
								//0～3只适用EPC、TID和User三个数据区；4～7只适用Kill Password和Access Password。
								if ( epcDlg->iFlagProtectOpt == 1 )////0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
								{
									lock	=	2;//2—带密码写
								}
								else if ( 2 == epcDlg->iFlagProtectOpt )
								{
									lock	=	3;//3-永不可写
								}
								
								

								memcpy(AccessPassword, mask, 8);
								
								Sleep(30);

								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									res=EPC1G2_SetLock(m_hScanner,EPC_Word,IDTemp,mem,lock,AccessPassword + 4,RS485Address);
									break;
								}

								if(res==_OK)
								{
									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_SetLock:EPC Lock(%d)\r\n", lock);
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,107,t);//锁EPC数据写入失败，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////



							//////////////////////////////////////////////////////////////////////////
							//第4步要进行密码保护，使用密码可读写来保护
							while(epcDlg->nStop)
							{
								EPC_Word	=	ID_len;//为EPC的长度
								ptr=0;	//掩码起始地址addr
								mem=1;	//0-- Kill Password 1--Access Password 2-- EPC号 3-- TID标签ID号 4--用户区User

								//0--可写         1--永久可写
								//2—带密码写     3--永不可写
								//4--可读写       5--永久可读写
								//6--带密码读写   7--永不可读写
								//0～3只适用EPC、TID和User三个数据区；4～7只适用Kill Password和Access Password。
								if ( epcDlg->iFlagProtectOpt == 1 )////0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
								{
									lock	=	6;//6—带密码读写
								}
								else if ( 2 == epcDlg->iFlagProtectOpt )
								{
									lock	=	7;//7--永不可读写
								}


								Sleep(30);

								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									//EPC_Word 为EPC的长度
									//IDTemp 传入的EPC号
									//mem 0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User, 这里为0
									//ptr 读取数据的起始地址, len 读取数据的长度
									//读到的东东放在这里 mask, 
									//AccessPassword 原访问密码
									res=EPC1G2_SetLock(m_hScanner,EPC_Word,IDTemp,mem,lock,AccessPassword + 4,RS485Address);
									if(res == _OK)
									{
										mem = 0;
										res=EPC1G2_SetLock(m_hScanner,EPC_Word,IDTemp,mem,lock,AccessPassword + 4,RS485Address);
									}
									break;
								}

								if(res==_OK)
								{
									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_SetLock:PasswordLock(%d)\r\n", lock);
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,108,t);//锁密码失败，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////
							//到止为止才算锁住EPC，要想解开必须知道密码。
							//////////////////////////////////////////////////////////////////////////


						}//end of if ( epcDlg->iFlagProtectOpt == 1 )


						//////////////////////////////////////////////////////////////////////////
						//add by mqs 20140514 增加EAS功能:
						//1.使用密码写保护(将来有一天要还原，可以找回)
						//2.永远不可写，这种作法跟死了没啥区别，相当于挂了一样，只能以后换标签。
						//iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
						//int iSuccess = 0 ;//成功
						if ( epcDlg->iFlagProtectOpt == 3 )
						{
							//////////////////////////////////////////////////////////////////////////
							//对了EAS报警来说，一般两步可以完成
							//第1步先写新密码，用界面上的密码值epcDlg->AccessPassword
							while(epcDlg->nStop)
							{
								len=4;	//读取数据的长度
								ptr=0;	//掩码起始地址addr
								mem=0;	//0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User
								EPC_Word	=	ID_len;//为EPC的长度
								
								memcpy(mask, epcDlg->AccessPassword, len);
								memcpy(mask+4, epcDlg->AccessPassword, len);
								
								Sleep(30);
								
								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									//EPC_Word 为EPC的长度
									//IDTemp 传入的EPC号
									//mem 0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User, 这里为0
									//ptr 如果是密码=0, len =4
									//这里是传的密码值 mask, 
									//AccessPassword 原访问密码
									res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
									break;
								}
								
								if(res==_OK)
								{
									char chTemp[512]={0};
									Bcd2AscEx(chTemp, (char*)mask, len*2*2);
									
									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_WriteWordBlock:New Password(%s)\r\n", chTemp);
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,109,t);//写新密码失败，请放入卡，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////



							//////////////////////////////////////////////////////////////////////////
							//第2步设置EAS报警位
							while(epcDlg->nStop)
							{
								len=4;	//读取数据的长度
								ptr=0;	//掩码起始地址addr
								mem=0;	//0--密码区,1-- EPC号,2-- TID标签ID号,3--用户区User
								EPC_Word	=	ID_len;//为EPC的长度
								
								memcpy(mask, epcDlg->AccessPassword, len);
								memcpy(mask+4, epcDlg->AccessPassword, len);
								
								Sleep(30);
								
								switch(ConnectMode)
								{
								case 0:		
									break;
								case 1:
								case 3://usb
									//EPC_Word 为EPC的长度
									//IDTemp 传入的EPC号
									//EASstate	1个字节	0-不报警 1-报警,这里指1
									//AccessPassword 原访问密码 
									//res=EPC1G2_ChangeEas(m_hScanner,EPC_Word,IDTemp, 1, AccessPassword, RS485Address);
									res=EPC1G2_ChangeEas(m_hScanner,EPC_Word,IDTemp, 1, mask, RS485Address);
									break;
								}
								
								if(res==_OK)
								{
									
									VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_ChangeEas is 1!\r\n");
									break;
								}
								else
								{
									j++;
									t=res*1000+j;
									SendMessage(epcDlg->m_hWnd,WM_MSG,110,t);//写EAS报警位，请放入卡，请放入卡:
									Sleep(300);
								}
							}
							if ( res != _OK )
							{
								iSuccess++;
							}
							//////////////////////////////////////////////////////////////////////////


						}
						//////////////////////////////////////////////////////////////////////////


						//到这里来写成功了
						SendMessage(epcDlg->m_hWnd,WM_MSG,1,iSuccess);//记录当前的写入数据

						//用户自定义报警命令 add by martrin 20140916
						#if defined (ENABLECUSTOMBUZZER)
						//开始报警
						if ( 3 == ConnectMode )
						{
							BuzzerAlarm(m_hScanner, 1);
						}
						
						Sleep(100);
						
						//停止报警
						if ( 3 == ConnectMode )
						{
							BuzzerAlarm(m_hScanner, 0);
						}
						#endif

						break;
					}
					else
					{
						SendMessage(epcDlg->m_hWnd,WM_MSG,1,1);//此卡未写入成功，请重新放入
						Sleep(1000);
					}
				}
				else
				{
					j++;
					t=res*1000+j;
					SendMessage(epcDlg->m_hWnd,WM_MSG,101,t);//等待写卡，请放入卡:%d-%d
					Sleep(100);
				}
			}

	     }//end of if(epcDlg->m_SetEPC)

		 //写入间隔时间
		 for(i=0;i<(int)epcDlg->m_Time;i++)
		 {
			 Sleep(1000);
			 SendMessage(epcDlg->m_hWnd,WM_MSG,2,i);//等待写卡，请放入卡,i指的是时间
		 }

	}

	return 0;
}


//WM_MSG
DWORD WINAPI ThreadEPCTest(LPVOID lpParameter)
{
	int t;
	int iSuccess = 0;
	CWriteISO6C *epcDlg = (CWriteISO6C *)lpParameter;

	apiReturn res	=	_no_scanner;
	int i,j,k,nCounter=0,ID_len=0,ID_len_temp=0;
	CString str,str_temp;
	BYTE temp[512];
	BYTE IDBuffer[30*256]={0};

	BYTE mask[512],IDTemp[512];
	int mem=0,ptr=0,len=0,EPC_Word=0; 
	BYTE AccessPassword[32]={0};

	char p[3];

	BYTE lock	=	0;
	VH_WriteAppLogFile(__FILE__, __LINE__, "ThreadEPC\r\n");

	while(epcDlg->nStop)
	{
		if (WaitForSingleObject(epcDlg->m_ScanExitThreadEvent, 0) == WAIT_OBJECT_0)
		{
			epcDlg->nStop = FALSE;
			break;
		}

		if(!ifConnectReader)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG,0,1);//读写器连接已断开断开，请重新连接
			epcDlg->nStop=0;
			break;
		}
		
		if(epcDlg->m_SetEPC) //读卡中数据检测是否与上次写入数据相同，避免重复写卡
		{
			//////////////////////////////////////////////////////////////////////////
			//这部分判断准备写入的数据是否正确
			//取列表数据，并检查数据
			str_temp=epcDlg->m_ListInData.GetItemText(0,1); 
			
			str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));
			
			if(str.GetLength()!=str_temp.GetLength())
			{
				SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入数据错误
				epcDlg->nStop=0;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			j=0;
			while(epcDlg->nStop) //写入数据的循环
			{
				if (WaitForSingleObject(epcDlg->m_ScanExitThreadEvent, 0) == WAIT_OBJECT_0)
				{
					epcDlg->nStop = FALSE;
					break;
				}
				memset(temp,0,512);
				memset(mask,0,512);
				memset(IDTemp,0,512);
				//设置写入的数据
				//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
				//str="0123456789ABCDEF";
				len=str.GetLength();
				memcpy(temp, str, len);

				len=str.GetLength()/4;
				k=0;
				for(i=0;i<(len*2);i++)
				{
					memset(p,0,3);
					memcpy(p,&temp[k],2);
					k=k+2;
					mask[i]=(BYTE)strtol(&p[0],NULL,16);
				}
				memcpy(IDTemp,mask,len*2);//IDTemp为准备写的数据
				memset(AccessPassword,0,4);

				Sleep(30);
				switch(ConnectMode)
				{
				case 0:
	//				res = Net_EPC1G2_WriteEPC(m_hSocket,len,&mask[0],AccessPassword);		
					break;
				case 1:
					res = EPC1G2_WriteEPC(m_hScanner,len,&mask[0],AccessPassword,RS485Address);		
					break;
				case 3://usb
					res = EPC1G2_WriteEPC(m_hScanner,len,&mask[0],AccessPassword,0);		
					break;
				}

				if(res==_OK) //写完后立即读取，以便检测写入数据是否正确
				{
					memcpy(gIDTemp, mask, 512);
					char chTemp[512]={0};
					Bcd2AscEx(chTemp, (char*)mask, len*2);
					
					VH_WriteAppLogFile(__FILE__, __LINE__, "EPC1G2_WriteEPC:OK(%s)\r\n", chTemp);

					
					//到这里来写成功了
					SendMessage(epcDlg->m_hWnd,WM_MSG,111,++iSuccess);//记录当前的写入数据

					//用户自定义报警命令 add by martrin 20140916
					#if defined (ENABLECUSTOMBUZZER)
					//开始报警
					if ( 3 == ConnectMode )
					{
						BuzzerAlarm(m_hScanner, 1);
					}
					
					Sleep(100);
					
					//停止报警
					if ( 3 == ConnectMode )
					{
						BuzzerAlarm(m_hScanner, 0);
					}
					#endif

					break;
					
				}
				else
				{
					j++;
					t=res*1000+j;
					SendMessage(epcDlg->m_hWnd,WM_MSG,101,t);//等待写卡，请放入卡:%d-%d
					Sleep(100);
				}
			}

	     }//end of if(epcDlg->m_SetEPC)

		 
	}

	return 0;
}

//WM_MSG
DWORD WINAPI ThreadWrite(LPVOID lpParameter)
{
	CWriteISO6C *epcDlg = (CWriteISO6C *)lpParameter;

	int t;
	apiReturn res;
	int i,j,k,nCounter=0,ID_len=0,ID_len_temp=0;
	CString str,str_temp;
	BYTE temp[512];
	BYTE IDBuffer[30*256];
	BYTE mask[512],IDTemp[512];
	int mem,ptr,len,EPC_Word;
	BYTE AccessPassword[4];

	int nRow;
	char p[3];

	while(epcDlg->nStop)
	{
		int iCounter=epcDlg->m_ListInData.GetItemCount();
		if(iCounter==0)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG,0,0);//列表数据完成或连接断开
			epcDlg->nStop=0;
			break;
		}

		if(!ifConnectReader)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG,0,1);//读写器连接已断开断开，请重新连接
			epcDlg->nStop=0;
			break;
		}

		len=0;	ptr=0;	mem=1;	ID_len=0;  j=0;   nRow=0;
		memset(mask,0,512);
		memset(IDTemp,0,512);
		
		Sleep(30);
		switch(ConnectMode)
		{
		case 0:		
			break;
		case 1:
		case 3://usb
			res=EPC1G2_ReadLabelID(m_hScanner,mem,ptr,len,mask,IDBuffer,&nCounter,RS485Address);	
			break;
		}

		if(res==_OK)
		{
			str.Empty(); 
			ID_len=(int)IDBuffer[0];  //IDBuffer[0]为Word总数;1word=2Byte
			memcpy(IDTemp,&IDBuffer[1],ID_len*2);
			for(i=0;i<ID_len*2;i++)
			{
				str_temp.Format(_T("%02X"),IDTemp[i]); 
				str=str+str_temp.Left(2);
			}
			str_temp=epcDlg->m_ListInData.GetItemText(0,1);

			if((str==str_temp)||(str_temp.GetLength()<=0))
			{
				if(epcDlg->m_SetUse)
				{
					//取列表数据，并检查数据
					str_temp=epcDlg->m_ListInData.GetItemText(0,2); 

					str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

					if(str.GetLength()!=str_temp.GetLength())
					{
						SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入数据错误
						epcDlg->nStop=0;
						break;
					}

					memset(temp,0,512);	memset(mask,0,512);	

					//设置写入的数据
					//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
					len=str.GetLength();
					memcpy(temp, str, len);

					len=str.GetLength()/4;
					k=0;
					for(i=0;i<(len*2);i++)
					{
						memset(p,0,3);
						memcpy(p,&temp[k],2);
						k=k+2;
						mask[i]=(BYTE)strtol(&p[0],NULL,16);
					}

					EPC_Word=ID_len;  mem=3; ptr=0; j=0;
					memset(AccessPassword,0,4);

					while(epcDlg->nStop)
					{
						Sleep(30);
						switch(ConnectMode)
						{
						case 0:		
							break;
						case 1:
						case 3://usb
							res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
							break;
						}

						if(res==_OK)
						{
							break;
						}
						else
						{
							j++;
							t=res*1000+j;
							SendMessage(epcDlg->m_hWnd,WM_MSG,103,t);//用户区数据写入失败，请放入卡:%d-%d
							Sleep(1000);
						}
					}
				}

				if(epcDlg->m_SetPass)
				{
					//取列表数据，并检查数据
					str_temp=epcDlg->m_ListInData.GetItemText(0,3); 

					str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

					if(str.GetLength()!=str_temp.GetLength())
					{
						SendMessage(epcDlg->m_hWnd,WM_MSG,100,NULL);//待写入的数据错误
						epcDlg->nStop=0;
						break;
					}

					memset(temp,0,512);	memset(mask,0,512);	

					//设置写入的数据
					//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char *)temp,str.GetLength()*2,NULL,NULL );
					len=str.GetLength();
					memcpy(temp, str, len);

					len=str.GetLength()/4;
					k=0;
					for(i=0;i<(len*2);i++)
					{
						memset(p,0,3);
						memcpy(p,&temp[k],2);
						k=k+2;
						mask[i]=(BYTE)strtol(&p[0],NULL,16);
					}

					EPC_Word=ID_len;  mem=0; ptr=0; j=0;
					memset(AccessPassword,0,4);

					while(epcDlg->nStop)
					{
						Sleep(30);
						switch(ConnectMode)
						{
						case 0:		
							break;
						case 1:
						case 3://usb
							res=EPC1G2_WriteWordBlock(m_hScanner,EPC_Word,IDTemp,mem,ptr,len,mask,AccessPassword,RS485Address);
							break;
						}

						if(res==_OK)
						{
							break;
						}
						else
						{
							j++;
							t=res*1000+j;
							SendMessage(epcDlg->m_hWnd,WM_MSG,104,t);//密码区数据写入失败，请放入卡:%d-%d,t指时间
							Sleep(1000);
						}
					}
				}

				SendMessage(epcDlg->m_hWnd,WM_MSG,1,0);//写入成功，发送成功消息
			}
			else
			{
				SendMessage(epcDlg->m_hWnd,WM_MSG,102,0);//卡错误，请更换正确的卡
				Sleep(1000);
			}

		}
		else
		{
			j++;
			t=res*1000+j;
			SendMessage(epcDlg->m_hWnd,WM_MSG,101,t);//等待写卡，请放入卡:%d-%d
			Sleep(100);
		}

		//写入间隔时间
		 for(i=0;i<(int)epcDlg->m_Time;i++)
		 {
			 Sleep(1000);
			 SendMessage(epcDlg->m_hWnd,WM_MSG,2,i);//等待写卡，请放入卡:%d,i为时间
		 }
	}

	return 0;
}


void CWriteISO6C::OnBnClickedButtonWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str,str1;
	UpdateData(TRUE);


	//////////////////////////////////////////////////////////////////////////
	iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了), 3---EAS报警(add by martrin20140514)
	if ( ((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->GetCheck() || ((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_EAS_ALARM))->GetCheck() )
	{
		char chTemp[33]={0};
		int i = 0;
		int str_len	=	0;
		CString m_RWAccessPassword;

		iFlagProtectOpt	=	1;//1---使用密码锁	

		GetDlgItem(IDC_EDIT1)->GetWindowText(m_RWAccessPassword);
		//毁灭密码
		str1 = m_RWAccessPassword.SpanIncluding(_T("0123456789ABCDEFabcdef"));
		if(lstrlen(str1)!=lstrlen(m_RWAccessPassword))
		{
			MessageBox(_T("Please input HEX!"), _T("Warning"),MB_ICONWARNING);
			GetDlgItem(IDC_EDIT_USERPASS)->SetFocus();
			((CEdit *) GetDlgItem(IDC_EDIT_USERPASS))->SetSel(0,-1);
			return;
		}
		str1 = m_RWAccessPassword;
		if (lstrlen(str1)!=8)
		{
			//MessageBox(_T("Please input enough Length of AccessPassword!"), _T("Warning"),MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_D0085"), theApp.g_LoadString("IDWQF_D0086"), MB_ICONWARNING);
			return;
		}

		GetDlgItem(IDC_EDIT_USERPASS)->GetWindowText(m_RWAccessPassword);		
		//访问密码
		str=m_RWAccessPassword.SpanIncluding(_T("0123456789ABCDEFabcdef"));
		if(lstrlen(str)!=lstrlen(m_RWAccessPassword))
		{
			MessageBox(_T("Please input HEX!"), _T("Warning"),MB_ICONWARNING);
			GetDlgItem(IDC_EDIT_USERPASS)->SetFocus();
			((CEdit *) GetDlgItem(IDC_EDIT_USERPASS))->SetSel(0,-1);
			return;
		}
		str=m_RWAccessPassword;
		str_len=lstrlen(str);
		if (str_len!=8)
		{
			//MessageBox(_T("Please input enough Length of AccessPassword!"), _T("Warning"),MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_D0085"), theApp.g_LoadString("IDWQF_D0086"), MB_ICONWARNING);
			return;
		}

		//WideCharToMultiByte( CP_ACP, 0, str.GetBuffer(), -1,(char*)chTemp,str.GetLength()*2,NULL,NULL );
		memcpy(chTemp, str1,8);
		memcpy(chTemp + 8, str, 8);

		Asc2Bcd((char*)AccessPassword, chTemp, 16);

		//str	=	_T("您确定要使用密码写来锁定EPC吗？");
		str	=	theApp.g_LoadString("IDWQF_D0087");

	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->GetCheck() )
	{
		iFlagProtectOpt	=	2;//2---永远锁住(死了)

		//str	=	_T("您确定要永远不可写来锁定EPC吗？");
		str	=	theApp.g_LoadString("IDWQF_D0088");
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_EAS_ALARM))->GetCheck() )
	{
		iFlagProtectOpt	=	3;//3---EAS报警(add by martrin20140514)
		
		//str	=	_T("您确定要EAS报警吗？");
		str	=	theApp.g_LoadString("IDWQF_D0122");
	}

	int k = 0;
	if ( 0 == iFlagProtectOpt )
	{
	}
	else
	{
		//k	=	MessageBox(str, _T("警告"),MB_YESNO|MB_ICONWARNING);
		k	=	MessageBox(str, theApp.g_LoadString("IDWQF_D0089"),MB_YESNO|MB_ICONWARNING);
		if ( k != 6 )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//增加测试(由于有时会弹出在线不在线问题) add by martrin 20150427
	//#if defined (ENABLETESTT)

	//#else
	int iCount=m_ListInData.GetItemCount();
	if(iCount==0)
	{
		//MessageBox(_T("待写入数据列表中没有数据"),_T("提示"),0);
		MessageBox(theApp.g_LoadString("IDWQF_D0090"), theApp.g_LoadString("IDWQF_D0091"), 0);
		return;
	}
	//#endif

	if(!ifConnectReader)
	{
		//MessageBox(_T("读写器未连接"),_T("提示"),0);
		MessageBox(theApp.g_LoadString("IDWQF_D0092"), theApp.g_LoadString("IDWQF_D0093"), 0);
		return;
	}

// 	if(nWRStatus!=1)
// 	{
// 		//MessageBox(_T("读写器正在使用中"),_T("提示"),0);
//		//MessageBox(theApp.g_LoadString("IDWQF_D0094"), theApp.g_LoadString("IDWQF_D0095"), 0);
// 		return;
// 	}
// 	else
// 	{
// 		nWRStatus=2;
// 	}

	nStop=1;

	nWord=m_Word;

	CString tmp;
	tmp  =m_ListInData.GetItemText(0,1);

	m_MsgIn.SetWindowText(tmp);  

	GetDlgItem(IDC_CHECK_EPC)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_USE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_PASS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(FALSE);

	if(m_SetEPC)
	{
		//使用EPC写要变不可用
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->EnableWindow(FALSE);

		m_ScanExitThreadEvent	=	CreateEvent(NULL,FALSE,FALSE,NULL);
		//增加测试(由于有时会弹出在线不在线问题) add by martrin 20150427
		#if defined (ENABLETESTT)
		m_WriteEpcHandle		=	CreateThread(NULL,0,ThreadEPCTest,this,0,&g6CWriteTheardID);	
		#else
		m_WriteEpcHandle		=	CreateThread(NULL,0,ThreadEPC,this,0,&g6CWriteTheardID);	
		#endif
	}
	else
	{
		CreateThread(NULL,0,ThreadWrite,this,0,NULL);
	}
}

void CWriteISO6C::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码

	nStop=0;
	//nWRStatus=1;

	GetDlgItem(IDC_CHECK_EPC)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_USE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_PASS)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);

	//使用EPC写要变不可用
	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->EnableWindow(TRUE);

	SetEvent(m_ScanExitThreadEvent);
	Sleep(1000);
	CloseHandle(m_ScanExitThreadEvent);
	m_ScanExitThreadEvent	=	NULL;
	
	TerminateThread(m_WriteEpcHandle,0);
	CloseHandle(m_WriteEpcHandle);
	m_WriteEpcHandle = INVALID_HANDLE_VALUE;

}



void CWriteISO6C::OnBnClickedButtonMove()
{
	//取写入列表的值，增加到完成列表

	int nTotal=m_ListInData.GetItemCount();
	if(nTotal<=0)
	{
		return;
	}

	CString sNo  =m_ListInData.GetItemText(0,0);
	CString sEPC =m_ListInData.GetItemText(0,1);
	CString sUse =m_ListInData.GetItemText(0,2);
	CString sPass=m_ListInData.GetItemText(0,3);

	int nRow=m_ListOutData.InsertItem(0,sNo);
	m_ListOutData.SetItemText(nRow, 1, sEPC);
	m_ListOutData.SetItemText(nRow, 2, sUse);
	m_ListOutData.SetItemText(nRow, 3, sPass);
	//m_ListOutData.SetItemText(nRow, 4, _T("失败"));
	m_ListOutData.SetItemText(nRow, isAscii ? 5 : 4, theApp.g_LoadString("IDWQF_D0096"));

	m_ListInData.DeleteItem(0);

	//处理数据配置文件
	CString tmp;
	stStartNo.nBegin++;
	if((10000-stStartNo.nBegin)==0)  //nBegin=10000时No+1,nBegin=0
	{
		stStartNo.nOne++;
		stStartNo.nBegin=0; 
	}
	stStartNo.nNumber++;
	stStartNo.nTotal--; 
	if(stStartNo.nTotal<=0)
	{
		DeleteFile(stFileName); 
	}
	else
	{
		tmp.Format(_T("%d"),stStartNo.nOne);
		WritePrivateProfileString(_T("Data"),_T("One"),tmp,stFileName);
		tmp.Format(_T("%d"),stStartNo.nBegin);
		WritePrivateProfileString(_T("Data"),_T("Begin"),tmp,stFileName);
		tmp.Format(_T("%d"),stStartNo.nCount);
		WritePrivateProfileString(_T("Data"),_T("Count"),tmp,stFileName);
		tmp.Format(_T("%d"),stStartNo.nNumber);
		WritePrivateProfileString(_T("Data"),_T("Number"),tmp,stFileName);
		tmp.Format(_T("%d"),stStartNo.nTotal);
		WritePrivateProfileString(_T("Data"),_T("Total"),tmp,stFileName);
	}

	//界面总数显示
	m_Falsh++;
	m_Mod--;
	m_False++;


	UpdateData(FALSE);
	
}

void CWriteISO6C::OnCbnKillfocusComboWord()
{
	// TODO: 在此添加控件通知处理程序代码

	switch(m_Word)
	{
	case 1:
		m_DisStr=_T("0000"); 
		break;
	case 2:
		m_DisStr=_T("0000-0000");
		break;
	case 3:
		m_DisStr=_T("0000-0000-0000");
		break;
	case 4:
		m_DisStr=_T("0000-0000-0000-0000");
		break;
	case 5:
		m_DisStr=_T("0000-0000-0000-0000-0000");
		break;
	case 6:
		m_DisStr=_T("0000-0000-0000-0000-0000-0000");
		break;
	case 7:
		m_DisStr=_T("0000-0000-0000-0000-0000-0000-0000");
		break;
	case 8:
		m_DisStr=_T("0000-0000-0000-0000-0000-0000-0000-0000");
		break;
	default:
		m_DisStr="";
		break;
	}

	UpdateData(FALSE);
}

LRESULT CWriteISO6C::WriteMsg(WPARAM wParam, LPARAM lParam)
{
	CString tmp;

	switch(wParam)
	{
	case 0:  //列表数据完成或连接断开
		nStop=0;
		//nWRStatus=1;
		GetDlgItem(IDC_CHECK_EPC)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_PASS)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
		if(lParam)
		{
			//MessageBox(_T("读写器连接已断开断开，请重新连接"),_T("警告"),MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_D0097"), theApp.g_LoadString("IDWQF_D0098"), MB_ICONWARNING);
		}
		break;
	case 1:  //数据写入完成
		if(lParam==0)
		{
			//记录当前的写入数据
			stInData.nLog=1;
			stInData.stNo  =m_ListInData.GetItemText(0,0);
			stInData.stEPC =m_ListInData.GetItemText(0,1);
			stInData.stUse =m_ListInData.GetItemText(0,2);
			stInData.stPass=m_ListInData.GetItemText(0,3);

			//增加完成列表
			int nRow=m_ListOutData.InsertItem(0,stInData.stNo);
			m_ListOutData.SetItemText(nRow, 1, stInData.stEPC);
			m_ListOutData.SetItemText(nRow, 2, stInData.stUse);
			m_ListOutData.SetItemText(nRow, 3, stInData.stPass);
			//m_ListOutData.SetItemText(nRow, 4, _T("成功"));
			m_ListOutData.SetItemText(nRow, isAscii ? 5 : 4, theApp.g_LoadString("IDWQF_D0099"));

			m_ListInData.DeleteItem(0); 

			//处理数据配置文件
			stStartNo.nBegin++;
			if((10000-stStartNo.nBegin)==0)
			{
				stStartNo.nOne++;
				stStartNo.nBegin=0;
			}
			stStartNo.nNumber++;
			stStartNo.nTotal--; 
			if(stStartNo.nTotal<=0)
			{
				DeleteFile(stFileName); 
			}
			else
			{
				tmp.Format(_T("%d"),stStartNo.nOne);
				WritePrivateProfileString(_T("Data"),_T("One"),tmp,stFileName);
				tmp.Format(_T("%d"),stStartNo.nBegin);
				WritePrivateProfileString(_T("Data"),_T("Begin"),tmp,stFileName);
				tmp.Format(_T("%d"),stStartNo.nCount);
				WritePrivateProfileString(_T("Data"),_T("Count"),tmp,stFileName);
				tmp.Format(_T("%d"),stStartNo.nNumber);
				WritePrivateProfileString(_T("Data"),_T("Number"),tmp,stFileName);
				tmp.Format(_T("%d"),stStartNo.nTotal);
				WritePrivateProfileString(_T("Data"),_T("Total"),tmp,stFileName);
			}
            //界面显示信息
			tmp = m_ListInData.GetItemText(0,1);
			m_MsgIn.SetWindowText(tmp); 
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OK));
			//m_MsgDis.SetWindowText(_T("写卡成功，请移开此卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0100"));
			m_MsgOut.SetWindowText(stInData.stEPC);  

			m_Falsh++;
			m_Mod--;
			m_True++;
			UpdateData(FALSE);
		}
		else 
		{
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
			//m_MsgDis.SetWindowText(_T("此卡未写入成功，请重新放入"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0101"));
		}

		break;
	case 2:
		//tmp.Format(_T("等待写卡，请放入卡:%d"),lParam); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0102"),lParam); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_WAIT));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 3:
		if(lParam==0)
		{
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		    //m_MsgDis.SetWindowText(_T("重复写卡，请更换下一张卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0103"));
		}
		else
		{
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		    //m_MsgDis.SetWindowText(_T("有多张卡存在，请放入一张卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0104"));
		}
		break;
	case 100://待写入数据错误
		nStop=0;
		//nWRStatus=1;
		GetDlgItem(IDC_CHECK_EPC)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_PASS)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		//m_MsgDis.SetWindowText(_T("错误，待写入的数据错误"));
		m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0105"));
		//MessageBox(_T("待写入的数据错误"),_T("警告"),MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_D0106"), theApp.g_LoadString("IDWQF_D0107"), MB_ICONWARNING);
		break;
	case 101:
		//tmp.Format(_T("等待写卡或写卡失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0108"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_WAIT));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 102:
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		//m_MsgDis.SetWindowText(_T("卡错误，请更换正确的卡"));
		m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0109"));
		break;
	case 103:
		//tmp.Format(_T("用户区数据写入失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0110"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 104:
		//tmp.Format(_T("密码区数据写入失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0111"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 105:
		//tmp.Format(_T("读原密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0112"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 106:
		//tmp.Format(_T("写新密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0113"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
	
	case 107:
		//tmp.Format(_T("锁EPC失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0114"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 108:
		//tmp.Format(_T("锁密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0115"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 109:
		//tmp.Format(_T("写新密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0113"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
		
	case 110:
		//tmp.Format(_T("设置EAS报警失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_D0123"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 111:
		//界面显示信息
		//增加完成列表
		{
			CString str;
			str.Format("%d", lParam);
			//m_ListOutData.SetItemText(1, 1, str);
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OK));
			//m_MsgDis.SetWindowText(_T("写卡成功，请移开此卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_D0100"));
			m_MsgOut.SetWindowText(str); 
			
			UpdateData(FALSE);
		}
		break;

	}

	return 0;
}

void CWriteISO6C::OnBnClickedButtonDelall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListInData.DeleteAllItems(); 
}

void CWriteISO6C::OnBnClickedButtonDelout()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListOutData.DeleteAllItems(); 
}

void CWriteISO6C::OnBnClickedRadioEpc()
{
	// TODO: 在此添加控件通知处理程序代码
	//点击选中EPC

	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.DeleteColumn(2);
			m_ListOutData.DeleteColumn(2);
			break;
		case 1:
			m_ListInData.DeleteColumn(3);
			m_ListOutData.DeleteColumn(3);
			break;
		case 2:
			m_ListInData.DeleteColumn(4);
			m_ListOutData.DeleteColumn(4);
			break;
	}

	m_WriteMode=0;

	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			break;
		case 1:
			m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			break;
		case 2:
			m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			break;
	}

	int i = 0;
	CString strTemp;

	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->ResetContent();
	for (i = 0; i < 9; i++)
	{
		strTemp.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->AddString(strTemp);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->SetCurSel(0);

}

void CWriteISO6C::OnBnClickedRadioUser()
{
	// TODO: 在此添加控件通知处理程序代码
	//点击选中用户区

	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.DeleteColumn(2);
			m_ListOutData.DeleteColumn(2);
			break;
		case 1:
			m_ListInData.DeleteColumn(3);
			m_ListOutData.DeleteColumn(3);
			break;
		case 2:
			m_ListInData.DeleteColumn(4);
			m_ListOutData.DeleteColumn(4);
			break;
	}

	m_WriteMode=1;

	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			break;
		case 1:
			m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			break;
		case 2:
			m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			break;
	}

	int i = 0;
	CString strTemp;
	
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->ResetContent();
	for (i = 0; i < 9; i++)
	{
		strTemp.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->AddString(strTemp);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->SetCurSel(0);

}

void CWriteISO6C::OnBnClickedRadioPass()
{
	// TODO: 在此添加控件通知处理程序代码
	//点击选中密码区
	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.DeleteColumn(2);
			m_ListOutData.DeleteColumn(2);
			break;
		case 1:
			m_ListInData.DeleteColumn(3);
			m_ListOutData.DeleteColumn(3);
			break;
		case 2:
			m_ListInData.DeleteColumn(4);
			m_ListOutData.DeleteColumn(4);
			break;
	}

	m_WriteMode=2;

	if(this->isAscii)
	switch(m_WriteMode)
	{
		case 0:
			m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
			break;
		case 1:
			m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
			break;
		case 2:
			m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			m_ListOutData.InsertColumn(4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
			break;
	}

	int i = 0;
	CString strTemp;

	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->ResetContent();
	for (i = 0; i < 5; i++)
	{
		strTemp.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->AddString(strTemp);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->SetCurSel(4);

}

void CWriteISO6C::OnBnClickedButtonIndata()
{
	// TODO: 在此添加控件通知处理程序代码

	CString filename;
	CFileDialog dlg(TRUE,NULL,NULL,OFN_EXPLORER | OFN_HIDEREADONLY, _T("TXT files(*.csv)|*.csv|")); 
    if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();

		CFileFind tempFind; 
        BOOL IsFinded = tempFind.FindFile(filename);

		if(!IsFinded)
		{
			//AfxMessageBox(_T("数据文件选择错误!"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_D0116"));
			return;
		}
	}
	else
	{
		return;
	}

	m_ListInData.DeleteAllItems(); 

	UpdateData(TRUE);

	CString strtmp,tmp,epcData;
	CStdioFile f;
	f.Open(filename,CFile::typeText);

	int nRow=0,nCol=0,nTotal=0;
    int nLen=0,strLen;
	while(f.ReadString(strtmp))
	{
		if(strtmp.GetLength()<=0)
		{
			continue;
		}
		nTotal=m_ListInData.GetItemCount(); 
		tmp.Format(_T("%d"),nTotal+1);
		nRow=m_ListInData.InsertItem(nTotal,tmp); 

		if(m_SetEPC)
		{
			nCol=1;
			strLen=strtmp.GetLength();
			if(strLen<=0)
			{
				continue;
			}
			nLen=strtmp.Find(',',0);
			if(nLen<=0)
			{
				epcData=strtmp;
				strtmp.Empty(); 
			}
			else
			{
				epcData=strtmp.Left(nLen);
				epcData.Replace("\t", "");
				tmp=strtmp.Right(strLen-nLen-1);
				strtmp=tmp;
			}
			m_ListInData.SetItemText(nRow, nCol, epcData);
		}

		if(m_SetUse)
		{
			nCol=2;
			strLen=strtmp.GetLength();
			if(strLen<=0)
			{
				continue;
			}
			nLen=strtmp.Find(',',0);
			if(nLen<=0)
			{
				epcData=strtmp;
				strtmp.Empty(); 
			}
			else
			{
				epcData=strtmp.Left(nLen);
				tmp=strtmp.Right(strLen-nLen-1);
				strtmp=tmp;
			}
			m_ListInData.SetItemText(nRow, nCol, epcData);
		}

		if(m_SetPass)
		{
			nCol=3;
			strLen=strtmp.GetLength();
			if(strLen<=0)
			{
				continue;
			}
			nLen=strtmp.Find(',',0);
			if(nLen<=0)
			{
				epcData=strtmp;
				strtmp.Empty(); 
			}
			else
			{
				epcData=strtmp.Left(nLen);
				tmp=strtmp.Right(strLen-nLen-1);
				strtmp=tmp;
			}
			m_ListInData.SetItemText(nRow, nCol, epcData);
		}
	}
	f.Close(); 

	m_Total=m_ListInData.GetItemCount();
	m_Falsh=0;
	m_Mod=m_ListInData.GetItemCount();
	m_True=0;
	m_False=0;
	
	UpdateData(FALSE);
}

int CWriteISO6C::OnExportData()
{
	//右键
	//导出写TXT文件
	CListCtrl *pListInfo	=	(CListCtrl*)GetDlgItem(IDC_LIST_INDATA);
	int nRow	=	pListInfo->GetItemCount();	//取得有多少行
	if ( nRow <= 0 )
	{
	
		//MessageBox("List is empty!","Warning", MB_ICONERROR|MB_OK );
		MessageBox(theApp.g_LoadString("IDWQF_D0125"),theApp.g_LoadString("IDWQF_D0126"), MB_ICONERROR|MB_OK );
		
		return 1;
	}


	CString filename;
	//取当前日期，时间
	SYSTEMTIME sysTime;

	GetLocalTime(&sysTime);
	CString strTime;
	
	strTime.Format("%04d%02d%02d.csv", sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	
	//CFileDialog dlg(FALSE,NULL,NULL,OFN_EXPLORER ,_T("TXT files(*.txt)|*.TXT"));
	CFileDialog dlg(FALSE,_T("csv"),strTime,
		OFN_EXPLORER,
		_T("csv files(*.csv)|*.csv|\0"),NULL);
	
	dlg.m_ofn.lpstrTitle	=	_T("export");
	//dlg.m_ofn.lpstrTitle	=	theApp.g_LoadString("IDWQB_D0157");
	if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();	//取得全路径名
	}
	else
	{
		return 1;
	}


	
	int i	=	0;
	char *chRecv	=	new char[2*1024*1024];
	int chCnt	=	0;
	CString strT;

	while ( i < nRow )
	{
		
			*(chRecv + chCnt++)	= '\x09';	//"\x27";
			strT	=	pListInfo->GetItemText(i,1);//epc
			memcpy(chRecv+chCnt, strT, strT.GetLength());
			chCnt	+=	strT.GetLength();

			*(chRecv + chCnt++)	=	',';

			*(chRecv + chCnt++)	= '\x09';
			strT	=	pListInfo->GetItemText(i,2);//tid
			memcpy(chRecv+chCnt, strT, strT.GetLength());
			chCnt	+=	strT.GetLength();

			*(chRecv + chCnt++)	=	',';

			*(chRecv + chCnt++)	= '\x09';
			strT	=	pListInfo->GetItemText(i,3);//password
			memcpy(chRecv+chCnt, strT, strT.GetLength());
			chCnt	+=	strT.GetLength();

			

			

			
		

		*(chRecv + chCnt++)	=	0x0D;
		*(chRecv + chCnt++)	=	0x0A;

		i++;

	}// end of while ( iCnt < nRow )


	*(chRecv + chCnt)	=	0x00;

	WriteBufFile(chRecv, chCnt, filename,0);

	delete[] chRecv;

	//AfxMessageBox("Export finished!");
	//MessageBox("Export finished!","VI-720", MB_ICONINFORMATION|MB_OK );
	MessageBox(theApp.g_LoadString("IDWQF_D0127"),theApp.g_LoadString("IDWQF_D0128"), MB_ICONINFORMATION|MB_OK );
	ShellExecute(NULL,"open",filename,NULL,NULL,SW_SHOWNORMAL);


	return 0;


}

void CWriteISO6C::OnBnClickedCheckEpc()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_SetEPC)
	{
		m_SetEPC=FALSE;
	}
	else
	{
		m_SetEPC=TRUE;
	}

	int iCheck = ((CButton*)GetDlgItem(IDC_CHECK_EPC))->GetCheck();
	if ( iCheck )
	{
		//选中
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->EnableWindow(TRUE);
	}
	else
	{
		//没有选中
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->EnableWindow(FALSE);

	}

	return;

}

void CWriteISO6C::OnBnClickedCheckUse()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_SetUse)
	{
		m_SetUse=FALSE;
	}
	else
	{
		m_SetUse=TRUE;
	}
}

void CWriteISO6C::OnBnClickedCheckPass()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_SetPass)
	{
		m_SetPass=FALSE;
	}
	else
	{
		m_SetPass=TRUE;
	}
}

void CWriteISO6C::OnBnClickedButtonSuccess()
{
	// TODO: 在此添加控件通知处理程序代码

	int nRow=m_ListOutData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要导出!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_D0117"));
		return;
	}

	SYSTEMTIME sysTime;
	char	chCurr[62];
	GetLocalTime(&sysTime);
	memset(chCurr, 0x00, sizeof(chCurr));
	sprintf(chCurr, "%04d%02d%02d%02d%02d%02d.txt",sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

	CString filename;
	CFileDialog dlg(FALSE, NULL, chCurr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER ,_T("TXT files(*.txt)|*.TXT|")); 
    if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();

	}
	else
	{
		return;
	}

	CString sEpcTmp,sUseTmp,sPassTmp,strtmp,sStatus;
	int nEpc=0,nUse=0,nPass=0;

	for(int i=0;i<nRow;i++)
	{
		sEpcTmp=m_ListOutData.GetItemText(i,1);
		sUseTmp=m_ListOutData.GetItemText(i,2);
		sPassTmp=m_ListOutData.GetItemText(i,3);
		if(sEpcTmp.GetLength()>0)
		{
			nEpc=1;
		}
		if(sUseTmp.GetLength()>0)
		{
			nUse=1;
		}
		if(sPassTmp.GetLength()>0)
		{
			nPass=1;
		}
	}

	CFile f;

	f.Open(filename,CFile::modeCreate|CFile::modeReadWrite);
	
    CString tmp=_T(",");

	BYTE str[1024];
	int len;

	for(int i=0;i<=nRow;i++)
	{
		strtmp.Empty(); 
		sEpcTmp=m_ListOutData.GetItemText(nRow-i,1);
		sUseTmp=m_ListOutData.GetItemText(nRow-i,2);
		sPassTmp=m_ListOutData.GetItemText(nRow-i,3);
		sStatus=m_ListOutData.GetItemText(nRow-i,4);
		//if(sStatus==_T("成功"))
		if(sStatus==theApp.g_LoadString("IDWQF_D0118"))
		{
			if(nEpc==1)
			{
				strtmp=sEpcTmp;
				if((nUse==1)||(nPass==1))
				{
					strtmp=strtmp+tmp;
				}
			}

			if(nUse==1)
			{
				strtmp=strtmp+sUseTmp;
				if(nPass==1)
				{
					strtmp=strtmp+tmp;
				}
			}
			if(nPass==1)
			{
				strtmp=strtmp+sPassTmp;
			}

			strtmp=strtmp+_T("\r\n");
			len=0;
			memset(str,0,1024);
			//WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
			len=strtmp.GetLength();
			memcpy(str, strtmp, len);

			f.Write(str,len); 
		}
	}

	f.Close(); 
}

void CWriteISO6C::OnBnClickedButtonError()
{
	// TODO: 在此添加控件通知处理程序代码

	int nRow=m_ListOutData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要导出!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_D0119"));
		return;
	}

	SYSTEMTIME sysTime;
	char	chCurr[62];
	GetLocalTime(&sysTime);
	memset(chCurr, 0x00, sizeof(chCurr));
	sprintf(chCurr, "%04d%02d%02d%02d%02d%02d.txt",sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

	CString filename;
	CFileDialog dlg(FALSE, NULL, chCurr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER ,_T("TXT files(*.txt)|*.TXT|")); 
    if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();

	}
	else
	{
		return;
	}

	CString sEpcTmp,sUseTmp,sPassTmp,strtmp,sStatus;
	int nEpc=0,nUse=0,nPass=0;

	for(int i=0;i<nRow;i++)
	{
		sEpcTmp=m_ListOutData.GetItemText(i,1);
		sUseTmp=m_ListOutData.GetItemText(i,2);
		sPassTmp=m_ListOutData.GetItemText(i,3);
		if(sEpcTmp.GetLength()>0)
		{
			nEpc=1;
		}
		if(sUseTmp.GetLength()>0)
		{
			nUse=1;
		}
		if(sPassTmp.GetLength()>0)
		{
			nPass=1;
		}
	}


	CFile f;

	f.Open(filename,CFile::modeCreate|CFile::modeReadWrite);
	
    CString tmp=_T(",");

	BYTE str[1024];
	int len;

	for(int i=0;i<=nRow;i++)
	{
		strtmp.Empty(); 
		sEpcTmp=m_ListOutData.GetItemText(nRow-i,1);
		sUseTmp=m_ListOutData.GetItemText(nRow-i,2);
		sPassTmp=m_ListOutData.GetItemText(nRow-i,3);
		sStatus=m_ListOutData.GetItemText(nRow-i,4);
		//if(sStatus==_T("失败"))
		if(sStatus==theApp.g_LoadString("IDWQF_D0120"))
		{
			if(nEpc==1)
			{
				strtmp=sEpcTmp;
				if((nUse==1)||(nPass==1))
				{
					strtmp=strtmp+tmp;
				}
			}

			if(nUse==1)
			{
				strtmp=strtmp+sUseTmp;
				if(nPass==1)
				{
					strtmp=strtmp+tmp;
				}
			}
			if(nPass==1)
			{
				strtmp=strtmp+sPassTmp;
			}

			strtmp=strtmp+_T("\r\n");
			len=0;
			memset(str,0,1024);
			//WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
			len=strtmp.GetLength();
			memcpy(str, strtmp, len);

			f.Write(str,len);
		}
	}

	f.Close(); 

}

void CWriteISO6C::OnBnClickedCheckLockepcPass()
{
	// TODO: 在此添加控件通知处理程序代码
	//使用密码写
	//如果使用密码写，打勾，那么永远不可写去掉打勾，

	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_Neverwriteable))->SetCheck(FALSE);
	if ( ((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->GetCheck() )
	{
		GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_USERPASS)->SetFocus();
	}

}

void CWriteISO6C::OnBnClickedCheckLockepcNeverwriteable()
{
	// TODO: 在此添加控件通知处理程序代码
	//永远不可写
	//如果使用永远不可写，打勾了，那么使用密码写去掉打勾。

	((CButton*)GetDlgItem(IDC_CHECK_LOCKEPC_PASS))->SetCheck(FALSE);
	GetDlgItem(IDC_EDIT_USERPASS)->EnableWindow(FALSE);

}
#endif

BOOL CWriteISO6C::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	//限制只能输入数字和ABCDEF
	if(!isAscii)
	if( (GetFocus()   ==   GetDlgItem(IDC_EDIT_BEFOR) || 
		GetFocus()   ==   GetDlgItem(IDC_EDIT_BEGIN) ||
		GetFocus()   ==   GetDlgItem(IDC_EDIT_END) 
		 ) &&     //EdIT控件获得焦点呢   
		pMsg->message   ==   WM_CHAR)                               //键盘输入   
	{   
		
		UCHAR uCh=(UCHAR)pMsg->wParam;
		switch(uCh)
		{
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			uCh-=32;
			
		default:;
		}
		pMsg->wParam=uCh;
		if(
			(pMsg->wParam)   ==0x20   ||
			(pMsg->wParam)   =='1'   ||
			(pMsg->wParam)   =='2'   ||
			(pMsg->wParam)   =='3'   ||   
			(pMsg->wParam)   =='4'   ||   
			(pMsg->wParam)   =='5'   ||   
			(pMsg->wParam)   =='6'   ||   
			(pMsg->wParam)   =='7'   ||   
			(pMsg->wParam)   =='8'   ||   
			(pMsg->wParam)   =='9'   ||
			(pMsg->wParam)   =='0'   ||
			(pMsg->wParam)   =='A'   ||
			(pMsg->wParam)   =='B'   ||
			(pMsg->wParam)   =='C'   ||
			(pMsg->wParam)   =='D'   ||
			(pMsg->wParam)   =='E'   ||
			(pMsg->wParam)   =='F'	 ||
			(int)(pMsg->wParam)   ==0x08 //退格键
			
			) 
			
			return	CDialog::PreTranslateMessage(pMsg);	
		else
			return TRUE;
		
	}
	
	//如果在列表中点右键
	if(WM_RBUTTONDOWN == pMsg->message )
	{
		if ( pMsg->hwnd == GetDlgItem(IDC_LIST_INDATA)->m_hWnd )
		{
			PopupMenu();
			return TRUE; 
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CWriteISO6C::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strIn1,strIn2,strIn3,strOut;
	byte str1[50] = { 0 };
	byte str2[50] = { 0 };
	byte str3[50] = { 0 };

	GetDlgItem(IDC_EDIT_BEFOR)->GetWindowText(strIn1);
	GetDlgItem(IDC_EDIT_BEGIN)->GetWindowText(strIn2);
	GetDlgItem(IDC_EDIT_END)->GetWindowText(strIn3);

	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())//ASCII
	{					
		StrToHex(str1, (byte*)strIn1.GetBuffer(), strIn1.GetLength());
		StrToHex(str2, (byte*)strIn2.GetBuffer(), strIn2.GetLength());
		StrToHex(str3, (byte*)strIn3.GetBuffer(), strIn3.GetLength());
		switch(m_WriteMode)
		{
			case 0:
				m_ListInData.InsertColumn( 2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
				m_ListOutData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 220 );
				break;
			case 1:
				m_ListInData.InsertColumn( 3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
				m_ListOutData.InsertColumn(3, theApp.g_LoadString("IDWQF_C0043"), LVCFMT_LEFT, 220 );
				break;
			case 2:
				m_ListInData.InsertColumn( 4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
				m_ListOutData.InsertColumn(4, theApp.g_LoadString("IDWQF_C0044"), LVCFMT_LEFT, 220 );
				break;
		}
		isAscii = true;
	} 
	else
	{
		HexToStr(str1, (byte*)strIn1.GetBuffer(), strIn1.GetLength());		
		HexToStr(str2, (byte*)strIn2.GetBuffer(), strIn2.GetLength());	
		HexToStr(str3, (byte*)strIn3.GetBuffer(), strIn3.GetLength());	
		switch(m_WriteMode)
		{
			case 0:
				m_ListInData.DeleteColumn(2);
				m_ListOutData.DeleteColumn(2);
				break;
			case 1:
				m_ListInData.DeleteColumn(3);
				m_ListOutData.DeleteColumn(3);
				break;
			case 2:
				m_ListInData.DeleteColumn(4);
				m_ListOutData.DeleteColumn(4);
				break;
		}
		
		isAscii = false;
	}

	GetDlgItem(IDC_EDIT_BEFOR)->SetWindowText((char*)str1);
	GetDlgItem(IDC_EDIT_BEGIN)->SetWindowText((char*)str2);
	GetDlgItem(IDC_EDIT_END)->SetWindowText((char*)str3);
}
