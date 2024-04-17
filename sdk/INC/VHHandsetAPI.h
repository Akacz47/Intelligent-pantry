#ifndef __VHHandsetAPI_H__
#define __VHHandsetAPI_H__
#include "WINSOCK2.h"
//#include "WINSOCK.h"

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")
#ifdef __cplusplus
extern "C"
{
#endif
//==============================常量定义==============================
#define	ID_MAX_SIZE_64BIT	8		//电子标签的ID号为64bit
#define ID_MAX_SIZE_96BIT	65		//电子标签的ID号
#define MAX_LABELS			100		// 一次读写操作最多不超过100个标签

//==============================API函数返回码==============================
#define	_OK					0x00	// 操作成功

//通信方面出错信息
#define _init_rs232_err		0x81	//  通信口初始化失败
#define _no_scanner			0x82	//  找不到读写器
#define _comm_error         0x83	//  收发通信数据出错
#define _baudrate_error     0x84	//  设置波特率出错
#define _init_net_error		0x85	//  网口初始化失败
#define _net_error          0x86	//  网口收发通信数据出错

// 读写器返回出错信息
#define _no_antenna			0x01   //天线连接失败
#define _no_label			0x02   //未检测到标签
#define _invalid_label		0x03   //非法标签
#define _less_power			0x04   //读写功率不够
#define _write_prot_error	0x05   //该区写保护
#define _check_sum_error	0x06   //校验和错误
#define _parameter_error	0x07   //参数错误
#define _memory_error		0x08   //数据区不存在
#define _password_error		0x09   //密码不正确
#define _killpassword_error	0x0a   //G2标签毁灭密码为全零
#define _nonlicet_command	0x0b   //非法命令
#define _nonlicet_user		0x0c   //密码不匹配的非法用户
#define _invalid_command	0x1e   //表示无效指令，如参数不正确的指令
#define _other_error		0x1f   //未知命令

#define Success					"正确"
#define TransportError			"通讯失败"
#define SendBufferError			"发送失败"
#define ReceiveBufferError		"接收失败"
#define CheckSumError			"校验和错误"
#define CommandError			"命令不匹配"
#define ReturnPacketError		"返回包错误"
#define ReturnPacketheadError	"返回包包头错误"

//函数输入错误
#define _no_cardID_input   0x20   //其它错误

//==============================数据类型定义==============================
typedef USHORT apiReturn;   // 函数返回值类型
//typedef DWORD apiReturn;   // 函数返回值类型
//typedef long apiReturn;   // 函数返回值类型

typedef struct tagReaderDate
{
	BYTE	Year;			//年
	BYTE	Month;			//月
	BYTE	Day;			//日
	BYTE	Hour;			//时
	BYTE	Minute;			//分
	BYTE	Second;			//秒
}ReaderDate;

typedef struct tagHandsetParam
{
	BYTE	TagType;		//标签种类：01H－ISO18000-6B，02H－EPCC1，04H－EPCC1G2，08H－EM4442。	(0)
	BYTE	Alarm;			//bit0-bit7 bit0:0-不报警，1-报警 bit1 0-不使用白名单 1-使用白名单。	(1)
	BYTE	OutputMode;	    //数据输出模式：0-保存并直接输出,1-保存但不直接输出,2-不保存但直接输出  (2)
	BYTE	USBBaudRate;	//USB接口波特率 04-9600 05-19200  06-38400  07-57600 08-115200			(3)
	BYTE	BtBaudRate;		//蓝牙串口通讯波特率													(4)
	BYTE	Min_Frequence;	//发射微波信号频率的起始点，取值： 1~63。								(5)
	BYTE	Max_Frequence;	//发射微波信号频率的起始点，取值： 1~63。								(6)
	BYTE	Power;			//发射功率值，取值：0~160。												(7)
	BYTE	RFhrdVer1;		//RF模块硬件主版本														(8)
	BYTE	RFhrdVer2;		//RF模块硬件次版本														(9)
	BYTE	RFSoftVer1;		//RF模块软件主版本														(10)
	BYTE	RFSoftVer2;		//RF模块软件次版本														(11)
	BYTE	ISTID;		    //是否读TID区															(12)
	BYTE	TIDAddr;		//TID读取起始位置														(13)
	BYTE	TIDLen;		    //TID读取长度															(14)
	BYTE	ISUSER;		    //是否读USER															(15)
	BYTE	USERAddr;		//USER读取起始位置														(16)
	BYTE	USERLen;		//USER读取长度															(17)
	BYTE	ISVibrate;		//电机振动,0-不，1-振动													(18)
	BYTE	Modutype;		//模块类型,0---R2000, 1---VM5E											(19)
	BYTE	Reserve21;		//保留																	(20)
	BYTE	Reserve22;		//保留																	(21)
	BYTE	Reserve23;		//保留																	(22)
	BYTE	Reserve24;		//保留																	(23)
	BYTE	Reserve25;		//保留																	(24)
	BYTE	Reserve26;		//保留																	(25)
	BYTE	Reserve27;		//保留																	(26)
	BYTE	Reserve28;		//保留																	(27)
	BYTE	Reserve29;		//保留																	(28)
	BYTE	Reserve30;		//保留																	(29)
	BYTE	Reserve31;		//保留																	(30)
	BYTE	Reserve32;		//保留																	(31)
}HandsetParam;

typedef struct tagReaderBasicParam
{
	BYTE	BaudRate;			//(1)串口的通信速率，取值：00H~08H，代表速率同"设定波特率"命令。
	BYTE	Power;				//(2)发射功率值，取值：20~30dBm。(0-63)
	BYTE	Min_Frequence;		//(3)发射微波信号频率的起始点，取值： 1~63。
	BYTE	Max_Frequence;		//(4)发射微波信号频率的终止点，取值： 1~63。
	BYTE	Reserve5;			//(5)保留,后来改为调制深度
	BYTE	WorkMode;			//(6)读写器工作方式：0-主动方式，1-命令方式
	BYTE	ReaderAddress;		//(7)RS485地址:0--255
	BYTE	NumofCard;			//(8)最多读卡数目。
	BYTE	TagType;			//(9)标签种类：01H－ISO18000-6B，02H－EPCC1，04H－EPCC1G2，08H－EM4442。
	BYTE	ReadDuration;		//(10)读卡持续时间：射频发射持续时间，只针对EM标签有效；0－10ms，1－20ms，2－30ms，3－40ms。
	BYTE	ReadTimes;			//(11)读卡次数M：收到上位机读卡命令，读写器执行M次此命令。
	BYTE	EnableBuzzer;		//(12)1:使能蜂鸣器0:不使能蜂鸣器
	BYTE	IP1;			    //(13)读写器IP地址
	BYTE	IP2;			    //(14)
	BYTE	IP3;			    //(15)
	BYTE	IP4;			    //(16)
	BYTE	Port1;				//(17)读写器端口高位
	BYTE	Port2;				//(18)
	BYTE	Mask1;				//(19)读写器掩码1
	BYTE	Mask2;				//(20)读写器掩码2
	BYTE	Mask3;				//(21)读写器掩码3
	BYTE	Mask4;				//(22)读写器掩码4
	BYTE	Gateway1;			//(23)读写器地址网关
	BYTE	Gateway2;			//(24)
	BYTE	Gateway3;			//(25)
	BYTE	Gateway4;			//(26)
	BYTE	MAC1;			    //(27)读写器MAC地址
	BYTE	MAC2;			    //(28)
	BYTE	MAC3;			    //(29)
	BYTE	MAC4;			    //(30)
	BYTE	MAC5;			    //(31)
	BYTE	MAC6;			    //(32)
	
}ReaderBasicParam;

//读写器主动工作参数
typedef struct tagReaderAutoParam
{
	BYTE	AutoMode;			//(1)读标签模式：0-定时方式，1-触发方式。
	BYTE	TimeH;				//(2)标签保留时间：单位：秒s。缺省值为1。
	BYTE	TimeL;				//(3)
	BYTE	Interval;			//(4)0-10ms，1-20ms，2-30ms，3-50ms，4-100ms。缺省值为2。每隔设定时间主动读取一次标签。
	BYTE	NumH;				//(5)标签保留数目：缺省值为1。已读取的标签ID在读写器内存中保留的数目。
	BYTE	NumL;				//(6)
	BYTE	OutputManner;	    //(7)数据输出格式：0-简化格式，1-标准格式，2-XML格式。缺省值为0。
	BYTE	OutInterface;		//(8)输出接口：0－RS232口，1－RS485口，2－RJ45口。缺省值为0。 3- Wiegand26    4- Wiegand34
	BYTE	WiegandWidth;		//(9)Weigand脉冲宽度值。
	BYTE	WiegandInterval;	//(10)Weigand脉冲间隔值。
	BYTE	ID_Start;			//(11)输出卡号的起始位，取值0～4。
	BYTE    IDPosition;			//(12)卡号在电子标签上的存放地址。EPC区--0, USER区--1，TID区--2
	BYTE	Report_Interval;	//(13)通知间隔：单位秒s。缺省值为1。每隔设定时间主动通知上位机一次。
	BYTE	Report_Condition;	//(14)通知条件：缺省值为1。0-被动通知，1-定时通知，2-增加新标签，3-减少标签，4-标签数变化	
	BYTE	Report_Output;		//(15)通知输出端
	BYTE	Antenna;			//(16)天线选择。1-ant1,2-ant2,4-ant4,8-ant8
	BYTE	TriggerMode;	    //(17)触发方式(缺省值为0): 0-低电平 1-高电平
	BYTE	HostIP1;			//(18)被通知IP地址
	BYTE	HostIP2;			//(19)
	BYTE	HostIP3;			//(20)
	BYTE	HostIP4;			//(21)
	BYTE	Port1;				//(22)被通知端口
	BYTE	Port2;				//(23)
	BYTE	Reserve24;			//(24)被通知MAC,mofi by mqs 20121207 保留
	BYTE	ArgentinaSim;		//(25)//仿真模式(阿根廷),0--非仿真，1--仿真
	BYTE	CardTime1;			//(26)//读卡总超时1
	BYTE	CardTime2;			//(27)//读卡总超时2
	BYTE	ArgentinaMode;		//(28)//阿根的五种模式,0---Only ATA  只读ATA;1---Only EPC 只读EPC;2---Only EPC & TID 只读EPC+TID;3---ATA + EPC   读ATA+EPC;4---ATA + EPC & TID读ATA+EPC&TID
	BYTE	AnSleepTime;		//(29)//天线睡眠时间，单位：ms,(1-255)
	BYTE	Alarm;				//(30)0-不报警，1-报警。在定时和触发方式下是否检测报警。
	BYTE	Reserve31;		    //(31)标准输出时间间隔，缺省值为120秒，1～255。
	BYTE	EnableRelay;		//(32)自动工作模式是否控制继电器1:控制 0:不控制
}ReaderAutoParam;



#define			PERFIXLEN			20   //前缀值的长度
//读写器仿真键盘参数
typedef struct tagSimParam
{
	BYTE	DataFormat;				//(0)数据格式3种：16进制--0, 10进制--1,	ASCII码--2								
	//BYTE	DataBank;				//(1)EPC区--0, USER区--1，TID区--2
	BYTE	IsPerfix;				//(2)是否前缀，1--带,0--不带												
	BYTE	PerfixCode[PERFIXLEN];	//(3-22)前缀值,不够0x00填充													
	BYTE	IsEnter;				//(23)是否回车换行, 1--带,0--不带											
	BYTE    StartAddress;			//(24)数据起始地址															
	BYTE	DataLen;				//(25)数据长度
	BYTE	OutputInterval;	//(26)标准输出间隔(0-255)(0--输出ID之间无间隔)

}SimParam;

typedef struct _tagReaderFreq
{
	char *chFreq;		//国家频率字符串
	
	int	iGrade;			//级数 = 50;
	int iSkip;			//步进 = 500KHz;
	int dwFreq;			//起始频率 = 902750;
	//公式：902750 + 级数*步进
}tagReaderFreq;

//关于各个国家的频率
static const tagReaderFreq stuctFreqCountry[]=
{
	{"00---FCC(American)", 63, 400, 902600},							//(0),{"00---FCC(American)", 50, 500, 902750},
	{"01---ETSI EN 300-220(Europe300-220)", 11, 200, 865500},			//(1),{"01---ETSI EN 300-220(Europe300-220)", -1, -1, -1},
	{"02---ETSI EN 302-208(Europe302-208)", 4, 600, 865700},			//(2)
	{"03---HK920-925(Hong Kong)", 10, 500, 920250},						//(3)
	{"04---TaiWan 922-928(Taiwan)", 12, 500, 922250},					//(4)
	{"05---Japan 952-954(Japan)", 0, 0, 0},								//(5)
	{"06---Japan 952-955(Japan)", 14,200, 952200},						//(6)
	{"07---ETSI EN 302-208(Europe)", 4, 600, 865700},					//(7)
	{"08---Korea 917-921(Korea)", 6, 600, 917300},						//(8)
	{"09---Malaysia 919-923(Malaysia)", 8, 500, 919250},				//(9)
	{"10--China 920-925(China)", 16, 250, 920625},						//(10)
	{"11--Japan 952-956(Japan)", 4, 1200, 952400},						//(11)
	{"12--South Africa 915-919(Poncho)", 17, 200, 915600},				//(12)
	{"13--Brazil 902-907/915-928(Brazil)", 35, 500, 902750},			//(13)
	{"14--Thailand 920-925(Thailand)", -1, -1, -1},						//(14)
	{"15--Singapore 920-925(Singapore)", 10, 500, 920250},				//(15)
	{"16--Australia 920-926(Australia)", 12, 500, 920250},				//(16)
	{"17--India 865-867(India)", 4, 600, 865100},						//(17)
	{"18--Uruguay 916-928(Uruguay)", 23, 500, 916250},					//(18)
	{"19--Vietnam 920-925(Vietnam)", 10, 500, 920250},					//(19)
	{"20--Israel 915-917", 1, 0, 916250},								//(20)
	{"21--Philippines 918-920(Philippines)", 4, 500, 918250},			//(21)
	{"22--Canada 902-928(Canada)", 42, 500, 902750},					//(22)
	{"23--Indonesia 923-925(Indonesia)", 4, 500, 923250},				//(23)
	{"24--New Zealand 921.5-928(New Zealand)", 11, 500, 922250},		//(24)
};

//==============================读写器应用函数定义==============================

//连接读写器
apiReturn _stdcall ConnectScanner(HANDLE *hScanner, char *szPort, int nBaudRate);

//断开连接
apiReturn _stdcall DisconnectScanner(HANDLE hScanner);

//连接读写器VH73B
apiReturn _stdcall ConnectScannerVH73B(HANDLE *hScanner, char *szPort, int nBaudRate);

//通过USB连接读写器
apiReturn _stdcall VH_ConnectScannerUsb(HANDLE *hScanner);

//断开连接
apiReturn _stdcall VH_DisconnectScannerUsb(HANDLE hScanner);

//检测USB在线,bDech---1为在线，否则不在线
typedef int (_stdcall *VH_fDechUsb)(BOOL bDech);

//初始化USB，可检测，传入函数地址
apiReturn _stdcall VH_InitUsb(VH_fDechUsb fFunc);

//关闭USB,释放资源
apiReturn _stdcall VH_CloseUsb();


//////////////////////////////////////////////////////////////////////////
//用途:		设置通信方式
//函数名:	Mux_SetCommunicationOpt
//功能:		对手持机的通信模式进行设置
//输入参数:	lOpt 为何种方式通信(0--TCP/IP, 1--RS232,2--RS485, 3--USB)
//			lpRHand 为读写器的句柄
//输出参数:	
//返回值:	返回 0 为成功，其它失败
//备注:		外部接口
//////////////////////////////////////////////////////////////////////////
//apiReturn _stdcall VH_SetCommunicationOpt(long lOpt, void *lpRWHand);
apiReturn _stdcall VH_SetCommunicationOpt(int lOpt, int lpRWHand);

//字符时间转十进制
//int StrtimetoDecimal(CString Command,BYTE *CmdBuffer);

//int strtodecimal(CString Command,BYTE *CmdBuffer);

//比较前后两个记录是否相等
int compare(BYTE *olddata, BYTE *data, int len);
//==============================上载控制命令==============================

//读取版本号
apiReturn _stdcall VH_GetVersion(HANDLE hScanner, WORD *wHardVer, WORD  *wSoftVer);
//这样做的目的跟原来保持一致
apiReturn _stdcall GetVersionx(HANDLE hScanner, WORD *wHardVer, WORD  *wSoftVer);

//启动/停止自动模式,0--stop, 1--start
apiReturn _stdcall VH_AutoMode(HANDLE hScanner,int Mode);

//启动/停止自动模式,0--stop, 1--start
apiReturn _stdcall AutoMode(HANDLE hScanner,int Mode);

//增加名单
apiReturn _stdcall AddLableID(HANDLE hScanner, int Count, int Len, BYTE * data);

//删除名单
apiReturn _stdcall DelLableID(HANDLE hScanner);

//增加单个或多个名单
apiReturn _stdcall DelSingleLableID(HANDLE hScanner, int Count, int Len, BYTE * data);

//获得名单
apiReturn _stdcall GetLableID(HANDLE hScanner, int startaddr, int listlen, int *nTotal, int *DataLen, BYTE * data);

//提交名单
apiReturn _stdcall SaveLableID(HANDLE hScanner);

//获取读写器基本工作参数
apiReturn _stdcall ReadHanderParam(HANDLE hScanner, HandsetParam * pParam);

//设置读写器基本工作参数
apiReturn _stdcall WriteHanderParam(HANDLE hScanner, HandsetParam * pParam);

//////////////////////////////////////////////////////////////////////////
//用途:		取仿真键盘小模块参数
//函数名:	ReadSimParam
//功能:		将小模块的参数读出来
//输入参数:	hScanner 为通信句柄,
//输出参数:	pParam 为结构体指针
//返回值:	返回 0 为成功，否则失败
//备注:
//////////////////////////////////////////////////////////////////////////
apiReturn _stdcall ReadSimParam(HANDLE hScanner, SimParam * pParam);

//////////////////////////////////////////////////////////////////////////
//用途:		设置仿真键盘小模块参数
//函数名:	WriteSimParam
//功能:		将小模块的参数写入其中
//输入参数:	hScanner 为通信句柄,pParam 为结构体指针
//输出参数:	
//返回值:	返回 0 为成功，否则失败
//备注:
//////////////////////////////////////////////////////////////////////////
apiReturn _stdcall WriteSimParam(HANDLE hScanner, SimParam * pParam);

//读取读写器基本工作参数
apiReturn _stdcall ReadBasicParam(HANDLE hScanner, ReaderBasicParam * pParam);

//设置读写器基本工作参数
apiReturn _stdcall WriteBasicParam(HANDLE hScanner, ReaderBasicParam * pParam);

//读取读写器自动工作参数
apiReturn _stdcall ReadAutoParam(HANDLE hScanner, ReaderAutoParam * pParam);

//设置读写器自动工作参数
apiReturn _stdcall WriteAutoParam(HANDLE hScanner, ReaderAutoParam * pParam);

//设置厂工作参数
apiReturn _stdcall WriteFactoryParam(HANDLE hScanner, SimParam * pParam);

//恢复读写器出厂参数
apiReturn _stdcall ReadFactoryParameter(HANDLE hScanner);

//获得读写器工作频率范围
apiReturn _stdcall GetFrequencyRange(HANDLE hScanner, BYTE *Frequency);

//获得读写器ID
apiReturn _stdcall GetReaderID(HANDLE hScanner, BYTE *ReaderID);

//设置时间
apiReturn _stdcall SetReaderTime(HANDLE hScanner, ReaderDate *time);

//获得时间
apiReturn _stdcall GetReaderTime(HANDLE hScanner, ReaderDate *time);

//设置标签过滤器
apiReturn _stdcall SetReportFilter(HANDLE hScanner, int ptr, int len, BYTE *mask);

//获得标签过滤器
apiReturn _stdcall GetReportFilter(HANDLE hScanner, int *ptr, int *len, BYTE *mask);

//////////////////////////////////////////////////////////////////////////
//用途:		获取时间段的记录总数
//函数名:	VH_GetRecordNum
//功能:		取记录总数
//输入参数:	hScanner 通信句柄
//			stime 为起始时间，etime 为结束时间
//输出参数:	nTotal 为记录总数
//返回值:	0--成功，其它失败
//备注:		注意：如果起始时间和结束时间都为0,则取机器中全部记录。
//////////////////////////////////////////////////////////////////////////
apiReturn _stdcall VH_GetRecordNum(HANDLE hScanner, ReaderDate *stime, ReaderDate *etime, int *nTotal);

//高速下载数据，取记录数据
typedef short (_stdcall *VH_fGetRecordData)(BYTE * data);
//高速获得记录
apiReturn _stdcall VH_GetRecordHigh(HANDLE hScanner, VH_fGetRecordData fFunc);

//高速下载数据测试
typedef short (_stdcall *VH_fGetRecordDataTest)();
//高速获得记录
apiReturn _stdcall VH_GetRecordHighTest(HANDLE hScanner, VH_fGetRecordDataTest fFunc);

//高速下载数据测试
//typedef USHORT ( *VH_fGetRecordDataTestS)(LPCSTR *lpData);
typedef USHORT ( *VH_fGetRecordDataTestS)(BYTE *lpData);
//高速获得记录
apiReturn _stdcall VH_GetRecordHighTestS(HANDLE hScanner, VH_fGetRecordDataTestS fFunc);


//////////////////////////////////////////////////////////////////////////
//用途:		获取时间段的记录
//函数名:	VH_GetRecord
//功能:		取记录
//输入参数:	hScanner 通信句柄
//			stime 为起始时间，etime 为结束时间
//输出参数:	nTotal 为记录总数, nDiNum 为第几条，从0开始算, data 指记录指针
//返回值:	0--成功，其它失败
//备注:		注意：如果起始时间和结束时间都为0,则取机器中全部记录。
//////////////////////////////////////////////////////////////////////////
apiReturn _stdcall VH_GetRecord(HANDLE hScanner, ReaderDate *stime, ReaderDate *etime, int *nTotal, int *nDiNum, BYTE * data);

//获得记录
apiReturn _stdcall GetRecord(HANDLE hScanner, ReaderDate *stime, ReaderDate *etime, int *nTotal, int *taglen, BYTE * data);

//删除全部记录
apiReturn _stdcall DeleteAllRecord(HANDLE hScanner);

//设置读写器ID
apiReturn _stdcall SetHandsetID(HANDLE hScanner, BYTE *HandsetID);

//获得读写器ID
apiReturn _stdcall GetHandsetID(HANDLE hScanner, BYTE *HandsetID);

//设置硬件版本
apiReturn _stdcall SetHardVersion(HANDLE hScanner, int wHardVer,int wHardVer2);

//设置蓝牙波特率
apiReturn _stdcall SetBtBaudRate(HANDLE hScanner, BYTE nBaudRate);

//获取蓝牙波特率
apiReturn _stdcall GetBtBaudRate(HANDLE hScanner, BYTE *nBaudRate);

//获得蓝牙名称
apiReturn _stdcall SetBluetoothName(HANDLE hScanner,int nLen, BYTE *BluetoothName);

//设置蓝牙名称
apiReturn _stdcall GetBluetoothName(HANDLE hScanner, BYTE *BluetoothName);

//设置手机进入读写器模式，即模块电源打开，1--打开，0--关闭
apiReturn _stdcall SetReaderMode(HANDLE hScanner, BYTE nMode);

//显示信息
//CString _stdcall ShowInfo();






//==============================网口函数定义==============================
//连接读写器
apiReturn _stdcall Net_ConnectScanner(SOCKET *hSocket,char *nTargetAddress,UINT nTargetPort,char *nHostAddress,UINT nHostPort);

//断开连接
apiReturn _stdcall Net_DisconnectScanner();


//==============================EPC C1G2数据读写命令==============================
//读取EPC1G2标签ID号
apiReturn _stdcall EPC1G2_ReadLabelID(HANDLE hScanner, BYTE mem, int ptr, BYTE len, BYTE *mask, BYTE *IDBuffer, int *nCounter,int Address);

//读一块数据
apiReturn _stdcall EPC1G2_ReadWordBlock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, BYTE *AccessPassword,int Address);

