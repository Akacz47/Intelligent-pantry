// ReadISO6C.cpp : implementation file
//

#include "stdafx.h"
#include "WriteCard.h"
#include "ReadISO6C.h"

#include <io.h>
#include <odbcinst.h>
#include <afxdb.h>
#pragma comment(lib, "shlwapi")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////参数界面定时器//////////////////////////////////////////
//定义定时器的ID
//时间
#define					TIME_IDENTIFIER_PARAM			(300)			//时间ID号
#define					INTERVALTIME_PARAM				(100)			//多长时间执行一次
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
HWND	g6CReadHwnd = NULL;	//读写器6C读窗体句柄
DWORD   g6CReadTheardID = 0;

/////////////////////////////////////////////////////////////////////////////
// CReadISO6C property page

IMPLEMENT_DYNCREATE(CReadISO6C, CPropertyPage)

CReadISO6C::CReadISO6C() : CPropertyPage(CReadISO6C::IDD)
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
  //{{AFX_DATA_INIT(CReadISO6C)
    // NOTE: the ClassWizard will add member initialization here
  m_EasState = 0;
  m_AAccessPassword = _T("");
  m_EasAlarm = FALSE;
  //}}AFX_DATA_INIT

  m_ScanExitThreadEvent = INVALID_HANDLE_VALUE;
  m_ReadEpcHandle = INVALID_HANDLE_VALUE;
}

CReadISO6C::~CReadISO6C()
{
}

void CReadISO6C::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CReadISO6C)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  DDX_Control(pDX, IDC_LIST_SHOW, m_ListShowData);
  DDX_Text(pDX, IDC_EDIT_TIME, m_Time);
  DDX_Control(pDX, IDC_STATIC_DIS, m_Dis);
  DDX_Check(pDX, IDC_CHECK_TID, m_CheckTID);
  DDX_Check(pDX, IDC_CHECK_PASS, m_CheckPass);
  DDX_Check(pDX, IDC_CHECK_USER, m_CheckUser);
  DDX_Text(pDX, IDC_EDIT_TIDB, m_TIDB);
  DDX_Text(pDX, IDC_EDIT_PASSB, m_PassB);
  DDX_Text(pDX, IDC_EDIT_USERB, m_UserB);
  DDX_Text(pDX, IDC_EDIT_TIDL, m_TIDL);
  DDX_Text(pDX, IDC_EDIT_PASSL, m_PassL);
  DDX_Text(pDX, IDC_EDIT_USERL, m_UserL);
  DDX_Control(pDX, IDC_STATIC_OUT, m_Out);
  DDX_Radio(pDX, IDC_RADIO23_NoAlarm_6C, m_EasState);
  DDX_Text(pDX, IDC_EDIT12, m_AAccessPassword);
  DDX_Check(pDX, IDC_CHECK7_EasAlarm_6C, m_EasAlarm);
  DDX_Control(pDX, IDC_EASALARM_STATIC, m_APic);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReadISO6C, CPropertyPage)
  //{{AFX_MSG_MAP(CReadISO6C)
  ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
  ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
  ON_MESSAGE(WM_READISO6C, ReadMsg)
  ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
  ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
  ON_BN_CLICKED(IDC_BUTTON_DELONE, OnBnClickedButtonDelone)
  ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
  ON_BN_CLICKED(IDC_BUTTON5_SetAlarm_6C, OnBnClickedButton5SetAlarm6C)
  ON_BN_CLICKED(IDC_CHECK7_EasAlarm_6C, OnBnClickedCheck7EasAlarm6C)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_6CREADDATAMSG, On6cReaddatamsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadISO6C message handlers
