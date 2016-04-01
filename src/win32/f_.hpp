#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <unistd.h>

#include "resource.h"
#include "src/globale.h"
#include "p_.hpp"

#define SS_REALSIZECONTROL    0x00000040L
#define PBS_MARQUEE           0x08
#define PBM_SETMARQUEE        (WM_USER+10)
#define WM_ICONSYSTRAY WM_USER

void Info_Message(const WCHAR *, int);
int OpenDialog(WCHAR *, int, const WCHAR *, const WCHAR *);
extern short main_work;
extern long PSO2_ProcessID;

void Gui_Update();

void PictureInPicture(HBITMAP, HBITMAP, int, int, COLORREF);
HBITMAP PictureBottomAdd(HBITMAP hBmp, HBITMAP hBmpMask, int xPos, int yPos, short);
HBITMAP LoadJPEG(HINSTANCE, LPCTSTR);
HBITMAP LoadJPEG(const char *lpName);
void Picture_write(HBITMAP hBmp, short fontID, const WCHAR *text, int x, int y, short align);


void kill_ProcessID(long ProcessID);
short running_ProcessID(long ProcessID);
short running_PSO2(long ProcessID);
long get_ProcessID(const WCHAR *);

int Images_Hide();
int Images_Show();
int Button_Hide();
int Button_Show();
void Images_update();
void globale_work_begin();
void globale_work_end();
void background_update(HWND, HDC, HDC);
void base_create(HINSTANCE, HWND);
void base_close();
DWORD WINAPI Play_thread(void *);

int Info_Hide();
int Info_Show();
void Info_Progress_Force(double, double);
void Info_Progress(double, double);
void Info_create(HINSTANCE, HWND);
void Info_taskbar();
void task_close();

void clean_tmpDIR();
