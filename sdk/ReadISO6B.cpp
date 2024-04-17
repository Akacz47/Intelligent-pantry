// ReadISO6B.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "ReadISO6B.h"

#include <io.h>
#include <odbcinst.h>
#include <afxdb.h>
#pragma comment(lib, "shlwapi")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
HWND	g6BReadHwnd	=	NULL;	//读写器6C读窗体句柄
extern BYTE ISO6B_IDBuffer[MAX_LABELS][ID_MAX_SIZE_64BIT+1];

/////////////////////////////////////////////////////////////////////////////
// CReadISO6B property page

IMPLEMENT_DYNCREATE(CReadISO6B, CPropertyPage)

CReadISO6B::CReadISO6B() : CPropertyPage(CReadISO6B::IDD)
, m_Time(0)
, m_CheckTID(FALSE)
, m_CheckPass(FALSE)
, m_CheckUser(FALSE)
, m_TIDB(0)
, m_PassB(0)
, m_UserB(0)
, m_TIDL(0)
, m_PassL(0)
, m_UserL(0)
{
	//{{AFX_DATA_INIT(CReadISO6B)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CReadISO6B::~CReadISO6B()
{
}

void CReadISO6B::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReadISO6B)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListShowData);
	DDX_Text(pDX, IDC_EDIT_TIME, m_Time);
	DDX_Control(pDX, IDC_STATIC_DIS, m_Dis);
	DDX_Control(pDX, IDC_STATIC_OUT, m_Out);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReadISO6B, CPropertyPage)
	//{{AFX_MSG_MAP(CReadISO6B)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_MESSAGE(WM_READISO6B,ReadMsg)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_DELONE, OnBnClickedButtonDelone)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_6BREADDATAMSG, On6bReaddatamsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadISO6B message handlers
void CReadISO6B::OnInitUIText()//界面和文本更新
{
	SetWindowText(theApp.g_LoadString("IDWQF_G0001"));//读ISO18000-6B类型电子标签
	
	
	GetDlgItem(IDC_BUTTON1)->SetWindowText(theApp.g_LoadString("IDWQF_G0002"));//开始读取
	GetDlgItem(IDC_STATIC_R6ReadList)->SetWindowText(theApp.g_LoadString("IDWQF_G0003"));//读取数据列表
	GetDlgItem(IDC_BUTTON_STOP)->SetWindowText(theApp.g_LoadString("IDWQF_G0014"));//停止读取
	GetDlgItem(IDC_STATIC_R6BETTIME)->SetWindowText(theApp.g_LoadString("IDWQF_G0015"));//间隔时间:秒
	GetDlgItem(IDC_STATIC_DIS)->SetWindowText(theApp.g_LoadString("IDWQF_G0016"));//状态
	GetDlgItem(IDC_STATIC_R6ReadStat)->SetWindowText(theApp.g_LoadString("IDWQF_G0017"));//读取状态
	GetDlgItem(IDC_STATIC_R6ReadData)->SetWindowText(theApp.g_LoadString("IDWQF_G0018"));//读取数据
	GetDlgItem(IDC_STATIC_OUT)->SetWindowText(theApp.g_LoadString("IDWQF_G0019"));//数据
	GetDlgItem(IDC_BUTTON_DELONE)->SetWindowText(theApp.g_LoadString("IDWQF_G0020"));//删除选中记录
	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(theApp.g_LoadString("IDWQF_G0021"));//保存到TXT文件
	GetDlgItem(IDC_BUTTON_DELALL)->SetWindowText(theApp.g_LoadString("IDWQF_G0022"));//清空数据列表
	GetDlgItem(IDC_BUTTON_SAVE2)->SetWindowText(theApp.g_LoadString("IDWQF_G0023"));//保存到Excel文件
	
	

	m_ListShowData.DeleteColumn(0);
	m_ListShowData.DeleteColumn(0);
	m_ListShowData.DeleteColumn(0);
	m_ListShowData.InsertColumn( 0, theApp.g_LoadString("IDWQF_G0024"), LVCFMT_LEFT, 40 );
    m_ListShowData.InsertColumn( 1, theApp.g_LoadString("IDWQF_G0025"), LVCFMT_LEFT, 300);
	m_ListShowData.InsertColumn( 2, theApp.g_LoadString("IDWQF_G0026"), LVCFMT_LEFT, 300);

}

