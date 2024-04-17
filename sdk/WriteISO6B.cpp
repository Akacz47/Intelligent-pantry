// WriteISO6B.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "WriteISO6B.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
HWND	g6BWriteHwnd	=	NULL;	//读写器6B写窗体句柄
extern char *big_num_add(char *ans, char *first, char *second);
BYTE ISO6B_IDBuffer[MAX_LABELS][ID_MAX_SIZE_64BIT+1]={0};

/////////////////////////////////////////////////////////////////////////////
// CWriteISO6B property page

IMPLEMENT_DYNCREATE(CWriteISO6B, CPropertyPage)

CWriteISO6B::CWriteISO6B() : CPropertyPage(CWriteISO6B::IDD)
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
	//{{AFX_DATA_INIT(CWriteISO6B)
	//}}AFX_DATA_INIT
	iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了)
	strcpy((char*)AccessPassword, "12341234");


}

CWriteISO6B::~CWriteISO6B()
{
}

void CWriteISO6B::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWriteISO6B)
	DDX_Control(pDX, IDC_LIST_INDATA, m_ListInData);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_Count);
	DDX_Text(pDX, IDC_EDIT_SUM, m_Sum);
	DDX_Text(pDX, IDC_EDIT_XXXX, m_DisStr);
	DDX_CBIndex(pDX, IDC_COMBO_WORD, m_Word);
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


