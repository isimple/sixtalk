/* 
 * File: stkui.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */


#include "stk.h"
#include "stkui.h"

StkWidget stkWidget;
LoginWidget loginWidget;

extern client_config client;

void stk_create_ui(HWND hwnd)
{
    BITMAP bmp;
    HBITMAP hBitmap;
    HMENU menu;
    HWND beautylabel;
    HWND closeBtn, minBtn;
    //HICON CloseIcon, MinIcon;
    HWND userlabel,usertext;
    HWND passlabel,passtext;
    HWND serverlabel,servertext;
    HWND loginbtn, cancelbtn;
    HWND loginglabel, resultlabel;
    int width, height;

    /* Popup Menu */
    menu = CreatePopupMenu();

    AppendMenu(menu,  MF_POPUP | MF_STRING,  IDM_SHOW,  "Show");
    AppendMenu(menu,  MF_POPUP | MF_STRING,  IDM_HIDE,  "Hide");
    AppendMenu(menu,  MF_POPUP | MF_STRING,  IDM_EXIT,  "Exit");

    /* Close/Min button and icon */
    closeBtn = CreateWindow(TEXT("BUTTON"), "", WS_CHILD | WS_VISIBLE | BS_BITMAP,//BS_OWNERDRAW,
                 261, 0, 39, 18, hwnd, (HMENU)IDM_CLOSE, NULL, NULL);
    minBtn   = CreateWindow(TEXT("BUTTON"), "", WS_CHILD | WS_VISIBLE | BS_BITMAP,// | BS_ICON,
                 233, 0, 28, 18, hwnd, (HMENU)IDM_MIN,   NULL, NULL);
#if 0
    CloseIcon = (HICON)LoadImage(NULL, STK_CLO_ICO, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    MinIcon   = (HICON)LoadImage(NULL, STK_MIN_ICO, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    SendMessage(CloseBtn, BM_SETIMAGE, IMAGE_ICON, (LPARAM)CloseIcon);
    SendMessage(MinBtn,   BM_SETIMAGE, IMAGE_ICON, (LPARAM)MinIcon);
#endif
    hBitmap = (HBITMAP)LoadImage(NULL, STK_CLO_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    SendMessage(closeBtn, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

    hBitmap   = (HBITMAP)LoadImage(NULL, STK_MIN_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    SendMessage(minBtn,   BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

    /* Draw the beauty picture in a static widget */
    hBitmap = (HBITMAP)LoadImage(NULL, STK_BEATU_BMP, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    GetObject(hBitmap, sizeof(bmp), &bmp);
    loginWidget.width = bmp.bmWidth;
    loginWidget.height = bmp.bmHeight;
    width = (STK_MAINWIN_WIDTH-bmp.bmWidth)/2;
    height = (STK_MAINWIN_HEIGHT-bmp.bmHeight)/4;
    beautylabel = CreateWindow(TEXT("STATIC"), "", WS_CHILD | WS_VISIBLE | SS_BITMAP,
                 width, height, bmp.bmWidth, bmp.bmHeight, hwnd, (HMENU)IDM_BEAUTY, NULL, NULL);
    SendMessage(beautylabel, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

    /* Used by following widgets */
    width  += 25;
    height += bmp.bmHeight;

    /* label and text fields */
    userlabel = CreateWindow(TEXT("STATIC"), "Username:", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
                 width, height+20, 200, 18, hwnd, NULL, NULL, NULL);
    usertext = CreateWindow(TEXT("EDIT"), "", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 width, height+40, 200, 18, hwnd, (HMENU)IDM_USERNAME, NULL, NULL);
    passlabel = CreateWindow(TEXT("STATIC"), "Password:", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
                 width, height+70, 200, 18, hwnd, NULL, NULL, NULL);
    passtext = CreateWindow(TEXT("EDIT"), "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD,
                 width, height+90, 200, 18, hwnd, (HMENU)IDM_USERPASS, NULL, NULL);
    serverlabel = CreateWindow(TEXT("STATIC"), "Server IP:", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
                 width, height+120, 200, 18, hwnd, NULL, NULL, NULL);
    servertext = CreateWindow(TEXT("EDIT"), "", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 width, height+140, 200, 18, hwnd, (HMENU)IDM_SERVERIP, NULL, NULL);

    /* Login button and result label*/
    loginbtn  = CreateWindow(TEXT("BUTTON"), "", WS_CHILD | WS_VISIBLE | BS_BITMAP | WS_TABSTOP,
                 width, height+170, 88, 27, hwnd, (HMENU)IDM_LOGIN,   NULL, NULL);
    hBitmap   = (HBITMAP)LoadImage(NULL, STK_LOGIN_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    SendMessage(loginbtn, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    resultlabel = CreateWindow(TEXT("STATIC"), "", WS_CHILD | SS_SIMPLE,
                 width, height+200, 200, 18, hwnd, (HMENU)IDM_RESULT, NULL, NULL);

    /* Logining label and cancel button */
    hBitmap = (HBITMAP)LoadImage(NULL, STK_LOGINING_BMP, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    GetObject(hBitmap, sizeof(bmp), &bmp);
    width = (STK_MAINWIN_WIDTH-bmp.bmWidth)/2;
	height = (STK_MAINWIN_HEIGHT-loginWidget.height)/3 + loginWidget.height + 20;
    loginglabel = CreateWindow(TEXT("STATIC"), "", WS_CHILD | SS_BITMAP,
                 width, height, bmp.bmWidth, bmp.bmHeight, hwnd, (HMENU)IDM_LOGINING, NULL, NULL);
    SendMessage(loginglabel, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    height += bmp.bmHeight + 10;
    cancelbtn = CreateWindow(TEXT("BUTTON"), "", WS_CHILD | BS_BITMAP,
                 width, height, 88, 27, hwnd, (HMENU)IDM_CANCEL,   NULL, NULL);
    hBitmap   = (HBITMAP)LoadImage(NULL, STK_CANCEL_BMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    SendMessage(cancelbtn, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

    /* Set focus */
    SetFocus(usertext);

    /* Store into global var */
    stkWidget.menu = menu;
    loginWidget.beautylabel = beautylabel;
    loginWidget.userlabel = userlabel;
    loginWidget.usertext = usertext;
    loginWidget.passlabel = passlabel;
    loginWidget.passtext = passtext;
    loginWidget.serverlabel = serverlabel;
    loginWidget.servertext = servertext;
    loginWidget.loginbtn = loginbtn;
    loginWidget.cancelbtn = cancelbtn;
    loginWidget.logininglabel = loginglabel;
    loginWidget.resultlabel = resultlabel;
}

void stk_init_treeview_imgs()
{
    HIMAGELIST himl;
    HBITMAP hbmp;

    // Create buddy the image list. 
    himl = ImageList_Create(40, 40, ILC_COLOR24, 1, 0);

    hbmp = (HBITMAP)LoadImage(NULL, STK_BUDDY_BMP, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    stkWidget.bimgindex = ImageList_Add(himl, hbmp, (HBITMAP)NULL); 
    DeleteObject(hbmp); 

    TreeView_SetImageList(stkWidget.buddytreeview, himl, TVSIL_NORMAL);

    // Create Group the image list. 
    himl = ImageList_Create(40, 40, ILC_COLOR24, 1, 0);

    hbmp = (HBITMAP)LoadImage(NULL, STK_GROUP_BMP, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    stkWidget.gimgindex = ImageList_Add(himl, hbmp, (HBITMAP)NULL); 
    DeleteObject(hbmp); 

    TreeView_SetImageList(stkWidget.grouptreeview, himl, TVSIL_NORMAL); 
}

void stk_init_treeview_items()
{
    int num;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    stk_buddy *buddy = NULL;
    stk_group *group = NULL;
	char buf[STK_DEFAULT_SIZE] = {0};    

    num = stk_get_buddynum();
    while (num--) {
        /* Get buddy infomation */
        buddy = stk_next_buddy(buddy);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%u  %s", buddy->uid, buddy->nickname);

		/* add buddy to treeview */
		tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
		tvi.pszText = buf; 
		tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
		tvi.iImage = stkWidget.bimgindex; 
		tvi.iSelectedImage = stkWidget.bimgindex; 
		tvins.item = tvi; 
		tvins.hInsertAfter = TVI_SORT; 
		tvins.hParent = TVI_ROOT; 
		SendMessage(stkWidget.buddytreeview, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
    }

    num = client.group_num;
    group = client.group;
    while (num-- && group != NULL) {
        /* Get group infomation */
		memset(buf, 0, sizeof(buf));
        sprintf(buf, "%u  %s", group->gid, group->gname);

        /* add group to treeview */
		tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
		tvi.pszText = buf; 
		tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 
		tvi.iImage = stkWidget.gimgindex; 
		tvi.iSelectedImage = stkWidget.gimgindex; 
		tvins.item = tvi; 
		tvins.hInsertAfter = TVI_SORT; 
		tvins.hParent = TVI_ROOT; 
		SendMessage(stkWidget.grouptreeview, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
        group = group->next;
    }
}

void stk_buddywin_create()
{
	BITMAP bmp;
    HBITMAP hBitmap;
    TCITEM tabItem;
    RECT tabrect, lvrect;
    INITCOMMONCONTROLSEX commonctl;
    char buf[STK_DEFAULT_SIZE] = {0};
    //char tabStr[2][64] = {"User List", "Group List"};

    /* Destory unused widgets */
    DestroyWindow(loginWidget.beautylabel);
    DestroyWindow(loginWidget.userlabel);
    DestroyWindow(loginWidget.usertext);
    DestroyWindow(loginWidget.passlabel);
    DestroyWindow(loginWidget.passtext);
    DestroyWindow(loginWidget.serverlabel);
    DestroyWindow(loginWidget.servertext);
    DestroyWindow(loginWidget.loginbtn);
    DestroyWindow(loginWidget.resultlabel);
    DestroyWindow(loginWidget.cancelbtn);
    DestroyWindow(loginWidget.logininglabel);

    /* avatar label and profile label */
    if (client.gender == STK_GENDER_GIRL) {
        strcpy(buf, STK_AVATAR_1_BMP);
    } else {
        strcpy(buf, STK_AVATAR_2_BMP);
    }
    hBitmap = (HBITMAP)LoadImage(NULL, buf, IMAGE_BITMAP, 0, 0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    GetObject(hBitmap, sizeof(bmp), &bmp);
    stkWidget.avatarlabel = CreateWindow(TEXT("STATIC"), "", WS_CHILD | WS_VISIBLE | SS_BITMAP,
                 15, 50, bmp.bmWidth, bmp.bmHeight, stkWidget.hwnd, (HMENU)IDM_AVATAR, NULL, NULL);
    SendMessage(stkWidget.avatarlabel, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
    stkWidget.profilelabel = CreateWindow(TEXT("STATIC"), "", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
                 100, 75, 100, 15, stkWidget.hwnd, (HMENU)IDM_PROFILE, NULL, NULL);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d (%s)", client.uid, client.nickname);
    SetWindowText(stkWidget.profilelabel, buf);

    /* Ensure that the common control DLL is loaded. */
    commonctl.dwSize = sizeof(INITCOMMONCONTROLSEX);
    commonctl.dwICC  = ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&commonctl);

    /* Create tabcontrol */
    /* Background of tabctl is ugly!!!! we must change it */
    stkWidget.tabcontrol = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE , 6, 150, 
          STK_MAINWIN_WIDTH-6*2, STK_MAINWIN_HEIGHT-200, stkWidget.hwnd, (HMENU)IDM_TABCTRL, NULL, NULL);

    tabItem.mask = TCIF_TEXT | TCIF_IMAGE; 
    tabItem.iImage = -1; 
    tabItem.pszText = "User List"; 
    //strcpy(tabItem.pszText, tabStr[0]); 
    TabCtrl_InsertItem(stkWidget.tabcontrol, STK_TREE_BUDDY, &tabItem);
    tabItem.pszText = "Group List"; 
    //strcpy(tabItem.pszText, tabStr[1]);
    TabCtrl_InsertItem(stkWidget.tabcontrol, STK_TREE_GROUP, &tabItem);

    /* Get Treeview Rect */
	TabCtrl_GetItemRect(stkWidget.tabcontrol, STK_TREE_BUDDY, &tabrect);
	lvrect.left = 0;
	lvrect.top = tabrect.bottom - tabrect.top;
	GetClientRect(stkWidget.tabcontrol, &tabrect);
	lvrect.right = tabrect.right;
	lvrect.bottom = tabrect.bottom;

    /* Create Tree-view to show buddys and groups */
    stkWidget.buddytreeview = CreateWindow(WC_TREEVIEW , "", WS_CHILD | TVS_HASLINES, 
                 lvrect.left, lvrect.top, lvrect.right, lvrect.bottom, 
                 stkWidget.tabcontrol, (HMENU)IDM_BUDDYTREE, NULL, NULL);
    stkWidget.grouptreeview = CreateWindow(WC_TREEVIEW , "", WS_CHILD | TVS_HASLINES,
                 lvrect.left, lvrect.top, lvrect.right, lvrect.bottom,
                 stkWidget.tabcontrol, (HMENU)IDM_GROUPTREE, NULL, NULL);
    stk_init_treeview_imgs();
	stk_init_treeview_items();

	ShowWindow(stkWidget.buddytreeview, SW_SHOW);
	ShowWindow(stkWidget.grouptreeview, SW_HIDE);

    UpdateWindow(stkWidget.hwnd);

}

void stk_connectwin_show(BOOL flag)
{
    int width, height;

    if (flag == TRUE) {
        /* Hide these widget */
        ShowWindow(loginWidget.userlabel, SW_HIDE);
        ShowWindow(loginWidget.usertext, SW_HIDE);
        ShowWindow(loginWidget.passlabel, SW_HIDE);
        ShowWindow(loginWidget.passtext, SW_HIDE);
        ShowWindow(loginWidget.serverlabel, SW_HIDE);
        ShowWindow(loginWidget.servertext, SW_HIDE);
        ShowWindow(loginWidget.loginbtn, SW_HIDE);
        ShowWindow(loginWidget.resultlabel, SW_HIDE);
        /* Show these widget */
        width = (STK_MAINWIN_WIDTH-loginWidget.width)/2;
        height = (STK_MAINWIN_HEIGHT-loginWidget.height)/3;
        MoveWindow(loginWidget.beautylabel, width, height, loginWidget.width, loginWidget.height, TRUE);
        ShowWindow(loginWidget.cancelbtn, SW_SHOW);
        ShowWindow(loginWidget.logininglabel, SW_SHOW);
    } else {
        /* Show these widget */
        width = (STK_MAINWIN_WIDTH-loginWidget.width)/2;
        height = (STK_MAINWIN_HEIGHT-loginWidget.height)/4;
        MoveWindow(loginWidget.beautylabel, width, height, loginWidget.width, loginWidget.height, TRUE);
        ShowWindow(loginWidget.userlabel, SW_SHOW);
        ShowWindow(loginWidget.usertext, SW_SHOW);
        ShowWindow(loginWidget.passlabel, SW_SHOW);
        ShowWindow(loginWidget.passtext, SW_SHOW);
        ShowWindow(loginWidget.serverlabel, SW_SHOW);
        ShowWindow(loginWidget.servertext, SW_SHOW);
        ShowWindow(loginWidget.loginbtn, SW_SHOW);
        ShowWindow(loginWidget.resultlabel, SW_SHOW);
        /* Hide these widget */
        ShowWindow(loginWidget.cancelbtn, SW_HIDE);
        ShowWindow(loginWidget.logininglabel, SW_HIDE);
        /* Set focus */
        SetFocus(loginWidget.usertext);
    }
}

void stk_loginbtn_pressed()
{
    int size;
    RECT rect;
    char username[STK_DEFAULT_SIZE] = {0};
    char password[STK_DEFAULT_SIZE] = {0};
    char serverip[STK_DEFAULT_SIZE] = {0};
    char errbuf[STK_DEFAULT_SIZE] = {0};

    /* clear old config before */
    client.uid = 0;
    memset(client.pass, 0, sizeof(client.pass));
    memset(client.serverip, 0, sizeof(client.serverip));

	size = GetWindowText(loginWidget.usertext, username, STK_DEFAULT_SIZE);
    /* safety check */
    if (size == 0) {
        char *err = "Username is NULL";
        strcpy(errbuf, err);
        goto error;
    } else {
        stk_log("Username: %s\n", username);
        client.uid = atoi(username);
    }

	size = GetWindowText(loginWidget.passtext, password, STK_DEFAULT_SIZE);
    if (size == 0) {
        char *err = "Password is NULL";
        strcpy(errbuf, err);
        goto error;
    } else {
        stk_log("Password: %s\n", password);
        strncpy(client.pass, password, size);
    }

	size = GetWindowText(loginWidget.servertext, serverip, STK_DEFAULT_SIZE);
    if (size == 0) {
        char *err = "Server IP is NULL";
        strcpy(errbuf, err);
        goto error;
    } else {
        stk_log("ServerIP: %s\n", serverip);
        strncpy(client.serverip, serverip, size);
    }

    SetWindowText(loginWidget.resultlabel, "");
    stk_set_running(STK_CONNECTE_REQ);
    stk_connectwin_show(TRUE);
    return;

error:
    /* Show login result label */
    SetWindowText(loginWidget.resultlabel, errbuf);
    ShowWindow(loginWidget.resultlabel, SW_SHOW);
   /**
      * There is a bug, resultlabel does not refresh sometimes
      * TODO:
      */
#if 1
    GetClientRect(loginWidget.resultlabel, &rect);
	InvalidateRect(stkWidget.hwnd, &rect, TRUE);
	UpdateWindow(stkWidget.hwnd);
#else
    GetClientRect(loginWidget.resultlabel, &rect);
	InvalidateRect(stkWidget.hwnd, &rect, TRUE);
    RedrawWindow(stkWidget.hwnd, &rect, NULL, RDW_ERASE | RDW_INVALIDATE);
#endif
}
void stk_chatwin_create(stk_buddy *buddy)
{
    HWND hwnd;
    int width, height;
    HWND msgedit, inputedit;
    HWND sendbtn, closebtn;
    char buf[STK_DEFAULT_SIZE] = {0};

    if (buddy == NULL) {
        stk_log("stk_chatwin_create: Bad param\n");
        return;
    }

    sprintf(buf, "Chat With %s", buddy->nickname);
    hwnd = CreateWindowEx(0, STKCLIENT, buf,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU  | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, 
        CW_USEDEFAULT, STK_CHATWIN_WIDTH, STK_CHATWIN_HEIGHT, NULL, NULL, stkWidget.hins, NULL);

    SendMessage(hwnd, WM_SETICON, (WPARAM)TRUE, 
            (LPARAM)LoadImage(NULL, STK_BUDDY_ICO, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));

    msgedit = CreateWindowEx(0, TEXT("EDIT"), "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_TABSTOP | ES_AUTOVSCROLL,
        2, 5, STK_CHATWIN_WIDTH-10, STK_CHATWIN_HEIGHT*2/3-40, hwnd, NULL, stkWidget.hins, NULL);
    //Edit_Enable(msgedit, FALSE);
    Edit_SetReadOnly(msgedit, TRUE);

    inputedit = CreateWindowEx(0, TEXT("EDIT"), "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_TABSTOP | ES_AUTOVSCROLL,
        2, STK_CHATWIN_HEIGHT*2/3-25, STK_CHATWIN_WIDTH-10, STK_CHATWIN_HEIGHT/3-35, hwnd, NULL, stkWidget.hins, NULL);

    sendbtn  = CreateWindow(TEXT("BUTTON"), " Send ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        STK_CHATWIN_WIDTH-140, STK_CHATWIN_HEIGHT-55, 50, 20, hwnd, NULL, NULL, NULL);
    closebtn = CreateWindow(TEXT("BUTTON"), " Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        STK_CHATWIN_WIDTH-80,  STK_CHATWIN_HEIGHT-55, 50, 20, hwnd, NULL, NULL, NULL);

    buddy->chat.exist     = 1;
    buddy->chat.window    = hwnd;
    buddy->chat.msgedit   = msgedit;
    buddy->chat.inputedit = inputedit;
    buddy->chat.sendbtn   = sendbtn;
    buddy->chat.closebtn  = closebtn;

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    SetFocus(inputedit);
}

void stk_chatwin_show(int uid)
{
    stk_buddy *buddy;

    buddy = stk_find_buddy(uid);
    if (buddy == NULL) {
        stk_log("stk_chatwin_show: Bad buddy uid\n");
        return;
    }

    if (!buddy->chat.exist) {
        stk_chatwin_create(buddy);
    }
    else {
        ShowWindow(buddy->chat.window, SW_RESTORE);
        SetForegroundWindow(buddy->chat.window);
        UpdateWindow(buddy->chat.window);
        SetFocus(buddy->chat.inputedit);
    }
}

void stk_chatwin_display(stk_buddy *buddy)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    char msg[STK_MAX_PACKET_SIZE] = {0};
    int msg_len;

    if (!buddy->chat.exist)
        stk_chatwin_create(buddy);

    while (buddy->msg_num > 0) {
        if (stk_get_msg(buddy, msg, &msg_len, timestamp) == -1)
            break;
        sprintf(buf, "%s(%d) %s\r\n", buddy->nickname, buddy->uid, timestamp);
        Edit_SetSel(buddy->chat.msgedit, -1, -1);
        Edit_ReplaceSel(buddy->chat.msgedit, buf);
        memset(buf, 0, sizeof(buf));
		memcpy(buf, msg, msg_len);
        strcat(buf, "\r\n");
        Edit_SetSel(buddy->chat.msgedit, -1, -1);
        Edit_ReplaceSel(buddy->chat.msgedit, buf);
    }

	UpdateWindow(buddy->chat.window);
}

void stk_sendbtn_pressed(stk_buddy *buddy)
{
    char text[STK_DEFAULT_SIZE] = {0};
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    char buf[STK_MAX_SIZE] = {0};
    char tmp[STK_MAX_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    int size;

    if (buddy == NULL) {
        return;
    }

    size = Edit_GetText(buddy->chat.inputedit, text, STK_DEFAULT_SIZE);
    if (size <= 0) {
        MessageBox(buddy->chat.window,TEXT("No message to send"), TEXT("stkclient"), MB_OK);
        return;
    }

	Edit_SetText(buddy->chat.inputedit, "");
	SetFocus(buddy->chat.inputedit);
    stk_send_msg(client.fd, sendbuf, STK_MAX_PACKET_SIZE, text, STK_DEFAULT_SIZE, client.uid, buddy->uid, FALSE);

    stk_get_timestamp(timestamp);
    sprintf(buf, "%s(%d) %s\r\n", client.nickname, client.uid, timestamp);
    Edit_SetSel(buddy->chat.msgedit, -1, -1);
    Edit_ReplaceSel(buddy->chat.msgedit, buf);
    strcat(text, "\r\n");
    Edit_SetSel(buddy->chat.msgedit, -1, -1);
    Edit_ReplaceSel(buddy->chat.msgedit, text);
    UpdateWindow(buddy->chat.window);	
}

void stk_gchatwin_create(stk_group *group)
{
    HWND hwnd;
    int width, height;
    HWND msgedit, inputedit;
    HWND sendbtn, closebtn;
    char buf[STK_DEFAULT_SIZE] = {0};

    if (group == NULL) {
        stk_log("stk_gchatwin_create: Bad param\n");
        return;
    }

    sprintf(buf, "Chat In Group %s", group->gname);
    hwnd = CreateWindowEx(0, STKCLIENT, buf,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU  | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, 
        CW_USEDEFAULT, STK_CHATWIN_WIDTH, STK_CHATWIN_HEIGHT, NULL, NULL, stkWidget.hins, NULL);

    SendMessage(hwnd, WM_SETICON, (WPARAM)TRUE, 
            (LPARAM)LoadImage(NULL, STK_GROUP_ICO, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));

    msgedit = CreateWindowEx(0, TEXT("EDIT"), "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_TABSTOP | ES_AUTOVSCROLL,
        2, 5, STK_CHATWIN_WIDTH-10, STK_CHATWIN_HEIGHT*2/3-40, hwnd, NULL, stkWidget.hins, NULL);
    Edit_SetReadOnly(msgedit, TRUE);

    inputedit = CreateWindowEx(0, TEXT("EDIT"), "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | WS_TABSTOP | ES_AUTOVSCROLL,
        2, STK_CHATWIN_HEIGHT*2/3-25, STK_CHATWIN_WIDTH-10, STK_CHATWIN_HEIGHT/3-35, hwnd, NULL, stkWidget.hins, NULL);

    sendbtn  = CreateWindow(TEXT("BUTTON"), " Send ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        STK_CHATWIN_WIDTH-140, STK_CHATWIN_HEIGHT-55, 50, 20, hwnd, NULL, NULL, NULL);
    closebtn = CreateWindow(TEXT("BUTTON"), " Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        STK_CHATWIN_WIDTH-80,  STK_CHATWIN_HEIGHT-55, 50, 20, hwnd, NULL, NULL, NULL);

    group->gchat.exist     = 1;
    group->gchat.window    = hwnd;
    group->gchat.msgedit   = msgedit;
    group->gchat.inputedit = inputedit;
    group->gchat.sendbtn   = sendbtn;
    group->gchat.closebtn  = closebtn;

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    //SetFocus(inputedit);
}

void stk_gchatwin_show(int gid)
{
    stk_group *group;
    BOOL found;
    int num;

    found = FALSE;
    num = client.group_num;
    group = client.group;
    while (num-- && group != NULL) {
        if (group->gid == gid) {
            found = TRUE;
            break;
        }
        group = group->next;
    }
	
    if (!found || group == NULL) {
        stk_log("stk_gchatwin_show: Bad group gid\n");
        return;
    }
	
	if (!group->gchat.exist) {
        stk_gchatwin_create(group);
    }
    else {
        ShowWindow(group->gchat.window, SW_RESTORE);
        SetForegroundWindow(group->gchat.window);
        UpdateWindow(group->gchat.window);
        SetFocus(group->gchat.inputedit);
    }
}

void stk_gchatwin_display(stk_group *group)
{
    int msg_len;
    int uid;
    char msg[STK_MAX_PACKET_SIZE] = {0};
    char buf[STK_DEFAULT_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};

    if (!group->gchat.exist)
        stk_gchatwin_create(group);

    while (group->gmsg_num > 0) {
        if (stk_get_gmsg(group, msg, &msg_len, timestamp, &uid) == -1)
            break;
        sprintf(buf, "%s(%d) %s\r\n", group->gname, uid, timestamp);
        Edit_SetSel(group->gchat.msgedit, -1, -1);
        Edit_ReplaceSel(group->gchat.msgedit, buf);
        memset(buf, 0, sizeof(buf));
        memcpy(buf, msg, msg_len);
        strcat(buf, "\r\n");
        Edit_SetSel(group->gchat.msgedit, -1, -1);
        Edit_ReplaceSel(group->gchat.msgedit, buf);
    }

	UpdateWindow(group->gchat.window);
}

void stk_gsendbtn_pressed(stk_group *group)
{
    char text[STK_DEFAULT_SIZE] = {0};
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    char buf[STK_MAX_SIZE] = {0};
    char tmp[STK_MAX_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    int size;

    if (group == NULL) {
        return;
    }

    size = Edit_GetText(group->gchat.inputedit, text, STK_DEFAULT_SIZE);
    if (size <= 0) {
        MessageBox(group->gchat.window,TEXT("No message to send"), TEXT("stkclient"), MB_OK);
        return;
    }

	Edit_SetText(group->gchat.inputedit, "");
	SetFocus(group->gchat.inputedit);
    stk_send_msg(client.fd, sendbuf, STK_MAX_PACKET_SIZE, text, STK_DEFAULT_SIZE, client.uid, group->gid, TRUE);

    stk_get_timestamp(timestamp);
    sprintf(buf, "%s(%d) %s\r\n", client.nickname, client.uid, timestamp);
    Edit_SetSel(group->gchat.msgedit, -1, -1);
    Edit_ReplaceSel(group->gchat.msgedit, buf);
    strcat(text, "\r\n");
    Edit_SetSel(group->gchat.msgedit, -1, -1);
    Edit_ReplaceSel(group->gchat.msgedit, text);
    UpdateWindow(group->gchat.window);	
}



