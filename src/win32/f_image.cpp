#include <windows.h>
#include <gdiplus.h>
#include <gdiplus/gdiplusgraphics.h>
#include <stdio.h>

#include "f_.hpp"

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) | ((Color << 16) & 0xff0000)

#define RGBA(r,g,b,a) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))
#define GetAValue(rgba) (LOBYTE(rgba>>24))
#define AFilter(c,a) RGBA(GetRValue(c),GetGValue(c),GetBValue(c),a)

//Font *l_Font[2];
char *FONT_PATH();

void PictureInPicture(HBITMAP hBmp, HBITMAP hBmpMask, int xPos, int yPos, COLORREF cMaskColor)
{
	HDC BufferDC = CreateCompatibleDC(NULL);    // DC for Source Bitmap
	HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);

	BITMAP bm;
	HDC BufferDCMask = CreateCompatibleDC(NULL);    // DC for Source Mask
	HGDIOBJ PreviousBufferObjectMask = SelectObject(BufferDCMask, hBmpMask);
	GetObject(hBmpMask, sizeof(bm), &bm);

	cMaskColor = COLORREF2RGB(cMaskColor);

    // After all the inits we can do the job : Picture In Picture
	for (int h=0; h<bm.bmHeight; h++)
	{
		for (int w=0; w<bm.bmWidth; w++)
		{
			COLORREF cOldColor = GetPixel(BufferDCMask, w, h);
			if (cMaskColor!=cOldColor) SetPixel(BufferDC, w+xPos, h+yPos, cOldColor);
		}
	}
	
    // little clean up
	SelectObject(BufferDCMask,PreviousBufferObjectMask);
	DeleteDC(BufferDCMask);

    SelectObject(BufferDC,PreviousBufferObject);
	DeleteDC(BufferDC);
}

HBITMAP PictureBottomAdd(HBITMAP hBmp, HBITMAP hBmpMask, int xPos, int yPos, short ARGB)
{
	BITMAP bm;
	GetObject(hBmp, sizeof(bm), &bm);

	HDC BufferDC = CreateCompatibleDC(NULL);    // DC for Source Bitmap
	HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);

	HDC BufferDCMask = CreateCompatibleDC(NULL);    // DC for Source Mask
	HGDIOBJ PreviousBufferObjectMask = SelectObject(BufferDCMask, hBmpMask);

//	cMaskColor = COLORREF2RGB(cMaskColor);

    // After all the inits we can do the job : Picture In Picture
	for (int h=0; h<bm.bmHeight; h++)
	{
		for (int w=0; w<bm.bmWidth; w++)
		{
			COLORREF cOldColor1 = GetPixel(BufferDCMask, w+xPos, h+yPos);
				BYTE R1 = GetRValue(cOldColor1);
				BYTE G1 = GetGValue(cOldColor1);
				BYTE B1 = GetBValue(cOldColor1);
			COLORREF cOldColor2 = GetPixel(BufferDC, w, h);
				BYTE R2 = GetRValue(cOldColor2);
				BYTE G2 = GetGValue(cOldColor2);
				BYTE B2 = GetBValue(cOldColor2);
				BYTE a = 255;
				switch (ARGB) {
					case 1:
						a = R2 ? 255 : 0;
						break;
					case 2:
						a = G2 ? 255 : 0;
						break;
					case 3:
						a = B2 ? 255 : 0;
						break;
					case -1:
						a = R2;
						break;
					case -2:
						a = G2;
						break;
					case -3:
						a = B2;
						break;
				}
			SetPixel(BufferDC, w, h, RGB(
				R1 + ((R2-R1) * a / 255),
				G1 + ((G2-G1) * a / 255),
				B1 + ((B2-B1) * a / 255)
			));
		}
	}
    // little clean up
	SelectObject(BufferDCMask,PreviousBufferObjectMask);
	DeleteDC(BufferDCMask);

    SelectObject(BufferDC,PreviousBufferObject);
	DeleteDC(BufferDC);
	
	return NULL;
}

