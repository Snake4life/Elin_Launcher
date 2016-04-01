#include "f_.hpp"

extern WNDPROC oldProc[];
extern int MainWin_w, MainWin_h;
extern short Win_Mode;

extern const wchar_t *BUTTON_PLAY;
extern const wchar_t *BUTTON_CHECK;
extern const wchar_t *BUTTON_SITE;

HWND hWnd_Img;
HDC dcSkin;
HBITMAP hBitmapBG, hBitmapBGOld, hBmpImage;
HBITMAP hBmpButton[5][3];
HWND hWnd_btn[9];
int blitBG = 0;
int plug_use = -1;

int Images_Hide()
{
	ShowWindow(hWnd_Img, SW_HIDE);
	UpdateWindow(hWnd_Img);
	Gui_Update();
  return 0;
}

int Images_Show()
{
  if (!hBmpImage) return 0;
	ShowWindow(hWnd_Img, SW_SHOW);
	UpdateWindow(hWnd_Img);
	Gui_Update();
  return 0;
}

int Button_Hide()
{
  for (int i=2; i<9; i++) {
	ShowWindow(hWnd_btn[i], SW_HIDE);
	UpdateWindow(hWnd_btn[i]);
  }
  return 0;
}

int Button_Show()
{
	WCHAR *wTMP;
	if (loginName) {
		wTMP = to_WCHAR(loginName);
		Edit_SetText(hWnd_btn[5], wTMP);
		free(wTMP);
	}
	if (loginPass) {
		wTMP = to_WCHAR(loginPass);
		Edit_SetText(hWnd_btn[6], wTMP);
		free(wTMP);
	}
	Button_SetCheck(hWnd_btn[7], loginSave);
	for (int i=0; LANG_ARRAY[i][0]; i++) {
	    if (loginLang == LANG_ARRAY[i][1]) ComboBox_SetCurSel(hWnd_btn[8], i);
	}
  for (int i=2; i<9; i++) {
  	ShowWindow(hWnd_btn[i], SW_SHOW);
	UpdateWindow(hWnd_btn[i]);
  }
  return 0;
}

void Images_update()
{
	if (hBmpImage) {
		SendMessage(hWnd_Img, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmpImage);
		if (!Win_Mode) ShowWindow(hWnd_Img, SW_SHOW);
	} else ShowWindow(hWnd_Img, SW_HIDE);
}

void Gui_Button_Upd()
{
  if (Win_Mode) return;
  Button_Show();
}

void globale_work_begin()
{
	Button_Hide();
//	Info_Show();
}
void globale_work_end()
{
//	Info_Hide();
	Button_Show();
}

/*BLENDFUNCTION BlendF = {
	.BlendOp = AC_SRC_OVER,
	.BlendFlags = 0,
	.SourceConstantAlpha = 150,
	.AlphaFormat = AC_SRC_ALPHA
};*/
void background_update(HWND hWnd, HDC hdc, HDC Skin)
{
    /*if (Skin)*/ BitBlt(hdc, 0,0, MainWin_w, MainWin_h, Skin, 0, 0, SRCCOPY);
    SetLayeredWindowAttributes(hWnd, blitBG, 255, ULW_COLORKEY/*ULW_ALPHA*/);

/*	AlphaBlend(hdc, 0, 0, MainWin_w, MainWin_h, Skin, 0, 0, MainWin_w, MainWin_h, BlendF);
	BLENDFUNCTION BlendF = { 0 };
	BlendF.BlendOp = AC_SRC_OVER;
	BlendF.SourceConstantAlpha = 255;
	BlendF.AlphaFormat = AC_SRC_ALPHA;

	POINT ptZero = { 0 };
	SIZE sizeSplash = { MainWin_w, MainWin_h };
	UpdateLayeredWindow(hWnd, NULL, NULL, &sizeSplash, Skin, &ptZero, 0, &BlendF, ULW_ALPHA);
	*/
}

void btn_set_burn_txt(HINSTANCE hInstance, LPCTSTR lpName1, LPCTSTR lpName2, HBITMAP *l_hBmpButton, const WCHAR *text, short size, int xtPos, int ytPos, short align, HBITMAP hBmpBackground, int xPos, int yPos, short ARGB)
{
	for (short i=0; i<2; i++) {
		l_hBmpButton[i] = LoadJPEG(hInstance, i?lpName2:lpName1);
			PictureBottomAdd(l_hBmpButton[i], hBmpBackground, xPos, yPos, ARGB);
				Picture_write(l_hBmpButton[i], size, text, xtPos, ytPos, align);
	}
}

