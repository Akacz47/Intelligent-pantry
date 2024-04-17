// Param.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "Param.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
HWND	gReaderParamHwnd	=	NULL;	//读写器参数窗体句柄


/////////////////////////////////////////////////////////////////////////////
// CParam property page

//IMPLEMENT_DYNCREATE(CParam, CPropertyPage)

CParam::CParam() : CPropertyPage(CParam::IDD)
{
	//{{AFX_DATA_INIT(CParam)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CParam::~CParam()
{
}

void CParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//ClxDialogCProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParam)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParam, CPropertyPage)
	//{{AFX_MSG_MAP(CParam)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1_MSGET, OnButton1Msget)
	ON_BN_CLICKED(IDC_BUTTON1_MSUPDATE, OnButton1Msupdate)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_READERPARMDATAMSG, OnReaderparmdatamsg)
	ON_BN_CLICKED(IDC_RADIO1_Hex16, &CParam::OnBnClickedRadio1Hex16)
	ON_BN_CLICKED(IDC_RADIO1_Hex10, &CParam::OnBnClickedRadio1Hex10)
	ON_BN_CLICKED(IDC_RADIO1, &CParam::OnBnClickedRadio1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParam message handlers
void CParam::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_C0001"));//发卡机参数
	
	GetDlgItem(IDC_STATIC_HANDVER)->SetWindowText(theApp.g_LoadString("IDWQF_C0002"));//硬件版本号:
	GetDlgItem(IDC_STATIC_SOFTVER)->SetWindowText(theApp.g_LoadString("IDWQF_C0003"));//软件版本号:    
	GetDlgItem(IDC_STATIC_BASICPARAM)->SetWindowText(theApp.g_LoadString("IDWQF_C0004"));//基本参数
	GetDlgItem(IDC_STATIC_MINFREQ)->SetWindowText(theApp.g_LoadString("IDWQF_C0005"));//最小载波频率(MHz):
	GetDlgItem(IDC_STATIC_MAXFREQ)->SetWindowText(theApp.g_LoadString("IDWQF_C0006"));//最大载波频率(MHz):
	GetDlgItem(IDC_STATIC_OUTPOWER)->SetWindowText(theApp.g_LoadString("IDWQF_C0007"));//射频输出功率:
	GetDlgItem(IDC_STATIC_TAGOPT)->SetWindowText(theApp.g_LoadString("IDWQF_C0008"));//被读取标签类型
	GetDlgItem(IDC_STATIC_MSDATAF)->SetWindowText(theApp.g_LoadString("IDWQF_C0009"));//数据格式
	GetDlgItem(IDC_RADIO1_Hex16)->SetWindowText(theApp.g_LoadString("IDWQF_C0010"));//十六进制
	GetDlgItem(IDC_RADIO1_Hex10)->SetWindowText(theApp.g_LoadString("IDWQF_C0011"));//十进制IDWQF_C0040
	GetDlgItem(IDC_RADIO1)->SetWindowText(theApp.g_LoadString("IDWQF_C0040"));//ASCII码
	GetDlgItem(IDC_STATIC_MSDATABANK)->SetWindowText(theApp.g_LoadString("IDWQF_C0012"));//数据区
	GetDlgItem(IDC_RADIO_DATAID)->SetWindowText(theApp.g_LoadString("IDWQF_C0013"));//EPC号
	GetDlgItem(IDC_RADIO_DATAUSER)->SetWindowText(theApp.g_LoadString("IDWQF_C0014"));//用户区
	GetDlgItem(IDC_CHECK1_PERFIX)->SetWindowText(theApp.g_LoadString("IDWQF_C0015"));//前缀
	GetDlgItem(IDC_STATIC_PERFIXT)->SetWindowText(theApp.g_LoadString("IDWQF_C0016"));//前缀值:
	GetDlgItem(IDC_CHECK1_LF)->SetWindowText(theApp.g_LoadString("IDWQF_C0017"));//带回车换行
	GetDlgItem(IDC_STATIC_MSDATAB)->SetWindowText(theApp.g_LoadString("IDWQF_C0018"));//数据地址
	GetDlgItem(IDC_STATIC_MSDATAStart)->SetWindowText(theApp.g_LoadString("IDWQF_C0019"));//数据起始地址:
	GetDlgItem(IDC_STATIC_MSDATALEN)->SetWindowText(theApp.g_LoadString("IDWQF_C0020"));//长度:
	GetDlgItem(IDC_BUTTON1_MSUPDATE)->SetWindowText(theApp.g_LoadString("IDWQF_C0021"));//更新参数
	GetDlgItem(IDC_BUTTON1_MSGET)->SetWindowText(theApp.g_LoadString("IDWQF_C0022"));//获取参数
	GetDlgItem(IDC_STATIC_HARDID)->SetWindowText(theApp.g_LoadString("IDWQF_C0023"));//硬件ID:
	GetDlgItem(IDC_STATIC_READBET)->SetWindowText(theApp.g_LoadString("IDWQF_C0024"));//读卡间隔时间
	GetDlgItem(IDC_STATIC_OUTPUTBET)->SetWindowText(theApp.g_LoadString("IDWQF_C0025"));//标准输出间隔(1-255):

	
	
	
}

