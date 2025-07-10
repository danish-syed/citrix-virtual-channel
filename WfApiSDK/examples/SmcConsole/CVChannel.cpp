// Copyright 2009-2010 Citrix Systems, Inc.
/*
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */
// CVChannel.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CVChannel.h"
#include "CMonitor.h"

#include <stdio.h>

///////////////////////////////////////////////////////////////////////
// CVChannel::CVChannel
//
//  This method is a constructor for CVChannel
//
//  Inputs - hWnd = Handle of Dialog Window
//
CVChannel::CVChannel(HWND hWnd) :
    CDisplay(hWnd), m_idxFirstBlank(0)
{
    // WARNING - mapping assumes all ids are contiguous
    //           and that each VC has 4 ids
    WCHAR rgwchTemp[64];

    int  id = IDC_VCHANNEL_BASE;
    for (int i = 0; i < CVCHANNEL_MAX_VC; i++)
    {
        // set mapping index to -1 for the VC
        m_rgDisplayMapToVC[i].idxVC = -1;

        // add the button id for WM_COMMAND processing
        m_rgDisplayMapToVC[i].idButton = id + CVCHANNEL_BUTTON;

        // get the index of the first blank button
        if (m_idxFirstBlank == 0)
        {
            rgwchTemp[0] = 0;
            GetDlgItemText(m_hWnd, id + CVCHANNEL_BUTTON, rgwchTemp, 64);
            if (0 == rgwchTemp[0])
                m_idxFirstBlank = i;
        }

        // move to next set
        id += CVCHANNEL_STEP_ID;
    }

    // load the display strings for priorities
    m_rgwchPriority[0][0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_PRIORITY_VERYHIGH,
               &m_rgwchPriority[0][0],
               CVCHANNEL_MAX_PRIORITY_STR);

    if (!m_rgwchPriority[0][0])
        wcscpy_s(&m_rgwchPriority[0][0], CVCHANNEL_MAX_PRIORITY_STR, L"Very High");

    m_rgwchPriority[1][0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_PRIORITY_HIGH,
               &m_rgwchPriority[1][0],
               CVCHANNEL_MAX_PRIORITY_STR);

    if (!m_rgwchPriority[1][0])
        wcscpy_s(&m_rgwchPriority[1][0], CVCHANNEL_MAX_PRIORITY_STR, L"High");

    m_rgwchPriority[2][0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_PRIORITY_NORMAL,
               &m_rgwchPriority[2][0],
               CVCHANNEL_MAX_PRIORITY_STR);

    if (!m_rgwchPriority[2][0])
        wcscpy_s(&m_rgwchPriority[2][0], CVCHANNEL_MAX_PRIORITY_STR, L"Normal");

    m_rgwchPriority[3][0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_PRIORITY_LOW,
               &m_rgwchPriority[3][0],
               CVCHANNEL_MAX_PRIORITY_STR);

    if (!m_rgwchPriority[3][0])
        wcscpy_s(&m_rgwchPriority[3][0], CVCHANNEL_MAX_PRIORITY_STR, L"Low");

    // and for no limit on a channel
    m_rgwchNoLimit[0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_BWIDTH_NOLIMIT,
               m_rgwchNoLimit,
               CVCHANNEL_MAX_PRIORITY_STR);

    if (!m_rgwchNoLimit[0])
        wcscpy_s(m_rgwchNoLimit, _countof(m_rgwchNoLimit), L"None");

    // and for no limit on a session
    m_rgwdescriptiveNoLimit[0] = 0;
    LoadString(GetModuleHandle(NULL),
               IDS_SESS_BWIDTH_NOLIMIT,
               m_rgwdescriptiveNoLimit,
               CVCHANNEL_MAX_SESSION_BANDWIDTH_STR);

    if (!m_rgwdescriptiveNoLimit[0])
        wcscpy_s(m_rgwdescriptiveNoLimit, _countof(m_rgwdescriptiveNoLimit), L"Unrestricted bandwidth");
}

///////////////////////////////////////////////////////////////////////
// CVChannel::~CVChannel
//
//  This method is the CVChannel destructon
//
CVChannel::~CVChannel()
{
}

