// Copyright 2009-2010 Citrix Systems, Inc. 
/*
 * Copyright (c) 2001-2004 Citrix Systems, Inc. All Rights Reserved.
 */
// CtxSmcDlg.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CSelect.h"
#include "CMonitor.h"
#include "CSession.h"
#include "CVChannel.h"
#include "COptions.h"
#include "CWinStation.h"

extern HWND      g_hDlg;
extern HINSTANCE g_hInstance;

HWND g_hwndTT = 0; //tooltip control window handle
volatile HWND g_hwndSession=0;
typedef struct _TOOLTIPS
{
    unsigned int uId;
} TOOLTIP_IDS, *PTOOLTIP_IDS;

// these are the tootip IDs that are used to obtain the window handle of the
// corresponding child window controls this order is exactly the same as that in
// the resource file.  Do Not change the order.
TOOLTIP_IDS rgttMessageArray[] =
{
    IDC_BUTTON_LPT1,
    IDC_BUTTON_LPT2,
    IDC_BUTTON_CPM,
    IDC_BUTTON_COM1,
    IDC_BUTTON_COM2,
    IDC_BUTTON_CCM,
    IDC_BUTTON_CDM,
    IDC_BUTTON_CLIP,
    IDC_BUTTON_TW,
    IDC_BUTTON_TWI,
    IDC_BUTTON_ZLFK,
    IDC_BUTTON_ZLC,
    IDC_BUTTON_CTL,
    IDC_BUTTON_PASS,
    IDC_BUTTON_CAM,
    IDC_BUTTON_CM,
    IDC_BUTTON_LIC,
    IDC_BUTTON_VFM,
    IDC_BUTTON_PN,
    IDC_BUTTON_SCRD,
    IDC_BUTTON_MM,
    IDC_BUTTON_SBR,
    IDC_BUTTON_TWN,
    IDC_BUTTON_OEM1,
    IDC_BUTTON_OEM2,
    IDC_BUTTON_OEM3,
    IDC_BUTTON_OEM4,
    IDC_BUTTON_OEM5
};

#define NUM_TOOLTIPS (sizeof(rgttMessageArray) / sizeof(TOOLTIP_IDS))

// count of tabs on control
#define C_PAGES 3

// container struct for each pages dialog
typedef struct tag_dlghdr
{
    int          idxSel;        // current child index
    HWND         hwndTab;       // tab control
    RECT         rcDisplay;     // display rectangle for the tab control
    DLGTEMPLATE* apRes[C_PAGES];
    CDisplay*    rgDisp[C_PAGES];
} DLGHDR;

static DLGHDR s_tabHdr;

///////////////////////////////////////////////////////////////////////
// prototypes
void TabbedDialogInit(HWND hwndDlg);
void OnSelChanged(void);

