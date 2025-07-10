// Copyright 2009-2010 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 - 2002 Citrix Systems, Inc. All Rights Reserved.
 */ 
// CMonitor.cpp
 
#include <process.h>

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CMonitor.h"
#include "CSelect.h"
#include "CDataLogger.h"

extern HWND g_hDlg;
extern volatile HWND g_hwndSession;
// To do Msi
// Make sure that vaiable names SMC_QOS_LOW reflcts lower pririty...as they are not now.

///////////////////////////////////////////////////////////////////////
// CMonitor::StartThread
//
//    This method creats and starts a new thread.
//
//    Inputs - None.
//
//    Outputs - bool = true if thread created, false otherwise.
//
bool CMonitor::StartThread()
{
    unsigned int lThreadId;

    ms_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, 0, 0, &lThreadId);
    ASSERT(ms_hThread != 0 && ms_hThread != (HANDLE)-1);

    return (ms_hThread != 0 && ms_hThread != (HANDLE)-1);
}

///////////////////////////////////////////////////////////////////////
// CMonitor::StopThread
//
//    This Method stops the thread created in CMonitor::StartThread.
//
//    Inputs - None.
//
//    Outputs - None.
//
void CMonitor::StopThread()
{
    if (ms_hThread != 0 && ms_hThread != (HANDLE)-1)
    {
        // tell the thread to stop
        ms_fStop = true;

        // pulse the event to wake up the thread
        if (ms_hEvent[0] != NULL)
        {
            SetEvent(ms_hEvent[0]);
        }

        // now wait for the thread to stop
        WaitForSingleObject(ms_hThread, INFINITE);

        CloseHandle(ms_hThread);
        ms_hThread = NULL;
    }
}

