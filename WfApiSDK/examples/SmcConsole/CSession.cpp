// Copyright 2009-2010 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
// CSession.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CSession.h"
#include "CMonitor.h"

#include <stdio.h>



///////////////////////////////////////////////////////////////////////
// CSession::CSession
//
//  This method is the CSession cosnstructor
//
//  Inputs - hWnd = Window handle of sessin dialog
//
CSession::CSession(HWND hWnd) :
  CDisplay(hWnd)
{
}

///////////////////////////////////////////////////////////////////////
// CSession::~CSession
//
//  This method is the CSession destructor
//
CSession::~CSession()
{
}

///////////////////////////////////////////////////////////////////////
// CSession::SetRanges,
//
//  This method sets the ranges of the progress bar controls.
//
//  Inputs - None.
//
//  Output - None.
//
void CSession::SetRanges()
{
    // initialize the session sent progress bars

    // set the bandwidth range
    SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_BW, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_displayBandwidthBits));

    SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_BW, PBM_SETPOS, 0, 0);

    // set the compression % to a range of 0 to COPTIONS_MAX_PERCENT
    SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_COMP, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COPTIONS_MAX_PERCENT));

    SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_COMP, PBM_SETPOS, 0, 0);

    // initialize the session received progress bars

    // set the bandwidth range
    SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_BW, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_displayBandwidthBits));

    SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_BW, PBM_SETPOS, 0, 0);

    // set the compression % to a range of 0 to COPTIONS_MAX_PERCENT
    SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_COMP, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COPTIONS_MAX_PERCENT));

    SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_COMP, PBM_SETPOS, 0, 0);

    // initialize the latency bars

    // set the last latency range
    SendDlgItemMessage(m_hWnd, IDC_LATENCY_LAST, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_latencyRangeMs));

    SendDlgItemMessage(m_hWnd, IDC_LATENCY_LAST, PBM_SETPOS, 0, 0);

    // set the average latency range
    SendDlgItemMessage(m_hWnd, IDC_LATENCY_AVE, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_latencyRangeMs));

    SendDlgItemMessage(m_hWnd, IDC_LATENCY_AVE, PBM_SETPOS, 0, 0);

    // set the round trip deviation range
    SendDlgItemMessage(m_hWnd, IDC_LATENCY_TRIP, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_latencyRangeMs));

    SendDlgItemMessage(m_hWnd, IDC_LATENCY_TRIP, PBM_SETPOS, 0, 0);


    // set the linespeed range
    SendDlgItemMessage(m_hWnd, IDC_SESS_OUTPUT_LS, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_displayBandwidthBits));

    SendDlgItemMessage(m_hWnd, IDC_SESS_OUTPUT_LS, PBM_SETPOS, 0, 0);

    SendDlgItemMessage(m_hWnd, IDC_SESS_INPUT_LS, PBM_SETRANGE, 0,
                       MAKELPARAM(0, COptions::ms_displayBandwidthBits));

    SendDlgItemMessage(m_hWnd, IDC_SESS_INPUT_LS, PBM_SETPOS, 0, 0);

}

///////////////////////////////////////////////////////////////////////
// CSession::NewSession
//
//  This method updates the base class pointers
//
//  Inputs - None.
//
//  Output - None.
//
void CSession::NewSession()
{
    // ensure the base class has updated its pointers
    CDisplay::Update();
}

///////////////////////////////////////////////////////////////////////
// GetFormattedString
//
// Formats a 64 bit number and returns it in terms of Kilo, Mega etc
// bytes.
//
// if the byte count is < 1M then we display the raw count
// if it is < 2^42, we display the count in KBytes
// otherwise if it is < 2^52, we display the count in MBytes
// otherwise if it is < 2^62, we display the count in GBytes
// otherwise we display it in Tera bytes.
// We have to do this processing because NT4 header do not have a
// _uiI64tow() function.
//
void CSession::GetFormattedString(UINT64 nBytes, UINT32 buffSize, WCHAR *pwchString)
{
    if (nBytes < 1048576L)
        swprintf_s(pwchString, buffSize, L"%u ", nBytes);
    else if (nBytes < (((UINT64)1) << 42))
        swprintf_s(pwchString, buffSize, L"%u K", nBytes / 1024L);
    else if (nBytes < (((UINT64)1) << 52))
        swprintf_s(pwchString, buffSize, L"%u M", nBytes / 1048576L);
    else if (nBytes < (((UINT64)1) << 62))
        swprintf_s(pwchString, buffSize, L"%u G", nBytes / (((UINT64)1) << 30));
    else
        swprintf_s(pwchString, buffSize, L"%u T", nBytes / (((UINT64)1) << 40));
}

