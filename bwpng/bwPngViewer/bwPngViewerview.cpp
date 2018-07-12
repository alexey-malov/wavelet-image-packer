// bwPngViewerView.cpp : implementation of the CBwPngViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CompressionSettingsDialog.h"

#include "bwPngViewerView.h"


BOOL CBwPngViewerView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CBwPngViewerView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//dc.FillSolidRect(&m_rcClient, RGB(0,0,0));

	//TODO: Add your drawing code here
	if (m_Image.IsCreated()) 
	{
		DrawImage(dc);
		//
	}

	return 0;
}

LRESULT CBwPngViewerView::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE, _T("png"), m_strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, 
		_T("Png files (*.png)\0*.png\0All Files (*.*)\0*.*\0\0"), m_hWnd);
	
	if (dlg.DoModal() == IDOK) 
	{
		if (m_Image.LoadPng(dlg.m_ofn.lpstrFile))
		{
			m_ptImgSrc.x = m_ptImgSrc.y = 0;
		}
		AdjustImagePos();
		Invalidate();
	}

	return 0;
}

void CBwPngViewerView::DrawImage(CDC &dc)
{
	int srcX = m_ptImgSrc.x;
	int srcY = m_ptImgSrc.y;
	int dstX = 0;
	int dstY  = 0;
	int w = m_rcClient.Width();
	int h = m_rcClient.Height();
	int iw = m_Image.GetWidth();
	int ih = m_Image.GetHeight();

	if (srcX < 0) 
	{
		// output centered image
		dstX = - srcX;
		srcX = 0;
	}
	else
	{
	}

	if (srcY < 0) 
	{
		// output centered image
		dstY = -srcY;
		srcY = 0;
	}
	else
	{
		//ih -= srcY;
		srcY = -srcY;
	}

	if (ih < h) 
	{
		dc.FillSolidRect(0, 0, w, dstY, UNUSED_AREA_COLOR);
		dc.FillSolidRect(0, dstY + ih, w, h - ih - dstY, UNUSED_AREA_COLOR);
	}

	if (iw < w) 
	{
		int hh = min (h, ih);
		dc.FillSolidRect(0, dstY, dstX, hh , UNUSED_AREA_COLOR);
		dc.FillSolidRect(dstX + iw, dstY, w - iw - dstX, hh, UNUSED_AREA_COLOR);
	}

	m_Image.Draw(dc, dstX, dstY, iw, ih, srcX, srcY);
}

void CBwPngViewerView::AdjustImagePos()
{
	int w = m_rcClient.Width();
	int h = m_rcClient.Height();
	int x = m_ptImgSrc.x;
	int y = m_ptImgSrc.y;
	int iw = m_Image.GetWidth();
	int ih = m_Image.GetHeight();

	if (iw > w) // image cannot fit to view
	{
		if ((x >= 0) && (iw >= x + w))
		{
		}
		else // right bound alignment
		{
			x = iw - w;
		}
	}
	else // iw <= w
	{
		// need to center image
		x = (iw - w) / 2;
	}

	if (ih > h) 
	{
		if ((y>=0) && (ih >= y + h))
		{
		}
		else
		{
			y = ih - h;
		}
	}
	else // ih <= h
	{
		// need to center image
		y = (ih - h) / 2;
	}

	m_ptImgSrc.x = x;
	m_ptImgSrc.y = y;

}

CBwPngViewerView::CBwPngViewerView()
	:m_bLMB(FALSE)
{
	m_crHandPressed = LoadCursor(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDC_HAND_PRESSED));
	m_crHandUnpressed = LoadCursor(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDC_HAND_UNPRESSED));
}

BOOL CBwPngViewerView::MoveImage(int dx, int dy)
{
	CPoint oldPos = m_ptImgSrc;
	if ((m_ptImgSrc.x += dx) < 0)
	{
		m_ptImgSrc.x = 0;
	}

	if ((m_ptImgSrc.y += dy) < 0)
	{
		m_ptImgSrc.y = 0;
	}
	AdjustImagePos();
	return oldPos != m_ptImgSrc;
}

BOOL CBwPngViewerView::IsImageMoveable() const
{
	return (m_Image.IsCreated() && ((int)m_Image.GetWidth() > m_rcClient.Width() || (int)m_Image.GetHeight() > m_rcClient.Height() ));
}

void CBwPngViewerView::OnEditCompressionSettings()
{
	CCompressionSettingsDialog dlg(this, &m_Image);
	if (dlg.DoModal() == IDOK) 
	{
		//m_Image.CreateBitmap(true);
		ATLTRACE("packed size: %d\n", m_Image.GetPicturePacker()->GetPackedSize());
		Invalidate();
	}
}

