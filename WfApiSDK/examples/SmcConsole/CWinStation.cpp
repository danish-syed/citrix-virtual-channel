// Copyright 2009 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
// CWinStation.cpp

#include <process.h>

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CWinStation.h"
#include "CMonitor.h"

#include "wfapi.h"

extern HWND g_hDlg;

///////////////////////////////////////////////////////////////////////
// CWinStation::StartThread
//
//  This method starts a thread.
//
//  Inputs - None.
//
//  Output - bool = true if thread started, false otherwise.
//
bool CWinStation::StartThread()
{
    unsigned int lThreadId;

    ms_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, 0, 0, &lThreadId);
    if (ms_hThread == 0 || ms_hThread == (HANDLE)-1)
    {
        ASSERT(0);
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////
// CWinStation::StopThread
//
//  This method stops the thread from executing and destroys it.
//
//  Inputs - None.
//
//  Output - None.
//
void CWinStation::StopThread()
{
    DWORD dwEventFlags;

    if (ms_hThread != 0 && ms_hThread != (HANDLE)-1)
    {
        // tell the thread to stop
        ms_fStop = true;

        // now trigger a winstaion event
        WFWaitSystemEvent(WF_CURRENT_SERVER, WF_EVENT_FLUSH, &dwEventFlags);

        // now wait for the thread to stop
        WaitForSingleObject(ms_hThread, INFINITE);

        CloseHandle(ms_hThread);
        ms_hThread = NULL;
    }
}

///////////////////////////////////////////////////////////////////////
// CWinStation::ThreadProc
//
//  This method is the thread procedure.  It waits for logon, logoff,
//  connect, and disconnect events form ICA winstations and refreshes the
//  monitor when one occurrs.
//
//  Inputs - Not used.
//
//  Output - Thread return code.
//
UINT APIENTRY CWinStation::ThreadProc(PVOID)
{
    DWORD dwWinStationEvent;

    while (!ms_fStop)
    {
        // Wait for a winstation logon / connect event
        dwWinStationEvent = 0;
        if (WFWaitSystemEvent(WF_CURRENT_SERVER,
                              WF_EVENT_LOGON | WF_EVENT_CONNECT |
                              WF_EVENT_LOGOFF | WF_EVENT_DISCONNECT,
                              &dwWinStationEvent) == FALSE)
        {
            // WFWaitSystemEvent failed
            // so exit the thread here.
            CloseHandle(ms_hThread);
            ms_hThread = 0;
            break;
        }

        // A WinStationEvent occured, so we need to refresh
        CMonitor::DoRefresh(dwWinStationEvent);
    }

    return ERROR_SUCCESS;
}