HBITMAP LoadJPEG(HINSTANCE hInstance, LPCTSTR lpName)
{
  HRSRC hrcMyData = FindResource(hInstance, lpName, RT_RCDATA);
  HGLOBAL hMyData = LoadResource(hInstance, hrcMyData);
  LPVOID pImgData = LockResource(hMyData);
  DWORD imgSize = SizeofResource(hInstance, hrcMyData);

  HBITMAP hBmp = NULL;
  Gdiplus::Bitmap* m_pBitmap;
  LPVOID m_hBuffer = GlobalAlloc(GMEM_MOVEABLE, imgSize);
  void* pBuffer = GlobalLock(m_hBuffer);
  CopyMemory(pBuffer, pImgData, imgSize);
  IStream* pStream = NULL;
  if (CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK) {
    m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
    pStream->Release();
    if (m_pBitmap) { 
      if (m_pBitmap->GetLastStatus() == Gdiplus::Ok) {
		  Gdiplus::Color color;
		  m_pBitmap->GetHBITMAP(color, &hBmp);
		}
      delete m_pBitmap;
      m_pBitmap = NULL;
    }
  }
  m_pBitmap = NULL;
  GlobalUnlock(m_hBuffer);
  GlobalFree(m_hBuffer);
  m_hBuffer = NULL;
  return hBmp;
}

HBITMAP LoadJPEG(const char *lpName)
{
  HBITMAP hBmp = NULL;
  Gdiplus::Bitmap* m_pBitmap;
  wchar_t wcstring[1024];
  MultiByteToWideChar(CP_ACP,0,lpName,-1,wcstring, 1024);
    m_pBitmap = Gdiplus::Bitmap::FromFile(wcstring);
    if (m_pBitmap) { 
      if (m_pBitmap->GetLastStatus() == Gdiplus::Ok) {
		  Gdiplus::Color color;
		  m_pBitmap->GetHBITMAP(color, &hBmp);
		} else {
			delete m_pBitmap;
			m_pBitmap = NULL;
			return NULL;
		}
		delete m_pBitmap;
		m_pBitmap = NULL;
    }
  m_pBitmap = NULL;
  return hBmp;
}

/**
align:
	0 = left
	1 = center
	2 = right
**/
void Picture_write(HBITMAP hBmp, short fontID, const WCHAR *text, int x, int y, short align)
{
	using namespace Gdiplus;
	if(hBmp == NULL)
		return ;

	HDC draw = CreateCompatibleDC(NULL);	// DC for Source Bitmap
	HGDIOBJ PreviousBufferObject = SelectObject(draw, hBmp);
	BITMAP bm; 
	GetObject(hBmp, sizeof(BITMAP), &bm);
	Graphics gr(draw);

	RectF therect;
	StringFormat format;
	format.SetLineAlignment(StringAlignmentCenter);
    switch(align) {
		case 1:
			format.SetAlignment(StringAlignmentCenter);
			break;
		case 2:
			format.SetAlignment(StringAlignmentFar);
			break;
		default:
			format.SetAlignment(StringAlignmentNear);
	}
	FontFamily *family = new FontFamily(L"Arial");
	Font font(family, fontID?12:20,  FontStyleBold);

	gr.SetTextRenderingHint(TextRenderingHintAntiAlias);

	SolidBrush brush(Color::White);
	SolidBrush brushX(Color::Black);
	short h=1;
	for (int i=0-h; i<h+1; i++) {
		for (int j=0-h; j<h+1; j++) {
			therect.Width = bm.bmWidth-5-(align==2?x:0)+i;
			therect.Height = bm.bmHeight-5-(align==2?y:0)+j;
			therect.X = 5+(align!=2?x:0)+i;
			therect.Y = 5+(align!=2?y:0)+j;
			gr.DrawString(text, -1, &font, therect, &format, &brushX);
		}
	}

	therect.Width = bm.bmWidth-5-(align==2?x:0);
	therect.Height = bm.bmHeight-5-(align==2?y:0);
	therect.X = 5+(align!=2?x:0);
	therect.Y = 5+(align!=2?y:0);

	gr.DrawString(text, -1, &font, therect, &format, &brush);
	gr.Flush();

    SelectObject(draw, PreviousBufferObject);
	DeleteDC(draw);
}