void CReadISO6B::UpdateStatus()//控制使能与不使能
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

BOOL CReadISO6B::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	g6BReadHwnd	=	this->GetSafeHwnd();

	DWORD dwStyle = m_ListShowData.GetExtendedStyle(); 
	
    //选中某行使整行高度
    dwStyle |= LVS_EX_FULLROWSELECT;
    dwStyle |= LVS_EX_GRIDLINES;//网格线
	
    m_ListShowData.SetExtendedStyle(dwStyle); //设置扩展风格
	
	
	
    //m_ListInData.SetBkColor(RGB(200, 200, 200)); //设置背景颜色
    //m_ListInData.SetTextBkColor(RGB(200, 200, 200)); //设置文本背景颜色
    //m_ListInData.SetTextColor(RGB(10, 10, 80)); //设置文本颜色
	
	
    //插入列的标题
    //m_ListShowData.InsertColumn( 0, _T("序号"), LVCFMT_LEFT, 40 );
    //m_ListShowData.InsertColumn( 1, _T(" ID "), LVCFMT_LEFT, 300);
	//m_ListShowData.InsertColumn( 2, _T(" User "), LVCFMT_LEFT, 300);
    
	m_ListShowData.InsertColumn( 0, theApp.g_LoadString("IDWQF_G0024"), LVCFMT_LEFT, 40 );
    m_ListShowData.InsertColumn( 1, theApp.g_LoadString("IDWQF_G0025"), LVCFMT_LEFT, 300);
	m_ListShowData.InsertColumn( 2, theApp.g_LoadString("IDWQF_G0026"), LVCFMT_LEFT, 300);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CReadISO6B::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	//6B读页
	::PostMessage(g6BReadHwnd,WM_6BREADDATAMSG,2,0);	//convert language

	return CPropertyPage::OnSetActive();
}

BOOL CReadISO6B::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}


//WM_6BREADDATAMSG
LRESULT CReadISO6B::On6bReaddatamsg(WPARAM wParam, LPARAM lParam)
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

static DWORD WINAPI ThreadReadEPC(LPVOID lpParameter)
{
	CReadISO6B *epcDlg = (CReadISO6B *)lpParameter;

	int t;
	apiReturn res=_no_scanner;
	apiReturn res1=_no_scanner;
	int i,j=0,nCounter=0,ID_len=0,ID_len_temp=0;
	CString str,str_temp;
	BYTE temp[512];
	//BYTE IDBuffer[30*256];
    BYTE DB[128];
	BYTE mask[512],IDTemp[512];
	int mem,ptr,len,EPC_Word;
	BYTE AccessPassword[4];

	int nRow;

	while(epcDlg->nStop)
	{
		if(!ifConnectReader)
		{
			SendMessage(epcDlg->m_hWnd,WM_READISO6B,0,1);
			epcDlg->nStop=0;
			break;
		}

		len=0;	ptr=0;	mem=1;	ID_len=0;  nRow=0;
		memset(mask,0,512);
		
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
			memcpy(temp,&ISO6B_IDBuffer[0][2],ID_len*2);
			for(i=0;i<ID_len*2;i++)
			{
				str_temp.Format(_T("%02X"),temp[i]); 
				str=str+str_temp;
			}
			epcDlg->stEpc=str;
			
			while(epcDlg->nStop)
			{

				memset(AccessPassword,0,4);
				EPC_Word=ID_len;
				memcpy(IDTemp,&ISO6B_IDBuffer[0][2],ID_len*2);
				mem=2;
				ptr=0;
				len=4;

				Sleep(30);
				switch(ConnectMode)
				{
				case 0:		
					break;
				case 1:
				case 3://usb
					ptr = 30;
					len = 2;//为什么是3,因为历史原因，如返回:F0 03 F6 08 0F,从03开始拷,拷2,最后就是要的数03 08,取08
					res = _no_scanner;
					res1=ISO6B_ReadByteBlock(m_hScanner,IDTemp,ptr,len,&(DB[0]),0);
					if ( res1 == _OK )
					{
						
						ptr = 31;
						len = DB[1];
						res=ISO6B_ReadByteBlock(m_hScanner,IDTemp,ptr,len,&(DB[1]),0);
						break;

					}
					
					break;
				}

				if(res==_OK)
				{
					str.Empty(); 
					memset(temp,0,512);
					DB[0]	=	len;
					memcpy(temp,DB+1,len);
					for(i=0;i<len;i++)
					{
						str_temp.Format(_T("%02X"),temp[i]);
						str=str+str_temp;

					}
					epcDlg->stUsr=str;
					break;
				}
				else
				{
					//epcDlg->stTid=_T("错误");
					epcDlg->stUsr=_T("");
					j++;
					t=1*1000+j;
					SendMessage(epcDlg->m_hWnd,WM_READISO6B,101,t);
					Sleep(1);

				}
			}

			

			SendMessage(epcDlg->m_hWnd,WM_READISO6B,1,0);
		}
		else
		{
			j++;
			t=1*1000+j;
			SendMessage(epcDlg->m_hWnd,WM_READISO6B,101,t);
			Sleep(1);
		}

		for(i=0;i<(int)epcDlg->m_Time;i++)
		{
			 //Sleep(1000);
			Sleep(100);
			SendMessage(epcDlg->m_hWnd,WM_READISO6B,2,i);
		}
	}

	return 0;
}