///////////////////////////////////////////////////////////////////////
// SmcDlgProc
//
//  This function processes messages for the Dialog Box
//
//  Inputs - hDlg = Handle of Dialog Box window
//           msg  = Message ID
//           wParam = Message specific parameter
//           lParam = Message specific parameter
//
//  Output - int =
//
int APIENTRY SmcDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int          wmId;
    LPNMHDR      lpnmhdr;
    static HMENU hMenu = 0;

    switch (msg)
    {
        case WM_INITDIALOG:
            // set up the global
            g_hDlg = hDlg;

            // disable menu items CLOSE & MAXIMIZE
            hMenu = GetSystemMenu(hDlg, FALSE);
            EnableMenuItem(hMenu, SC_SIZE, MF_GRAYED);
            EnableMenuItem(hMenu, SC_MAXIMIZE, MF_GRAYED);
            EnableMenuItem(hMenu, SC_RESTORE, MF_GRAYED);

            // change the system icon.
            PostMessage(hDlg, WM_SETICON, ICON_BIG,
                        (LPARAM)LoadIcon(g_hInstance,
                                         MAKEINTRESOURCE(IDI_ICON_CHANNEL)));

            // fill in the combo
            CSelect::Init();
            CSelect::Refresh(GetDlgItem(hDlg, IDC_SESSION_DROP), 0);

            // now start a monitor thread to get data for display
            if (!CMonitor::StartThread())
                return FALSE;

            // and start a thread to listen for new sessions
            CWinStation::StartThread();

            // initialize the child dlg on the tab
            TabbedDialogInit(hDlg);

            return (TRUE);

        case CMONITOR_MSG_REFRESH:
            // refresh the screen display
            CSelect::Refresh(GetDlgItem(hDlg, IDC_SESSION_DROP), lParam);
            break;

        case CMONITOR_MSG_DISPLAY:
            s_tabHdr.rgDisp[s_tabHdr.idxSel]->Update();
            break;

        case CMONITOR_MSG_CHANGESESSION:
            // since we have changed to a new session initialize the display if
            // necessary
            s_tabHdr.rgDisp[s_tabHdr.idxSel]->NewSession();
            if (1 != s_tabHdr.idxSel)
            {
                // we MUST always re-initialize the Channel displays
                s_tabHdr.rgDisp[1]->NewSession();
            }
            break;


        case WM_NOTIFY:
            wmId = LOWORD(wParam);
            if (wmId == IDC_TAB_DATA)
            {
                lpnmhdr = (LPNMHDR)lParam;
                if (TCN_SELCHANGE == lpnmhdr->code)
                    OnSelChanged();
            }

            break;

        case WM_SYSCOMMAND:
            // if restored then we can only Minimize
            // if Minimized then we can only restore
            // the code below handles the System menu items.
            EnableMenuItem(hMenu, SC_MINIMIZE,
                           (wParam == SC_MINIMIZE) ? MF_GRAYED : MF_ENABLED);

            EnableMenuItem(hMenu, SC_RESTORE,
                           (wParam == SC_RESTORE) ? MF_GRAYED : MF_ENABLED);

            break;

        case WM_COMMAND:
            wmId = LOWORD(wParam);
            switch (wmId)
            {
                case IDC_SESSION_DROP:
                    if (CBN_SELENDOK == HIWORD(wParam))
                        CSelect::ChangeSelection((HWND)lParam);

                    break;


                case IDOK:
                case IDCANCEL:
                    // stop worker threads
                    CWinStation::StopThread();
                    CMonitor::StopThread();

                    // free up any memory etc
                    CSelect::Cleanup();
                    delete s_tabHdr.rgDisp[0];
                    delete s_tabHdr.rgDisp[1];
                    delete s_tabHdr.rgDisp[2];
                    EndDialog(hDlg, wmId);
                    return (TRUE);

                default:
                    // pass down to the class to process
                    s_tabHdr.rgDisp[s_tabHdr.idxSel]->DoMsg(msg, wParam,
                                                            lParam);
                    break;
            }

            break;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////
// ChildDlgProc
//
//  This function processes messages for the tabbed child dialogs
//
//  Inputs - hDlg = Handle of Dialog Box window
//           msg  = Message ID
//           wParam = Message specific parameter
//           lParam = Message specific parameter
//
//  Output - int =
//
int APIENTRY ChildDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int wmId;

    switch (msg)
    {
        case WM_INITDIALOG:
            // default to no size so its not visible
            SetWindowPos(hDlg, HWND_TOP,
                        s_tabHdr.rcDisplay.left,
                        s_tabHdr.rcDisplay.top,
                        0,
                        0,
                        SWP_HIDEWINDOW);
            return (TRUE);

        case WM_HSCROLL:
            // pass down to the class to process
            s_tabHdr.rgDisp[s_tabHdr.idxSel]->DoMsg(msg, wParam, lParam);
            break;

        case WM_COMMAND:
            wmId = LOWORD(wParam);
            switch (wmId)
            {
				
				case IDC_SESSION_C_DROP:
                    if (CBN_SELENDOK == HIWORD(wParam))
                        CMonitor::ChangeSelection((HWND)lParam);

                    break;

                case IDOK:
                case IDCANCEL:
                    // this should never be called
                    ASSERT(0);
                    EndDialog(hDlg, wmId);
                    return (TRUE);

                default:
                    // pass down to the class to process
                    s_tabHdr.rgDisp[s_tabHdr.idxSel]->DoMsg(msg, wParam,
                                                           lParam);
                    break;
            }
            break;

        default:
            break;
    }

    return (FALSE);
}