//写一块数据
apiReturn _stdcall EPC1G2_WriteWordBlock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, BYTE *AccessPassword,int Address);

//设置读写保护状态
apiReturn _stdcall EPC1G2_SetLock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE Lock, BYTE *AccessPassword,int Address);

//擦除标签数据
apiReturn _stdcall EPC1G2_EraseBlock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len,int Address);

//永久休眠标签
apiReturn _stdcall EPC1G2_KillTag(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE *KillPassword,int Address);

//写EPC
apiReturn _stdcall EPC1G2_WriteEPC(HANDLE hScanner, BYTE len, BYTE *Data, BYTE *AccessPassword,int Address);

//块锁命令
apiReturn _stdcall EPC1G2_BlockLock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE ptr, BYTE *AccessPassword,int Address);

//EAS状态操作命令
apiReturn _stdcall EPC1G2_ChangeEas(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE State, BYTE *AccessPassword,int Address);

//EAS报警命令
apiReturn _stdcall EPC1G2_EasAlarm(HANDLE hScanner,int Address);

//读保护设置
apiReturn _stdcall EPC1G2_ReadProtect(HANDLE hScanner,BYTE *AccessPassword, BYTE EPC_WORD, BYTE *IDBuffer,int Address);

//复位读保护设置
apiReturn _stdcall EPC1G2_RStreadProtect(HANDLE hScanner, BYTE *AccessPassword,int Address);