void CReadISO6B::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	if(!ifConnectReader)
	{
		//MessageBox(_T("读写器未连接"),_T("提示"),0);
		MessageBox(theApp.g_LoadString("IDWQF_G0031"), theApp.g_LoadString("IDWQF_G0032"), 0);
		return;
	}

// 	if(nWRStatus!=1)
// 	{
// 		MessageBox(_T("读写器正在使用中"),_T("提示"),0);
// 		return;
// 	}
// 	else
// 	{
// 		nWRStatus=2;
// 	}

	nStop=1;

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);

	CreateThread(NULL,0,ThreadReadEPC,this,0,NULL);	
}

void CReadISO6B::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	nStop=0;
	//nWRStatus=1;

	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);

}

LRESULT CReadISO6B::ReadMsg(WPARAM wParam, LPARAM lParam)
{
	CString strTemp;
	CString tmp;
	int nS,nRow,i;
	switch(wParam)
	{
	case 0:
		nStop=0;
		//nWRStatus=1;
		if(lParam)
		{
			//MessageBox(_T("读写器连接已断开断开，请重新连接"),_T("警告"),MB_ICONWARNING);
			MessageBox(theApp.g_LoadString("IDWQF_G0033"), theApp.g_LoadString("IDWQF_G0034"), MB_ICONWARNING);
		}
		break;
	case 1: 
		nS=m_ListShowData.GetItemCount();
		for(i=0;i<nS;i++)
		{
			strTemp	=	m_ListShowData.GetItemText(i,1);
			if(stEpc==strTemp)
			{
				break;
			}
		}
		if(i==nS)
		{
			//新增
			tmp.Format(_T("%d"),nS+1);
			//nRow=m_ListShowData.InsertItem(0,tmp); 
			nRow=m_ListShowData.InsertItem(nS+1,tmp); 
			m_ListShowData.SetItemText(nRow, 1,stEpc);
			m_ListShowData.SetItemText(nRow, 2,stUsr);
			
		}
// 		else
// 		{
// 			m_ListShowData.SetItemText(i, 2,stUsr);
// 		}

		
		tmp=stEpc;
		m_Out.SetWindowText(tmp);
		break;
	case 2:
		//tmp.Format(_T("等待读卡，请放入卡:%d"),lParam); 
		tmp.Format(theApp.g_LoadString("IDWQF_G0035"),lParam); 
		m_Dis.SetWindowText(tmp);
		break;
	case 101:
		//tmp.Format(_T("等待读卡，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
		tmp.Format(theApp.g_LoadString("IDWQF_G0036"),lParam/1000,lParam%1000); 
		m_Dis.SetWindowText(tmp);
		break;

	}

	return 0;
}

void CReadISO6B::OnBnClickedButtonDelall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListShowData.DeleteAllItems(); 
}