BEGIN_MESSAGE_MAP(CWriteISO6B, CPropertyPage)
	//{{AFX_MSG_MAP(CWriteISO6B)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_DATA, OnBnClickedButtonData)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_CBN_SELENDOK(IDC_COMBO_WORD, OnCbnSelendokComboWord)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnBnClickedButtonMove)
	ON_CBN_KILLFOCUS(IDC_COMBO_WORD, OnCbnKillfocusComboWord)
	ON_MESSAGE(WM_MSG_W6B,WriteMsg)
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
	ON_MESSAGE(WM_6BWRITEDATAMSG, On6bWritedatamsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWriteISO6B message handlers
void CWriteISO6B::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_F0001"));//写ISO18000-6B类型电子标签
	
	
	GetDlgItem(IDC_STATIC_DATABIT)->SetWindowText(theApp.g_LoadString("IDWQF_F0002"));//数据位数(Word)
	GetDlgItem(IDC_STATIC_XUHAO)->SetWindowText(theApp.g_LoadString("IDWQF_F0003"));//起始序号
	GetDlgItem(IDC_STATIC_BUCHANG)->SetWindowText(theApp.g_LoadString("IDWQF_F0004"));//累加步长
	GetDlgItem(IDC_BUTTON_DATA)->SetWindowText(theApp.g_LoadString("IDWQF_F0005"));//按规则生成数据
	GetDlgItem(IDC_STATIC_MAKENUM)->SetWindowText(theApp.g_LoadString("IDWQF_F0006"));//生成数量
	GetDlgItem(IDC_STATIC_LISTD)->SetWindowText(theApp.g_LoadString("IDWQF_F0007"));//待写入数据列表
	GetDlgItem(IDC_STATIC_CODESET)->SetWindowText(theApp.g_LoadString("IDWQF_F0008"));//编码规则设置
	GetDlgItem(IDC_BUTTON_FILE)->SetWindowText(theApp.g_LoadString("IDWQF_F0009"));//导入数据生成规则
	GetDlgItem(IDC_STATIC_SAVEOPT)->SetWindowText(theApp.g_LoadString("IDWQF_F0010"));//记数器
	GetDlgItem(IDC_STATIC_NUMT)->SetWindowText(theApp.g_LoadString("IDWQF_F0011"));//数据总数
	GetDlgItem(IDC_STATIC_SHENGNUM)->SetWindowText(theApp.g_LoadString("IDWQF_F0012"));//剩余数
	GetDlgItem(IDC_STATIC_SUCCESSNUM)->SetWindowText(theApp.g_LoadString("IDWQF_F0013"));//成功数
	GetDlgItem(IDC_STATIC_FINISHNUM)->SetWindowText(theApp.g_LoadString("IDWQF_F0014"));//完成数
	GetDlgItem(IDC_STATIC_FAILNUM)->SetWindowText(theApp.g_LoadString("IDWQF_F0015"));//失败数
	GetDlgItem(IDC_STATIC_W6CEXAMPLE)->SetWindowText(theApp.g_LoadString("IDWQF_F0016"));//示例
	GetDlgItem(IDC_CHECK_BEFOR)->SetWindowText(theApp.g_LoadString("IDWQF_F0017"));//前缀
	GetDlgItem(IDC_CHECK_END)->SetWindowText(theApp.g_LoadString("IDWQF_F0018"));//后缀
	GetDlgItem(IDC_STATIC_BETT)->SetWindowText(theApp.g_LoadString("IDWQF_F0019"));//间隔时间:秒
	GetDlgItem(IDC_BUTTON_WRITE)->SetWindowText(theApp.g_LoadString("IDWQF_F0020"));//开始写卡
	GetDlgItem(IDC_BUTTON_STOP)->SetWindowText(theApp.g_LoadString("IDWQF_F0021"));//暂停写卡
	GetDlgItem(IDC_BUTTON_MOVE)->SetWindowText(theApp.g_LoadString("IDWQF_F0022"));//移除当前数据
	GetDlgItem(IDC_STATIC_WRITEOP)->SetWindowText(theApp.g_LoadString("IDWQF_F0023"));//写卡操作
	
	GetDlgItem(IDC_STATIC_FINLIST)->SetWindowText(theApp.g_LoadString("IDWQF_F0026"));//完成数据列表
	GetDlgItem(IDC_BUTTON_SUCCESS)->SetWindowText(theApp.g_LoadString("IDWQF_F0027"));//生成成功数据TXT
	GetDlgItem(IDC_BUTTON_ERROR)->SetWindowText(theApp.g_LoadString("IDWQF_F0028"));//生成失败数据TXT
	GetDlgItem(IDC_BUTTON_DELOUT)->SetWindowText(theApp.g_LoadString("IDWQF_F0029"));//清空完成数据列表
	GetDlgItem(IDC_STATIC_DIS)->SetWindowText(theApp.g_LoadString("IDWQF_F0030"));//状态
	GetDlgItem(IDC_STATIC_OUT)->SetWindowText(theApp.g_LoadString("IDWQF_F0031"));//数据
	GetDlgItem(IDC_BUTTON_DELALL)->SetWindowText(theApp.g_LoadString("IDWQF_F0032"));//清空写入数据列表
	GetDlgItem(IDC_STATIC_WWriteDATA)->SetWindowText(theApp.g_LoadString("IDWQF_F0033"));//待写入数据
	GetDlgItem(IDC_STATIC_IN)->SetWindowText(theApp.g_LoadString("IDWQF_F0034"));//数据
	GetDlgItem(IDC_STATIC_WWriteSTAT)->SetWindowText(theApp.g_LoadString("IDWQF_F0035"));//写入状态
	GetDlgItem(IDC_STATIC_FWriteData)->SetWindowText(theApp.g_LoadString("IDWQF_F0036"));//写入数据
	GetDlgItem(IDC_STATIC_STATTIP)->SetWindowText(theApp.g_LoadString("IDWQF_F0037"));//状态提示
	
	GetDlgItem(IDC_BUTTON_INDATA)->SetWindowText(theApp.g_LoadString("IDWQF_F0040"));//导入数据
	
	
	m_ListInData.DeleteColumn(0);
	m_ListInData.DeleteColumn(0);
	m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_F0046"), LVCFMT_LEFT, 40 );
	m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_F0047"), LVCFMT_LEFT, 220 );


	
	m_ListOutData.DeleteColumn(0);
	m_ListOutData.DeleteColumn(0);
	m_ListOutData.DeleteColumn(0);
	m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_F0050"), LVCFMT_LEFT, 40 );
	m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_F0052"), LVCFMT_LEFT, 220 );
	m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_F0054"), LVCFMT_LEFT, 80 );

	
}