//设置用户区数据块读保护
apiReturn _stdcall EPC1G2_BlockReadLock(HANDLE hScanner, BYTE EPC_WORD, BYTE *IDBuffer, BYTE Lock, BYTE *AccessPassword,int Address);

//识别EPC同时读数据
apiReturn _stdcall EPC1G2_ReadEPCandData(HANDLE hScanner, BYTE *EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, int Address);

//频谱校验
apiReturn _stdcall EPC1G2_Calibrate(HANDLE hScanner, BYTE *AccessPassword, BYTE Kword);

//侦测标签
apiReturn _stdcall EPC1G2_DetectTag(HANDLE hScanner,int Address);





//==============================EPC C1G2数据读写命令==============================
//读取EPC1G2标签ID号
apiReturn _stdcall Net_EPC1G2_ReadLabelID(SOCKET hSocket, BYTE mem, int ptr, BYTE len, BYTE *mask, BYTE *IDBuffer, int *nCounter);

//读一块数据
apiReturn _stdcall Net_EPC1G2_ReadWordBlock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, BYTE *AccessPassword);

//写一块数据
apiReturn _stdcall Net_EPC1G2_WriteWordBlock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, BYTE *AccessPassword);

//设置读写保护状态
apiReturn _stdcall Net_EPC1G2_SetLock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE Lock, BYTE *AccessPassword);