///////////////////////////////////////////////////////////////////////
// CMonitor::ThreadProc
//
//    This method is the thread that updats the information in the
//    dialog if necessary.
//
//    Inputs - None.
//
//    Output - Always returns 0
//
UINT APIENTRY CMonitor::ThreadProc(PVOID /*p*/)
{
    //UNREFERENCED_PARAMETER(p);
    UINT32 count,
           rgIDs[CMONITOR_MAX_VC],
           SessionID;
    DWORD  rVal = 0;
    DWORD dwEvent = 0;    
    UINT32 selectedQos = 0;
    wchar_t* msg = (wchar_t*) malloc(sizeof(wchar_t)* MSG_BUFFER);
    volatile BOOL bSessionEvent = FALSE;
    // initialize static members
    ms_SessionID = CSELECT_NO_SESSION;
  
    memset(&ms_ServerData, 0, sizeof(ms_ServerData));
    memset(&ms_SessionData, 0, sizeof(ms_SessionData));
    memset(ms_rgVCData, 0, sizeof(ms_rgVCData));

    // create an event on which to wait
    ms_hEvent[0] = CreateEvent(0, FALSE, FALSE, 0);
    ms_hEvent[1] = CreateEvent(0, FALSE, FALSE, 0);
    // temporary variables
    SessionID = 0;
    ms_selectedQos = 0;
    // now until told to stop keep refreshing data

    while (!ms_fStop)
    {
        // wait until there is something to do
        bSessionEvent = FALSE;
        if (ms_hEvent[0] != NULL && ms_hEvent[1]!= NULL)
        {
            if (ms_Display == CMONITOR_DISPLAY_NONE && !ms_fNeedRefresh) {
                //WaitForSingleObject(ms_hEvent, INFINITE);
                dwEvent = WaitForMultipleObjects(2, ms_hEvent, false, INFINITE);
            } else {
                //WaitForSingleObject(ms_hEvent, COptions::ms_refreshIntervalMs);
                dwEvent = WaitForMultipleObjects(2, ms_hEvent, false, COptions::ms_refreshIntervalMs);

            }
        }
        switch(dwEvent)
        {
            case WAIT_OBJECT_0 + 0:
                break;
            
            case WAIT_OBJECT_0 + 1:
                    ms_selectedQos = GetPrimaryStream();
                    bSessionEvent = TRUE;                

        }
        // check in case we woke up to stop
        if (ms_fStop) {
            break;
        }

        
        // If the user intends to turn ON/OFF Data logging then let the
        // data logger know.
        if (TRUE == COptions::ms_bLogStatus)
        {
            CDataLogger::StartLog(&COptions::ms_szlogFileName[0]);
        }
        else
        {
            CDataLogger::StopLog();
        }

        // check in case some other thread advised us to refresh
        if (ms_fNeedRefresh)
        {
            ms_fNeedRefresh = false;         
            SendMessage(g_hDlg, CMONITOR_MSG_REFRESH, 0, (LPARAM)ms_dwWFEvent);            
            
        }

        if ((ms_Display == CMONITOR_DISPLAY_SESSION) ||
            (ms_Display == CMONITOR_DISPLAY_VCS))
        {
            // update the data for display
            count = 0;

            // get the server data
            ms_ServerData.VersionNumber = CTXSMCAPI_VERSION_NUMBER;
            rVal = WFSmcGetServerData(WF_CURRENT_SERVER_HANDLE,
                                      sizeof(ms_ServerData),
                                      &ms_ServerData,
                                      0,
                                      0,
                                      &count);

            if ((ERROR_SUCCESS != rVal) &&
                (ERROR_MORE_DATA != rVal))
            {
                // what can we do short of killing the app
                break;
            }

            
            // Log server data
            CDataLogger::WriteServerData(&ms_ServerData, sizeof(ms_ServerData));
        
            // check we have the correct session open
            if (SessionID != ms_SessionID)
            {
                // close the old session handle
                if (ms_hSession)
                {
                    WFSmcCloseSessionHandle(WF_CURRENT_SERVER_HANDLE, ms_hSession);
                    ms_hSession = 0;
                }

                // now if we have a session open it
                SessionID = ms_SessionID;
                if (CSELECT_NO_SESSION != SessionID)
                {
                    rVal = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionID, &ms_hSession);
                    if (ERROR_SUCCESS != rVal)
                    {
                        // The session may no loger exist anymore.  This
                        // happens when a session is being logged off and
                        // we receive the log off event.  But the session
                        // may still remain in the enumeration result while
                        // it is being logged off.  There could be other
                        // situations that cause a bogus session being the
                        // current session.
                        SessionID = CSELECT_NO_SESSION;
                        SendMessage(g_hDlg, CMONITOR_MSG_REFRESH, 0, 0);
                    }

                    ms_cSessionVCs = CMONITOR_MAX_VC;
                    ms_SessionData.VersionNumber = CTXSMCAPI_VERSION_NUMBER_V2;
                    memset(&ms_SessionDataMsi, 0, sizeof(ms_SessionDataMsi));
                    for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
                    {
                        ms_SessionDataMsi[j].VersionNumber = CTXSMCAPI_VERSION_NUMBER_V2;
                    }
                    rVal = WFSmcGetSessionDataV2(WF_CURRENT_SERVER_HANDLE,
                                                ms_hSession,
                                                sizeof(ms_SessionDataMsi),
                                                (pCtx_Smc_Session_Data_V2)&ms_SessionDataMsi,
                                                sizeof(ms_rgVCData),
                                                ms_rgVCData,
                                                &ms_cSessionVCs);

                    if(!IsMultiStreamActive())
                    {
                        ms_selectedQos = GetPrimaryStream();
                    }
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[ms_selectedQos], sizeof(ms_SessionData));
                    //ms_SessionDataMsi[0].IsConnectionActive = 1;    

                    if (IsMultiStreamActive())
                    {
                        if (bSessionEvent)
                        {
                            ShowWindow(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), SW_SHOW);
                            ShowWindow(GetDlgItem(g_hwndSession, IDC_MSI_LABEL), SW_SHOW);
                        }
                    }
                    else
                    {
                        ShowWindow(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), SW_HIDE);
                        ShowWindow(GetDlgItem(g_hwndSession, IDC_MSI_LABEL), SW_HIDE);                        
                    }

                    if ( IsMultiStreamActive() && bSessionEvent)
                    {
                    

                        WCHAR    rgNoneSelected[STATIONNAME_LENGTH + 1];            
                        SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_RESETCONTENT, 0, 0);        
                    

                        for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
                        {

                            
                            if(ms_SessionDataMsi[j].IsConnectionActive == 0)
                                continue;

                            switch(j)
                            {
                                case SMC_QOS_LOW:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSLOW);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);                                    
                                    break;

                                case SMC_QOS_MED:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSNORMAL);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);                                    									
                                    SendDlgItemMessage(g_hwndSession, IDC_SESSION_C_DROP, CB_SELECTSTRING,  (WPARAM)-1, (LPARAM)rgNoneSelected);
                                    selectedQos = GetPrimaryStream();									
                                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[selectedQos], sizeof(ms_SessionData));                                    break;
                                    break;

                                case SMC_QOS_HIGH:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSHIGH);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);
                                    break;

                                case SMC_QOS_REAL:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSVERYHIGH);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);
                                    break;
                                    
                            }
                        }
                    }



                    if (rVal == ERROR_SUCCESS)
                    {
                        
                        for (int i = 0; i < (int)ms_cSessionVCs; i++)
                            rgIDs[i] = ms_rgVCData[i].idVC;
                    }
            


                    if (rVal == ERROR_SUCCESS)
                    {
                        // get the VC bandwidth limits
                        rVal = WFSmcGetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE,
                                                               ms_hSession,
                                                               ms_cSessionVCs,
                                                               rgIDs,
                                                               ms_rgBWLimits);
                    }

                    if (rVal == ERROR_SUCCESS)
                    {
                        // get the VC priorities
                        rVal = WFSmcGetSessionPriorities(WF_CURRENT_SERVER_HANDLE,
                                                          ms_hSession,
                                                          ms_cSessionVCs,
                                                          rgIDs,
                                                          ms_rgPriorities);
                    }
                
                    if (rVal == ERROR_SUCCESS)
                    {
                        // get the session bandwidth limit
                        UINT32      dummySession = 0xFFFFFFFF;
                        rVal = WFSmcGetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE,
                                                              ms_hSession,
                                                              1,
                                                              &dummySession,
                                                              &ms_SessionBWLimitKBits);            
                    }
                
                    
                    if (rVal != ERROR_SUCCESS)
                    {
                        // Simply reset the display.
                        SessionID = CSELECT_NO_SESSION;
                        SendMessage(g_hDlg, CMONITOR_MSG_REFRESH, 0, 0);
                    }
                    
                   
                    // Log session change
                    CDataLogger::LogNewSession();
                }

                // notify the display that we have a new session
                SendMessage(g_hDlg, CMONITOR_MSG_CHANGESESSION, 0, 0);
            }

            // get the session data
            if (SessionID != CSELECT_NO_SESSION)
            {
                count = CMONITOR_MAX_VC;


                ms_SessionData.VersionNumber = CTXSMCAPI_VERSION_NUMBER_V2;
                memset(&ms_SessionDataMsi, 0, sizeof(ms_SessionDataMsi));

                for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
                {
                    ms_SessionDataMsi[j].VersionNumber = CTXSMCAPI_VERSION_NUMBER_V2;
                }
                rVal = WFSmcGetSessionDataV2(WF_CURRENT_SERVER_HANDLE,
                                            ms_hSession,
                                            sizeof(ms_SessionDataMsi),
                                            ms_SessionDataMsi,
                                            sizeof(ms_rgVCData),
                                            ms_rgVCData,
                                            &count);
    
                    if(!IsMultiStreamActive())
                    {
                        ms_selectedQos = GetPrimaryStream();
                    }
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[ms_selectedQos], sizeof(ms_SessionData));


                    if (IsMultiStreamActive())
                    {
                        if (bSessionEvent)
                        {
                            ShowWindow(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), SW_SHOW);
                            ShowWindow(GetDlgItem(g_hwndSession, IDC_MSI_LABEL), SW_SHOW);
                        }
                    }
                    else
                    {
                        ShowWindow(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), SW_HIDE);
                        ShowWindow(GetDlgItem(g_hwndSession, IDC_MSI_LABEL), SW_HIDE);                        
                    }
                    if (IsMultiStreamActive() && bSessionEvent)
                    {                        

                        
                        WCHAR    rgNoneSelected[STATIONNAME_LENGTH + 1];            
                        SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_RESETCONTENT, 0, 0);        
                        
                        for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
                        {

                            
                            if (ms_SessionDataMsi[j].IsConnectionActive == 0)
                                continue;

                            switch(j)
                            {
                                case SMC_QOS_LOW:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSLOW);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);                                    
                                    break;

                                case SMC_QOS_MED:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSNORMAL);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);
                                    SendDlgItemMessage(g_hwndSession, IDC_SESSION_C_DROP, CB_SELECTSTRING,  (WPARAM)-1, (LPARAM)rgNoneSelected);
                                    selectedQos = GetPrimaryStream();
                                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[selectedQos], sizeof(ms_SessionData));
                                    break;

                                case SMC_QOS_HIGH:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSHIGH);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);
                                    break;

                                case SMC_QOS_REAL:
                                    wcscpy_s(&rgNoneSelected[0], _countof(rgNoneSelected), QOSVERYHIGH);
                                    SendMessage(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), CB_ADDSTRING, 0, (LPARAM)rgNoneSelected);
                                    break;
                                    
                            }
                        }
                    }

                if ((rVal != ERROR_SUCCESS) &&
                    (rVal != ERROR_MORE_DATA))
                {
                    SessionID = CSELECT_NO_SESSION;
                    SendMessage(g_hDlg, CMONITOR_MSG_REFRESH, 0, 0);
                }
                else
                {
                    if(msg)
                    {
                        swprintf_s(msg,  MSG_BUFFER, L"Values BandwidthSentBitsPerSecond=%u and BandwidthRcvdBitsPerSecond=%u",ms_SessionData.BandwidthSentBitsPerSecond,ms_SessionData.BandwidthReceivedBitsPerSecond);
                        OutputDebugString(msg);
                    }
                    // Log session data with Virtual Channel:
                    CDataLogger::WriteSessionData(ms_SessionID, &ms_SessionData, ms_cSessionVCs, &ms_rgVCData[0]);
                }
            }

            if (ms_SessionID == CSELECT_NO_SESSION)
            {
                ShowWindow(GetDlgItem(g_hwndSession, IDC_SESSION_C_DROP), SW_HIDE);
                ShowWindow(GetDlgItem(g_hwndSession, IDC_MSI_LABEL), SW_HIDE);
            }
            // ok we have updated the data so get it displayed
            SendMessage(g_hDlg, CMONITOR_MSG_DISPLAY, 0, 0);

        }
    }

    // shutdown and scram
    if (ms_hSession)
    {
        WFSmcCloseSessionHandle(WF_CURRENT_SERVER_HANDLE, ms_hSession);
        ms_hSession = NULL;
    }

    if (ms_hEvent[0] != NULL)
    {
        CloseHandle(ms_hEvent[0]);
        ms_hEvent[0] = NULL;
    }

    if (ms_hEvent[1] != NULL)
    {
        CloseHandle(ms_hEvent[1]);
        ms_hEvent[1] = NULL;
    }
    
    // stop the logger
    CDataLogger::StopLog();

    if(msg)
        free(msg);
    return 0;
}