void CReadISO6B::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	int nRow=m_ListShowData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要保存!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_G0037"));
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

	CString sEpcTmp,sUseTmp,sPassTmp,strtmp,sTID;
	
	CFile f;

	f.Open(filename,CFile::modeCreate|CFile::modeReadWrite);
	
    CString tmp=_T(",");

	BYTE str[1024];
	int len;

	for(int i=0;i<nRow;i++)
	{
		strtmp.Empty(); 
		sEpcTmp=m_ListShowData.GetItemText(i,1);
		sUseTmp=m_ListShowData.GetItemText(i,2);
		
		
		strtmp=sEpcTmp;
		
		strtmp=strtmp+sUseTmp;
			
		strtmp=strtmp+_T("\r\n");
		len=0;
		memset(str,0,1024);
		//WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
		len=strtmp.GetLength();
		memcpy(str, strtmp, len);

		f.Write(str,len); 

	}

	f.Close(); 
}

void CReadISO6B::OnBnClickedButtonDelone()
{
	// TODO: 在此添加控件通知处理程序代码
	int nRow=m_ListShowData.GetSelectedCount();

	if(nRow<=0)
	{
		return;
	}
	int  nItem = -1;
    nItem = m_ListShowData.GetNextItem(nItem, LVNI_SELECTED);

	m_ListShowData.DeleteItem(nItem); 

}

//////////////////////////////////////////////////////////////////////////////
//名称：GetExcelDriver
//功能：获取ODBC中Excel驱动
//作者：
//组织：未来工作室(Future Studio)
//日期：2002.9.1
/////////////////////////////////////////////////////////////////////////////
static CString GetExcelDriver()
{
	CHAR szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	CHAR *pszBuf = szBuf;
	CString sDriver;

	// 获取已安装驱动的名称(涵数在odbcinst.h里)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return _T("");

	// 检索已安装的驱动是否有Excel...
	do
	{
		if (strstr(pszBuf, _T("Excel")) != 0)
		{
			//发现 !
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, _T('\0')) + 1;
	}
	while (pszBuf[1] != _T('\0'));

	return sDriver;
}

///////////////////////////////////////////////////////////////////////////////
//	BOOL MakeSurePathExists( CString &Path,bool FilenameIncluded)
//	参数：
//		Path				路径
//		FilenameIncluded	路径是否包含文件名
//	返回值:
//		文件是否存在
//	说明:
//		判断Path文件(FilenameIncluded=true)是否存在,存在返回TURE，不存在返回FALSE
//		自动创建目录
//
///////////////////////////////////////////////////////////////////////////////
static BOOL MakeSurePathExists( CString &Path,
						bool FilenameIncluded)
{
	int Pos=0;
	while((Pos=Path.Find('\\',Pos+1))!=-1)
		CreateDirectory(Path.Left(Pos),NULL);
	if(!FilenameIncluded)
		CreateDirectory(Path,NULL);
	//	return ((!FilenameIncluded)?!_access(Path,0):
	//	!_access(Path.Left(Path.ReverseFind('\\')),0));

	return !_access(Path,0);
}