//擦除标签数据
apiReturn _stdcall Net_EPC1G2_EraseBlock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len);

//永久休眠标签
apiReturn _stdcall Net_EPC1G2_KillTag(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE *KillPassword);

//写EPC
apiReturn _stdcall Net_EPC1G2_WriteEPC(SOCKET hSocket,BYTE len, BYTE *Data, BYTE *AccessPassword);

//块锁命令
apiReturn _stdcall Net_EPC1G2_BlockLock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE ptr, BYTE *AccessPassword);

//EAS状态操作命令
apiReturn _stdcall Net_EPC1G2_ChangeEas(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE State, BYTE *AccessPassword);

//EAS报警命令
apiReturn _stdcall Net_EPC1G2_EasAlarm(SOCKET hSocket);

//读保护设置
apiReturn _stdcall Net_EPC1G2_ReadProtect(SOCKET hSocket,BYTE *AccessPassword, BYTE EPC_WORD, BYTE *IDBuffer);

//复位读保护设置
apiReturn _stdcall Net_EPC1G2_RStreadProtect(SOCKET hSocket, BYTE *AccessPassword);

//识别EPC同时读数据
apiReturn _stdcall Net_EPC1G2_ReadEPCandData(SOCKET hSocket, BYTE *EPC_WORD, BYTE *IDBuffer, BYTE mem, BYTE ptr, BYTE len, BYTE *Data, int Address);

