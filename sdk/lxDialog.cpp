#include "stdafx.h"
#include "lxDialog.h"

//
//      自动改变控件位置和大小的对话框类
//      文件名：lxDialog.cpp
//      作者：StarLee(coolstarlee@sohu.com)
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

ClxDialog::ClxDialog(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
	, m_iClientWidth(0)
	, m_iClientHeight(0)
	, m_iMinWidth(0)
	, m_iMinHeight(0)
	, m_pControlArray(NULL)
	, m_iControlNumber(0)
	, m_bShowSizeIcon(TRUE)
{}

BEGIN_MESSAGE_MAP(ClxDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

BOOL ClxDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(0, WS_SIZEBOX);

	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_iMinWidth = rectDlg.Width();
	m_iMinHeight = rectDlg.Height();

	CRect rectClient;
	GetClientRect(rectClient);
	m_iClientWidth = rectClient.Width();
	m_iClientHeight = rectClient.Height();

	m_bmpSizeIcon.LoadOEMBitmap(OBM_SIZE);

	m_bmpSizeIcon.GetBitmap(&m_bitmap);

	m_wndSizeIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight), this, 0);
	m_wndSizeIcon.SetBitmap(m_bmpSizeIcon);
	m_wndSizeIcon.MoveWindow(m_iClientWidth - m_bitmap.bmWidth, m_iClientHeight - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	m_wndSizeIcon.ShowWindow(m_bShowSizeIcon);

	return TRUE;
}

void ClxDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
		
	int iIncrementX = cx - m_iClientWidth;
	int iIncrementY = cy - m_iClientHeight;

	if (nType == SIZE_MINIMIZED)
	{
		iIncrementX = iIncrementY = 0;
	}

	for (int i = 0; i < m_iControlNumber; i++)
	{
		CWnd *pWndCtrl = NULL;

		int iId = m_pControlArray[i].iId;
		int iFlag = m_pControlArray[i].iFlag;
		int iPercent = m_pControlArray[i].iPercent;

		if ((iPercent < 0) || (iPercent > 100))
			continue;

		pWndCtrl = GetDlgItem(iId);
		if ((NULL != pWndCtrl) && IsWindow(pWndCtrl->GetSafeHwnd()))
		{
			CRect rectCtrl;
			pWndCtrl->GetWindowRect(rectCtrl);

			ScreenToClient(rectCtrl);

			int iLeft = rectCtrl.left;
			int iTop = rectCtrl.top;
			int iWidth = rectCtrl.Width();
			int iHeight = rectCtrl.Height();

			switch (iFlag)
			{
			case MOVEX:
				iLeft += (iIncrementX * iPercent / 100);
				break;

			case MOVEY:
				iTop += (iIncrementY * iPercent / 100);
				break;

			case MOVEXY:
				iLeft += (iIncrementX * iPercent / 100);
				iTop += (iIncrementY * iPercent / 100);
				break;

			case ELASTICX:
				iWidth += (iIncrementX * iPercent / 100);
				break;

			case ELASTICY:
				iHeight += (iIncrementY * iPercent / 100);
				break;

			case ELASTICXY:
				iWidth += (iIncrementX * iPercent / 100);
				iHeight += (iIncrementY * iPercent / 100);
				break;

			default:
				;
			}

			pWndCtrl->MoveWindow(iLeft, iTop, iWidth, iHeight);
		}
	}

	if (IsWindow(m_wndSizeIcon.GetSafeHwnd()))
		m_wndSizeIcon.MoveWindow(cx - m_bitmap.bmWidth, cy - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	if (nType != SIZE_MINIMIZED)
	{
		m_iClientWidth = cx;
		m_iClientHeight = cy;
	}
}

void ClxDialog::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	int iWidth = lpRect->right - lpRect->left;
	int iHeight = lpRect->bottom - lpRect->top;

	if (iWidth <= m_iMinWidth)
		lpRect->right = lpRect->left + m_iMinWidth;
		
	if(iHeight <= m_iMinHeight)
		lpRect->bottom = lpRect->top + m_iMinHeight;
}

BOOL ClxDialog::SetControlProperty(PDLGCTLINFO lp, int nElements)
{
	if (NULL == lp)
		return FALSE;

	if (nElements <= 0)
		return FALSE;

	m_pControlArray = lp;
	m_iControlNumber = nElements;

	return TRUE;
}

void ClxDialog::ShowSizeIcon(BOOL bShow /*=NULL*/)
{
	m_bShowSizeIcon = bShow;
}