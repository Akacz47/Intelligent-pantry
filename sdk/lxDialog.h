#if !defined(AFX_LXDIALOG_H__CDFF636C_885C_4BDD_AC8C_DA280B55ECFF__INCLUDED_)
#define AFX_LXDIALOG_H__CDFF636C_885C_4BDD_AC8C_DA280B55ECFF__INCLUDED_

//
//      自动改变控件位置和大小的对话框类
//      文件名：lxDialog.h
//      作者：StarLee(coolstarlee@sohu.com)
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ClxDialog : public CDialog
{
public:
	ClxDialog(UINT nID, CWnd* pParent = NULL);

	typedef struct _dlgControlTag {
        int iId;
        int iFlag;
        int iPercent;
    } DLGCTLINFO, *PDLGCTLINFO;

	enum
	{
		MOVEX = 0,
		MOVEY,
		MOVEXY,
		ELASTICX,
		ELASTICY,
		ELASTICXY
	};

	BOOL SetControlProperty(PDLGCTLINFO lp, int nElements);
	void ShowSizeIcon(BOOL bShow = TRUE);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	DECLARE_MESSAGE_MAP()

private:
	int m_iClientWidth;
	int m_iClientHeight;
	int m_iMinWidth;
	int m_iMinHeight;
	PDLGCTLINFO m_pControlArray;
	int m_iControlNumber;
	BOOL m_bShowSizeIcon;
	CStatic m_wndSizeIcon;
	CBitmap m_bmpSizeIcon;
	BITMAP m_bitmap;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LXDIALOG_H__CDFF636C_885C_4BDD_AC8C_DA280B55ECFF__INCLUDED_)