///////////////////////////////////////////////////////////////////////
// OnSelChanged
//
// This procedure process a change in the active tab
//
// Inputs - None.
//
// Output - None.
//
void OnSelChanged()
{
    HWND hWnd;

    // set size of old to zero to hide
    if (s_tabHdr.idxSel != -1)
    {
        hWnd = s_tabHdr.rgDisp[s_tabHdr.idxSel]->GetWnd();
        SetWindowPos(hWnd, HWND_TOP,
                     s_tabHdr.rcDisplay.left,
                     s_tabHdr.rcDisplay.top,
                     s_tabHdr.rcDisplay.right - s_tabHdr.rcDisplay.left,
                     s_tabHdr.rcDisplay.bottom - s_tabHdr.rcDisplay.top,
                     SWP_HIDEWINDOW);
    }

    // adjust the index
    s_tabHdr.idxSel = TabCtrl_GetCurSel(s_tabHdr.hwndTab);

    // resize the visible window
    hWnd = s_tabHdr.rgDisp[s_tabHdr.idxSel]->GetWnd();
    SetWindowPos(hWnd, HWND_TOP,
                 s_tabHdr.rcDisplay.left,
                 s_tabHdr.rcDisplay.top,
                 s_tabHdr.rcDisplay.right - s_tabHdr.rcDisplay.left,
                 s_tabHdr.rcDisplay.bottom - s_tabHdr.rcDisplay.top,
                 SWP_SHOWWINDOW);

    // get the monitor thread to display the data
    if (s_tabHdr.idxSel == 0)
    {
        // setup the current display ranges
        s_tabHdr.rgDisp[0]->SetRanges();

        // now get the monitor thread to display data
        CMonitor::SetDisplay(CMONITOR_DISPLAY_SESSION);
    }
    else if (s_tabHdr.idxSel == 1)
    {
        // setup the current display ranges
        s_tabHdr.rgDisp[1]->SetRanges();

        // now get the monitor thread to display data
        CMonitor::SetDisplay(CMONITOR_DISPLAY_VCS);
    }
    else
    {
        // setup the current display ranges
        s_tabHdr.rgDisp[2]->SetRanges();

        // there is no update from the monitor thread
        CMonitor::SetDisplay(CMONITOR_DISPLAY_NONE);

        // so call the display function of the dialog
        s_tabHdr.rgDisp[2]->Update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CreateTooltipControlWindow
//
//  Thes procedure will create a tooltip window that will handle tooltips
//
// Inputs - None.
//
// Output - None.
//
void CreateTooltipControlWindow()
{
    /* CREATE A TOOLTIP WINDOW */
    g_hwndTT = CreateWindowEx(WS_EX_TOPMOST,
                              TOOLTIPS_CLASS,
                              NULL,
                              WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              g_hDlg,
                              NULL,
                              g_hInstance,
                              NULL
                             );

    SetWindowPos(g_hwndTT,
                 HWND_TOPMOST,
                 0,
                 0,
                 0,
                 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

////////////////////////////////////////////////////////////////////////////////
// AssignChannelTooltips
//
// This procedure assignes tooltips to all buttons in the channel
//
//  Inputs - hwnd = Window handle
//
//  Output - None.
//
void AssignChannelTooltips(HWND hwnd)
{
    TOOLINFO     ti;
    HWND         hwndctrl;
    unsigned int ibaseStringId = IDS_DESCR_BASE;
    WCHAR        rgwchTemp[64];
    int          nToolTips = NUM_TOOLTIPS;

    // can't assign tooltips if we don't have a control window
    if (g_hwndTT == 0)
        return;

    memset(&ti, 0, sizeof(ti));

    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = hwnd;
    ti.hinst = g_hInstance;

    for (int i = 0; i<nToolTips; i++)
    {
        hwndctrl =  GetDlgItem(hwnd, rgttMessageArray[i].uId);
        if (hwndctrl)
        {
            rgwchTemp[0] = 0;
            LoadString(GetModuleHandle(NULL),
                       ibaseStringId + i,
                       &rgwchTemp[0],
                       64);

            if (rgwchTemp[0])
            {
                ti.lpszText = rgwchTemp;
                ti.uId = (UINT_PTR)hwndctrl;
                SendMessage(g_hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////
// TabbedDialogInit
//
//  This procedure initializes the tab control and creates the child
//  dialogs for the tabs.
//
//  Inputs - hwndDlg = Dialog Handle
//
//  Output - None.
//
void TabbedDialogInit(HWND hwndDlg)
{
    HWND       hWnd;
    int        i,
               iRC;
    TCITEM     tie;
    TEXTMETRIC tmData;
    WCHAR*     pwcszDefaultTabTag[3] = {L"Server Data",
                                        L"Session Channels",
                                        L"Options"};
    WCHAR      rgwchTemp[32];

    memset(&s_tabHdr, 0, sizeof(s_tabHdr));
    s_tabHdr.idxSel = -1;

    // create the tootip controlw window
    CreateTooltipControlWindow();

    // get the tab controls window and its client size
    s_tabHdr.hwndTab = GetDlgItem(hwndDlg, IDC_TAB_DATA);
    GetClientRect(s_tabHdr.hwndTab, &s_tabHdr.rcDisplay);

    GetTextMetrics(GetDC(hwndDlg), &tmData);
    s_tabHdr.rcDisplay.top += tmData.tmHeight + tmData.tmExternalLeading + 9;
    s_tabHdr.rcDisplay.left += 2;
    s_tabHdr.rcDisplay.bottom -= 2;
    s_tabHdr.rcDisplay.right -= 2;

    // Add a tab for each of the three child dialog boxes.

    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;

    for (i = 0; i < 3; i++)
    {
        rgwchTemp[0] = 0;
        iRC = LoadString(GetModuleHandle(NULL), IDS_TAB_ONE + i, &rgwchTemp[0],
                         32);

        tie.pszText = (iRC > 0) ? rgwchTemp : pwcszDefaultTabTag[i];
        TabCtrl_InsertItem(s_tabHdr.hwndTab, i, &tie);
    }

    // Load and Lock the resources for the three child dialog boxes.
    // create the window and wrapper class

    for (i = 0; i < 3; i++)
    {
        s_tabHdr.apRes[i] = (DLGTEMPLATE*)
            LockResource(LoadResource(NULL,
                             FindResource(NULL,
                                          MAKEINTRESOURCE(IDD_DLG_SESSION + i),
                                          RT_DIALOG)));
        ASSERT(s_tabHdr.apRes[i]);
        hWnd = CreateDialogIndirect(NULL,
                                    s_tabHdr.apRes[i],
                                    s_tabHdr.hwndTab,
                                    (DLGPROC)ChildDlgProc);
        ASSERT(hWnd);
        switch (i)
        {
            case 0:
                s_tabHdr.rgDisp[i] = new CSession(hWnd);
				g_hwndSession = hWnd;
                break;

            case 1:
                s_tabHdr.rgDisp[i] = new CVChannel(hWnd);
                break;

            case 2:
                s_tabHdr.rgDisp[i] = new COptions(hWnd);
                break;

            default:
                s_tabHdr.rgDisp[i] = NULL;
        }

        if (s_tabHdr.rgDisp[i] == NULL)
        {
            ASSERT(0);
            return;
        }
    }

    // Simulate selection of the first item.
    OnSelChanged();
}