///////////////////////////////////////////////////////////////////////
// CVChannel::SetRanges
//
//  This method initializes the limits on the progress bars and the session
//  control
//
//  Inputs - None.
//
//  Outputs - None.
//
void CVChannel::SetRanges()
{
    int   id;
    WCHAR upperLimit[100];

    // set the range for the session bandwidth progress bar

    SendDlgItemMessage(m_hWnd,
                       IDC_SESSION_BWIDTH,
                       PBM_SETRANGE,
                       0,
                       MAKELPARAM(0, COptions::ms_displayBandwidthBits));

    // Set the text for the upper limit of the session control
    swprintf_s(upperLimit, _countof(upperLimit), L"%d", COptions::ms_BandwidthLimitkBits);

    SetDlgItemText(m_hWnd, IDC_SESSION_LIMIT_MAX_TEXT, upperLimit);

    SendDlgItemMessage(m_hWnd,
                       IDC_SESSION_LIMIT,
                       TBM_SETRANGE,
                       0,
                       (LPARAM)MAKELPARAM(0, COptions::ms_BandwidthLimitkBits));

    id = IDC_VCHANNEL_BASE;
    for (int i = 0; i < CVCHANNEL_MAX_VC; i++)
    {
        // set the range for the bandwidth progress bar
        SendDlgItemMessage(m_hWnd,
                           id + CVCHANNEL_PROGRESS,
                           PBM_SETRANGE,
                           0,
                           MAKELPARAM(0, COptions::ms_displayBandwidthBits));

        SendDlgItemMessage(m_hWnd,
                           id + CVCHANNEL_PROGRESS,
                           PBM_SETPOS,
                           0,
                           0);

        id += CVCHANNEL_STEP_ID;
    }
}

///////////////////////////////////////////////////////////////////////
// CVChannel::NewSession
//
//  This method changes the display when a new session is selected.
//
//  Inputs - None.
//
//  Output - None.
//
void CVChannel::NewSession()
{
    // ensure the base class has updated its pointers
    CDisplay::Update();

    // clean up from last time
    int  id = IDC_VCHANNEL_BASE;

    int i = 0;
    for (i = 0; i < CVCHANNEL_MAX_VC; i++)
    {
        // set mapping index to -1
        m_rgDisplayMapToVC[i].idxVC = -1;

        // get rid of any session specific entries
        if (m_idxFirstBlank <= i)
            SetDlgItemText(m_hWnd, id + CVCHANNEL_BUTTON, L"");

        id += CVCHANNEL_STEP_ID;
    }

    // Reset the Session Limit
    SendDlgItemMessage(m_hWnd, IDC_SESSION_LIMIT, TBM_SETPOS, TRUE, 0);
    SetDlgItemInt(m_hWnd, IDC_SESSION_LIMIT_COUNT, 0, false);

    // now if we have a session map its entries
    if (CMonitor::IsSessionSelected())
    {
        WCHAR rgwchButton[64];
        WCHAR rgwchTemp[CVCHANNEL_MAX_SESSION_BANDWIDTH_STR];
        memset(rgwchButton, 0, sizeof(rgwchButton));
        memset(rgwchTemp, 0, sizeof(rgwchTemp));

        // for each session VC, find the corresponding display set
        // if there is no match use a blank at the end
        // there must be no duplicates
        int idxNextBlank = m_idxFirstBlank;
        for (int idx = 0; idx < (int)*m_pCountVC; idx++)
        {
            // some of the predefined names have trialing spaces
            // so strip them off the end
            WCHAR* pSrc = m_pVCData[idx].rgwchName;
            WCHAR* pDst = rgwchTemp;
            while (*pSrc)
                *pDst++ = *pSrc++;
            *pDst-- = 0;
            while ((L' ' == *pDst) && (pDst > rgwchTemp))
                *pDst-- = 0;

            id = IDC_VCHANNEL_BASE;

            int j = 0;
            for (j = 0; j < idxNextBlank; j++)
            {
                rgwchButton[0] = 0;
                GetDlgItemText(m_hWnd, id + CVCHANNEL_BUTTON, rgwchButton, 64);
                if (0 == _wcsicmp(rgwchButton, rgwchTemp))
                {
                    // we have a match so set the mapping index
                    m_rgDisplayMapToVC[j].idxVC = idx;
                    break;
                }

                id += CVCHANNEL_STEP_ID;
            }
            if ((j == idxNextBlank) &&
                (CVCHANNEL_MAX_VC > idxNextBlank))
            {
                // we didn't match any known
                // so add the label to the button
                SetDlgItemText(m_hWnd, id + CVCHANNEL_BUTTON, rgwchTemp);

                // move the next blank index
                idxNextBlank += 1;

                // set the mapping index
                m_rgDisplayMapToVC[j].idxVC = idx;
            }
        }
    }

    // ok now spin through and disable non-active channels
    // also set the bandwidth limits and priorities
    id = IDC_VCHANNEL_BASE;
    HWND hWnd;
    for (i = 0; i < CVCHANNEL_MAX_VC; i++)
    {
        if (-1 == m_rgDisplayMapToVC[i].idxVC)
        {
            // we don't have a channel for this one
            for (int j = 0; j < CVCHANNEL_STEP_ID; j++)
            {
                hWnd = GetDlgItem(m_hWnd, id + j);
                EnableWindow(hWnd, FALSE);
            }

            // display the default no limit bandwidth
            SetDlgItemText(m_hWnd, id + CVCHANNEL_BWIDTH, m_rgwchNoLimit);

            // display the default priority string
            SetDlgItemText(m_hWnd, id + CVCHANNEL_PRIORITY,
                &m_rgwchPriority[CTX_SESSION_VC_PRIORITY_NORMAL][0]);
        }
        else
        {
            // we have a valid channel
            for (int j = 0; j < CVCHANNEL_STEP_ID; j++)
            {
                hWnd = GetDlgItem(m_hWnd, id + j);
                EnableWindow(hWnd, TRUE);
            }

            DisplayBandwidth(i);

            DisplayPriority(i);
        }

        // step to next set
        id += CVCHANNEL_STEP_ID;
    }

    // if we have a session set its limit slider
    if (CMonitor::IsSessionSelected())
    {
        WCHAR rgwchTemp[CVCHANNEL_MAX_SESSION_BANDWIDTH_STR];

        int newPos = *m_pSessionLimitKBits;
        if (newPos > COptions::ms_BandwidthLimitkBits)
            newPos = COptions::ms_BandwidthLimitkBits;

        SendDlgItemMessage(m_hWnd, IDC_SESSION_LIMIT, TBM_SETPOS, TRUE, newPos);
        if (0 >= newPos)
            swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%s", m_rgwdescriptiveNoLimit);
        else
            swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", *m_pSessionLimitKBits);

        SetDlgItemText(m_hWnd, IDC_SESSION_LIMIT_COUNT, rgwchTemp);
    }
}