///////////////////////////////////////////////////////////////////////
// GetFormattedLatencyString
//
// Formats a 32 bit latency ms count and returns it in terms of  Minutes,
// seconds and milliseconds
//
// if the count is < 1000 then we display the raw count in ms
// otherwise if it is < 60000 (60 s), we display it in seconds, millisec
// otherwise e display it in Minutes, Seconds and Millisecs.
//
void CSession::GetFormattedLatencyString(UINT32 LatencyTickCount,
                                         UINT32 buffSize,
										 WCHAR* pwchString)
{
    UINT32 LatencyUpdate = GetTickCount() - LatencyTickCount;
    UINT32 LatencyMin;
    UINT32 LatencySec;
    UINT32 LatencyMs;

    if (LatencyUpdate < 1000)
        swprintf_s(pwchString, buffSize, L"%ums", LatencyUpdate);
    else if (LatencyUpdate < 60000)
    {
        LatencySec = LatencyUpdate / 1000;
        LatencyMs  = LatencyUpdate % 1000;

        swprintf_s(pwchString, buffSize, L"%us %ums", LatencySec, LatencyMs);
    }
    else
    {
        LatencyMin = LatencyUpdate / 60000;
        LatencySec = LatencyUpdate % 60000;
        LatencyMs  = LatencySec % 1000;
        LatencySec /= 1000;

        swprintf_s(pwchString, buffSize, L"%um %us %ums", LatencyMin, LatencySec,
                 LatencyMs);
    }
}