//设置用户区数据块读保护
apiReturn _stdcall Net_EPC1G2_BlockReadLock(SOCKET hSocket, BYTE EPC_WORD, BYTE *IDBuffer, BYTE Lock, BYTE *AccessPassword);

//侦测标签
apiReturn _stdcall Net_EPC1G2_DetectTag(SOCKET hSocket);


//==============================ISO-6B数据读写命令==============================
//检测标签存在
apiReturn _stdcall ISO6B_LabelPresent(HANDLE hScanner, int *nCounter,int Address);

//读取ISO6B标签ID号
apiReturn _stdcall ISO6B_ReadLabelID(HANDLE hScanner, BYTE *IDBuffer, int *nCounter,int Address);

//列出选定标签
apiReturn _stdcall ISO6B_ListSelectedID(HANDLE hScanner, int Cmd, int ptr, BYTE Mask, BYTE *Data, BYTE *IDBuffer, int *nCounter,int Address);

//读一块数据
apiReturn _stdcall ISO6B_ReadByteBlock(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr, BYTE len,BYTE *Data,int Address);

//一次写4字节数据
apiReturn _stdcall ISO6B_WriteByteBlock(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr, BYTE len, BYTE *Data,int Address);

//一次写一字节数据
apiReturn _stdcall ISO6B_WriteAByte(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr, BYTE len, BYTE *Data,int Address);