void CParam::UpdateStatus()//控制使能与不使能
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

BOOL CParam::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	//////////////////////////////////////////////////////////////////////////
	UpdateStatus();//控制使能与不使能

	if ( ifConnectReader )
	{
		//连上了的
		//参数更新
		OnGetParameter();
	}

	//参数页
	::PostMessage(gReaderParamHwnd,WM_READERPARMDATAMSG,2,0);	//convert language

	return CPropertyPage::OnSetActive();
}

BOOL CParam::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

BOOL CParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	gReaderParamHwnd	=	this->GetSafeHwnd();

	//读卡的持续时间
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->AddString("10ms");
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->AddString("20ms");
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->AddString("30ms");
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->AddString("50ms");
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->AddString("100ms");
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->SetCurSel(0);


	//版本只读
	GetDlgItem(IDC_EDIT_HANDVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HANDVER2)->EnableWindow(FALSE);

	//读写器ID
	GetDlgItem(IDC_EDIT_HARDID)->EnableWindow(FALSE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//sel为国家频率下标,imin为最小频率下标,imax为最大频率下标, -1为最小为最小，最大为最大
void CParam::OnCbnSetFre(int sel, int imin, int imax)
{
	int i = 0;
	int k = 0;
	int iCount = 0;
	long iValue = 0;
	CString strFreqTmp;
	tagReaderFreq tmpFreq;
	
	CComboBox *m_MinFrequency	=	(CComboBox*)GetDlgItem(IDC_COMBO_MINFREQ);
	CComboBox *m_MaxFrequency	=	(CComboBox*)GetDlgItem(IDC_COMBO_MAXFREQ);
	
	//频率计算公式
	//级数 = 50;
	//步进 = 500KHz;
	//起始频率 = 902750;
	//902750 + 级数*步进;
	m_MinFrequency->ResetContent();
	m_MaxFrequency->ResetContent();
	switch(sel)
	{
	case 0://{"0---FCC（美国）", 50, 500, 902750},								//(0)
	case 1://{"1---ETSI EN 300-220（欧洲300-220）", -1, -1, -1},				//(1)
	case 2://{"2---ETSI EN 302-208（欧洲302-208）", 4, 600, 865700},			//(2)
	case 3://"3---HK920-925香港", 10, 500, 920250},							//(3)
	case 4://{"4---TaiWan 922-928台湾", 12, 500, 922250},						//(4)
	case 5://{"5---Japan 952-954日本", 0, 0, 0},								//(5)
	case 6://{"6---Japan 952-955日本", 14,200, 952200},						//(6)
	case 7://{"7---ETSI EN 302-208欧洲", 4, 600, 865700},						//(7)
	case 8://{"8---Korea 917-921韩国", 6, 600, 917300},						//(8)
	case 9://{"9---Malaysia 919-923马来西亚", 8, 500, 919250},					//(9)
	case 10://{"10--China 920-925中国", 16, 250, 920625},						//(10)
	case 11://{"11--Japan 952-956日本", 4, 1200, 952400},						//(11)
	case 12://{"12--South Africa 915-919南美", 17, 200, 915600},				//(12)
	case 13://{"13--Brazil 902-907/915-928巴西", 35, 500, 902750},				//(13)
	case 14://{"14--Thailand 920-925泰国", -1, -1, -1},						//(14)
	case 15://{"15--Singapore 920-925新加坡", 10, 500, 920250},				//(15)
	case 16://{"16--Australia 920-926澳洲", 12, 500, 920250},					//(16)
	case 17://{"17--India 865-867印度", 4, 600, 865100},						//(17)
	case 18://{"18--Uruguay 916-928乌拉圭", 23, 500, 916250},					//(18)
	case 19://{"19--Vietnam 920-925越南", 10, 500, 920250},					//(19)
	case 20://{"20--Israel 915-917", 1, 0, 916250},							//(20)
	case 21://{"21--Philippines 918-920菲律宾", 4, 500, 918250},				//(21)
	case 22://{"22--Canada 902-928加拿大", 42, 500, 902750},					//(22)
	case 23://{"23--Indonesia 923-925印度尼西亚", 4, 500, 923250},				//(23)
	case 24://{"24--New Zealand 921.5-928新西兰", 11, 500, 922250},			//(24)
		tmpFreq.iGrade	=	stuctFreqCountry[sel].iGrade;
		tmpFreq.iSkip	=	stuctFreqCountry[sel].iSkip;
		tmpFreq.dwFreq	=	stuctFreqCountry[sel].dwFreq;
		k = 0;
		iCount	=	tmpFreq.iGrade;
		
		if ( 22 == sel )
		{
			int iArray[3] = {902750, 903250, 905750};
			//加拿大的特述
			for (i = 0; i < 3; i++)
			{
				strFreqTmp.Format("%02d--%d", i+1, iArray[i]);
				m_MinFrequency->AddString(strFreqTmp);
				m_MaxFrequency->AddString(strFreqTmp);
			}
			tmpFreq.dwFreq	=	iArray[i-1];
			iCount			=	tmpFreq.iGrade - 3;
			k = 3;
		}
		
		for (i = k; i < iCount + k; i++)
		{
			iValue	=	tmpFreq.dwFreq + i * tmpFreq.iSkip;
			strFreqTmp.Format("%02d--%d", i+1, iValue);
			m_MinFrequency->AddString(strFreqTmp);
			m_MaxFrequency->AddString(strFreqTmp);
		}
		
		iCount	=	tmpFreq.iGrade;
		if ( i > 0 )
		{
			if ( imin == -1 && imax == -1 )
			{
				m_MinFrequency->SetCurSel(0); 
				m_MaxFrequency->SetCurSel(iCount - 1); 
			}
			else
			{
				m_MinFrequency->SetCurSel(imin); 
				m_MaxFrequency->SetCurSel(imax); 
			}
		}
		
		break;
		
		
	default:;
	}
	
}

//连上了的
//参数更新
void CParam::OnGetParameter()
{
	CString strText;

	UpdateData(TRUE);

	//基本参数更新
	strText.Format("%04x", m_HardVersion);
	GetDlgItem(IDC_EDIT_HANDVER)->SetWindowText(strText);
	strText.Format("%04x", m_SoftVersion);
	GetDlgItem(IDC_EDIT_HANDVER2)->SetWindowText(strText);


	//读写器ID
	strText.Format("%s", gReaderID);
	GetDlgItem(IDC_EDIT_HARDID)->SetWindowText(strText);


	//最小频率
	//最大频率
	BYTE imin	=	gBasicParam->Min_Frequence-1;
	BYTE imax	=	gBasicParam->Max_Frequence-1;
	OnCbnSetFre(gFre, imin, imax);//因为通过国家来，每个国家的频率有些不一样的


	//RF power
	strText.Format("%d", gBasicParam->Power);
	GetDlgItem(IDC_EDIT_OUTPOWER)->SetWindowText(strText);


	//哪种标签
	//0-6B,1-6C
	((CButton*)GetDlgItem(IDC_RADIO_ISO180006B))->SetCheck((int)(gBasicParam->TagType & 0x01));
	((CButton*)GetDlgItem(IDC_RADIO_ISO180006C))->SetCheck((int)(gBasicParam->TagType & 0x04));
	

	//定时间隔
	//0-10ms，1-20ms，2-30ms，3-50ms，4-100ms。缺省值为2。每隔设定时间主动读取一次标签。
	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->SetCurSel(gAutoParam->Interval);

	//////////////////////////////////////////////////////////////////////////
	//16进制输出，还是10进制输出
	//0-16进,1-10进
	if ( 0 == Param.DataFormat )
	{
		((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(0);
	}
	else if ( 1 == Param.DataFormat )
	{
		((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(0);
	}
	else if( 2 == Param.DataFormat)
	{
		((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(1);
	}


	//数据区
	//(1)EPC区--0, USER区--1，TID区--2,add by mqs20131216改成了取自动模式下的参数
	//if ( Param.DataBank == 0 )
	if ( gAutoParam->IDPosition == 0 )
	{
		((CButton*)GetDlgItem(IDC_RADIO_DATAID))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_DATAUSER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DATATID))->SetCheck(0);
	}
	//else if ( Param.DataBank == 1 )
	else if ( gAutoParam->IDPosition == 1 )
	{
		((CButton*)GetDlgItem(IDC_RADIO_DATAID))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DATAUSER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_DATATID))->SetCheck(0);
	}
	//else if ( Param.DataBank == 2 )
	else if ( gAutoParam->IDPosition == 2 )
	{
		((CButton*)GetDlgItem(IDC_RADIO_DATAID))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DATAUSER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_DATATID))->SetCheck(1);
	}
	
	//是否前缀，1--带,0--不带		
	((CButton*)GetDlgItem(IDC_CHECK1_PERFIX))->SetCheck(Param.IsPerfix);

	//前缀值,不够0x00填充		
	strText.Format("%s", Param.PerfixCode);
	GetDlgItem(IDC_EDIT2_PERFIXVALUE)->SetWindowText(strText);
	
	//是否回车换行, 1--带,0--不带	
	((CButton*)GetDlgItem(IDC_CHECK1_LF))->SetCheck(Param.IsEnter);

	//数据起始地址
	strText.Format("%d", Param.StartAddress);
	GetDlgItem(IDC_EDIT2_MSDATAStart)->SetWindowText(strText);

	//数据长度
	strText.Format("%d", Param.DataLen);
	GetDlgItem(IDC_EDIT2_MSDATALEN)->SetWindowText(strText);

	//标准输出间隔(0-255)(0--输出ID之间无间隔)
	strText.Format("%d", Param.OutputInterval);
	GetDlgItem(IDC_EDIT_OUTPUTBET)->SetWindowText(strText);

	//////////////////////////////////////////////////////////////////////////

	UpdateData(FALSE);

	return;
}

void CParam::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CParam::OnButton1Msget() 
{
	// TODO: Add your control notification handler code here
	//获取参数
	int				i			=	0;
	apiReturn		res			=	_no_scanner;

	UpdateData(TRUE);

	
	for ( i = 0; i < 3; i++ )
	{
		//读基本参数
		res=ReadBasicParam(m_hScanner,gBasicParam);
		if ( _OK == res )
		{
			break;
		}
	}

	if ( _OK == res )
	{
	
		
	}
	else
	{
		//MessageBox("Get fail0!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0026"), theApp.g_LoadString("IDWQF_C0027"), MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////////////////////////////////////////


	for ( i = 0; i < 3; i++ )
	{
		//读auto参数
		res=ReadAutoParam(m_hScanner,gAutoParam);
		if ( _OK == res )
		{
			break;
		}
	}
	
	if ( _OK == res )
	{
		
		
	}
	else
	{
		//MessageBox("Get fail1!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0028"), theApp.g_LoadString("IDWQF_C0029"), MB_ICONWARNING);
		return;
	}
	//////////////////////////////////////////////////////////////////////////


	for ( i = 0; i < 3; i++ )
	{
		//读仿真参数
		res=ReadSimParam(m_hScanner,&Param);
		if ( _OK == res )
		{
			break;
		}
	}
	
	if ( _OK == res )
	{
		
		
	}
	else
	{
		//MessageBox("Get fail2!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0030"), theApp.g_LoadString("IDWQF_C0031"), MB_ICONWARNING);
		return;
	}

	//连上了的
	//参数更新
	OnGetParameter();

	UpdateData(FALSE);

	return;
}

void CParam::OnButton1Msupdate() 
{
	// TODO: Add your control notification handler code here
	//更新参数
	int				i			=	0;
	apiReturn		res			=	_no_scanner;
	CString			strText		=	_T("");
	int				iValue		=	0;

	UpdateData(TRUE);
	//最小频率
	//最大频率
	CComboBox *m_MinFrequency	=	(CComboBox*)GetDlgItem(IDC_COMBO_MINFREQ);
	CComboBox *m_MaxFrequency	=	(CComboBox*)GetDlgItem(IDC_COMBO_MAXFREQ);
	int imin	=	m_MinFrequency->GetCurSel();
	int imax	=	m_MaxFrequency->GetCurSel();
	if ( imin < 0 )
	{
		imin	=	0;
	}
	if ( imax < 0 )
	{
		imax	=	0;
	}
	//因为通过国家来，每个国家的频率有些不一样的
	gBasicParam->Min_Frequence = (BYTE)(imin+1);
	gBasicParam->Max_Frequence = (BYTE)(imax+1);
	
	//RF power
	GetDlgItem(IDC_EDIT_OUTPOWER)->GetWindowText(strText);
	iValue	=	atoi(strText);
	if ((iValue<0) || (iValue>63))
	{
		iValue	=	63;
	}
	gBasicParam->Power	=	(BYTE)iValue;
	
	
	//哪种标签,//0-6B,1-6C
	gBasicParam->TagType	=	0x00;
	//6C
	if ( ((CButton*)GetDlgItem(IDC_RADIO_ISO180006C))->GetCheck() == 1 )
	{
		gBasicParam->TagType	= 0x04;
	}
	else
	{
		gBasicParam->TagType	= 0x01;
	}

	for ( i = 0; i < 3; i++ )
	{
		//写基本参数
		res=WriteBasicParam(m_hScanner,gBasicParam);
		if ( _OK == res )
		{
			break;
		}
	}
	
	if ( _OK == res )
	{
		
		
	}
	else
	{
		//MessageBox("Set fail0!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0032"), theApp.g_LoadString("IDWQF_C0033"), MB_ICONWARNING);
		return;
	}
	

	//////////////////////////////////////////////////////////////////////////

	//定时间隔
	//0-10ms，1-20ms，2-30ms，3-50ms，4-100ms。缺省值为2。每隔设定时间主动读取一次标签。
	gAutoParam->Interval	=	((CComboBox*)GetDlgItem(IDC_COMBO_READBET))->GetCurSel();

	//数据区
	//(1)EPC区--0, USER区--1，TID区--2,add by mqs20131216改成了取自动模式下的参数
	if ( ((CButton*)GetDlgItem(IDC_RADIO_DATAID))->GetCheck() == 1 )
	{
		//Param.DataBank	=	0;
		gAutoParam->IDPosition	=	0;
	}
	if ( ((CButton*)GetDlgItem(IDC_RADIO_DATAUSER))->GetCheck() == 1 )
	{
		//Param.DataBank	=	1;
		gAutoParam->IDPosition	=	1;
	}
	if ( ((CButton*)GetDlgItem(IDC_RADIO_DATATID))->GetCheck() == 1 )
	{
		//Param.DataBank	=	2;
		gAutoParam->IDPosition	=	2;
	}

	for ( i = 0; i < 3; i++ )
	{
		//写auto参数
		res=WriteAutoParam(m_hScanner,gAutoParam);
		if ( _OK == res )
		{
			break;
		}
	}
	
	if ( _OK == res )
	{
		
		
	}
	else
	{
		//MessageBox("Set fail1!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0034"), theApp.g_LoadString("IDWQF_C0035"), MB_ICONWARNING);
		return;
	}
	


	//////////////////////////////////////////////////////////////////////////
	//16进制输出，还是10进制输出
	//0-16进,1-10进
	if ( ((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->GetCheck() == 1 )
	{
		Param.DataFormat	=	0;
	}
	if ( ((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->GetCheck() == 1 )
	{
		Param.DataFormat	=	1;
	}
	if(  ((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->GetCheck() == 1 )

		Param.DataFormat    =   2;
	
	
	//是否前缀，1--带,0--不带		
	Param.IsPerfix	=	((CButton*)GetDlgItem(IDC_CHECK1_PERFIX))->GetCheck();
	
	//前缀值,不够0x00填充		
	GetDlgItem(IDC_EDIT2_PERFIXVALUE)->GetWindowText(strText);
	sscanf(strText, "%s", Param.PerfixCode);
	
	//是否回车换行, 1--带,0--不带	
	Param.IsEnter	=	((CButton*)GetDlgItem(IDC_CHECK1_LF))->GetCheck();
	
	//数据起始地址
	GetDlgItem(IDC_EDIT2_MSDATAStart)->GetWindowText(strText);
	Param.StartAddress	=	atoi(strText);
	//sscanf(strText, "%d", Param.StartAddress);
	
	//数据长度
	GetDlgItem(IDC_EDIT2_MSDATALEN)->GetWindowText(strText);
	Param.DataLen	=	atoi(strText);
	//sscanf(strText, "%d", Param.DataLen);
	
	
	//标准输出间隔(0-255)(0--输出ID之间无间隔)
	GetDlgItem(IDC_EDIT_OUTPUTBET)->GetWindowText(strText);
	Param.OutputInterval	=	atoi(strText);
	//sscanf(strText, "%d", Param.OutputInterval);
	
	//////////////////////////////////////////////////////////////////////////

	for ( i = 0; i < 3; i++ )
	{
		//写仿真参数
		res=WriteSimParam(m_hScanner,&Param);
		if ( _OK == res )
		{
			break;
		}
	}
	
	if ( _OK == res )
	{
		
		
	}
	else
	{
		//MessageBox("Set fail2!","Warning",MB_ICONWARNING);
		MessageBox(theApp.g_LoadString("IDWQF_C0036"), theApp.g_LoadString("IDWQF_C0037"), MB_ICONWARNING);
		return;
	}
	
	UpdateData(FALSE);

	//MessageBox("Set paramter successful!","Info",MB_ICONINFORMATION);
	MessageBox(theApp.g_LoadString("IDWQF_C0038"), theApp.g_LoadString("IDWQF_C0039"), MB_ICONINFORMATION);
	
	return;

}

//WM_READERPARMDATAMSG
afx_msg LRESULT CParam::OnReaderparmdatamsg(WPARAM wParam, LPARAM lParam)
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



void CParam::OnBnClickedRadio1Hex16()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(0);
}

void CParam::OnBnClickedRadio1Hex10()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(0);
}

void CParam::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton*)GetDlgItem(IDC_RADIO1_Hex16))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO1_Hex10))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO1_ASCII))->SetCheck(1);
}