//获得默认的文件名
static BOOL GetDefaultXlsFileName(CString& sExcelFile)
{
	///默认文件名：yyyymmddhhmmss.xls
	CString timeStr;
	CTime day;
	day=CTime::GetCurrentTime();
	int filenameday,filenamemonth,filenameyear,filehour,filemin,filesec;
	filenameday=day.GetDay();//dd
	filenamemonth=day.GetMonth();//mm月份
	filenameyear=day.GetYear();//yyyy
	filehour=day.GetHour();//hh
	filemin=day.GetMinute();//mm分钟
	filesec=day.GetSecond();//ss
	timeStr.Format(_T("%04d%02d%02d%02d%02d%02d"), filenameyear, filenamemonth, filenameday, filehour, filemin, filesec);

	sExcelFile =  timeStr + _T(".xls");
	// prompt the user (with all document templates)
	CFileDialog dlgFile(FALSE,_T(".xls"),sExcelFile);
	CString title;
	CString strFilter;

	title = _T("导出");
	strFilter = _T("Excel文件(*.xls)");
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.xls");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;
	dlgFile.m_ofn.nFilterIndex = 1;
	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;

	if (dlgFile.DoModal()==IDCANCEL)
		return FALSE; // open cancelled
	sExcelFile.ReleaseBuffer();
	if (MakeSurePathExists(sExcelFile,true)) {
		if(!DeleteFile(sExcelFile)) {    // delete the file
			//AfxMessageBox(_T("覆盖文件时出错！"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_G0038"));
			return FALSE;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	void GetExcelDriver(CListCtrl* pList, CString strTitle)
//	参数：
//		pList		需要导出的List控件指针
//		strTitle	导出的数据表标题
//	说明:
//		导出CListCtrl控件的全部数据到Excel文件。Excel文件名由用户通过“另存为”
//		对话框输入指定。创建名为strTitle的工作表，将List控件内的所有数据（包括
//		列名和数据项）以文本的形式保存到Excel工作表中。保持行列关系。
//	
//	
///////////////////////////////////////////////////////////////////////////////
static void ExportListToExcel(CListCtrl* pList, CString strTitle, CString sExcelFile)
{
	CString warningStr;
	if (pList->GetItemCount ()>0) {	
		CDatabase database;
		CString sDriver;
		//CString sExcelFile; 
		CString sSql;
		CString tableName = strTitle;

		// 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
		sDriver = GetExcelDriver();
		if (sDriver.IsEmpty())
		{
			// 没有发现Excel驱动
			//AfxMessageBox(_T("没有安装Excel!\n请先安装Excel软件才能使用导出功能!"));
			AfxMessageBox(theApp.g_LoadString("IDWQF_G0039"));
			return;
		}

		///默认文件名
		//if (!GetDefaultXlsFileName(sExcelFile))
		//	return;

		// 创建进行存取的字符串
		sSql.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"),sDriver, sExcelFile, sExcelFile);

		// 创建数据库 (既Excel表格文件)
		if( database.OpenEx(sSql,CDatabase::noOdbcDialog) )
		{
			// 创建表结构
			int i;
			LVCOLUMN columnData;
			CString columnName;
			int columnNum = 0;
			CString strH;
			CString strV;

			sSql = "";
			strH = "";
			columnData.mask = LVCF_TEXT;
			columnData.cchTextMax =100;
			columnData.pszText = columnName.GetBuffer (100);
			for(i=0;pList->GetColumn(i,&columnData);i++)
			{
				if (i!=0)
				{
					sSql = sSql + _T(", ") ;
					strH = strH + _T(", ") ;
				}
				sSql = sSql + _T(" ") + columnData.pszText +_T(" TEXT");
				strH = strH + _T(" ") + columnData.pszText +_T(" ");
			}
			columnName.ReleaseBuffer ();
			columnNum = i;

			sSql = _T("CREATE TABLE ") + tableName + _T(" ( ") + sSql +  _T(" ) ");
			database.ExecuteSQL(sSql);

			// 插入数据项
			int nItemIndex;
			for (nItemIndex=0;nItemIndex<pList->GetItemCount ();nItemIndex++)
			{
				strV = "";
				for(i=0;i<columnNum;i++)
				{
					if (i!=0)
					{
						strV = strV + _T(", ");
					}
					strV = strV + _T(" '") + pList->GetItemText(nItemIndex,i) +_T("' ");
				}

				sSql = _T("INSERT INTO ")+ tableName 
					+_T(" (")+ strH + _T(")")
					+_T(" VALUES(")+ strV + _T(")");
				database.ExecuteSQL(sSql);
			}

		}      

		// 关闭数据库
		database.Close();

		//warningStr.Format(_T("导出文件保存于%s!"),sExcelFile);
		warningStr.Format(theApp.g_LoadString("IDWQF_G0040"),sExcelFile);
		AfxMessageBox(warningStr);
	}
}

//在这里处理我们感兴趣的消息，想要哪个，截哪个! 47C为编辑的ID
static UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( WM_NOTIFY == uMsg )
	{
		LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
		if ( pofn->hdr.code == CDN_FILEOK )
		{
			// 加入这两句就可以了
			_AFX_THREAD_STATE*   pThreadState = AfxGetThreadState(); 
			pThreadState-> m_pAlternateWndInit = NULL;
		}
		else if ( pofn->hdr.code == CDN_TYPECHANGE )
		{
			int nIndex = pofn->lpOFN->nFilterIndex;
			HWND hwndDlg = ::GetParent(hDlg);
			HWND hwndFilter = ::GetDlgItem(hwndDlg, 0x470);
			//过滤器COMBOBOX                 
			char strExt[MAX_PATH] = "";
			WCHAR* wstr = (WCHAR*)SendMessage(hwndFilter, CB_GETITEMDATA, nIndex - 1, 0);
			WideCharToMultiByte (CP_OEMCP, NULL, wstr, -1, strExt, sizeof(strExt), NULL, FALSE);
			if (strcmp(strExt + 1, "*.*") != 0)
			{
				char szFileName[MAX_PATH] = "";
				//::GetDlgItemTextA(hwndDlg, 0x480, szFileName, sizeof(szFileName));//文件名EDIT 
				::GetDlgItemTextA(hwndDlg, 0x47C, szFileName, sizeof(szFileName));//文件名EDIT 
				PathRemoveExtensionA(szFileName);
				PathAddExtensionA(szFileName, strExt + 1);
				::SetDlgItemTextA(hwndDlg, 0x47C, szFileName);//文件名EDIT 
			} 
		}
		
	}

	return 1;
}

void CReadISO6B::OnBnClickedButtonSave2()
{
	// TODO: 在此添加控件通知处理程序代码
	//导出EXCEL
	//ExportListToExcel(&m_ListShowData, _T("九九乘法口诀表"));
	//return;

	//取当前日期，时间
	SYSTEMTIME sysTime;

	GetLocalTime(&sysTime);
	CString strTime;

	strTime.Format(_T("%04d%02d%02d.csv"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	int nRow=m_ListShowData.GetItemCount();
	if(nRow<=0)
	{
		//AfxMessageBox(_T("没有数据需要保存!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_G0041"));
		return;
	}

	CString fileext;
	CString filename;
	CFileDialog dlg(FALSE,_T(""),strTime,OFN_EXPLORER | OFN_ENABLEHOOK, _T("CSV files(*.csv)|*.csv|Worksheet Files (*.xls)|*.xls||")); 
	dlg.m_ofn.lpfnHook =  (LPOFNHOOKPROC)OFNHookProc;
	dlg.m_ofn.hInstance = AfxGetInstanceHandle();
	
	if(dlg.DoModal()==IDOK)
	{
		filename=dlg.GetPathName();
		//filename=dlg.GetFileName();

	}
	else
	{
		return;
	}
	fileext = dlg.GetFileExt();
	fileext.MakeLower();
	if ( fileext.Find(_T("csv")) >= 0 )
	{
		CString sEpcTmp,sUseTmp,sPassTmp,strtmp,sTID;

		CFile f;

		f.Open(filename,CFile::modeCreate|CFile::modeReadWrite);

		CString tmp=_T(",");

		BYTE str[1024]={0};
		int len;

		CListCtrl* pList = &m_ListShowData;
		LVCOLUMN columnData;
		CString columnName;
		columnData.mask = LVCF_TEXT;
		columnData.cchTextMax =100;
		columnData.pszText = columnName.GetBuffer (100);
		strtmp = _T("");
		for(int i=0;pList->GetColumn(i,&columnData);i++)
		{
			strtmp	+=	columnData.pszText;
			strtmp	=	strtmp+tmp;
			
		}
		strtmp = strtmp.Left(strtmp.GetLength()-1);
		strtmp=strtmp+_T("\r\n");
		len=0;
		memset(str,0,1024);
		//WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
		len=strtmp.GetLength();
		memcpy(str, strtmp, len);

		len = strlen((char*)str);
		f.Write(str,len);

		strtmp = _T("");
		for(int i=0;i<nRow;i++)
		{
			strtmp.Empty(); 
			strtmp=m_ListShowData.GetItemText(i,0);

			sEpcTmp=m_ListShowData.GetItemText(i,1);
			sUseTmp=m_ListShowData.GetItemText(i,2);
			

			strtmp=strtmp+tmp;
			strtmp=strtmp+sEpcTmp;
			strtmp=strtmp+sUseTmp;
				

			strtmp=strtmp+_T("\r\n");
			len=0;
			memset(str,0,1024);
			//WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
			len=strtmp.GetLength();
			memcpy(str, strtmp, len);

			f.Write(str,len); 

		}

		f.Close();
		//AfxMessageBox(_T("导出完成!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_G0042"));
	}
	else if ( fileext.Find(_T("xls")) >= 0 )
	{
		//ExportListToExcel(&m_ListShowData, _T("读取数据列表"), filename);
		ExportListToExcel(&m_ListShowData, theApp.g_LoadString("IDWQF_G0043"), filename);
	}
	else
	{
		//AfxMessageBox(_T("导出失败!"));
		AfxMessageBox(theApp.g_LoadString("IDWQF_G0044"));
	}

	return;

}