//写大块数据，字节数超过16
apiReturn _stdcall ISO6B_WriteLongBlock(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr, BYTE len, BYTE *Data,int Address);

//置写保护状态
apiReturn _stdcall ISO6B_WriteProtect(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr,int Address);

//读写保护状态
apiReturn _stdcall ISO6B_ReadWriteProtect(HANDLE hScanner, BYTE *IDBuffer, BYTE ptr, BYTE *Protected,int Address);

//全部清除
apiReturn _stdcall ISO6B_ClearMemory(HANDLE hScanner, BYTE CardType, BYTE *IDBuffer,int Address);

//==============================ISO-6B数据读写命令==============================
//检测标签存在
apiReturn _stdcall Net_ISO6B_LabelPresent(SOCKET hSocket, int *nCounter);

//读取ISO6B标签ID号
apiReturn _stdcall Net_ISO6B_ReadLabelID(SOCKET hSocket, BYTE *IDBuffer, int *nCounter);

//列出选定标签
apiReturn _stdcall Net_ISO6B_ListSelectedID(SOCKET hSocket, int Cmd, int ptr, BYTE Mask, BYTE *Data, BYTE *IDBuffer, int *nCounter);

//读一块数据
apiReturn _stdcall Net_ISO6B_ReadByteBlock(SOCKET hSocket, BYTE *IDBuffer, BYTE ptr, BYTE len,BYTE *Data);

