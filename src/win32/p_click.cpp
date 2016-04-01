#include "f_.hpp"

extern HWND hWnd;
extern HWND hWnd_btn[];
extern WNDPROC oldProc[];
extern HBITMAP hBmpButton[][3];

LRESULT CALLBACK btn_on_click(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int id = (int)GetMenu(hwnd);
  switch (msg)
  {
    case WM_LBUTTONDOWN:
        InvalidateRect(hwnd, NULL, TRUE);
        SendMessage(hWnd, WM_COMMAND, (WPARAM)id, (LPARAM)NULL);
        break;
	default:
        return CallWindowProc(oldProc[id-101], hwnd, msg, wParam, lParam);
  }
  return 0;
}

LRESULT CALLBACK B_Color(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#define IDM_BUTTONUP 1
#define IDM_BUTTONDOWN 2
#define IDM_BUTTONHOVER 3

    TRACKMOUSEEVENT tme;
    PAINTSTRUCT ps;
    HDC hdc, memDC;
    static int action=IDM_BUTTONUP;
	int id = (int)GetMenu(hwnd);

    tme.cbSize=sizeof(TRACKMOUSEEVENT);
    tme.dwFlags=TME_LEAVE | TME_HOVER;
    tme.dwHoverTime=1;
    tme.hwndTrack=hwnd;

	HBITMAP *l_hBmpButton = hBmpButton[id-101+(hwnd == hWnd_btn[0] ?0:1)];
    switch(message)
    {
        case WM_PAINT:
            hdc=BeginPaint(hwnd, &ps);
            memDC=CreateCompatibleDC(hdc);
            switch(action)
            {
                case IDM_BUTTONUP:
                    SelectObject(memDC, l_hBmpButton[0]);
                    break;
                case IDM_BUTTONHOVER:
                    SelectObject(memDC, l_hBmpButton[1]);
                    break;
                case IDM_BUTTONDOWN:
                    SelectObject(memDC, l_hBmpButton[l_hBmpButton[2]?2:0]);
                    break;
            }
			RECT t_RECT;
			GetWindowRect(hwnd, &t_RECT);
			BitBlt(hdc, 0,0, t_RECT.right-t_RECT.left, t_RECT.bottom-t_RECT.top, memDC, 0, 0, SRCCOPY);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            break;

        case WM_MOUSEMOVE:
            TrackMouseEvent(&tme);
            break;

        //mouse hover over the button
        case WM_MOUSEHOVER:
            if(action!=IDM_BUTTONDOWN)
            {
                    action=IDM_BUTTONHOVER;
                    InvalidateRect(hwnd, NULL, TRUE);
            }
            break;

        case WM_LBUTTONDOWN:
            action=IDM_BUTTONDOWN;
            InvalidateRect(hwnd, NULL, TRUE);
            //send a message to parent widnow of a button when user presses the left mouse button
			if (hwnd == hWnd_btn[1]) return ShowWindow(hWnd, SW_MINIMIZE);
            SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)id, (LPARAM)NULL);
            break;

        case WM_LBUTTONUP:
            action=IDM_BUTTONUP;
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        //mouse leave the button
        case WM_MOUSELEAVE:
            action=IDM_BUTTONUP;
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        default:
            //passes message to parent window of this button.
            return CallWindowProc(oldProc[id-101], hwnd, message, wParam, lParam);

    }
    return 0;
}