void CWriteISO6B::UpdateStatus()//控制使能与不使能
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

BOOL CWriteISO6B::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	g6BWriteHwnd	=	this->GetSafeHwnd();


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
	//m_ListInData.InsertColumn( 1, _T(" ID "), LVCFMT_LEFT, 220 );

	m_ListInData.InsertColumn( 0, theApp.g_LoadString("IDWQF_F0046"), LVCFMT_LEFT, 40 );
	m_ListInData.InsertColumn( 1, theApp.g_LoadString("IDWQF_F0047"), LVCFMT_LEFT, 220 );

    //m_ListInData.InsertColumn( 1, _T("EPC区"), LVCFMT_LEFT, 220 );
    //m_ListInData.InsertColumn( 2, _T("用户区"), LVCFMT_LEFT, 220 );
	//m_ListInData.InsertColumn( 3, _T("密码区"), LVCFMT_LEFT, 120 );
	
	
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
	//m_ListOutData.InsertColumn( 1, _T("用户区"), LVCFMT_LEFT, 220 );
	//m_ListOutData.InsertColumn( 2, _T("状态"), LVCFMT_LEFT, 80 );

	m_ListOutData.InsertColumn( 0, theApp.g_LoadString("IDWQF_F0050"), LVCFMT_LEFT, 40 );
	m_ListOutData.InsertColumn( 1, theApp.g_LoadString("IDWQF_F0052"), LVCFMT_LEFT, 220 );
	m_ListOutData.InsertColumn( 2, theApp.g_LoadString("IDWQF_F0054"), LVCFMT_LEFT, 80 );

    //m_ListOutData.InsertColumn( 1, _T("EPC区"), LVCFMT_LEFT, 200 );
    //m_ListOutData.InsertColumn( 2, _T("用户区"), LVCFMT_LEFT, 200 );
	//m_ListOutData.InsertColumn( 3, _T("密码区"), LVCFMT_LEFT, 120 );
	//m_ListOutData.InsertColumn( 4, _T("状态"), LVCFMT_LEFT, 80 );
	
	
	strEpc="0054100B";
	
	stInData.nLog=0; 
	
	m_SetEPC=FALSE;
	m_SetUse=TRUE;
	
	m_Time=1;
	
	nStop=0;
	
	nWord=0;
	
	
	UpdateData(FALSE);
	
	for (i = 0; i < 9; i++)
	{
		strTemp.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->AddString(strTemp);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_WORD))->SetCurSel(0);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWriteISO6B::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CPropertyPage::OnCancel();
}

//WM_6BWRITEDATAMSG
LRESULT CWriteISO6B::On6bWritedatamsg(WPARAM wParam, LPARAM lParam)
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
	
	return 0;
}

BOOL CWriteISO6B::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

BOOL CWriteISO6B::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	//6B写页
	::PostMessage(g6BWriteHwnd,WM_6BWRITEDATAMSG,2,0);	//convert language

	return CPropertyPage::OnSetActive();
}