//一次写4字节数据
apiReturn _stdcall Net_ISO6B_WriteByteBlock(SOCKET hSocket, BYTE *IDBuffer, BYTE ptr, BYTE len, BYTE *Data);

//一次写一字节数据
apiReturn _stdcall Net_ISO6B_WriteAByte(SOCKET hSocket, BYTE *IDBuffer, BYTE ptr, BYTE len, BYTE *Data);

//置写保护状态
apiReturn _stdcall Net_ISO6B_WriteProtect(SOCKET hSocket, BYTE *IDBuffer, BYTE ptr);

//读写保护状态
apiReturn _stdcall Net_ISO6B_ReadWriteProtect(SOCKET hSocket, BYTE *IDBuffer, BYTE ptr, BYTE *Protected);

//蜂鸣器报警,0--stop, 1--start
apiReturn _stdcall BuzzerAlarm(HANDLE hScanner,int iAlarm);

//网络通信蜂鸣器报警,0--stop, 1--start
apiReturn _stdcall Net_BuzzerAlarm(SOCKET hSocket, int iAlarm);

//////////////////////////////////////////////////////////////////////////
//用途:		设置输出日志文件
//函数名:	VH_SetLogFile
//功能:		将日志功能写入到指定文件中
//输入参数:	FileName 指全路径名
//输出参数:	无
//返回值:	返回0为成功，否则-1为失败
//备注:
//////////////////////////////////////////////////////////////////////////
BOOL _stdcall VH_SetLogFile(char* FileName);