///////////////////////////////////////////////////////////////////////
// DisplayBandwidth,
//
//  This method displays the bandwidth for the specified Virtual Channel
//
//  Inputs - idx = Virtual Channel index
//
//  Output - None.
//
void CVChannel::DisplayBandwidth(int idx)
{
    int    id = IDC_VCHANNEL_BASE + idx * CVCHANNEL_STEP_ID;
    int    newPos;
    WCHAR* pTempTxt;
    WCHAR  rgwcTemp[16];

    // display the Bandwidth Limit
    newPos = m_pBWLimits[m_rgDisplayMapToVC[idx].idxVC];
    if (newPos <= 0)
        pTempTxt = m_rgwchNoLimit;
    else
    {
        swprintf_s(rgwcTemp, _countof(rgwcTemp), L"%d", newPos);
        pTempTxt = &rgwcTemp[0];
    }

    SetDlgItemText(m_hWnd, id + CVCHANNEL_BWIDTH, pTempTxt);
}

///////////////////////////////////////////////////////////////////////
// CVChannel::DisplayPriority
//
//  This method displays the priority for a Virtual Channel
//
//  Inputs - idx = Virtual Channel number
//
//  Output - None.
//
void CVChannel::DisplayPriority(int idx)
{
    int id = IDC_VCHANNEL_BASE + idx * CVCHANNEL_STEP_ID;
    int priority = m_pPriorities[m_rgDisplayMapToVC[idx].idxVC];

    if ((priority < 0) || (priority > 3))
        priority = 2;

    // display the priority string
    SetDlgItemText(m_hWnd, id + CVCHANNEL_PRIORITY,
                   &m_rgwchPriority[priority][0]);
}