///////////////////////////////////////////////////////////////////////
// CMonitor::IsSessionSelected
//
//  This method is used to determine if a session has been selected.
//
//  Inputs - None.
//
//  Output - true if session selected, false if no session selected
//
bool CMonitor::IsSessionSelected()
{
    return (CSELECT_NO_SESSION != ms_SessionID);
    
}

///////////////////////////////////////////////////////////////////////
// CMonitor::SetSession
//
//  This method sets the ID of the session being monitored and wakes
//  up the update thread to update the dialog information.
//
//  Inputs - id = Session ID
//
//  Output - None.
//
void CMonitor::SetSession(int id)
{
    // save the new session id
    ms_SessionID = id;

    // pulse the event to wake up the thread
    if (ms_hEvent[1] != NULL)
    {
        SetEvent(ms_hEvent[1]);
    }
}

///////////////////////////////////////////////////////////////////////
// CMonitor::SetDisplay
//
//  This method updates the type of information being displayed and
//  wakes up the update thread to update the dialog information.
//
//  Input - type = The new type of information to be displayed
//
//  Output - None.
//
void CMonitor::SetDisplay(int type)
{
    if ((type != CMONITOR_DISPLAY_SESSION) &&
        (type != CMONITOR_DISPLAY_VCS))
    {
        // check if its none
        if (type != CMONITOR_DISPLAY_NONE)
        {
            // we don't know what they want
            ASSERT(0);
            type = CMONITOR_DISPLAY_NONE;
        }
    }

    // save the type to the static
    ms_Display = type;

    // pulse the event to wake up the thread
    if (ms_hEvent[0] != NULL)
    {
        SetEvent(ms_hEvent[0]);
    }
}