//////////////////////////////////////////////////////////////////////////
//用途:		写日志
//函数名:	VH_WriteAppLogFile
//功能:		将日志功能写入到指定文件中
//输入参数:	FileName 指全路径名
//			Line 指当前行号，这里暂不使用
//			fmt 指格式化，你懂的
//输出参数:	无
//返回值:	返回0为成功，否则-1为失败
//备注:
//////////////////////////////////////////////////////////////////////////
int _stdcall VH_WriteAppLogFile(char* FileName, int Line, char* fmt, ...);


//////////////////////////////////////////////////////////////////////////
//用途:		Bcd码转成ASC码并补空格
//函数名:	Bcd2AscEx
//功能:		将BCD码转成ASC码,即16进制转成字符串
//输入参数:	bcd 指16进制的指针
//			len 指其长度
//输出参数:	bcd 指转后输出的指针
//返回值:	无
//备注:		与Bcd2Asc不同的是多出空格，如0x40 0x03==>40 03 ,而不是4003
//////////////////////////////////////////////////////////////////////////
void _stdcall Bcd2AscEx(char *asc, char *bcd, int len);

//Asc转Bcd
void _stdcall Asc2Bcd(char *bcd, char *asc, int len);

/* ans = first + second */ 
//实现两个16进制字符串的大数相加,如：
// first:ABCDEF123456ABCDEF123456ABCDEF123456
// second:ABCDEF123456ABCDEF123456ABCDEF123456
//结果存入 ans:1579BDE2468AD579BDE2468AD579BDE2468AC
void _stdcall VH_big_num_add(char *ans, char *first, char *second);

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif	//end of 