void CWriteISO6B::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}
#if 1
void CWriteISO6B::OnBnClickedButtonData()
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
		MessageBox(theApp.g_LoadString("IDWQF_F0055"), theApp.g_LoadString("IDWQF_F0056"), MB_ICONWARNING);
		return;
	}

	if(m_Count<=0)
	{
		//MessageBox(_T("数据总数不能小于等于0"),_T("警告"),MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_F0057"), theApp.g_LoadString("IDWQF_F0058"), MB_ICONWARNING);
		return;
	}

	int j;

	j=m_WriteMode+1;

	if(nStatus)
	{
		CString str;
		if((m_CBefor)&&(!m_CEnd))
		{
			nBLen=m_Befro.GetLength(); 
			if((nBLen<4)||((nBLen%4)!=0)||(nBLen>=m_Word*4))
			{
				//MessageBox(_T("前缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0059"), theApp.g_LoadString("IDWQF_F0060"), MB_ICONWARNING);
				return;
			}
			str=m_Befro.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=nBLen)
			{
				//MessageBox(_T("前缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0061"), theApp.g_LoadString("IDWQF_F0062"), MB_ICONWARNING);
				return;
			}
			
			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*4)-nBLen)/4))
				{
				case 1:
					//epcData.Format(_T("%s%04X"),m_Befro,nB);
					epcData.Format(_T("%s%04s"),m_Befro,strTemp);
					break;
				case 2:
					//epcData.Format(_T("%s%08X"),m_Befro,nB);
					epcData.Format(_T("%s%08s"),m_Befro,strTemp);
					break;
				case 3:
					//epcData.Format(_T("%s%012X"),m_Befro,nB);
					epcData.Format(_T("%s%012s"),m_Befro,strTemp);
					break;
				case 4:
					//epcData.Format(_T("%s%016X"),m_Befro,nB);
					epcData.Format(_T("%s%016s"),m_Befro,strTemp);
					break;
				case 5:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					epcData.Format(_T("%s%020s"),m_Befro,strTemp);
					break;
				case 6:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					epcData.Format(_T("%s%024s"),m_Befro,strTemp);
					break;
				case 7:
					//epcData.Format(_T("%s%020X"),m_Befro,nB);
					epcData.Format(_T("%s%028s"),m_Befro,strTemp);
					break;
				default:
					//MessageBox(_T("前缀数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_F0063"), theApp.g_LoadString("IDWQF_F0064"), MB_ICONWARNING);
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

				epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				m_ListInData.SetItemText(nRow, j, epcData);
			}
		}

		if((m_CEnd)&&(!m_CBefor))
		{
			nELen=m_End.GetLength(); 
			if((nELen<4)||((nELen%4)!=0)||(nELen>=m_Word*4))
			{
				//MessageBox(_T("后缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0065"), theApp.g_LoadString("IDWQF_F0066"), MB_ICONWARNING);
				return;
			}
			str=m_End.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=nELen)
			{
				//MessageBox(_T("后缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0067"), theApp.g_LoadString("IDWQF_F0068"), MB_ICONWARNING);
				return;
			}

			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*4)-nELen)/4))
				{
				case 1:
					//epcData.Format(_T("%04X%s"),nB,m_End);
					epcData.Format(_T("%04s%s"),strTemp,m_End);
					break;
				case 2:
					//epcData.Format(_T("%08X%s"),nB,m_End);
					epcData.Format(_T("%08s%s"),strTemp,m_End);
					break;
				case 3:
					//epcData.Format(_T("%012X%s"),nB,m_End);
					epcData.Format(_T("%012s%s"),strTemp,m_End);
					break;
				case 4:
					//epcData.Format(_T("%016X%s"),nB,m_End);
					epcData.Format(_T("%016s%s"),strTemp,m_End);
					break;
				case 5:
					//epcData.Format(_T("%020X%s"),nB,m_End);
					epcData.Format(_T("%020s%s"),strTemp,m_End);
					break;
				case 6:
					//epcData.Format(_T("%024X%s"),nB,m_End);
					epcData.Format(_T("%024s%s"),strTemp,m_End);
					break;
				case 7:
					//epcData.Format(_T("%028X%s"),nB,m_End);
					epcData.Format(_T("%028s%s"),strTemp,m_End);
					break;
				default:
					//MessageBox(_T("后数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_F0069"), theApp.g_LoadString("IDWQF_F0070"), MB_ICONWARNING);
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

				epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				m_ListInData.SetItemText(nRow, j, epcData);
			}
		}

		if(m_CEnd&&m_CBefor)
		{
			nBLen=m_Befro.GetLength(); 
			if((nBLen<4)||((nBLen%4)!=0)||(nBLen>=m_Word*4))
			{
				//MessageBox(_T("前缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0071"), theApp.g_LoadString("IDWQF_F0072"), MB_ICONWARNING);
				return;
			}
			str=m_Befro.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=nBLen)
			{
				//MessageBox(_T("前缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0073"), theApp.g_LoadString("IDWQF_F0074"), MB_ICONWARNING);
				return;
			}

			nELen=m_End.GetLength(); 
			if((nELen<4)||((nELen%4)!=0)||(nELen>=m_Word*4))
			{
				//MessageBox(_T("后缀数据错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0075"), theApp.g_LoadString("IDWQF_F0076"), MB_ICONWARNING);
				return;
			}
			str=m_End.SpanIncluding(_T("0123456789ABCDEFabcdef"));

			if(str.GetLength()!=nELen)
			{
				//MessageBox(_T("后缀数据错误:0-9,A-F"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0077"), theApp.g_LoadString("IDWQF_F0078"), MB_ICONWARNING);
				return;
			}

			if((nBLen+nELen)>=m_Word*4)
			{
				//MessageBox(_T("前缀和后缀数据长度错误"),_T("警告"),MB_ICONWARNING);
				MessageBox(theApp.g_LoadString("IDWQF_F0079"), theApp.g_LoadString("IDWQF_F0080"), MB_ICONWARNING);
				return;
			}

			m_ListInData.DeleteAllItems();
			int nB=m_BeginData;

			for(i=0;i<m_Sum;i++)
			{
				switch((((m_Word*4)-nBLen-nELen)/4))
				{
				case 1:
					//epcData.Format(_T("%s%04X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%04s%s"),m_Befro,strTemp,m_End);
					break;
				case 2:
					//epcData.Format(_T("%s%08X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%08s%s"),m_Befro,strTemp,m_End);
					break;
				case 3:
					//epcData.Format(_T("%s%012X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%012s%s"),m_Befro,strTemp,m_End);
					break;
				case 4:
					//epcData.Format(_T("%s%016X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%016s%s"),m_Befro,strTemp,m_End);
					break;
				case 5:
					//epcData.Format(_T("%s%020X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%020s%s"),m_Befro,strTemp,m_End);
					break;
				case 6:
					//epcData.Format(_T("%s%024X%s"),m_Befro,nB,m_End);
					epcData.Format(_T("%s%024s%s"),m_Befro,strTemp,m_End);
					break;
				default:
					//MessageBox(_T("后数据设置错误"),_T("警告"),MB_ICONWARNING);
					MessageBox(theApp.g_LoadString("IDWQF_F0081"), theApp.g_LoadString("IDWQF_F0082"), MB_ICONWARNING);
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

				epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				m_ListInData.SetItemText(nRow, j, epcData);
			}
		}

		if(!m_CEnd&&!m_CBefor)
		{
			m_ListInData.DeleteAllItems();

			int nB=m_BeginData;

			for(i=0;i<m_Sum;i++)
			{
				switch(m_Word)
				{
				case 1:
					//epcData.Format(_T("%04X"),nB);
					epcData.Format(_T("%04s"),strTemp);
					break;
				case 2:
					//epcData.Format(_T("%08X"),nB);
					epcData.Format(_T("%08s"),strTemp);
					break;
				case 3:
					//epcData.Format(_T("%012X"),nB);
					epcData.Format(_T("%012s"),strTemp);
					break;
				case 4:
					//epcData.Format(_T("%016X"),nB);
					epcData.Format(_T("%016s"),strTemp);
					break;
				case 5:
					//epcData.Format(_T("%020X"),nB);
					epcData.Format(_T("%020s"),strTemp);
					break;
				case 6:
					//epcData.Format(_T("%024X"),nB);
					epcData.Format(_T("%024s"),strTemp);
					break;
				case 7:
					//epcData.Format(_T("%028X"),nB);
					epcData.Format(_T("%028s"),strTemp);
					break;
				default:
					//epcData.Format(_T("%032X"),nB);
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

				epcData.MakeUpper();

				int nS=m_ListInData.GetItemCount(); 

				tmp.Format(_T("%d"),nS+1);

				int nRow=m_ListInData.InsertItem(nS,tmp); 

				m_ListInData.SetItemText(nRow, j, epcData);
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

void CWriteISO6B::OnBnClickedButtonFile()
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
			AfxMessageBox(theApp.g_LoadString("IDWQF_F0083"));
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
			AfxMessageBox(theApp.g_LoadString("IDWQF_F0084"));
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

void CWriteISO6B::OnCbnSelendokComboWord()
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

//WM_MSG,add by martrin 写用户区
static DWORD WINAPI ThreadWrite(LPVOID lpParameter)
{
	CWriteISO6B *epcDlg = (CWriteISO6B *)lpParameter;

	int t;
	apiReturn res;
	int i,j,k,nCounter=0,ID_len=0,ID_len_temp=0;
	CString str,str_temp;
	BYTE temp[512];
	//BYTE IDBuffer[30*256];
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
			SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,0,0);//列表数据完成或连接断开
			epcDlg->nStop=0;
			break;
		}

		if(!ifConnectReader)
		{
			SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,0,1);//读写器连接已断开断开，请重新连接
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
			res=ISO6B_ReadLabelID(m_hScanner,&(ISO6B_IDBuffer[0][0]),&nCounter,0);
			break;
		}

		if(res==_OK)
		{
			str.Empty(); 
			ID_len=ISO6B_IDBuffer[0][1];//6B是定长的ID,8byte
			memcpy(IDTemp,&ISO6B_IDBuffer[0][2],ID_len*2);
			for(i=0;i<ID_len*2;i++)
			{
				str_temp.Format(_T("%02X"),IDTemp[i]); 
				str=str+str_temp.Left(2);
			}
			str_temp=epcDlg->m_ListInData.GetItemText(0,1);

			//if((str==str_temp)||(str_temp.GetLength()<=0))
			if((str_temp.GetLength()>0))
			{
				if(epcDlg->m_SetUse)
				{
					//取列表数据，并检查数据
					str_temp=epcDlg->m_ListInData.GetItemText(0,1); 

					str=str_temp.SpanIncluding(_T("0123456789ABCDEFabcdef"));

					if(str.GetLength()!=str_temp.GetLength())
					{
						SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,100,NULL);//待写入数据错误
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
					mask[0]=(BYTE)(len*2);
					for(i=1;i<=(len*2);i++)
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
							ptr = 30;
							len = str.GetLength()/2+1;
							res=ISO6B_WriteAByte(m_hScanner,IDTemp,ptr,len,mask,0);
							break;
						}

						if(res==_OK)
						{
							res = _OK;
							break;
						}
						else
						{
							j++;
							t=res*1000+j;
							SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,103,t);//用户区数据写入失败，请放入卡:%d-%d
							//Sleep(1000);
						}
					}
				}

				

				SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,1,0);//写入成功，发送成功消息
			}
			else
			{
				SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,102,0);//卡错误，请更换正确的卡
				Sleep(1000);
			}

		}
		else
		{
			j++;
			t=res*1000+j;
			SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,101,t);//等待写卡，请放入卡:%d-%d
			Sleep(100);
		}

		//写入间隔时间
		 for(i=0;i<(int)epcDlg->m_Time;i++)
		 {
			 Sleep(1000);
			 SendMessage(epcDlg->m_hWnd,WM_MSG_W6B,2,i);//等待写卡，请放入卡:%d,i为时间
		 }
	}

	return 0;
}