///////////////////////////////////////////////////////////////////////
// CVChannel::Update,
//
//  This method updates the displays for all the VCs
//
//  Inputs - None.
//
//  Output - None
//
void CVChannel::Update()
{
    int id,
        iVCDataIx,
        newPos;

    // get the base class to update its pointers
    CDisplay::Update();

    // show the session total bandwidth in Kbits/sec
    newPos = m_pSessionData->BandwidthSentBitsPerSecond/1024;
    if (newPos > COptions::ms_displayBandwidthBits)
        newPos = COptions::ms_displayBandwidthBits;

    SendDlgItemMessage(m_hWnd, IDC_SESSION_BWIDTH, PBM_SETPOS, (WPARAM)(newPos),
                       0);

    // now for all enabled channels show their data
    id = IDC_VCHANNEL_BASE;
    for (int i = 0; i < CVCHANNEL_MAX_VC; i++)
    {
        iVCDataIx = m_rgDisplayMapToVC[i].idxVC;
        if (iVCDataIx != -1)
        {
            // set the position of the bandwidth progress bar in Kbits/sec
            newPos = m_pVCData[iVCDataIx].BandwidthSentPostCompressionEstimate /
                     1024;

            if (newPos > COptions::ms_displayBandwidthBits)
                newPos = COptions::ms_displayBandwidthBits;

            SendDlgItemMessage(m_hWnd, id + CVCHANNEL_PROGRESS, PBM_SETPOS,
                               (WPARAM)(newPos), 0);
        }

        id += CVCHANNEL_STEP_ID;
    }
}

///////////////////////////////////////////////////////////////////////
// DoMsg,
//
void CVChannel::DoMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    WCHAR rgwchTemp[256];
    int   bwLimit,
          i,
          wmId;

    switch (msg)
    {
        case WM_HSCROLL:
            if (LOWORD(wParam) == TB_ENDTRACK)
            {
                // now lParam contains the hWnd of the slider

                // The SendMessage function usually returns an LRESULT ( LONG_PTR ) type.
                // In this case it will be an int ( due to TBM_GETPOS ), so we can cast
                // it into an int.
                bwLimit = (int) SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                if (bwLimit <= 0)
                    bwLimit = 0;

                CMonitor::SetBWLimit(-1, bwLimit);
                bwLimit = *m_pSessionLimitKBits;
                if (bwLimit <= 0)
                    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%s", m_rgwdescriptiveNoLimit);
                else
                    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", bwLimit);

                SetDlgItemText(m_hWnd, IDC_SESSION_LIMIT_COUNT, rgwchTemp);
            }

            break;

        case WM_COMMAND:
            // find the entry for this button
            wmId = LOWORD(wParam);
            for (i = 0; i < CVCHANNEL_MAX_VC; i++)
            {
                if (m_rgDisplayMapToVC[i].idButton == wmId)
                    break;
            }

            if (i < CVCHANNEL_MAX_VC)
            {
                // intialize the statics for the dlgbox
                ms_pCVChannel = this;
                ms_idxChannel = i;

                DialogBox(NULL, MAKEINTRESOURCE(IDD_DLG_CHANNEL), m_hWnd,
                          (DLGPROC)SmcDlgChannel);
            }
            break;

        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////
// CVChannel::SmcDlgChannel
//
//  This method handles messages for the CVChannel Dialog
//
//  Inputs - hDlg = Handle of the CVChannel Dialog Box
//           msg = ID of the message.
//           wParam = Message dependent parameter
//           lParam = Message dependent parameter
//
//  Output - int = Response to Windows.
//
int APIENTRY CVChannel::SmcDlgChannel(HWND   hDlg,
                                      UINT   msg,
                                      WPARAM wParam,
                                      LPARAM lParam)
{
    int   bwLimit;
    WCHAR rgwchTemp[256];

    switch (msg)
    {
        case WM_INITDIALOG:
            ProcessInitDlg(hDlg);
            return (TRUE);

        case WM_HSCROLL:
            if (LOWORD(wParam) == TB_ENDTRACK)
            {
                // The SendMessage function usually returns an LRESULT ( LONG_PTR ) type.
                // In this case it will be an int ( due to TBM_GETPOS ), so we can cast
                // it into an int.
                bwLimit = (int) SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);

                if (bwLimit <= 0)
                    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%s", ms_pCVChannel->m_rgwdescriptiveNoLimit);
                else
                    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", bwLimit);

                SetDlgItemText(hDlg, IDC_CHANNEL_CONTROL_COUNT, rgwchTemp);
            }

            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    ProcessOKBtn(hDlg);
                    // fall through

                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return (TRUE);

                default:
                    break;
            }
            break;
    }

    return (FALSE);
}