void btn_set_burn_txt_plug(HBITMAP *l_hBmpButton, const WCHAR *text, short size, int xtPos, int ytPos, short align, HBITMAP hBmpBackground, int xPos, int yPos, short ARGB)
{
	PictureBottomAdd(l_hBmpButton[0], hBmpBackground, xPos, yPos, ARGB);
	Picture_write(l_hBmpButton[0], size, text, xtPos, ytPos, align);

	PictureBottomAdd(l_hBmpButton[1], hBmpBackground, xPos, yPos, ARGB);
	Picture_write(l_hBmpButton[1], size, text, xtPos, ytPos, align);
}

void base_create(HINSTANCE hInstance, HWND hWnd)
{
  hBitmapBG = LoadJPEG(hInstance, MAKEINTRESOURCE(IDB_BG_0));

  BITMAP bm;
  GetObject(hBitmapBG, sizeof(bm), &bm);
  MainWin_h = bm.bmHeight;
  MainWin_w = bm.bmWidth;

	hWnd_Img = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | SS_REALSIZECONTROL , 3, 25, 408, 306, hWnd, 0, NULL, NULL);
    ShowWindow(hWnd_Img, SW_HIDE);


	// BUTTON IMG
  for (short i=0; i<2; i++)
	btn_set_burn_txt(hInstance, MAKEINTRESOURCE(IDB_BTN_Close_0), MAKEINTRESOURCE(IDB_BTN_Close_1), hBmpButton[i+0], i?L"-":L"X", 1, -2, -2, 1, hBitmapBG, MainWin_w-(3+20)*(i+1), 3, 3);
  for (short i=0; i<3; i++)
	btn_set_burn_txt(hInstance, MAKEINTRESOURCE(IDB_BTN_Options_0), MAKEINTRESOURCE(IDB_BTN_Options_1), hBmpButton[i+2], i?i>1?BUTTON_SITE:BUTTON_CHECK:BUTTON_PLAY, 1, i?0:25, 0, 0, hBitmapBG, MainWin_w-(6+60)*(i>1?1:2), MainWin_h-(6+32)*(i?1:2), 3);

  blitBG = RGB(0,255,0);
  dcSkin = CreateCompatibleDC(0);
  hBitmapBGOld = (HBITMAP)SelectObject(dcSkin, hBitmapBG);

	// BUTTON PICT
  hWnd_btn[0] = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP , MainWin_w-(3+20)*1, 3, 20, 20, hWnd, (HMENU) 101, NULL, NULL);
	oldProc[0] = (WNDPROC) SetWindowLong(hWnd_btn[0], GWL_WNDPROC, (LONG)B_Color);
  hWnd_btn[1] = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP , MainWin_w-(3+20)*2, 3, 20, 20, hWnd, (HMENU) 101, NULL, NULL);
	oldProc[1] = (WNDPROC) SetWindowLong(hWnd_btn[1], GWL_WNDPROC, (LONG)B_Color);
  hWnd_btn[2] = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP , MainWin_w-(6+60)*2, MainWin_h-(6+32)*2, 6+60*2, 32, hWnd, (HMENU) 102, NULL, NULL);
	oldProc[2] = (WNDPROC) SetWindowLong(hWnd_btn[2], GWL_WNDPROC, (LONG)B_Color);
  hWnd_btn[3] = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP , MainWin_w-(6+60)*2, MainWin_h-(6+32)*1, 60, 32, hWnd, (HMENU) 103, NULL, NULL);
	oldProc[3] = (WNDPROC) SetWindowLong(hWnd_btn[3], GWL_WNDPROC, (LONG)B_Color);
  hWnd_btn[4] = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP , MainWin_w-(6+60)*1, MainWin_h-(6+32)*1, 60, 32, hWnd, (HMENU) 104, NULL, NULL);
	oldProc[4] = (WNDPROC) SetWindowLong(hWnd_btn[4], GWL_WNDPROC, (LONG)B_Color);
  hWnd_btn[5] = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 6, MainWin_h-(6+25)*2, 135, 25, hWnd, 0, NULL, NULL);
  hWnd_btn[6] = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 6, MainWin_h-(6+25)*1, 135, 25, hWnd, 0, NULL, NULL);
  hWnd_btn[7] = CreateWindow(L"BUTTON", L"Remind", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX, 12+135, MainWin_h-6-15-6-25, 15, 15, hWnd, 0, NULL, NULL);
  hWnd_btn[8] = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST , 12+135, MainWin_h-6-25, 50, 25, hWnd, 0, NULL, NULL);
	for (int i=0; LANG_ARRAY[i][0]; i++)
		SendMessage(hWnd_btn[8], CB_ADDSTRING, 0, (LPARAM)LANG_ARRAY[i][0]);
}

void base_close()
{
  SelectObject(dcSkin, hBitmapBGOld);
  DeleteObject(hBitmapBG);
  DeleteDC(dcSkin);
}