void CWriteISO6B::OnBnClickedButtonWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	UpdateData(TRUE);


	//////////////////////////////////////////////////////////////////////////
	iFlagProtectOpt	=	0;//0---不保护(不锁), 1---使用密码锁，2---永远锁住(死了)

	int k = 0;
	if ( 0 == iFlagProtectOpt )
	{
	}
	else
	{
		//k	=	MessageBox(str, _T("警告"),MB_YESNO|MB_ICONWARNING);
		k	=	MessageBox(str, theApp.g_LoadString("IDWQF_F0089"),MB_YESNO|MB_ICONWARNING);
		if ( k != 6 )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	int iCount=m_ListInData.GetItemCount();
	if(iCount==0)
	{
		//MessageBox(_T("待写入数据列表中没有数据"),_T("提示"),0);
		MessageBox(theApp.g_LoadString("IDWQF_F0090"), theApp.g_LoadString("IDWQF_F0091"), 0);
		return;
	}

	if(!ifConnectReader)
	{
		//MessageBox(_T("读写器未连接"),_T("提示"),0);
		MessageBox(theApp.g_LoadString("IDWQF_F0092"), theApp.g_LoadString("IDWQF_F0093"), 0);
		return;
	}

// 	if(nWRStatus!=1)
// 	{
// 		//MessageBox(_T("读写器正在使用中"),_T("提示"),0);
//		//MessageBox(theApp.g_LoadString("IDWQF_F0094"), theApp.g_LoadString("IDWQF_F0095"), 0);
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

	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(FALSE);

	if(m_SetEPC)
	{
		
		MessageBox(_T("Error_abcd!"),_T("Error"),MB_ICONWARNING);
		
	}
	else
	{
		CreateThread(NULL,0,ThreadWrite,this,0,NULL);
	}
}

