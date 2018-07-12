// CompressionSettingsDialog.h: interface for the CCompressionSettingsDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPRESSIONSETTINGSDIALOG_H__A5CFF95D_3C07_47FE_AC93_05447137E0F2__INCLUDED_)
#define AFX_COMPRESSIONSETTINGSDIALOG_H__A5CFF95D_3C07_47FE_AC93_05447137E0F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BWImage.h"

class CCompressionSettingsDialog : public CDialogImpl <CCompressionSettingsDialog>
{
public:
	enum { IDD = IDD_COMPRESSION_SETTINGS_DIALOG };

	BEGIN_MSG_MAP(CCompressionSettingsDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	CCompressionSettingsDialog(CWindow *pParentWindow, CBWImage *pImage);
	virtual ~CCompressionSettingsDialog();

private:
	void InitControls();
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CBWImage		*m_pImage;
	CWindow			*m_pParentWindow;
private:
	BOOL InitControl(CWindow &wnd, UINT id);
	// sliders
	CTrackBarCtrl	m_sldYR;
	CTrackBarCtrl	m_sldUG;
	CTrackBarCtrl	m_sldVB;
	CTrackBarCtrl	m_sldA;

	// labels
	CStatic			m_lblYR;
	CStatic			m_lblUG;
	CStatic			m_lblVB;
	CStatic			m_lblA;

	// edits
	CEdit			m_txtYR;
	CEdit			m_txtUG;
	CEdit			m_txtVB;
	CEdit			m_txtA;

	// buttons
	CButton			m_btnYR;
	CButton			m_btnUG;
	CButton			m_btnVB;
	CButton			m_btnA;
};

#endif // !defined(AFX_COMPRESSIONSETTINGSDIALOG_H__A5CFF95D_3C07_47FE_AC93_05447137E0F2__INCLUDED_)
