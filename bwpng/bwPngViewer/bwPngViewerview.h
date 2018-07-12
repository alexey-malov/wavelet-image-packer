// bwPngViewerView.h : interface of the CBwPngViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BWPNGVIEWERVIEW_H__10CEF617_DEF8_4714_B2A8_2613FF76E233__INCLUDED_)
#define AFX_BWPNGVIEWERVIEW_H__10CEF617_DEF8_4714_B2A8_2613FF76E233__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BWImage.h"

class CBwPngViewerView : public CWindowImpl<CBwPngViewerView>
{
private:
	enum
	{
		UNUSED_AREA_COLOR = RGB(80,80,80),
		BORDER_COLOR = RGB(0,0,0),
	};
public:
	void OnEditCompressionSettings();
	BOOL IsImageMoveable()const;
	CBwPngViewerView();

	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CBwPngViewerView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUP)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);



	WTL::CString m_strFileName;

	CBWImage m_Image;
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		if (!m_Image.IsCreated()) 
		{
			bHandled = FALSE;
		}
		return 0;
	}

private:
	CPoint	m_ptImgSrc;	// coords of source image rectangle
	CRect	m_rcClient;
	CPoint	m_ptMouseDownPoint;
	BOOL	m_bLMB;

private:
	BOOL MoveImage(int dx, int dy);
	void AdjustImagePos();
	void DrawImage(CDC &dc);
	HCURSOR m_crHandPressed;
	HCURSOR m_crHandUnpressed;

	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		GetClientRect(&m_rcClient);

		AdjustImagePos();
		return 0;
	}
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		SetCapture();
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		m_ptMouseDownPoint.SetPoint(x, y);
		m_bLMB = TRUE;
		if (IsImageMoveable()) 
		{
			SetCursor(m_crHandPressed);
		}
		return 0;
	}
	LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		ReleaseCapture();
		m_bLMB = FALSE;
		if (IsImageMoveable()) 
		{
			SetCursor(m_crHandUnpressed);
		}
		return 0;
	}
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		if (!(wParam & MK_LBUTTON) ) 
		{
			m_bLMB = FALSE;
		}

		if (m_bLMB) 
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			CPoint newPoint(x, y);
			if (m_Image.IsCreated()) 
			{
				if (MoveImage(m_ptMouseDownPoint.x - x, m_ptMouseDownPoint.y - y))
				{
					Invalidate();
				}						
			}
			m_ptMouseDownPoint = newPoint;

			if (IsImageMoveable()) 
			{
				SetCursor(m_crHandPressed);
			}
		}
		else
		{
			if (IsImageMoveable()) 
			{
				SetCursor(m_crHandUnpressed);
			}
		}
		return 0;
	}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BWPNGVIEWERVIEW_H__10CEF617_DEF8_4714_B2A8_2613FF76E233__INCLUDED_)
