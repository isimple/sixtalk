/* 
 * File: stkclient.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdlib.h>
#include <string.h>

#include "stk.h"
#include "stkui.h"

client_config client;
HANDLE socketThread;

extern LoginWidget loginWidget;
extern StkWidget stkWidget;

DWORD WINAPI stk_socket(void *param)
{
	client_config *client = NULL;
    char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    int running;
    int ret;

	client = (client_config *)param;

    ret = stk_init_socket();
    if (ret != 0) {
        stk_log("stk_init_socket error\n");
        exit(0);
    }

    stk_log("stk_socket initialization\n");

connect:

    while (1) {
        running = stk_get_running();
        if (running == STK_EXITING) {
            goto exit;
        }
        else if (running == STK_CONNECTE_REQ) {
            stk_set_running(STK_CONNECTING);
            ret = stk_connect(client);
            if (stk_get_running() != STK_CONNECTING) { /* user cancel the login */
                closesocket(client->fd);
                continue;
            }

            if (ret == -1) {
                stk_set_running(STK_CONNECTE_ERR);
                PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
            } else {
                ret = stk_login(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client->pass);
                if (ret == STK_CLIENT_LOGIN_SUCCESS){
                    stk_buddy buddy;
                    client->state = STK_CLIENT_ONLINE;
                    memset(&buddy, 0 ,sizeof(stk_buddy));

                    ret = stk_send_getbuddyinfo(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client->uid, &buddy);
                    if (ret == -1){
                        stk_log("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    } else {
                        memcpy(client->nickname, buddy.nickname, STK_NICKNAME_SIZE);
                        memcpy(client->city, buddy.city,STK_CITY_SIZE);
                        client->phone = buddy.phone;
                        client->gender = buddy.gender;
                    }

                    ret = stk_send_getbuddy(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid);
                    if (ret == -1){
                        stk_log("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    }

                    ret = stk_send_getgroup(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client);
                    if (ret == -1){
                        stk_log("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    }
					stk_log("Connect Success\n");
                    stk_set_running(STK_CONNECTED);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    break;
                }
                else if (ret == STK_CLIENT_LOGIN_ERROR){
					stk_log("Login Err\n");
                    stk_set_running(STK_SOCKET_ERR);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_INVALID_UID){
					stk_log("Login With Bad Username\n");
                    stk_set_running(STK_USERID_ERR);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_INVALID_PASS){
					stk_log("Login With Bad Password\n");
                    stk_set_running(STK_PASSWORD_ERR);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_AGAIN){
					stk_log("Login Again\n");
                    stk_set_running(STK_ALREADY_LOGGIN);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    goto connect;
                }
                else {
					stk_log("Login Socket Err\n");
                    stk_set_running(STK_SOCKET_ERR);
                    PostMessage(stkWidget.hwnd, WMAPP_SOCKET, 0, 0);
                    goto connect;
                }
            }
        }
        else {
            Sleep(500);
        }
    }

    /* In fact, this loop will go into stk_recv_msg then blocked~~~ */
    while (1) {
        running = stk_get_running();
        if (running == STK_EXITING) {
            break;
        } else if (running == STK_RUNNING){
            /* receive message from server, this will be blocked */
            ret = stk_recv_msg(client);
            if (ret == STK_SOCKET_ERROR) {
                stk_set_running(STK_CONNECTE_ERR);
            } else if (ret == STK_SOCKET_CLOSED) {
                stk_set_running(STK_SERVER_EXIT);
            }
        } else {
            Sleep(500);
            continue;
        }
    }

exit:
   closesocket(client->fd);
   return 0;
}

/* TAB Use, Ref: http://blog.csdn.net/tdgx2004/article/details/6552926 */
void SetTabFocus(HWND hwnd)
{
    BOOL bFound = FALSE;
    HWND hwndNext = NULL;
    HWND firstChild = NULL;

    while(hwndNext = FindWindowEx(hwnd,hwndNext,NULL,NULL))
    {
        DWORD dwStyle = GetWindowLong(hwndNext, GWL_STYLE);
        if ((dwStyle & WS_TABSTOP) && (dwStyle & WS_VISIBLE) && !(dwStyle & WS_DISABLED))
        {
            if (!bFound)
            {
                if (firstChild==NULL) firstChild=hwndNext;
                if (GetFocus()==hwndNext)
                {
                    bFound=TRUE;
                }
            } else {
                bFound=FALSE;
                SetFocus(hwndNext);
                break;
            }
        }
    }
    if (bFound) 
        SetFocus(firstChild);
}

void stk_msg_event(BOOL isgroup, void *ptr)
{
    /* Play a wav to nitify user msg is coming */
    //CreateThread(NULL, 0, stk_play_wav, (void *)&STK_MSG_COMING_WAV, 0, NULL);
    stk_play_wav(STK_MSG_COMING_WAV);

    /* Notify window */
    PostMessage(stkWidget.hwnd, WMAPP_MSG, (WPARAM)isgroup, (LPARAM)ptr);
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT uVar;
    DWORD dwVar;
    HDC hdc;
    POINT pt;
    HBRUSH hbr;
    TVITEM tvitem;
    HWND window;
    void *ptr = NULL;
    char buf[STK_DEFAULT_SIZE] = {0};

    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_SHOW:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    break;
                case IDM_HIDE:
                case IDM_MIN:
                    ShowWindow(hwnd, SW_HIDE);
                    break;
                case IDM_EXIT:
                case IDM_CLOSE:
                    Shell_NotifyIcon(NIM_DELETE, &stkWidget.nid);
                    stk_log("WinMain set STK_EXITING\n");
                    stk_set_running(STK_EXITING);
                    WaitForSingleObject(socketThread, INFINITE);
                    PostQuitMessage (0);
                    break;
                case IDM_LOGIN:
                    stk_loginbtn_pressed();
                    break;
                case IDM_CANCEL:
                    stk_connectwin_show(FALSE);
                    break;
                default:
                    window = GetParent((HWND)lParam);
                    if (window != NULL && window != stkWidget.hwnd) {
                        ptr = (void *)stk_find_buddy_by_window(window);
                        if (ptr != NULL) {
                            if (((stk_buddy *)ptr)->chat.closebtn == (HWND)lParam) {
                                SendMessage(((stk_buddy *)ptr)->chat.window, WM_CLOSE, 0, 0);
                            } else if (((stk_buddy *)ptr)->chat.sendbtn == (HWND)lParam) {
                                stk_sendbtn_pressed((stk_buddy *)ptr);
                            }
                        } else {
                            int num;
                            BOOL found = FALSE;
                            stk_group *group;

                            num = client.group_num;
                            group = client.group;
                            while (num-- && group != NULL) {
                                if (group->gchat.window == window) {
                                    found = TRUE;
                                    break;
                                }
                                group = group->next;
                            }

                            if (found && group != NULL) {
                                if (group->gchat.closebtn == (HWND)lParam) {
                                    SendMessage(group->gchat.window, WM_CLOSE, 0, 0);
                                } else if (group->gchat.sendbtn == (HWND)lParam) {
                                    stk_gsendbtn_pressed(group);
                                }
                            }
                        }
                    }
                    break;
            }
            break;
        case WM_NCHITTEST:
            uVar = DefWindowProc(hwnd, message, wParam, lParam);
            if(uVar == HTCLIENT && GetAsyncKeyState(MK_LBUTTON) < 0) {
                uVar = HTCAPTION;
            }
            return uVar;
        case WMAPP_TRAY:
            switch (LOWORD(lParam))
            {
                case WM_LBUTTONDBLCLK:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    break;
                case WM_RBUTTONUP:
                case WM_CONTEXTMENU:
                    GetCursorPos(&pt);
					uVar = TPM_RIGHTBUTTON;
					SetForegroundWindow(hwnd);
					// respect menu drop alignment
					if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
						uVar |= TPM_RIGHTALIGN;
					else
						uVar |= TPM_LEFTALIGN;
					TrackPopupMenuEx(stkWidget.menu, uVar, pt.x, pt.y, hwnd, NULL);
                    break;
            }
            break;
        case WMAPP_SOCKET:
            uVar = stk_get_running();
            switch(uVar) {
                case STK_SOCKET_ERR:
                case STK_CONNECTE_ERR:
                    strcpy(buf, "Connect to stkserver falied.");
                    break;
                case STK_USERID_ERR:
                    strcpy(buf, "Invalid Username.");
                    break;
                case STK_PASSWORD_ERR:
                    strcpy(buf, "Invalid Password.");
                    break;
                case STK_ALREADY_LOGGIN:
                    strcpy(buf, "Already Logined in.");
                    break;
                case STK_CONNECTED:
                    sprintf(buf, "%s:%d(%s)", STKCLIENT, client.uid, client.nickname);
                    strcpy(stkWidget.nid.szTip, buf);
                    stk_buddywin_create();
                    stk_set_running(STK_RUNNING);
					stk_log("User %d Login in.\n", client.uid);
                    return;
                default:
                    break;
            }
            if (buf[0] != '\0') {
                /* Login failed */
                SetWindowText(loginWidget.resultlabel, buf);
                stk_connectwin_show(FALSE);
                UpdateWindow(stkWidget.hwnd);
                stk_set_running(STK_INITIALIZED);
			}
            break;
        case WMAPP_MSG:
            if ((BOOL)wParam) {
				stk_gchatwin_display((stk_group *)lParam);
            } else {
				stk_chatwin_display((stk_buddy *)lParam);
            }
        case WM_NOTIFY:
            if(LOWORD(wParam) == IDM_TABCTRL)
            {
                if(((LPNMHDR)lParam)->code == TCN_SELCHANGE)
                {	
                    uVar = TabCtrl_GetCurSel(stkWidget.tabcontrol);
                    switch(uVar)
                    {	
                        case STK_TREE_BUDDY:
							ShowWindow(stkWidget.buddytreeview, SW_SHOW);
							ShowWindow(stkWidget.grouptreeview, SW_HIDE);
							break;
                        case STK_TREE_GROUP:
							ShowWindow(stkWidget.buddytreeview, SW_HIDE);
							ShowWindow(stkWidget.grouptreeview, SW_SHOW);
							break;
                    }
                }
            }
            else if ((LOWORD(wParam) == IDM_BUDDYTREE) || (LOWORD(wParam) == IDM_GROUPTREE))
            {
                /* Just action with left double click */
                if(((LPNMHDR)lParam)->code == NM_DBLCLK)
                {	
                    memset(&tvitem, 0, sizeof(tvitem));
                    tvitem.hItem = TreeView_GetSelection(((LPNMHDR)lParam)->hwndFrom);;
                    tvitem.mask = TVIF_TEXT;
                    tvitem.pszText = buf;
                    tvitem.cchTextMax = STK_DEFAULT_SIZE;
                    TreeView_GetItem(((LPNMHDR)lParam)->hwndFrom, &tvitem);
                    dwVar = atoi(strtok(buf," "));
                    if (LOWORD(wParam) == IDM_BUDDYTREE)
                        stk_chatwin_show((int)dwVar);
                    else 
                        stk_gchatwin_show((int)dwVar);
                }
            }
            break;
        case WM_CREATE:
            if (hwnd == stkWidget.hwnd) {
                dwVar = GetWindowLong(hwnd, GWL_EXSTYLE);
                dwVar |= WS_EX_TOOLWINDOW | WS_EX_APPWINDOW;
                SetWindowLong(hwnd, GWL_EXSTYLE, dwVar);
            }
            break;
		//case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
            if ((stk_get_running() == STK_RUNNING)
                && (hwnd != stkWidget.profilelabel)) {
/* 
* Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; 
*             instead, they send the WM_CTLCOLORSTATIC  message. 
*/
                hdc = (HDC)wParam;
                hbr = (HBRUSH)CreateSolidBrush(RGB(255,255,255));
                //SetTextColor(hdc, RGB(255,255,255));
                SetBkColor(hdc, RGB(255,255,255));
                return (LRESULT)hbr;
            } else {
                hdc = (HDC)wParam;
                hbr = (HBRUSH)CreateSolidBrush(DEFAULT_RGB);
                //SetTextColor(hdc, RGB(255,255,255));
                SetBkColor(hdc, DEFAULT_RGB);
                if ((HWND)lParam == loginWidget.resultlabel) {
                    SetTextColor(hdc, RGB(255,0,0));
                }
                return (LRESULT)hbr;
            }
        case WM_DESTROY:
            if (hwnd == stkWidget.hwnd) {
                Shell_NotifyIcon(NIM_DELETE, &stkWidget.nid);
                stk_log("WinMain set STK_EXITING\n");
                stk_set_running(STK_EXITING);
                WaitForSingleObject(socketThread, INFINITE);
                PostQuitMessage (0);
            } else {
                ptr = (void *)stk_find_buddy_by_window(hwnd);
                if (ptr == NULL) {
                    int num;
                    BOOL found = FALSE;
                    stk_group *group;
					
                    num = client.group_num;
                    group = client.group;
                    while (num-- && group != NULL) {
						if (group->gchat.window == hwnd) {
							found = TRUE;
							break;
						}
						group = group->next;
                    }
                    if (found && group != NULL) {
                        group->gchat.exist = 0;
                        group->gchat.window = NULL;
                    }
                } 
                else {
                    ((stk_buddy *)ptr)->chat.exist = 0;
                    ((stk_buddy *)ptr)->chat.window = NULL;
                }
            }
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    HICON icon;

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = STKCLIENT;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mousee-pointer */
    /* Note: hIconSm is window title icon, hIcon is program icon */
    icon = (HICON)LoadImage(NULL, STK_CLIENT_ICO, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wincl.hIcon = icon;
    wincl.hIconSm = icon;

    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = CreateSolidBrush(DEFAULT_RGB);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* Register chat window class*/
    //stk_chatwin_register();

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                     /* Extended possibilites for variation */
           STKCLIENT,           /* Classname */
           STKCLIENT,           /* Title Text */
           WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX, /* 无标题栏窗口 */
           950,         /* Windows decides the position */
           80,         /* where the window ends up on the screen */
           STK_MAINWIN_WIDTH,     /* The programs width */
           STK_MAINWIN_HEIGHT,    /* and height in pixels */
           HWND_DESKTOP,          /* The window is a child-window to desktop */
           NULL,                  /* No menu */
           hThisInstance,         /* Program Instance handler */
           NULL                   /* No Window Creation data */
           );

    /* Store hwnd to global var */
	stkWidget.hins = hThisInstance;

    stkWidget.hwnd = hwnd;

    /* add tray */
    stkWidget.nid.cbSize = sizeof(stkWidget.nid);
    stkWidget.nid.hWnd = hwnd;
    stkWidget.nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    stkWidget.nid.uCallbackMessage = WMAPP_TRAY;
    stkWidget.nid.hIcon = icon;
    strcpy(stkWidget.nid.szTip, STKCLIENT);
    Shell_NotifyIcon(NIM_ADD, &stkWidget.nid);

    /* STK UI */
    stk_create_ui(hwnd);

    /* init global vars and socket thread */
	memset(&client, 0, sizeof(client));
    stk_init_running();
	socketThread = CreateThread(0, 0, stk_socket, (void*)&client, 0, NULL);

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nFunsterStil);

    /* Let socket know we have done the initialization*/
    stk_log("WinMain set STK_INITIALIZED\n");
    stk_set_running(STK_INITIALIZED);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);

        if (messages.message == WM_KEYDOWN && messages.wParam == VK_TAB)
        {
            if (stk_get_running() == STK_INITIALIZED) {
                /* Press Tab, switch focus */
                SetTabFocus(hwnd);
            }
        } else if (messages.message == WM_KEYDOWN && messages.wParam == VK_RETURN) {
            if (stk_get_running() == STK_INITIALIZED) {
                /* Press Enter, Login in */
                stk_loginbtn_pressed();
            }
        } else {
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

