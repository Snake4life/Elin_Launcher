#include "f_.hpp"

#include <ctime>
#include "TaskbarList.h"

extern short Win_Mode;
extern int MainWin_w, MainWin_h;
extern HWND hWnd;

HWND hWndX[3];
ITaskbarList3* m_pTaskBarlist;
float speed;
clock_t t_clock;

int Info_Hide()
{
  Win_Mode -= 100;
  for (int i=0; i<3; i++) {
	ShowWindow(hWndX[i], SW_HIDE);
	UpdateWindow(hWndX[i]);
  }
  UpdateWindow(hWnd);
  if (m_pTaskBarlist) m_pTaskBarlist->SetProgressState( hWnd, TBPF_NOPROGRESS );
  return 0;
}

int Info_Show()
{
  speed = 0;
  t_clock = clock();
  Info_Progress(-100, -100);
  Win_Mode += 100;
  SetWindowText(hWndX[0], L"");
  UpdateWindow(hWndX[0]);
  SetWindowText(hWndX[1], L"");
  UpdateWindow(hWndX[1]);
  SendMessage(hWndX[2], PBM_SETPOS, 0, 0);
  UpdateWindow(hWndX[2]);
  for (int i=0; i<3; i++) {
	ShowWindow(hWndX[i], SW_SHOW);
	UpdateWindow(hWndX[i]);
  }
  UpdateWindow(hWnd);
  if (m_pTaskBarlist) m_pTaskBarlist->SetProgressState( hWnd, TBPF_NORMAL );
  return 0;
}

void Info_Progress(double pos, double size)
{
  static double s_pos=-100, t_pos=0;
  static clock_t s_clock;
  if (s_clock + 30 > clock()) return;
  if (s_pos != pos && pos > -1) {
	if (clock() >= 5000 + t_clock) {
		speed = (pos - t_pos) * 1024 * 1000 / (clock() - t_clock);
		t_pos = pos;
		t_clock = clock();
	}
	WCHAR txt[1024];
	switch (sTat)
	{
	  case 1:
		SetWindowText(hWndX[1], L"Getting MD5...");
		UpdateWindow(hWndX[1]);
		break;
	  case 2:
		SetWindowText(hWndX[1], L"Checking...");
		UpdateWindow(hWndX[1]);
		break;
	  case 3:
		SetWindowText(hWndX[1], L"Launcher Updating...");
		UpdateWindow(hWndX[1]);
		break;
	  default:
		if (speed > 0) {
			long l_time = (size - pos) / (speed / 1024);
			int h = l_time /3600;
			int m = (l_time - h*3600)/60;
			int s = l_time - h*3600 - m*60;
			swprintf(txt, L"%.02f ko/s - %02i:%02i:%02i", speed, h, m, s);
			SetWindowText(hWndX[1], txt);
			UpdateWindow(hWndX[1]);
		}
		break;
	}
	swprintf(txt, L"%.02f / %.02f%s", pos, size, sTat?L"":L" mo");
    SetWindowText(hWndX[0], txt);
	UpdateWindow(hWndX[0]);
	SendMessage(hWndX[2], PBM_SETPOS, pos * 100 / size, 0);
	UpdateWindow(hWndX[2]);
    if (m_pTaskBarlist) m_pTaskBarlist->SetProgressValue( hWnd, pos * 100 / size , 100 );
	s_pos = pos;
  }
  s_clock = clock();
}

void Info_create(HINSTANCE hInstance, HWND hWndI)
{
	// WORK INFO BOX
  hWndX[0] = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD, 1, MainWin_h-18*3, 219, 18, hWnd, 0, NULL, NULL);
  hWndX[1] = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD, 1, MainWin_h-18*2, 219, 18, hWnd, 0, NULL, NULL);
  hWndX[2] = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 0, MainWin_h-18*1, MainWin_w, 18, hWnd, 0, NULL, NULL);
  SetLayeredWindowAttributes(hWnd, 0, 255, ULW_ALPHA);
  m_pTaskBarlist = NULL;
  for (int i=0; i<3; i++) {
	ShowWindow(hWndX[i], SW_HIDE);
	UpdateWindow(hWndX[i]);
  }
  UpdateWindow(hWnd);
}

void Info_taskbar()
{
  RegisterWindowMessage(L"TaskbarButtonCreated");
  HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<void**> (&(m_pTaskBarlist)));
  if (SUCCEEDED(hr)) {
    hr = m_pTaskBarlist->HrInit();
    if (FAILED(hr)) {
      m_pTaskBarlist->Release();
      m_pTaskBarlist = NULL;
    }
  }
}

void task_close()
{
  if (m_pTaskBarlist) m_pTaskBarlist->Release();
  m_pTaskBarlist = NULL;
}
