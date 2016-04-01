#include "f_.hpp"

#include <gdiplus.h>

HWND hWnd;
WNDPROC oldProc[100];
NOTIFYICONDATA icone; 
int MainWin_w = 600, MainWin_h = 340;
extern short DEV;
extern HDC dcSkin;
extern char *argv1;
extern int plug_use;
extern short argTest;
extern short Win_Mode;
extern HBITMAP hBmpImage;
TCHAR WindowTitle[20];
bool updated;



void Info_Message(const WCHAR *mess, int etat)
{
	MessageBox(NULL, mess, etat ? L"Error..." : L"Warning...", etat ? MB_ICONERROR : MB_ICONINFORMATION | MB_OK);
}

typedef struct IM_t_s_t{
	WCHAR *mess;
	int etat;
} IM_t_s_t;

DWORD WINAPI Info_Message_tt(void *argv)
{
	IM_t_s_t *IM_t_s = (IM_t_s_t*)argv;
	Info_Message(IM_t_s->mess, IM_t_s->etat);
	free(IM_t_s->mess);
	free(IM_t_s);
	return 0;
}

void Info_Message_t(const WCHAR *mess, int etat)
{
  IM_t_s_t *IM_t_s = (IM_t_s_t*)malloc(sizeof(IM_t_s_t));
  IM_t_s->etat = etat;
  IM_t_s->mess = (WCHAR*)malloc((wcslen(mess)+1)*sizeof(WCHAR));
  wcscpy(IM_t_s->mess, mess);
  CreateThread(NULL, 0, Info_Message_tt, (PDWORD)IM_t_s, 0, NULL);
}

void Gui_Repaint()
{
	RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW|RDW_ALLCHILDREN);
}

void Gui_Update()
{
  UpdateWindow(hWnd);
  Gui_Repaint();
}

void Gui_Exit()
{
	PostMessage(hWnd, WM_QUIT, 0, 0);
}

DWORD WINAPI Update_Launcher_thread(void *argv)
{
	Button_Hide();
	Info_Show();
	updated = update_launcher();
	Info_Hide();
	Button_Show();
	Gui_Exit();
	return 0;
}

// Our application entry point.
int MainWin()
{
  MSG msg;
  WNDCLASS wc;

  LoadString(GetModuleHandle(NULL), WINDOW_TITLE, WindowTitle, 20);

  // Class for our main window.
  wc.style         = CS_DBLCLKS;
  wc.lpfnWndProc   = &MainWin_WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.hIcon         = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON_App));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = WindowTitle;

  // Register our window classes, or error.
  if (! RegisterClass(&wc))
  {
    Info_Message(L"Can't create main Class !", 1);
    return 0;
  }

  // Create instance of main window.
  hWnd = CreateWindowEx( WS_EX_LAYERED, WindowTitle, WindowTitle, WS_POPUP | WS_MINIMIZEBOX, 0, 0, 1, 1, NULL, NULL, wc.hInstance, NULL);

  // Error if window creation failed.
  if (! hWnd)
  {
    Info_Message(L"Can't create main Window !", 1);
    return 0;
  }

   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR          			gdiplusToken;
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	base_create(wc.hInstance, hWnd);
	Info_create(wc.hInstance, hWnd);

  Gdiplus::GdiplusShutdown(gdiplusToken);

  int xPos = (GetSystemMetrics(SM_CXSCREEN) - MainWin_w) /2;
  int yPos = (GetSystemMetrics(SM_CYSCREEN) - MainWin_h)/2;
  SetWindowPos(hWnd, NULL, xPos, yPos, MainWin_w, MainWin_h, SWP_DEFERERASE);

  // Show window and force a paint.
  ShowWindow(hWnd, SW_SHOW);
  Button_Show();
  UpdateWindow(hWnd);
  Info_taskbar();

  Button_Hide();
  Info_Show();
  UpdateWindow(hWnd);

  updated = FALSE;
  CreateThread(NULL, 0, Update_Launcher_thread, NULL, 0, NULL);

  // Update message loop.
  while(GetMessage(&msg, NULL, 0, 0) > 0)
  {
    if (! TranslateAccelerator(msg.hwnd, NULL, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  if (!updated) {
    Info_Hide();
    Button_Show();
    UpdateWindow(hWnd);

    // Main message loop.
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
      if (! TranslateAccelerator(msg.hwnd, NULL, &msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
	}
  }

  ShowWindow(hWnd, SW_HIDE);

//  task_close();
  base_close();

  UnregisterClass(WindowTitle, wc.hInstance);

  return (int) msg.wParam;
}
