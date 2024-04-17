// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E711E176_C22D_432B_A416_BD6064541359__INCLUDED_)
#define AFX_STDAFX_H__E711E176_C22D_432B_A416_BD6064541359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT




#include "./INC/VHDllLib.h"
#include "./INC/VHHandsetAPI.h"
#pragma comment(lib, "./INC/VHLib.lib")
#include "cout.h"


//EPC读写16进制
#define					EPCREADWRITEHEX

//用户自定义报警命令 add by martrin 20140916
#define					ENABLECUSTOMBUZZER

//增加测试(由于有时会弹出在线不在线问题) add by martrin 20150427
//#define					ENABLETESTT

////////////////////综合查询界面定时器/////////////////////////////////////
//定义定时器的ID
//时间
#define					TIME_IDENTIFIER_LISTDISP		(200)			//时间ID号
#define					INTERVALTIME_LISTDISP			(300)			//多长时间执行一次
//////////////////////////////////////////////////////////////////////////


//是否编译带进度提示框 ENABLE_PROGRESSDISPTIP 定义就有进度提示框功能,否则没有
//#define					ENABLE_PROGRESSDISPTIP


#define					WM_LISTUSERDATA				(WM_USER+3)		//主界面页子
#define					WM_CONNECTDATACUSTOM		(WM_USER+4)		//连接读写器
#define					WM_READERPARMDATAMSG		(WM_USER+5)		//参数页
#define					WM_6CWRITEDATAMSG			(WM_USER+6)		//6C的写页子
#define					WM_6CREADDATAMSG			(WM_USER+7)		//6C的读页子
#define					WM_6BWRITEDATAMSG			(WM_USER+8)		//6B的写页子
#define					WM_6BREADDATAMSG			(WM_USER+9)		//6B的读页子
#define					WM_EPC6CTRANDATAMSG			(WM_USER+18)		//6C



extern CString			g_szCurPath;		//当前可执行文件的路径名
//////////////////////////////////////////////////////////////////////////
extern HWND				gMainHwnd;			//主界面页子
extern HWND				gConnectReaderHwnd;	//连接界面页子

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E711E176_C22D_432B_A416_BD6064541359__INCLUDED_)
