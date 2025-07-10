// Copyright 2009 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
// CtxSmcUI.cpp
//

#include <stdio.h>
#include <stdlib.h>

#define CTXSMC_INSTANTIATE_STATICS
#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CSelect.h"
#include "CMonitor.h"
#include "CWinStation.h"
#include "CVChannel.h"
#include "CDatalogger.h"

///////////////////////////////////////////////////////////////////////
// globals
HWND        g_hDlg = 0;
HINSTANCE   g_hInstance = 0;

///////////////////////////////////////////////////////////////////////
// prototypes
extern int APIENTRY SmcDlgProc(HWND hDlg, UINT msg, WPARAM wParam,
                               LPARAM lParam);

///////////////////////////////////////////////////////////////////////
//
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    INITCOMMONCONTROLSEX cex;

    cex.dwSize = sizeof(cex);
    cex.dwICC  = ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&cex);

    g_hInstance = hInstance;

    DialogBoxParam(hInstance,
                   MAKEINTRESOURCE(IDD_DLG_MAIN),
                   0,
                   (DLGPROC)SmcDlgProc,
                   (LPARAM)0);

    return 0;
}

