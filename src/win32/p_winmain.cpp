#include "f_.hpp"

extern int MainWin_w, MainWin_h;
extern HDC dcSkin;
extern HWND hWnd_btn[8];
//#define no_real_work

#include <Tlhelp32.h>
#include <Aclapi.h>
#include <ctype.h>

HMENU Hmenu, Hmenu2;
short Win_Mode = 0;


DWORD WINAPI Play_thread(void *argv)
{
	Button_Hide();
	WCHAR wTMP[1024];
	Edit_GetText(hWnd_btn[5], wTMP, 1024);
	if (loginName) free(loginName);
	loginName = to_CHAR(wTMP);
	Edit_GetText(hWnd_btn[6], wTMP, 1024);
	if (loginPass) free(loginPass);
	loginPass = to_CHAR(wTMP);
	int lng = ComboBox_GetCurSel(hWnd_btn[8]);
	loginLang = LANG_ARRAY[lng][1];
	loginSave = Button_GetCheck(hWnd_btn[7]);
	play();
	Button_Show();
	Win_Mode = 0;
	return 0;
}

DWORD WINAPI Update_thread(void *argv)
{
	Button_Hide();
	Info_Show();
	update();
	Info_Hide();
	Button_Show();
	Win_Mode = 0;
	return 0;
}

DWORD WINAPI Site_thread(void *argv)
{
	wchar_t *tmp = to_WCHAR(URL_WEB);
	ShellExecute(NULL, L"open", tmp, NULL, NULL, SW_SHOW);
	free(tmp);
	Win_Mode = 0;
	return 0;
}

short Gui_Busy()
{
	return Win_Mode;
}



// Window procedure for our main window.
LRESULT CALLBACK MainWin_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
        case 101:
        {
			if (Win_Mode == 100) {
				cURL_cleanup();
				exit(0);
			}
			if (!Win_Mode || Win_Mode > 100) return DestroyWindow(hWnd);
			Win_Mode = 0;
			return 0;
        }
		default:
		{
			switch (Win_Mode)
			{
				case 0:
				{
					switch (LOWORD(wParam))
					{
						case 102:
						{
							Win_Mode = 1;
							CreateThread(NULL, 0, Play_thread, NULL, 0, NULL);
							return 0;
						}
						case 103:
						{
							Win_Mode = 2;
							CreateThread(NULL, 0, Update_thread, NULL, 0, NULL);
							return 0;
						}
						case 104:
						{
							Win_Mode = 3;
							CreateThread(NULL, 0, Site_thread, NULL, 0, NULL);
							return 0;
						}
					}
					return 0;
				}
				return 0;
			}
		}
      }
      break;
    }

	case WM_CTLCOLORBTN:
		return (LRESULT) GetStockObject(HOLLOW_BRUSH);

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd,&ps);
	  background_update(hWnd, ps.hdc, dcSkin);
	  UpdateWindow(hWnd);
      EndPaint(hWnd,&ps);
      break;
    }

    case WM_LBUTTONDOWN:
    {
	  int xPos = GET_X_LPARAM(lParam);
	  int yPos = GET_Y_LPARAM(lParam);
	  if (xPos < MainWin_w-50 && yPos < 23) SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)NULL);
      break;
    }

    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }
	
	case WM_CREATE:
		background_update(hWnd, NULL, dcSkin);
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
