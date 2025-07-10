// Copyright 2009-2011 Citrix Systems, Inc. 
/*
 * Copyright (c) 2001-2004 Citrix Systems, Inc. All Rights Reserved.
 */
// CSelect.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CSelect.h"
#include "CMonitor.h"

#include "wfapi.h"

#ifdef UNICODE
#define STRNCMP wcsncmp
#else
#define STRNCMP strncmp
#endif

///////////////////////////////////////////////////////////////////////
// CSelect::Init
//
//  This method initializes the "none selected" data member
//
//  Inputs - None.
//
//  Outputs - None.
//
void CSelect::Init()
{
    ms_rgNoneSelected[0] = 0;

    LoadString(GetModuleHandle(NULL), IDS_SELECT_NONE, &ms_rgNoneSelected[0],
               STATIONNAME_LENGTH + 1);

    if (ms_rgNoneSelected[0] == 0)
        wcscpy_s(&ms_rgNoneSelected[0], _countof(ms_rgNoneSelected), L"<None Selected>");
}

///////////////////////////////////////////////////////////////////////
// CSelect::Cleanup
//
//  This method deletes the list of combo box entries
//
//  Inputs - None.
//
//  Output - None.
//
void CSelect::Cleanup()
{
    // clear any list entries
    delete ms_pRoot;
    ms_pRoot = 0;
}