void CWriteISO6B::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码

	nStop=0;
	//nWRStatus=1;

	
	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);

	
}



void CWriteISO6B::OnBnClickedButtonMove()
{
	//取写入列表的值，增加到完成列表

	int nTotal=m_ListInData.GetItemCount();
	if(nTotal<=0)
	{
		return;
	}

	CString sNo  =m_ListInData.GetItemText(0,0);
	CString sEPC =m_ListInData.GetItemText(0,1);
	

	int nRow=m_ListOutData.InsertItem(0,sNo);
	m_ListOutData.SetItemText(nRow, 1, sEPC);
	//m_ListOutData.SetItemText(nRow, 2, _T("失败"));
	m_ListOutData.SetItemText(nRow, 2, theApp.g_LoadString("IDWQF_F0096"));

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

void CWriteISO6B::OnCbnKillfocusComboWord()
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

LRESULT CWriteISO6B::WriteMsg(WPARAM wParam, LPARAM lParam)
{
	CString tmp;

	switch(wParam)
	{
	case 0:  //列表数据完成或连接断开
		nStop=0;
		//nWRStatus=1;
		
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
		if(lParam)
		{
			//MessageBox(_T("读写器连接已断开断开，请重新连接"),_T("警告"),MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_F0097"), theApp.g_LoadString("IDWQF_F0098"), MB_ICONWARNING);
		}
		break;
	case 1:  //数据写入完成
		if(lParam==0)
		{
			//记录当前的写入数据
			stInData.nLog=1;
			stInData.stNo  =m_ListInData.GetItemText(0,0);
			stInData.stEPC =m_ListInData.GetItemText(0,1);
			

			//增加完成列表
			int nRow=m_ListOutData.InsertItem(0,stInData.stNo);
			m_ListOutData.SetItemText(nRow, 1, stInData.stEPC);
			//m_ListOutData.SetItemText(nRow, 2, _T("成功"));
			m_ListOutData.SetItemText(nRow, 2, theApp.g_LoadString("IDWQF_F0099"));

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
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0100"));
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
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0101"));
		}

		break;
	case 2:
		//tmp.Format(_T("等待写卡，请放入卡:%d"),lParam); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0102"),lParam); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_WAIT));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 3:
		if(lParam==0)
		{
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		    //m_MsgDis.SetWindowText(_T("重复写卡，请更换下一张卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0103"));
		}
		else
		{
			m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		    //m_MsgDis.SetWindowText(_T("有多张卡存在，请放入一张卡"));
			m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0104"));
		}
		break;
	case 100://待写入数据错误
		nStop=0;
		//nWRStatus=1;
		
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DELALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DATA)->EnableWindow(TRUE);
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		//m_MsgDis.SetWindowText(_T("错误，待写入的数据错误"));
		m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0105"));
		//MessageBox(_T("待写入的数据错误"),_T("警告"),MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_F0106"), theApp.g_LoadString("IDWQF_F0107"), MB_ICONWARNING);
		break;
	case 101:
		//tmp.Format(_T("等待写卡或写卡失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0108"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_WAIT));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 102:
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		//m_MsgDis.SetWindowText(_T("卡错误，请更换正确的卡"));
		m_MsgDis.SetWindowText(theApp.g_LoadString("IDWQF_F0109"));
		break;
	case 103:
		//tmp.Format(_T("用户区数据写入失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0110"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
	case 104:
		//tmp.Format(_T("密码区数据写入失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0111"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 105:
		//tmp.Format(_T("读原密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0112"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 106:
		//tmp.Format(_T("写新密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0113"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
	
	case 107:
		//tmp.Format(_T("锁EPC失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0114"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;

	case 108:
		//tmp.Format(_T("锁密码失败，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_F0115"),lParam/1000,lParam%1000); 
		m_MsgImg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ERR));
		m_MsgDis.SetWindowText(tmp);
		break;
		
	}

	return 0;
}