///////////////////////////////////////////////////////////////////////
// CVChannel::ProcessInitDlg
//
//   This method processes the WM_DIALOG_INIT message.
//
//   Inputs - hDlg = Handle of the Dialog Box
//
//   Output - None.
//
void CVChannel::ProcessInitDlg(HWND hDlg)
{
    int   bwLimit,
          idxPriority;
    WCHAR rgwchTemp[256];

    // get the Channel Label
    GetDlgItemText(ms_pCVChannel->m_hWnd,
                   (ms_idxChannel * CVCHANNEL_STEP_ID) + IDC_VCHANNEL_BASE +
                       CVCHANNEL_BUTTON,
                   rgwchTemp,
                   256);

    SetDlgItemText(hDlg, IDC_CHANNEL_LABEL, rgwchTemp);

    // now get the friendly name
    LoadString(GetModuleHandle(NULL),
               ms_idxChannel + IDS_DESCR_BASE,
               rgwchTemp,
               256);

    SetDlgItemText(hDlg, IDC_CHANNEL_FRIENDLY, rgwchTemp);

    // Set the bandwidth control slider range to what was set in the
    // options page

    SendDlgItemMessage(hDlg,
                       IDC_CHANNEL_CONTROL,
                       TBM_SETRANGE,
                       0,
                       (LPARAM)MAKELPARAM(0, COptions::ms_BandwidthLimitkBits));

    // Set the text boxes for the lower and upper limit to what was
    // specified,  Note that the lower limit is always 1

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d",COptions::ms_BandwidthLimitkBits);
    SetDlgItemText(hDlg, IDC_VC_CONTROL_TEXT_MAX,rgwchTemp);

    // set the bandwidth limit
    bwLimit = ms_pCVChannel->m_pBWLimits
                       [ms_pCVChannel->m_rgDisplayMapToVC[ms_idxChannel].idxVC];
    if (bwLimit <= 0)
        swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%s",ms_pCVChannel->m_rgwdescriptiveNoLimit);
    else
        swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", bwLimit);

    SetDlgItemText(hDlg, IDC_CHANNEL_CONTROL_COUNT, rgwchTemp);
    if (bwLimit > COptions::ms_BandwidthLimitkBits)
        bwLimit = COptions::ms_BandwidthLimitkBits;

    SendDlgItemMessage(hDlg,
                       IDC_CHANNEL_CONTROL,
                       TBM_SETPOS,
                       TRUE,
                       bwLimit);

    // set the priority
    idxPriority = ms_pCVChannel->m_pPriorities
                       [ms_pCVChannel->m_rgDisplayMapToVC[ms_idxChannel].idxVC];

    SendDlgItemMessage(hDlg,
                       IDC_CHANNEL_VERYHIGH + idxPriority,
                       BM_SETCHECK,
                       (WPARAM)BST_CHECKED,
                       0);
}

///////////////////////////////////////////////////////////////////////
// CVChannel::ProcessOKBtn
//
//   This method process the OK button being clicked
//
//   Inputs - hDlg = Handle of the Dialog Box
//
//   Output - None.
//
void CVChannel::ProcessOKBtn(HWND hDlg)
{
    int   bwLimit,
          i,
          idxPriority;

    // the user has pressed apply so update the channel values
    // first the bandwidth limit

    // The SendDlgItemMessage function usually returns an LRESULT ( LONG_PTR ) type.
    // In this case it will be an int ( due to TBM_GETPOS ), so we can cast
    // it into an int.
    bwLimit = (int) SendDlgItemMessage(hDlg,
                                    IDC_CHANNEL_CONTROL,
                                    TBM_GETPOS,
                                    0,
                                    0);

    if (bwLimit != (int)ms_pCVChannel->m_pBWLimits
                        [ms_pCVChannel->m_rgDisplayMapToVC
                            [ms_idxChannel].idxVC])
    {
        if (CMonitor::SetBWLimit(ms_pCVChannel->m_rgDisplayMapToVC
                                    [ms_idxChannel].idxVC,
                                bwLimit))
        {
            ms_pCVChannel->DisplayBandwidth(ms_idxChannel);
        }
    }

    // now the channel priority
    // set the priority
    idxPriority = 0;
    for (i = IDC_CHANNEL_VERYHIGH; i < IDC_CHANNEL_LOW; i++)
    {
        if (SendDlgItemMessage(hDlg,
                               i,
                               BM_GETCHECK,
                               (WPARAM)BST_CHECKED,
                               0) == BST_CHECKED)
        {
            break;
        }
    }

    idxPriority = i - IDC_CHANNEL_VERYHIGH;
    if ((int)ms_pCVChannel->
            m_pPriorities[ms_pCVChannel->
                              m_rgDisplayMapToVC[ms_idxChannel].idxVC] !=
        idxPriority)
    {
        if (CMonitor::SetPriority(ms_pCVChannel->m_rgDisplayMapToVC
                                      [ms_idxChannel].idxVC,
                                  idxPriority))
        {
            ms_pCVChannel->DisplayPriority(ms_idxChannel);
        }
    }
}