///////////////////////////////////////////////////////////////////////
// CSelect::Refresh
//
//  This method rebuilds the entries in the Combo Box
//
//  Inputs - hCombo = Window handle of Combo Box
//
//  Output - None.
//
void CSelect::Refresh(HWND hCombo, LPARAM WFEvent)
{
    // default the current selection
    UINT32           SelectedID    = CSELECT_NO_SESSION;
    WCHAR*           pSelectedName = ms_rgNoneSelected;
    PWF_SESSION_INFO pSessionInfo,
                     pCurrSessionInfo;
    ULONG            ulSessionCnt,
                     ul;
    WCHAR            wcszInstanceName[STATIONNAME_LENGTH + 1]; // instance name
    DWORD            dwBytesReturned;
    LPTSTR           lpszSessionInfo;
    CSelect*         pCSelect;

    pCurrSessionInfo = NULL;

    // clear any previous list entries
    Cleanup();

    // clear any previous contents
    SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)ms_rgNoneSelected);

    // If this refresh request is a result of a session log off event,
    // wait a little longer for the session to completely logoff.  If we
    // enumerate the sessions too soon, the session being logged off may
    // be returned the list as an active session.
    //
    // If the wait time is not proper for the environment, a longer or
    // shorter time may be used.  A more complicated algorithm may be
    // used to consolidate many events occuring in a short period of time
    // into one refresh request.

    if (WFEvent & WF_EVENT_LOGOFF)
    {
        Sleep(1000);
    }

    // Enumerate the winstations
    // Internally the WFEnumerateSessions allocates memory which we'll need to
    // later free with a call to WFFreeMemory
    if (!WFEnumerateSessions(WF_CURRENT_SERVER, 0, 1, &pSessionInfo,
                             &ulSessionCnt))
    {
        // Call failed
        ASSERT(0);
        return;
    }
    else
    {
        // now walk each entry to add to the list
        for (ul = 0; ul < ulSessionCnt; ul++)
        {
            pCurrSessionInfo = &pSessionInfo[ul];

            // check if its an ICA session
            if (pCurrSessionInfo->State != WFActive)
            {
                // skip the console and inactive sessions
                continue;
            }
			
			else if (( WFGetActiveProtocol( pCurrSessionInfo->SessionId ) != WFProtocolICA ))
            {
                // must be an MS RDP session
                continue;
            }
			
            if (!WFQuerySessionInformation(WF_CURRENT_SERVER,
                                           pCurrSessionInfo->SessionId,
                                           WFUserName,
                                           &lpszSessionInfo,
                                           &dwBytesReturned))
            {
                ASSERT(0);
                continue;
            }

            if (dwBytesReturned == 0)
            {
                ASSERT(0);
                continue;
            }

            // Get the instance name
            swprintf_s(wcszInstanceName,
                     _countof(wcszInstanceName),
                     L"%s (%s)",
                     pCurrSessionInfo->pWinStationName,
                     lpszSessionInfo);

            // Free memory allocatec by WFQuerySessionInfo call
            WFFreeMemory(lpszSessionInfo);

            // ok so now create a wrapper class
            pCSelect = new CSelect(ms_pRoot,
                                   pCurrSessionInfo->SessionId,
                                   wcszInstanceName);
            if (pCSelect == NULL)
            {
                // we ran out of memory so scram
                break;
            }
            else
                ms_pRoot = pCSelect;

            // if this is the currently selected session
            // save the pointer to its name to insert into the combo
            if (ms_SelectedSession == pCSelect->m_SessionID)
            {
                pSelectedName = pCSelect->m_rgWinStationName;
                SelectedID    = pCSelect->m_SessionID;
            }

            // now add this entry to the listbox
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)wcszInstanceName);
        }
    }

    // Free up the memory allocated by the WFEnumerateSessions call
    if (pSessionInfo != NULL)
    {
        WFFreeMemory(pSessionInfo);
    }

    // set the current selection
    SendMessage(hCombo, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)pSelectedName);

    // now tell the rest of the world if the selection changed
    if (ms_SelectedSession != SelectedID)
    {
        ms_SelectedSession = SelectedID;
        CMonitor::SetSession(SelectedID);
    }
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
void CSelect::ChangeSelection(HWND hCombo)
{
    INT_PTR  index;
    WCHAR    rgTemp[STATIONNAME_LENGTH + 1];
    CSelect* p;

	memset(rgTemp, 0, sizeof(rgTemp));

    index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if (index == CB_ERR)
    {
        ASSERT(0);
        return;
    }

    SendMessage(hCombo, CB_GETLBTEXT, (WPARAM)index, (LPARAM)&rgTemp[0]);

    // check if they selected <None Selected>
    if (wcscmp(rgTemp, ms_rgNoneSelected) == 0)
    {
        if (ms_SelectedSession != CSELECT_NO_SESSION)
        {
            ms_SelectedSession = CSELECT_NO_SESSION;
            CMonitor::SetSession(ms_SelectedSession);
        }

        return;
    }

    // if there are no sessions just scram so we don't just keep recursing
    // through Refresh()
    if (!ms_pRoot)
        return;

    // now scan the list looking for the match
    p = ms_pRoot;
    while (p)
    {
        if (wcscmp(rgTemp, p->m_rgWinStationName) == 0)
            break;

        p = p->m_pNext;
    }

    if (p == NULL)
    {
        // something is screwed so reset everything
        ASSERT(0);
        Refresh(hCombo, 0);
        return;
    }

    // check if its the same as before
    if (ms_SelectedSession != p->m_SessionID)
    {
        ms_SelectedSession = p->m_SessionID;
        CMonitor::SetSession(ms_SelectedSession);
    }
}

///////////////////////////////////////////////////////////////////////
// CSelect::CSelect
//
//  This method is the CSelect constructor
//
//  Inputs - pPrev = Pointer to the previous CSelect list member
//           SessionID = ID of session being constructed
//           pwcszName = Name of the session
//
CSelect::CSelect(CSelect* pPrev, UINT32 SessionID, WCHAR* pwcszName) :
   m_pNext(pPrev), m_SessionID(SessionID)
{
    wcsncpy_s(m_rgWinStationName, STATIONNAME_LENGTH, pwcszName, STATIONNAME_LENGTH);
    m_rgWinStationName[STATIONNAME_LENGTH] = 0;
}

///////////////////////////////////////////////////////////////////////
// CSelect::~CSelect
//
//  This method is the CSelect destructor
//
CSelect::~CSelect()
{
    delete m_pNext;
}