void CWriteISO6B::OnBnClickedButtonDelall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListInData.DeleteAllItems(); 
}

void CWriteISO6B::OnBnClickedButtonDelout()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListOutData.DeleteAllItems(); 
}

void CWriteISO6B::OnBnClickedRadioEpc()
{
	// TODO: 在此添加控件通知处理程序代码
	m_WriteMode=0;
}

void CWriteISO6B::OnBnClickedRadioUser()
{
	// TODO: 在此添加控件通知处理程序代码

	m_WriteMode=1;

}

void CWriteISO6B::OnBnClickedRadioPass()
{
	// TODO: 在此添加控件通知处理程序代码
	m_WriteMode=2;
}

void CWriteISO6B::OnBnClickedButtonIndata()
{
	// TODO: 在此添加控件通知处理程序代码
	//取当前日期，时间
	SYSTEMTIME sysTime;
	
	GetLocalTime(&sysTime);
	CString strTime;
	
	strTime.Format(_T("%04d%02d%02d%02d%02d%02d.csv"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);


	CString filename;
	CFileDialog dlg(TRUE,NULL,strTime,OFN_EXPLORER ,_T("TXT files(*.csv)|*.*|")); 
    if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();

		CFileFind tempFind; 
        BOOL IsFinded = tempFind.FindFile(filename);

		if(!IsFinded)
		{
			//AfxMessageBox(_T("数据文件选择错误!"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_F0116"));
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

		//if(m_SetEPC)
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

void CWriteISO6B::OnBnClickedCheckEpc()
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

	return;

}

void CWriteISO6B::OnBnClickedCheckUse()
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

void CWriteISO6B::OnBnClickedCheckPass()
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

void CWriteISO6B::OnBnClickedButtonSuccess()
{
	// TODO: 在此添加控件通知处理程序代码

	int nRow=m_ListOutData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要导出!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_F0117"));
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
		
		if(sEpcTmp.GetLength()>0)
		{
			nEpc=1;
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
		sStatus=m_ListOutData.GetItemText(nRow-i,2);
		//if(sStatus==_T("成功"))
		if(sStatus==theApp.g_LoadString("IDWQF_F0118"))
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

void CWriteISO6B::OnBnClickedButtonError()
{
	// TODO: 在此添加控件通知处理程序代码

	int nRow=m_ListOutData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要导出!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_F0119"));
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
		if(sEpcTmp.GetLength()>0)
		{
			nEpc=1;
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
		sStatus=m_ListOutData.GetItemText(nRow-i,2);
		//if(sStatus==_T("失败"))
		if(sStatus==theApp.g_LoadString("IDWQF_F0120"))
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

void CWriteISO6B::OnBnClickedCheckLockepcPass()
{
	// TODO: 在此添加控件通知处理程序代码
	//使用密码写
	//如果使用密码写，打勾，那么永远不可写去掉打勾，



}

void CWriteISO6B::OnBnClickedCheckLockepcNeverwriteable()
{
	// TODO: 在此添加控件通知处理程序代码
	//永远不可写
	//如果使用永远不可写，打勾了，那么使用密码写去掉打勾。


}
#endif

BOOL CWriteISO6B::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	//限制只能输入数字和ABCDEF
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
	
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}