///////////////////////////////////////////////////////////////////////
// CSession::Update
//
//  This method updates the session data display.
//
//  Input - None.
//
//  Output - None.
//
void CSession::Update()
{
    int    aveComp;
    UINT64 den,
           num;
    WCHAR  rgwchTemp[64];

	memset(rgwchTemp, 0, sizeof(rgwchTemp));

    // get the base class to update its pointers
    NewSession();

    // display the sent server data
    GetFormattedString(m_pServerData->TotalBytesSentPreCompression, _countof(rgwchTemp), rgwchTemp);
    SetDlgItemText(m_hWnd, IDC_SRV_SENT_BEFORE, rgwchTemp);

    GetFormattedString(m_pServerData->TotalBytesSentPostCompression, _countof(rgwchTemp), rgwchTemp);
    SetDlgItemText(m_hWnd, IDC_SRV_SENT_AFTER, rgwchTemp);

    // scale down to divide for display
    num = m_pServerData->TotalBytesSentPostCompression;
    den = m_pServerData->TotalBytesSentPreCompression;

    // if we are greater than the largest +ve number that can be
    // represented in 32 bits we scale down
    while ((num >= 0x7FFFFFFF) || (den >= 0x7FFFFFFF))
    {
        num >>= 1;
        den >>= 1;
    }

    if (0 >= den)
        den = 1;

    aveComp = (int)((100 * (float)((int)num) / (float)((int)den)));

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", aveComp);
    SetDlgItemText(m_hWnd, IDC_SRV_SENT_AVE, rgwchTemp);

    // display the received server data
    GetFormattedString(m_pServerData->TotalBytesReceivedPreExpansion,
                       _countof(rgwchTemp),
                       rgwchTemp);

    SetDlgItemText(m_hWnd, IDC_SRV_RECV_BEFORE, rgwchTemp);

    GetFormattedString(m_pServerData->TotalBytesReceivedPostExpansion,
                       _countof(rgwchTemp),
                       rgwchTemp);

    SetDlgItemText(m_hWnd, IDC_SRV_RECV_AFTER, rgwchTemp);

    num = m_pServerData->TotalBytesReceivedPreExpansion;
    den = m_pServerData->TotalBytesReceivedPostExpansion;
    while ((0x7FFFFFFF <= num) || (0x7FFFFFFF <= den))
    {
        num >>= 1;
        den >>= 1;
    }
    if (0 >= den)
        den = 1;

    aveComp = (int)((100 * (float)((int)num) /(float)((int)den)));
    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", aveComp);
    SetDlgItemText(m_hWnd, IDC_SRV_RECV_AVE, rgwchTemp);

    // now see if there is a valid sesion
    if (CMonitor::IsSessionSelected())
    {
        int newPos;

		
        // sent session data
        GetFormattedString(m_pSessionData->BytesSentPostCompression, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_SESS_SENT_COUNT, rgwchTemp);

        // bandwidth in kbits/sec
        GetFormattedString(m_pSessionData->BandwidthSentBitsPerSecond/1024, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_SENT_BW_COUNT, rgwchTemp);
        newPos = m_pSessionData->BandwidthSentBitsPerSecond / 1024;

        if (newPos > COptions::ms_displayBandwidthBits)
            newPos = COptions::ms_displayBandwidthBits;

        if (newPos < 0)
            newPos = 0;

        SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_BW, PBM_SETPOS, (WPARAM)newPos,
                           0);

        // compression
        newPos = (int)((100 * (float)m_pSessionData->CompressionNumeratorSent) /
                       (float)(m_pSessionData->CompressionDenominatorSent + 1));
        if (newPos > COPTIONS_MAX_PERCENT)
            newPos = COPTIONS_MAX_PERCENT;

		if (m_pSessionData->CompressionNumeratorSent == 0 && m_pSessionData->CompressionDenominatorSent == 0)
			newPos = 0;
		
		// CompressionNumeratorSent seems to be 1 by default intially
		if (m_pSessionData->CompressionNumeratorSent == 1 && m_pSessionData->CompressionDenominatorSent == 0)
		{
			newPos = 0;
		}

        if (newPos < 0)
            newPos = 0;

        SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_COMP, PBM_SETPOS,
                           (WPARAM)newPos, 0);

        // received session data
        GetFormattedString(m_pSessionData->BytesReceivedPreExpansion, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_SESS_RECV_COUNT, rgwchTemp);

        // bandwidth in kbits/sec
        GetFormattedString(m_pSessionData->BandwidthReceivedBitsPerSecond/1024, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_RECV_BW_COUNT, rgwchTemp);

        newPos = m_pSessionData->BandwidthReceivedBitsPerSecond/1024;
        if (COptions::ms_displayBandwidthBits < newPos)
            newPos = COptions::ms_displayBandwidthBits;

        if (newPos < 0)
            newPos = 0;

        SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_BW, PBM_SETPOS, (WPARAM)newPos,
                           0);

        newPos = (int)((100 *
                        (float)m_pSessionData->CompressionNumeratorReceived) /
                       (float)(m_pSessionData->CompressionDenominatorReceived +
                               1));

		if (m_pSessionData->CompressionNumeratorReceived == 0 && m_pSessionData->CompressionDenominatorReceived == 0)
			newPos = 0;

		if (m_pSessionData->CompressionNumeratorReceived == 1 && m_pSessionData->CompressionDenominatorReceived == 0)
		{
			newPos = 0;
		}
        if (COPTIONS_MAX_PERCENT < newPos)
            newPos = COPTIONS_MAX_PERCENT;

        SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_COMP, PBM_SETPOS,
                           (WPARAM)newPos, 0);


        // input Line speed in kbits/src
        GetFormattedString(m_pSessionData->InputSpeedInBitsPerSecond, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_SESS_INPUT_COUNT, rgwchTemp);

        newPos = m_pSessionData->InputSpeedInBitsPerSecond/1024;
        if (COptions::ms_displayBandwidthBits < newPos)
            newPos = COptions::ms_displayBandwidthBits;

        if (newPos < 0)
            newPos = 0;

        SendDlgItemMessage(m_hWnd, IDC_SESS_INPUT_LS, PBM_SETPOS, (WPARAM)newPos, 0);


        // output Line speed in kbits/src
        GetFormattedString(m_pSessionData->OutputSpeedInBitsPerSecond, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_SESS_OUTPUT_COUNT, rgwchTemp);

        newPos = m_pSessionData->OutputSpeedInBitsPerSecond/1024;
        if (COptions::ms_displayBandwidthBits < newPos)
            newPos = COptions::ms_displayBandwidthBits;

        if (newPos < 0)
            newPos = 0;

        SendDlgItemMessage(m_hWnd, IDC_SESS_OUTPUT_LS, PBM_SETPOS, (WPARAM)newPos, 0);


        // session latency data
        newPos = m_pSessionData->LastClientLatency;
        if (newPos < 0)
            newPos = 0;

        swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", newPos);
        SetDlgItemText(m_hWnd, IDC_LATENCY_LAST_COUNT, rgwchTemp);

        if (COptions::ms_latencyRangeMs < newPos)
            newPos = COptions::ms_latencyRangeMs;

        SendDlgItemMessage(m_hWnd, IDC_LATENCY_LAST, PBM_SETPOS,
                           (WPARAM)newPos, 0);

        newPos = m_pSessionData->AverageClientLatency;
        if (newPos < 0)
            newPos = 0;

        swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", newPos);
        SetDlgItemText(m_hWnd, IDC_LATENCY_AVE_COUNT, rgwchTemp);

        if (COptions::ms_latencyRangeMs < newPos)
            newPos = COptions::ms_latencyRangeMs;

        SendDlgItemMessage(m_hWnd, IDC_LATENCY_AVE, PBM_SETPOS, (WPARAM)newPos,
                           0);

        newPos = m_pSessionData->RoundTripDeviation;
        if (newPos < 0)
            newPos = 0;

        swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", newPos);
        SetDlgItemText(m_hWnd, IDC_LATENCY_TRIP_COUNT, rgwchTemp);

        if (COptions::ms_latencyRangeMs < newPos)
            newPos = COptions::ms_latencyRangeMs;

        SendDlgItemMessage(m_hWnd, IDC_LATENCY_TRIP, PBM_SETPOS, (WPARAM)newPos,
                           0);

        // The last update time, is the TickCount of the last update, minus the
        // current tick count
        GetFormattedLatencyString(m_pSessionData->LastUpdateTime, _countof(rgwchTemp), rgwchTemp);
        SetDlgItemText(m_hWnd, IDC_LATENCY_LAST_TIME, rgwchTemp);
    }
    else
    {
        // sent session data
        SetDlgItemText(m_hWnd, IDC_SESS_SENT_COUNT, L"0");

        SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_BW, PBM_SETPOS, 0, 0);

        SendDlgItemMessage(m_hWnd, IDC_SESS_SENT_COMP, PBM_SETPOS, 0, 0);

        // received session data
        SetDlgItemText(m_hWnd, IDC_SESS_RECV_COUNT, L"0");

        SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_BW, PBM_SETPOS, 0, 0);

        SendDlgItemMessage(m_hWnd, IDC_SESS_RECV_COMP, PBM_SETPOS, 0, 0);

        // session latency data
        SendDlgItemMessage(m_hWnd, IDC_LATENCY_LAST, PBM_SETPOS, 0, 0);

        SetDlgItemText(m_hWnd, IDC_LATENCY_LAST_COUNT, L"0");

        SendDlgItemMessage(m_hWnd, IDC_LATENCY_AVE, PBM_SETPOS, 0, 0);

        SetDlgItemText(m_hWnd, IDC_LATENCY_AVE_COUNT, L"0");

        SendDlgItemMessage(m_hWnd, IDC_LATENCY_TRIP, PBM_SETPOS, 0, 0);

        SetDlgItemText(m_hWnd, IDC_LATENCY_TRIP_COUNT, L"0");

        SetDlgItemText(m_hWnd, IDC_LATENCY_LAST_TIME, L"0");
    }
}