void CReadISO6C::OnInitUIText()//界面和文本更新
{
  SetWindowText(theApp.g_LoadString("IDWQF_E0001"));//读ISO18000-6C类型电子标签


  GetDlgItem(IDC_BUTTON1)->SetWindowText(theApp.g_LoadString("IDWQF_E0002"));//开始读取
  GetDlgItem(IDC_STATIC_R6ReadList)->SetWindowText(theApp.g_LoadString("IDWQF_E0003"));//读取数据列表
  GetDlgItem(IDC_CHECK_TID)->SetWindowText(theApp.g_LoadString("IDWQF_E0004"));//TID区
  GetDlgItem(IDC_CHECK_PASS)->SetWindowText(theApp.g_LoadString("IDWQF_E0005"));//密码区
  GetDlgItem(IDC_CHECK_USER)->SetWindowText(theApp.g_LoadString("IDWQF_E0006"));//用户区
  GetDlgItem(IDC_STATIC_TIDSTARTPOS)->SetWindowText(theApp.g_LoadString("IDWQF_E0007"));//起始位置(Word)
  GetDlgItem(IDC_STATIC_PASSSTARTPOS)->SetWindowText(theApp.g_LoadString("IDWQF_E0008"));//起始位置(Word)
  GetDlgItem(IDC_STATIC_USERSTARTPOS)->SetWindowText(theApp.g_LoadString("IDWQF_E0009"));//起始位置(Word)
  GetDlgItem(IDC_STATIC_R6DATAPART)->SetWindowText(theApp.g_LoadString("IDWQF_E0010"));//其余数据区
  GetDlgItem(IDC_STATIC_TIDLEN)->SetWindowText(theApp.g_LoadString("IDWQF_E0011"));//长度(Word)
  GetDlgItem(IDC_STATIC_PASSLEN)->SetWindowText(theApp.g_LoadString("IDWQF_E0012"));//长度(Word)
  GetDlgItem(IDC_STATIC_USERLEN)->SetWindowText(theApp.g_LoadString("IDWQF_E0013"));//长度(Word)
  GetDlgItem(IDC_BUTTON_STOP)->SetWindowText(theApp.g_LoadString("IDWQF_E0014"));//停止读取
  GetDlgItem(IDC_STATIC_R6BETTIME)->SetWindowText(theApp.g_LoadString("IDWQF_E0015"));//间隔时间:秒
  GetDlgItem(IDC_STATIC_DIS)->SetWindowText(theApp.g_LoadString("IDWQF_E0016"));//状态
  GetDlgItem(IDC_STATIC_R6ReadStat)->SetWindowText(theApp.g_LoadString("IDWQF_E0017"));//读取状态
  GetDlgItem(IDC_STATIC_R6ReadData)->SetWindowText(theApp.g_LoadString("IDWQF_E0018"));//读取数据
  GetDlgItem(IDC_STATIC_OUT)->SetWindowText(theApp.g_LoadString("IDWQF_E0019"));//数据
  GetDlgItem(IDC_BUTTON_DELONE)->SetWindowText(theApp.g_LoadString("IDWQF_E0020"));//删除选中记录
  GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(theApp.g_LoadString("IDWQF_E0021"));//保存到TXT文件
  GetDlgItem(IDC_BUTTON_DELALL)->SetWindowText(theApp.g_LoadString("IDWQF_E0022"));//清空数据列表
  GetDlgItem(IDC_BUTTON_SAVE2)->SetWindowText(theApp.g_LoadString("IDWQF_E0023"));//保存到Excel文件

  GetDlgItem(IDC_STATIC_AlarmCC_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0045"));//报警
  GetDlgItem(IDC_STATIC_SelectaTagAE_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0046"));//选择一个标签
  //GetDlgItem(IDC_BUTTON_SAVE2)->SetWindowText(theApp.g_LoadString("IDWQB_E0047"));//Eas状态
  GetDlgItem(IDC_RADIO23_NoAlarm_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0048"));//不报警
  GetDlgItem(IDC_RADIO24_Alarm_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0049"));//报警
  GetDlgItem(IDC_STATIC_AccessPSDDA_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0050"));//访问密码(8HEX):
  GetDlgItem(IDC_BUTTON5_SetAlarm_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0051"));//设置报警
  GetDlgItem(IDC_CHECK7_EasAlarm_6C)->SetWindowText(theApp.g_LoadString("IDWQB_E0052"));//检测报警

  GetDlgItem(IDC_STATIC_GroupREAD)->SetWindowText(theApp.g_LoadString("IDWQB_E0053"));//读取数据




  m_ListShowData.DeleteColumn(0);
  m_ListShowData.DeleteColumn(0);
  m_ListShowData.DeleteColumn(0);
  m_ListShowData.DeleteColumn(0);
  m_ListShowData.DeleteColumn(0);
  m_ListShowData.DeleteColumn(0);
  m_ListShowData.InsertColumn(0, theApp.g_LoadString("IDWQF_E0024"), LVCFMT_LEFT, 40);
  m_ListShowData.InsertColumn(1, theApp.g_LoadString("IDWQF_E0025"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(3, theApp.g_LoadString("IDWQF_E0026"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(4, theApp.g_LoadString("IDWQF_E0027"), LVCFMT_LEFT, 120);
  m_ListShowData.InsertColumn(5, theApp.g_LoadString("IDWQF_E0028"), LVCFMT_LEFT, 120);


}

void CReadISO6C::UpdateStatus()//控制使能与不使能
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

BOOL CReadISO6C::OnInitDialog()
{
  CPropertyPage::OnInitDialog();

  // TODO: Add extra initialization here
  g6CReadHwnd = this->GetSafeHwnd();

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
  //m_ListShowData.InsertColumn( 1, _T("EPC区"), LVCFMT_LEFT, 200);
  //m_ListShowData.InsertColumn( 2, _T("用户区"), LVCFMT_LEFT, 200);
//m_ListShowData.InsertColumn( 3, _T("TID区"), LVCFMT_LEFT, 120);
//m_ListShowData.InsertColumn( 4, _T("密码区"), LVCFMT_LEFT, 120);

  m_ListShowData.InsertColumn(0, theApp.g_LoadString("IDWQF_E0024"), LVCFMT_LEFT, 40);
  m_ListShowData.InsertColumn(1, theApp.g_LoadString("IDWQF_E0025"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(2, theApp.g_LoadString("IDWQF_C0042"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(3, theApp.g_LoadString("IDWQF_E0026"), LVCFMT_LEFT, 200);
  m_ListShowData.InsertColumn(4, theApp.g_LoadString("IDWQF_E0027"), LVCFMT_LEFT, 120);
  m_ListShowData.InsertColumn(5, theApp.g_LoadString("IDWQF_E0028"), LVCFMT_LEFT, 120);

  m_Time = 1;

  nStop = 0;

  m_TIDL = 4;
  m_PassL = 4;
  m_UserL = 6;

  UpdateData(FALSE);


  GetDlgItem(IDC_BUTTON5_SetAlarm_6C)->EnableWindow(FALSE);//设置报警
  GetDlgItem(IDC_CHECK7_EasAlarm_6C)->EnableWindow(FALSE);//检测报警
  GetDlgItem(IDC_EDIT12)->SetWindowText(_T("11112222"));

  //BOOL b=m_menuMain.LoadMenu(IDR_MENU2);
  //b=this->SetMenu(&m_menuMain);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CReadISO6C::OnSetActive()
{
  // TODO: Add your specialized code here and/or call the base class
  //6C读页
  ::PostMessage(g6CReadHwnd, WM_6CREADDATAMSG, 2, 0);	//convert language

  return CPropertyPage::OnSetActive();
}

BOOL CReadISO6C::OnKillActive()
{
  // TODO: Add your specialized code here and/or call the base class
  if (INVALID_HANDLE_VALUE != m_ScanExitThreadEvent)
  {
    SetEvent(m_ScanExitThreadEvent);
    Sleep(1000);
    CloseHandle(m_ScanExitThreadEvent);
    m_ScanExitThreadEvent = INVALID_HANDLE_VALUE;
  }

  if (INVALID_HANDLE_VALUE != m_ReadEpcHandle)
  {
    TerminateThread(m_ReadEpcHandle, 0);
    CloseHandle(m_ReadEpcHandle);
    m_ReadEpcHandle = INVALID_HANDLE_VALUE;
  }

  return CPropertyPage::OnKillActive();
}


//WM_6CREADDATAMSG
LRESULT CReadISO6C::On6cReaddatamsg(WPARAM wParam, LPARAM lParam)
{
  int iwParam = (int)wParam;
  int ilParam = (int)lParam;

  if (2 == iwParam)
  {
    OnInitUIText();	//更新语言
  }
  else if (1 == iwParam)
  {
    OnEasAlarmTest();//检测报警测试
  }


  return 0;
}

static DWORD WINAPI ThreadReadEPC(LPVOID lpParameter)
{
  CReadISO6C* epcDlg = (CReadISO6C*)lpParameter;

  int t;
  apiReturn res;
  int i, j, nCounter = 0, ID_len = 0, ID_len_temp = 0;
  CString str, str_temp;
  BYTE temp[512];
  BYTE IDBuffer[30 * 256];
  BYTE DB[128];
  BYTE mask[512], IDTemp[512];
  int mem, ptr, len, EPC_Word;
  BYTE AccessPassword[4];

  int nRow;
  int iCheckTid = epcDlg->m_CheckTID;//
  apiReturn resTid = _no_scanner;

  int iCheckUsr = epcDlg->m_CheckUser;//
  apiReturn resUsr = _no_scanner;

  int iCheckPas = epcDlg->m_CheckPass;//
  apiReturn resPas = _no_scanner;

  while (epcDlg->nStop)
  {

    resTid = _no_scanner;
    resUsr = _no_scanner;
    resPas = _no_scanner;

    if (WaitForSingleObject(epcDlg->m_ScanExitThreadEvent, 0) == WAIT_OBJECT_0)
    {
      epcDlg->nStop = FALSE;
      break;
    }

    if (!ifConnectReader)
    {
      SendMessage(epcDlg->m_hWnd, WM_READISO6C, 0, 1);
      epcDlg->nStop = 0;
      break;
    }

    len = 0;	ptr = 0;	mem = 1;	ID_len = 0;  j = 0;   nRow = 0;
    memset(mask, 0, 512);

    for (i = 0; i < 3; i++)
    {
      Sleep(300);
      switch (ConnectMode)
      {
      case 0:
        break;
      case 1:
      case 3://usb,读EPC号
        res = EPC1G2_ReadLabelID(m_hScanner, mem, ptr, len, mask, IDBuffer, &nCounter, RS485Address);
        break;
      }
      if (_OK == res)
      {
        break;
      }
    }

    if (res == _OK)
    {
      str.Empty();
      ID_len = (int)IDBuffer[0];  //IDBuffer[0]为Word总数;1word=2Byte
      memcpy(temp, &IDBuffer[1], ID_len * 2);
      for (i = 0; i < ID_len * 2; i++)
      {
        str_temp.Format(_T("%02X"), temp[i]);
        str = str + str_temp;
      }
      epcDlg->stEpc = str;



      //如果选中TID则读之
      if (epcDlg->m_CheckTID)
      {

        memset(AccessPassword, 0, 4);
        EPC_Word = ID_len;
        memcpy(IDTemp, &IDBuffer[1], ID_len * 2);
        mem = 2;
        ptr = epcDlg->m_TIDB;
        len = epcDlg->m_TIDL;

        for (i = 0; i < 3; i++)
        {
          Sleep(300);
          switch (ConnectMode)
          {
          case 0:
            break;
          case 1:
          case 3://usb
            res = EPC1G2_ReadWordBlock(m_hScanner, EPC_Word, IDTemp, mem, ptr, len, &DB[0], AccessPassword, RS485Address);
            break;
          }
          if (_OK == res)
          {
            resTid = _OK;
            break;
          }
        }
        if (res == _OK)
        {
          str.Empty();
          memset(temp, 0, 512);
          memcpy(temp, DB, len * 2);
          for (i = 0; i < len * 2; i++)
          {
            str_temp.Format(_T("%02X"), temp[i]);
            str = str + str_temp;

          }
          epcDlg->stTid = str;
        }
        else
        {
          //epcDlg->stTid=_T("错误");
          //epcDlg->stTid=theApp.g_LoadString("IDWQF_E0029");

          //epcDlg->stTid=_T("");//modify by martrin 20140512
        }
      }

      //如果选中USER则读之
      if (epcDlg->m_CheckUser)
      {
        memset(AccessPassword, 0, 4);
        EPC_Word = ID_len;
        memcpy(IDTemp, &IDBuffer[1], ID_len * 2);
        mem = 3;
        ptr = epcDlg->m_UserB;
        len = epcDlg->m_UserL;

        for (i = 0; i < 3; i++)
        {
          Sleep(300);
          switch (ConnectMode)
          {
          case 0:
            break;
          case 1:
          case 3://usb
            res = EPC1G2_ReadWordBlock(m_hScanner, EPC_Word, IDTemp, mem, ptr, len, &DB[0], AccessPassword, RS485Address);
            break;
          }
          if (_OK == res)
          {
            resUsr = _OK;
            break;
          }
        }

        if (res == _OK)
        {
          str.Empty();
          memset(temp, 0, 512);
          memcpy(temp, DB, len * 2);
          for (i = 0; i < len * 2; i++)
          {
            str_temp.Format(_T("%02X"), temp[i]);
            str = str + str_temp;
          }
          epcDlg->stUsr = str;
        }
        else
        {
          //epcDlg->stUsr=_T("错误");
          //epcDlg->stUsr=theApp.g_LoadString("IDWQF_E0030");
          //epcDlg->stUsr=_T("");//modify by martrin 20140512
        }

      }

      //如果选中PASSWORD则读之
      if (epcDlg->m_CheckPass)
      {
        memset(AccessPassword, 0, 4);
        EPC_Word = ID_len;
        memcpy(IDTemp, &IDBuffer[1], ID_len * 2);
        mem = 0;
        ptr = epcDlg->m_PassB;
        len = epcDlg->m_PassL;

        memset(DB, 0, 128);

        for (i = 0; i < 3; i++)
        {
          Sleep(300);
          switch (ConnectMode)
          {
          case 0:
            break;
          case 1:
          case 3://usb
            res = EPC1G2_ReadWordBlock(m_hScanner, EPC_Word, IDTemp, mem, ptr, len, &DB[0], AccessPassword, RS485Address);
            break;
          }
          if (_OK == res)
          {
            resPas = _OK;
            break;
          }
        }
        if (res == _OK)
        {
          str.Empty();
          memset(temp, 0, 512);
          memcpy(temp, DB, len * 2);
          for (i = 0; i < len * 2; i++)
          {
            str_temp.Format(_T("%02X"), temp[i]);
            str = str + str_temp;
          }
          epcDlg->stPass = str;
        }
        else
        {
          //epcDlg->stPass=_T("错误");
          //epcDlg->stPass=theApp.g_LoadString("IDWQF_E0030");
          //epcDlg->stPass=_T("");//modify by martrin 20140512
        }
      }

      SendMessage(epcDlg->m_hWnd, WM_READISO6C, 1, 0);

      int ichFlag = 0;
      if (iCheckTid && iCheckUsr && iCheckPas) //全选
      {
        if (resTid == _OK && resPas == _OK && resUsr == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckTid && iCheckUsr)
      {
        if (resTid == _OK && resUsr == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckTid && iCheckPas)
      {
        if (resTid == _OK && resPas == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckUsr && iCheckPas)
      {
        if (resUsr == _OK && resPas == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckTid)
      {
        if (resTid == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckUsr)
      {
        if (resUsr == _OK)
        {
          ichFlag = 1;
        }
      }
      else if (iCheckPas)
      {
        if (resPas == _OK)
        {
          ichFlag = 1;
        }
      }
      else
      {
        ichFlag = 1;

      }


      if (1 == ichFlag)
      {
        //开始报警
        TRACE("x:Buzzer\n");
        if (3 == ConnectMode)
        {
          BuzzerAlarm(m_hScanner, 1);
        }

        Sleep(100);

        //停止报警
        if (3 == ConnectMode)
        {
          BuzzerAlarm(m_hScanner, 0);
        }
      }


    }
    else
    {
      j++;
      t = res * 1000 + j;
      SendMessage(epcDlg->m_hWnd, WM_READISO6C, 101, t);
      Sleep(500);
    }

    for (i = 0; i < (int)epcDlg->m_Time; i++)
    {
      //Sleep(1000);
      Sleep(100);
      SendMessage(epcDlg->m_hWnd, WM_READISO6C, 2, i);
    }
  }

  return 0;
}

void CReadISO6C::OnBnClickedButton1()
{
  // TODO: 在此添加控件通知处理程序代码

  UpdateData(TRUE);

  if (!ifConnectReader)
  {
    //MessageBox(_T("读写器未连接"),_T("提示"),0);
    MessageBox(theApp.g_LoadString("IDWQF_E0031"), theApp.g_LoadString("IDWQF_E0032"), 0);
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

  nStop = 1;

  GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
  GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);

  GetDlgItem(IDC_BUTTON5_SetAlarm_6C)->EnableWindow(FALSE);//设置报警
  GetDlgItem(IDC_CHECK7_EasAlarm_6C)->EnableWindow(FALSE);//检测报警

  m_ScanExitThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_ReadEpcHandle = CreateThread(NULL, 0, ThreadReadEPC, this, 0, &g6CReadTheardID);
}

void CReadISO6C::OnBnClickedButtonStop()
{
  // TODO: 在此添加控件通知处理程序代码
  //nStop=0;
  //nWRStatus=1;

  SetEvent(m_ScanExitThreadEvent);
  Sleep(1000);
  CloseHandle(m_ScanExitThreadEvent);
  m_ScanExitThreadEvent = NULL;

  TerminateThread(m_ReadEpcHandle, 0);
  CloseHandle(m_ReadEpcHandle);
  m_ReadEpcHandle = INVALID_HANDLE_VALUE;

  GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
  GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);

  GetDlgItem(IDC_BUTTON5_SetAlarm_6C)->EnableWindow(TRUE);//设置报警
  GetDlgItem(IDC_CHECK7_EasAlarm_6C)->EnableWindow(TRUE);//检测报警

  nStop = 0;


  int nRow = m_ListShowData.GetItemCount();
  if (nRow <= 0)
  {
    return;
  }


  CString sEpcTmp, sUseTmp, sPassTmp, strtmp, sTID;


  CString tmp = _T(",");

  BYTE str[1024];
  int len;

  ((CComboBox*)GetDlgItem(IDC_COMBO6))->ResetContent();

  for (int i = 0; i < nRow; i++)
  {
    strtmp.Empty();
    sEpcTmp = m_ListShowData.GetItemText(i, 1);


    strtmp = sEpcTmp;

    len = 0;
    memset(str, 0, 1024);
    //WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
    len = strtmp.GetLength();
    memcpy(str, strtmp, len);

    ((CComboBox*)GetDlgItem(IDC_COMBO6))->AddString((char*)str);

  }

  ((CComboBox*)GetDlgItem(IDC_COMBO6))->SetCurSel(0);
  return;

}

LRESULT CReadISO6C::ReadMsg(WPARAM wParam, LPARAM lParam)
{
  CString strTemp;
  CString tmp;
  int nS, nRow, i;
  switch (wParam)
  {
  case 0:
    nStop = 0;
    //nWRStatus=1;
    if (lParam)
    {
      //MessageBox(_T("读写器连接已断开断开，请重新连接"),_T("警告"),MB_ICONWARNING);
      MessageBox(theApp.g_LoadString("IDWQF_E0033"), theApp.g_LoadString("IDWQF_E0034"), MB_ICONWARNING);
    }
    break;
  case 1:
    nS = m_ListShowData.GetItemCount();
    for (i = 0; i < nS; i++/*i+=2*/)
    {
      strTemp = m_ListShowData.GetItemText(i, 1);
      if (stEpc == strTemp)
      {
        break;
      }
    }
    if (i == nS)
    {
      //新增
      tmp.Format(_T("%d"), nS + 1);
      nRow = m_ListShowData.InsertItem(nS + 1, tmp);
      m_ListShowData.SetItemText(nRow, 1, stEpc);



      byte str1[256] = { 0 };
      StrToHex(str1, (byte*)stEpc.GetBuffer(), stEpc.GetLength());
      for (int j = 0; j < stEpc.GetLength() / 2; j++)
        if (str1[j] == 0)
          str1[j] = ' ';
      m_ListShowData.SetItemText(nRow, 2, (char*)str1);



      if (m_CheckUser)
        m_ListShowData.SetItemText(nRow, 3, stUsr);
      if (m_CheckTID)
      {
        m_ListShowData.SetItemText(nRow, 4, stTid);
      }
      if (m_CheckPass)
        m_ListShowData.SetItemText(nRow, 5, stPass);

      /*tmp = " ";
      nRow=m_ListShowData.InsertItem(nS+2,tmp);

      byte str1[256] = { 0 };
      char strc[2] = {0};
      int sum = 0;

      CString strD[4];
      strD[0] = stEpc;strD[1] = stUsr;strD[2] = stTid;strD[3] = stPass;

      for(int j = 1; j < 5; j++)
      {
        StrToHex((byte*)str1, (byte*)strD[j - 1].GetBuffer(), strD[j - 1].GetLength());
        for(int k = 0; k < strD[j - 1].GetLength()/2; k++)
        {
          if(str1[k] == 0)
            str1[k] = ' ';
        }
        m_ListShowData.SetItemText(nRow , j,(char*)str1);
      }	*/

      m_ListShowData.EnsureVisible(nRow, FALSE);
      //m_ListShowData.EnsureVisible(nRow + 1, FALSE);

    }
    else
    {
      if (m_CheckUser)
        m_ListShowData.SetItemText(i, 3, stUsr);
      if (m_CheckTID)
      {
        if (FALSE == stTid.IsEmpty())
        {
          //不为空给值
          m_ListShowData.SetItemText(i, 4, stTid);
          stTid = _T("");
        }
      }
      if (m_CheckPass)
        m_ListShowData.SetItemText(i, 5, stPass);

      /*	byte str1[256] = { 0 };

        CString strD[4];
        strD[0] = stEpc;strD[1] = stUsr;strD[2] = stTid;strD[3] = stPass;

        for(int j = 1; j < 5; j++)
        {
          HexToStr(str1, (byte*)strD[j-1].GetBuffer(), strD[j-1].GetLength());
          m_ListShowData.SetItemText(i*2 , j,(char*)str1);
        }
        m_ListShowData.EnsureVisible(i*2 , FALSE);*/

      m_ListShowData.EnsureVisible(i, FALSE);
    }


    tmp = stEpc;
    m_Out.SetWindowText(tmp);
    break;
  case 2:
    //tmp.Format(_T("等待读卡，请放入卡:%d"),lParam); 
    tmp.Format(theApp.g_LoadString("IDWQF_E0035"), lParam);
    m_Dis.SetWindowText(tmp);
    break;
  case 101:
    //tmp.Format(_T("等待读卡，请放入卡:%d-%d"),lParam/1000,lParam%1000); 
    tmp.Format(theApp.g_LoadString("IDWQF_E0036"), lParam / 1000, lParam % 1000);
    m_Dis.SetWindowText(tmp);
    break;

  }

  return 0;
}

void CReadISO6C::OnBnClickedButtonDelall()
{
  // TODO: 在此添加控件通知处理程序代码
  m_ListShowData.DeleteAllItems();
}

void CReadISO6C::OnBnClickedButtonSave()
{
  // TODO: 在此添加控件通知处理程序代码
  int nRow = m_ListShowData.GetItemCount();
  if (nRow <= 0)
  {
    //AfxMessageBox(_T("没有数据需要保存!"));
    AfxMessageBox(theApp.g_LoadString("IDWQF_E0037"));
    return;
  }

  SYSTEMTIME sysTime;
  char	chCurr[62];
  GetLocalTime(&sysTime);
  memset(chCurr, 0x00, sizeof(chCurr));
  sprintf(chCurr, "%04d%02d%02d%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

  CString filename;
  CFileDialog dlg(FALSE, NULL, chCurr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, _T("TXT files(*.txt)|*.TXT|"));
  if (dlg.DoModal() == IDOK)
  {
    filename = dlg.GetPathName();

  }
  else
  {
    return;
  }

  CString sEpcTmp, sEpcAscii, sUseTmp, sPassTmp, strtmp, sTID;

  CFile f;

  f.Open(filename, CFile::modeCreate | CFile::modeReadWrite);

  CString tmp = _T(",");

  BYTE str[1024];
  int len;

  for (int i = 0; i < nRow; i++)
  {
    strtmp.Empty();
    sEpcTmp = m_ListShowData.GetItemText(i, 1);
    sEpcAscii = m_ListShowData.GetItemText(i, 2);
    sUseTmp = m_ListShowData.GetItemText(i, 3);
    sTID = m_ListShowData.GetItemText(i, 4);
    sPassTmp = m_ListShowData.GetItemText(i, 5);


    //epc
    strtmp = strtmp + sEpcTmp;
    strtmp = strtmp + tmp;

    //ascii epc
    strtmp = strtmp + sEpcAscii;
    strtmp = strtmp + tmp;

    // user
    strtmp = strtmp + sUseTmp;
    strtmp = strtmp + tmp;

    // tid
    strtmp = strtmp + sTID;
    strtmp = strtmp + tmp;

    // user
    strtmp = strtmp + sUseTmp;
    strtmp = strtmp + tmp;

    strtmp = strtmp + _T("\r\n");
    len = 0;
    memset(str, 0, 1024);
    //WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
    len = strtmp.GetLength();
    memcpy(str, strtmp, len);

    f.Write(str, len);

  }

  f.Close();
}

void CReadISO6C::OnBnClickedButtonDelone()
{
  // TODO: 在此添加控件通知处理程序代码
  int nRow = m_ListShowData.GetSelectedCount();

  if (nRow <= 0)
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
  CHAR* pszBuf = szBuf;
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
  } while (pszBuf[1] != _T('\0'));

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
static BOOL MakeSurePathExists(CString& Path,
  bool FilenameIncluded)
{
  int Pos = 0;
  while ((Pos = Path.Find('\\', Pos + 1)) != -1)
    CreateDirectory(Path.Left(Pos), NULL);
  if (!FilenameIncluded)
    CreateDirectory(Path, NULL);
  //	return ((!FilenameIncluded)?!_access(Path,0):
  //	!_access(Path.Left(Path.ReverseFind('\\')),0));

  return !_access(Path, 0);
}

//获得默认的文件名
static BOOL GetDefaultXlsFileName(CString& sExcelFile)
{
  ///默认文件名：yyyymmddhhmmss.xls
  CString timeStr;
  CTime day;
  day = CTime::GetCurrentTime();
  int filenameday, filenamemonth, filenameyear, filehour, filemin, filesec;
  filenameday = day.GetDay();//dd
  filenamemonth = day.GetMonth();//mm月份
  filenameyear = day.GetYear();//yyyy
  filehour = day.GetHour();//hh
  filemin = day.GetMinute();//mm分钟
  filesec = day.GetSecond();//ss
  timeStr.Format(_T("%04d%02d%02d%02d%02d%02d"), filenameyear, filenamemonth, filenameday, filehour, filemin, filesec);

  sExcelFile = timeStr + _T(".xls");
  // prompt the user (with all document templates)
  CFileDialog dlgFile(FALSE, _T(".xls"), sExcelFile);
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

  if (dlgFile.DoModal() == IDCANCEL)
    return FALSE; // open cancelled
  sExcelFile.ReleaseBuffer();
  if (MakeSurePathExists(sExcelFile, true)) {
    if (!DeleteFile(sExcelFile)) {    // delete the file
      //AfxMessageBox(_T("覆盖文件时出错！"));
      AfxMessageBox(theApp.g_LoadString("IDWQF_E0038"));
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
  if (pList->GetItemCount() > 0) {
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
      AfxMessageBox(theApp.g_LoadString("IDWQF_E0039"));
      return;
    }

    ///默认文件名
    //if (!GetDefaultXlsFileName(sExcelFile))
    //	return;

    // 创建进行存取的字符串
    sSql.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"), sDriver, sExcelFile, sExcelFile);

    // 创建数据库 (既Excel表格文件)
    if (database.OpenEx(sSql, CDatabase::noOdbcDialog))
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
      columnData.cchTextMax = 100;
      columnData.pszText = columnName.GetBuffer(100);
      for (i = 0; pList->GetColumn(i, &columnData); i++)
      {
        if (i != 0)
        {
          sSql = sSql + _T(", ");
          strH = strH + _T(", ");
        }
        sSql = sSql + _T(" ") + columnData.pszText + _T(" TEXT");
        strH = strH + _T(" ") + columnData.pszText + _T(" ");
      }
      columnName.ReleaseBuffer();
      columnNum = i;

      sSql = _T("CREATE TABLE ") + tableName + _T(" ( ") + sSql + _T(" ) ");
      database.ExecuteSQL(sSql);

      // 插入数据项
      int nItemIndex;
      for (nItemIndex = 0; nItemIndex < pList->GetItemCount(); nItemIndex++)
      {
        strV = "";
        for (i = 0; i < columnNum; i++)
        {
          if (i != 0)
          {
            strV = strV + _T(", ");
          }
          strV = strV + _T(" '") + pList->GetItemText(nItemIndex, i) + _T("' ");
        }

        sSql = _T("INSERT INTO ") + tableName
          + _T(" (") + strH + _T(")")
          + _T(" VALUES(") + strV + _T(")");
        database.ExecuteSQL(sSql);
      }

    }

    // 关闭数据库
    database.Close();

    //warningStr.Format(_T("导出文件保存于%s!"),sExcelFile);
    warningStr.Format(theApp.g_LoadString("IDWQF_E0040"), sExcelFile);
    AfxMessageBox(warningStr);
  }
}

//在这里处理我们感兴趣的消息，想要哪个，截哪个! 47C为编辑的ID
static UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (WM_NOTIFY == uMsg)
  {
    LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
    if (pofn->hdr.code == CDN_FILEOK)
    {
      // 加入这两句就可以了
      _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
      pThreadState->m_pAlternateWndInit = NULL;
    }
    else if (pofn->hdr.code == CDN_TYPECHANGE)
    {
      int nIndex = pofn->lpOFN->nFilterIndex;
      HWND hwndDlg = ::GetParent(hDlg);
      HWND hwndFilter = ::GetDlgItem(hwndDlg, 0x470);
      //过滤器COMBOBOX                 
      char strExt[MAX_PATH] = "";
      WCHAR* wstr = (WCHAR*)SendMessage(hwndFilter, CB_GETITEMDATA, nIndex - 1, 0);
      WideCharToMultiByte(CP_OEMCP, NULL, wstr, -1, strExt, sizeof(strExt), NULL, FALSE);
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

void CReadISO6C::OnBnClickedButtonSave2()
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

  int nRow = m_ListShowData.GetItemCount();
  if (nRow <= 0)
  {
    //AfxMessageBox(_T("没有数据需要保存!"));
    AfxMessageBox(theApp.g_LoadString("IDWQF_E0041"));
    return;
  }

  CString fileext;
  CString filename;
  CFileDialog dlg(FALSE, _T(""), strTime, OFN_EXPLORER | OFN_ENABLEHOOK, _T("CSV files(*.csv)|*.csv|Worksheet Files (*.xls)|*.xls||"));
  dlg.m_ofn.lpfnHook = (LPOFNHOOKPROC)OFNHookProc;
  dlg.m_ofn.hInstance = AfxGetInstanceHandle();

  if (dlg.DoModal() == IDOK)
  {
    filename = dlg.GetPathName();
    //filename=dlg.GetFileName();

  }
  else
  {
    return;
  }
  fileext = dlg.GetFileExt();
  fileext.MakeLower();
  if (fileext.Find(_T("csv")) >= 0)
  {
    CString sEpcTmp, sEpcAscii, sUseTmp, sPassTmp, strtmp, sTID;

    CFile f;

    f.Open(filename, CFile::modeCreate | CFile::modeReadWrite);

    CString tmp = _T(",");

    BYTE str[1024] = { 0 };
    int len;

    CListCtrl* pList = &m_ListShowData;
    LVCOLUMN columnData;
    CString columnName;
    columnData.mask = LVCF_TEXT;
    columnData.cchTextMax = 100;
    columnData.pszText = columnName.GetBuffer(100);
    strtmp = _T("");
    for (int i = 0; pList->GetColumn(i, &columnData); i++)
    {
      strtmp += columnData.pszText;
      strtmp = strtmp + tmp;

    }
    strtmp = strtmp.Left(strtmp.GetLength() - 1);
    strtmp = strtmp + _T("\r\n");
    len = 0;
    memset(str, 0, 1024);
    //WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
    len = strtmp.GetLength();
    memcpy(str, strtmp, len);

    len = strlen((char*)str);
    f.Write(str, len);

    strtmp = _T("");
    for (int i = 0; i < nRow; i++)
    {
      strtmp.Empty();
      strtmp = m_ListShowData.GetItemText(i, 0);

      sEpcTmp = m_ListShowData.GetItemText(i, 1);
      sEpcAscii = m_ListShowData.GetItemText(i, 2);
      sUseTmp = m_ListShowData.GetItemText(i, 3);
      sTID = m_ListShowData.GetItemText(i, 4);
      sPassTmp = m_ListShowData.GetItemText(i, 5);

      strtmp = strtmp + tmp;
      strtmp = strtmp + sEpcTmp;
      strtmp = strtmp + tmp;
      strtmp = strtmp + sEpcAscii;

      strtmp = strtmp + tmp;

      // user
      strtmp = strtmp + sUseTmp;
      strtmp = strtmp + tmp;

      // tid
      strtmp = strtmp + sTID;
      strtmp = strtmp + tmp;

      // user
      strtmp = strtmp + sUseTmp;
      strtmp = strtmp + tmp;

      strtmp = strtmp + _T("\r\n");
      len = 0;
      memset(str, 0, 1024);
      //WideCharToMultiByte( CP_ACP, 0, strtmp.GetBuffer(), -1,(char*)str,strtmp.GetLength()*2,NULL,NULL );
      len = strtmp.GetLength();
      memcpy(str, strtmp, len);

      f.Write(str, len);

    }

    f.Close();
    //AfxMessageBox(_T("导出完成!"));
    AfxMessageBox(theApp.g_LoadString("IDWQF_E0042"));
  }
  else if (fileext.Find(_T("xls")) >= 0)
  {
    //ExportListToExcel(&m_ListShowData, _T("读取数据列表"), filename);
    ExportListToExcel(&m_ListShowData, theApp.g_LoadString("IDWQF_E0043"), filename);
  }
  else
  {
    //AfxMessageBox(_T("导出失败!"));
    AfxMessageBox(theApp.g_LoadString("IDWQF_E0044"));
  }

  return;

}

CString CReadISO6C::ReportError(USHORT res)
{
  CString str;
  switch (res)
  {
  case 0x01:
    str = "Connect antenna fail!";
    break;
  case 0x02:
    str = "No Tag!";
    break;
  case 0x03:
    str = "Illegal Tag!";
    break;
  case 0x04:
    str = "Power is not enough!";
    break;
  case 0x05:
    str = "The memory has been protected!";
    break;
  case 0x06:
    str = "Check sum error!";
    break;
  case 0x07:
    str = "Parameter error!";
    break;
  case 0x08:
    str = "The memory don't exist!";
    break;
  case 0x09:
    str = "The Access Password is error!";
    break;
  case 0x0A:
    str = "The Kill Password cannot be 000000!";
    break;
  case 0x0E:
    str = "Locked Tags in the field!";
    break;
  case 0x1E:
    str = "Invalid Command!";
    break;
  case 0x1F:
    str = "Other Error!";
    break;
  default:
    str = "Unbeknown Error!";
  }

  return str;
}

void CReadISO6C::OnBnClickedButton5SetAlarm6C()
{
  apiReturn res;
  CString str;
  BYTE AccessPassword[4];
  int i;
  BYTE IDTemp[512] = { 0 };
  UpdateData(TRUE);

  if (((CComboBox*)GetDlgItem(IDC_COMBO6))->GetCount() < 1)
  {
    //MessageBox("Please read first then choose a tag!","Warning",MB_ICONWARNING);
    MessageBox(theApp.g_LoadString("IDWQB_E0054"), theApp.g_LoadString("IDWQB_E0055"), MB_ICONWARNING);
    return;
  }

  str = m_AAccessPassword.SpanIncluding("0123456789ABCDEFabcdef");
  if (strlen(str) != strlen(m_AAccessPassword))
  {
    //MessageBox("Please input HEX!","Warning",MB_ICONWARNING);
    MessageBox(theApp.g_LoadString("IDWQB_E0056"), theApp.g_LoadString("IDWQB_E0057"), MB_ICONWARNING);
    GetDlgItem(IDC_EDIT12)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_EDIT12))->SetSel(0, -1);
    return;
  }
  str = m_AAccessPassword;
  int str_len = strlen(str);
  if (str_len != 8)
  {
    //MessageBox("Please input enough Length of AccessPassword!","Warning",MB_ICONWARNING);
    MessageBox(theApp.g_LoadString("IDWQB_E0058"), theApp.g_LoadString("IDWQB_E0059"), MB_ICONWARNING);
    return;
  }
  for (i = 0; i < 4; i++)
  {
    AccessPassword[i] = (BYTE)strtol((CString)str[i * 2] + (CString)str[2 * i + 1], NULL, 16);
  }

  int CurSel = ((CComboBox*)GetDlgItem(IDC_COMBO6))->GetCurSel();
  ((CComboBox*)GetDlgItem(IDC_COMBO6))->GetLBText(CurSel, str);
  BYTE EPC_Word = (BYTE)str.GetLength() / 2;

  for (i = 0; i < EPC_Word; i++)
  {
    IDTemp[i] = (BYTE)strtol((CString)str[i * 2] + (CString)str[2 * i + 1], NULL, 16);
  }

  EPC_Word = EPC_Word / 2;
  for (i = 0; i < 5; i++)
  {
    Sleep(30);
    switch (ConnectMode)
    {
    case 0://网口

      break;
    case 1://RS232
    case 3://USB
      res = EPC1G2_ChangeEas(m_hScanner, EPC_Word, IDTemp, m_EasState, AccessPassword, RS485Address);
      break;
    }
    if ((res == _OK) || (res == _password_error))
      break;
  }

  if (res == _OK)
  {
    //MessageBox("Set Eas Alarm successfully!","Notice",MB_ICONINFORMATION);
    MessageBox(theApp.g_LoadString("IDWQB_E0060"), theApp.g_LoadString("IDWQB_E0061"), MB_ICONINFORMATION);
  }
  else
  {

    str = ReportError(res);
    //if (str=="Unbeknown Error!")
    if (str == theApp.g_LoadString("IDWQB_E0062"))
    {
      //str="Set Eas Alarm Fail!";
      str = theApp.g_LoadString("IDWQB_E0063");
    }
    if (res == 0x09)
    {
      //str="The access password is error!";
      str = theApp.g_LoadString("IDWQB_E0064");
    }


    MessageBox(str, theApp.g_LoadString("IDWQB_E0065"), MB_ICONERROR);
  }

  return;
}

void CReadISO6C::OnBnClickedCheck7EasAlarm6C()
{

  UpdateData(TRUE);

  int i = 0;

  if (m_EasAlarm)
  {

    UpdateStatus();
    ::SetTimer(g6CReadHwnd, TIME_IDENTIFIER_PARAM, INTERVALTIME_PARAM, CallBack_MyTimeParam);
  }
  else
  {
    ::KillTimer(g6CReadHwnd, TIME_IDENTIFIER_PARAM);
    UpdateStatus();
  }

  return;
}

void CReadISO6C::OnEasAlarmTest()
{
  apiReturn res;

  switch (ConnectMode)
  {
  case 0://网口

    break;
  case 1://RS232
  case 3://usb
    res = EPC1G2_EasAlarm(m_hScanner, RS485Address);
    break;
  }
  if (res == _OK)
  {
    MessageBeep(-1);
    m_APic.SetIcon(AfxGetApp()->LoadIcon(IDI_ICONRED));
  }

  Sleep(100);
  m_APic.SetIcon(AfxGetApp()->LoadIcon(IDI_ICONNone));

  return;
}

//////////////////////////////////////////////////////////////////////////
//用途:		本模块的消息静态成员函数,回调函数
//函数名:	CallBack_MyTimeParam
//功能:		
//输入参数:	hWnd--handle of CWnd that called SetTimer
//			nMsg--WM_TIMER
//			nIDEvent--timer identification
//			dwTime--system time 
//输出参数:	无
//返回值:	无
//备注:TIME_IDENTIFIER_PARAM, INTERVALTIME_PARAM
//////////////////////////////////////////////////////////////////////////
void WINAPI CReadISO6C::CallBack_MyTimeParam(HWND   hWnd, UINT   nMsg, UINT   nIDEvent, DWORD   dwTime)
{
  //Debug_Printf("r:%s\r\n","hello");
  if (TIME_IDENTIFIER_PARAM == nIDEvent)
  {
    ::PostMessage(g6CReadHwnd, WM_6CREADDATAMSG, 1, 0);
  }
}