///////////////////////////////////////////////////////////////////////
// CMonitor::SetBWLimit
//
//  This method sets the bandwidth limit of a VC or the session.
//
//  Inputs - idxVC = VC ID, a negative value indicates session bandwidth
//           val = new bandwidth value desired
//
//  Output - bool = true if bandwidth changed successfully, false otherwise
//
bool CMonitor::SetBWLimit(int idxVC, int val)
{
    UINT32 bw = val,
           id = 0xFFFFFFFF;
    DWORD  rVal = 0;

    if (idxVC < 0)
    {
        // this is setting the session BW limit
        rVal = WFSmcSetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE, ms_hSession, 1, &id, &bw);

        // change the entry in the local table
        if (rVal == ERROR_SUCCESS)
        {
            ms_SessionBWLimitKBits = val;
            return true;
        }
    }
    else if (idxVC > (int)ms_cSessionVCs)
    {
        ASSERT(0);
    }
    else
    {
        // change the VC's bandwidth
        rVal = WFSmcSetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE,
                                               ms_hSession,
                                               1,
                                               &ms_rgVCData[idxVC].idVC,
                                               &bw);
        // change the entry in the local table
        if (rVal == ERROR_SUCCESS)
        {
            ms_rgBWLimits[idxVC] = val;
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// CMonitor::SetPriority
//
//  This method sets the priority of a VC.
//
//  Inputs - idxVC = VC ID for which the priority is to be set
//           val = new priority for the VC
//
//  Output - bool = true if priority changed, false otherwise
//
bool CMonitor::SetPriority(int idxVC, int val)
{
    UINT32 pr   = 0;
    DWORD  rVal = 0;

    if ((idxVC < 0) ||
        (idxVC > (int)ms_cSessionVCs))
    {
        ASSERT(0);
        return false;
    }

    // change the VC's priority
    pr = val;
    rVal = WFSmcSetSessionPriorities(WF_CURRENT_SERVER_HANDLE,
                                      ms_hSession,
                                      1,
                                      &ms_rgVCData[idxVC].idVC,
                                      &pr);

    // change the entry in the local table
    if (rVal == ERROR_SUCCESS)
    {
        ms_rgPriorities[idxVC] = val;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// CMonitor::DoRefresh
//
//  This method flags the dialog for refresh
//
//  Inputs - None.
//
//  Outputs - None.
//
void CMonitor::DoRefresh(DWORD WFEvent)
{
    ms_fNeedRefresh = true;
    ms_dwWFEvent = WFEvent;
}
///////////////////////////////////////////////////////////////////////
// CMonitor::DoRefresh
//
//  Checks if multistream session is active
//
//  Inputs - None.
//
//  Outputs - returns true if multistream is active.
//
bool CMonitor::IsMultiStreamActive()
{
    int k=0;
    
    for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
    {
        if (ms_SessionDataMsi[j].IsConnectionActive)
            k++;
    }
    
    
    if (k > 1)
        return true;
    else
        return false;
}
///////////////////////////////////////////////////////////////////////
// CMonitor::DoRefresh
//
//  Checks if multistream session is active
//
//  Inputs - None.
//
//  Outputs - returns true if multistream is active.
//
UINT32 CMonitor::GetPrimaryStream()
{
    int k=SMC_QOS_MED;
    
    for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
    {
        if (ms_SessionDataMsi[j].IsPrimaryStream)
            return j;
    }
    
    return k;    
}

///////////////////////////////////////////////////////////////////////
// CMonitor::CMonitor
//
//  This method is the constructor for CMonitor
//
//  Inputs - None.
//
CMonitor::CMonitor()
{
    ms_hThread = NULL;
    ms_hEvent[0] = NULL;
    ms_hEvent[1] = NULL;
    ms_hSession = NULL;
}

///////////////////////////////////////////////////////////////////////
// CSelect::ChangeSelection
//
//  This method changes the session to the one selected in the Combo Box
//
//  Inputs - hCombo = Combo Box window handle.
//
//  Output - None.
//
void CMonitor::ChangeSelection(HWND hCombo)
{
    INT_PTR  index;
    WCHAR    rgTemp[STATIONNAME_LENGTH + 1];
    
    memset(rgTemp, 0, sizeof(rgTemp));

    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if (index == CB_ERR)
    {
        ASSERT(0);
        return;
    }
    ms_selectedQos = GetPrimaryStream();
    SendMessage(hCombo, CB_GETLBTEXT, (WPARAM)index, (LPARAM)&rgTemp[0]);

    for (int j=SMC_QOS_LOW ; j < SMC_QOS_MAXIMUM; j++)
    {
        switch (j)
        {
            case SMC_QOS_LOW:
                if (wcscmp(rgTemp, QOSLOW ) == 0)
                {
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[SMC_QOS_LOW], sizeof(ms_SessionData));
                    ms_selectedQos = SMC_QOS_LOW;
                }
                break;

            case SMC_QOS_MED:
                if (wcscmp(rgTemp, QOSNORMAL ) == 0)
                {
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[SMC_QOS_MED], sizeof(ms_SessionData));
                    ms_selectedQos = SMC_QOS_MED;
                }
                break;

            case SMC_QOS_HIGH:
                if (wcscmp(rgTemp, QOSHIGH ) == 0)
                {
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData), &ms_SessionDataMsi[SMC_QOS_HIGH], sizeof(ms_SessionData));
                    ms_selectedQos = SMC_QOS_HIGH;
                }                
                break;

            case SMC_QOS_REAL:
                if (wcscmp(rgTemp, QOSVERYHIGH ) == 0)
                {
                    memcpy_s(&ms_SessionData, sizeof(ms_SessionData) ,&ms_SessionDataMsi[SMC_QOS_REAL], sizeof(ms_SessionData));
                    ms_selectedQos = SMC_QOS_REAL;
                }                
                break;

        }
    }
    SendMessage(g_hDlg, CMONITOR_MSG_DISPLAY, 0, 0);

   
}

///////////////////////////////////////////////////////////////////////
// CMonitor::~CMonitor
//
//  This method is the destructor for CMonitor
//
CMonitor::~CMonitor()
{
}

