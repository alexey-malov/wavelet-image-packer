// CompressionSettingsDialog.cpp: implementation of the CCompressionSettingsDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bwpngviewer.h"
#include "resource.h"
#include "CompressionSettingsDialog.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompressionSettingsDialog::CCompressionSettingsDialog(CWindow *pParentWindow, CBWImage *pImage)
:m_pParentWindow(pParentWindow)
,m_pImage(pImage)
{
	//m_sldYR.SetDlgCtrlID()
}

CCompressionSettingsDialog::~CCompressionSettingsDialog()
{

}

LRESULT CCompressionSettingsDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

	InitControls();
	
	return TRUE;

}

LRESULT CCompressionSettingsDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;

}

void CCompressionSettingsDialog::InitControls()
{
	InitControl(m_sldYR, IDC_YR_SLIDER);
	InitControl(m_sldUG, IDC_UG_SLIDER);
	InitControl(m_sldVB, IDC_VB_SLIDER);
	InitControl(m_sldA, IDC_A_SLIDER);
	
	InitControl(m_lblYR, IDC_LBL_YR);
	InitControl(m_lblUG, IDC_LBL_VB);
	InitControl(m_lblVB, IDC_LBL_VB);
	InitControl(m_lblA, IDC_LBL_A);

	InitControl(m_btnYR, IDC_APPLY_YR);
	InitControl(m_btnUG, IDC_APPLY_UG);
	InitControl(m_btnVB, IDC_APPLY_VB);
	InitControl(m_btnA, IDC_APPLY_A);

	InitControl(m_txtYR, IDC_YR_EDIT);
	InitControl(m_txtUG, IDC_UG_EDIT);
	InitControl(m_txtVB, IDC_VB_EDIT);
	InitControl(m_txtA, IDC_A_EDIT);
	
	///////////////////////////////////////////////////////////////////
	m_sldYR.SetRange(0, 100);
	m_sldUG.SetRange(0, 100);
	m_sldVB.SetRange(0, 100);
	m_sldA.SetRange(0, 100);

	
}

BOOL CCompressionSettingsDialog::InitControl(CWindow &wnd, UINT id)
{
	ATLASSERT(wnd.m_hWnd == NULL);
	HWND hwnd = GetDlgItem(id);
	ATLASSERT(hwnd != NULL);
	wnd.m_hWnd = hwnd;
	return TRUE;
}
