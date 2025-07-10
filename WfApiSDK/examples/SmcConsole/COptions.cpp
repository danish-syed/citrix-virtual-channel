// Copyright 2009-2010 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
// COptions.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "COptions.h"
#include "CMonitor.h"

#include <stdio.h>

///////////////////////////////////////////////////////////////////////
// CTOR
//
COptions::COptions(HWND hWnd) :
   CDisplay(hWnd)
{

    
    // set the refresh rate to a range of 100 to 10,000 ms in 100 ms intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_REFRESH, TBM_SETRANGE, 0,
                       MAKELPARAM(1, 100));

    // set the display bandwidth to a range of 1 to 100kB in 1 kB intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_BANDWIDTH, TBM_SETRANGE, 0,
                       MAKELPARAM(1, COPTIONS_MAX_DISPLAY_VCLIMIT_KBITS));

    // set the latency to a range of 1 to 10,000 ms in 10 ms intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_LATENCY, TBM_SETRANGE, 0,
                       MAKELPARAM(1, 1000));

    // set the bandwidth limit to a range of 1 to COPTIONS_MAX_VCLIMIT_KBITS
    // in 1 kbit intervals

    SendDlgItemMessage(m_hWnd, IDC_OPT_BW_LIMIT, TBM_SETRANGE, 0,
                       MAKELPARAM(1, COPTIONS_MAX_VCLIMIT_KBITS));

    // read the default logfile name from the resource file.
    LoadString(GetModuleHandle(NULL), IDS_DEFAULTLOGFILE, &ms_szlogFileName[0], MAX_PATH);
   
 }

///////////////////////////////////////////////////////////////////////
// DTOR
//
COptions::~COptions()
{
}

///////////////////////////////////////////////////////////////////////
// Update,
//
void COptions::Update()
{
    // get the base class to update its pointers
    CDisplay::Update();

    WCHAR rgwchTemp[64];

    // set the refresh rate to a range of 100 to 10,000 ms in 100 ms intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_REFRESH, TBM_SETPOS, TRUE,
                       ms_refreshIntervalMs / 100);

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", ms_refreshIntervalMs);
    SetDlgItemText(m_hWnd, IDC_OPT_REFRESH_COUNT, rgwchTemp);

    // set the display bandwidth to a range of 1 to 100 kB in 1 kB intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_BANDWIDTH, TBM_SETPOS, TRUE,
                       ms_displayBandwidthBits);

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", ms_displayBandwidthBits);
    SetDlgItemText(m_hWnd, IDC_OPT_BANDWIDTH_COUNT, rgwchTemp);

    // set the latency to a range of 1 to 10,000 ms in 10 ms intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_LATENCY, TBM_SETPOS, TRUE,
                       ms_latencyRangeMs / 10);

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", ms_latencyRangeMs);
    SetDlgItemText(m_hWnd, IDC_OPT_LATENCY_COUNT, rgwchTemp);

    // set the bandwidth limit to a range of 1 to 100 kbits in 1 kbit intervals
    SendDlgItemMessage(m_hWnd, IDC_OPT_BW_LIMIT, TBM_SETPOS, TRUE,
                       ms_BandwidthLimitkBits);

    swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", ms_BandwidthLimitkBits);
    SetDlgItemText(m_hWnd, IDC_OPT_BW_LIMIT_COUNT, rgwchTemp);

    SetDlgItemText(m_hWnd, IDC_LOG_FILENAME_LABEL,ms_szlogFileName);
}

///////////////////////////////////////////////////////////////////////
// COptions::DoMsg
//
//  This method processes Windows messages.
//
//  Inputs - msg = Message ID
//           wParam = Message dependent parameter
//           lParam = Message dependent parameter
//
//  Output - None.
//
void COptions::DoMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    int   pos           = 0;
    int   iCntlID       = 0;
    int*  piVal         = NULL;
    WCHAR rgwchTemp[64] = {0};

    if ((msg == WM_HSCROLL) && (LOWORD(wParam) == TB_ENDTRACK))
    {
        // now lParam contains the hWnd of the slider so see which one we have
		// The SendMessage function usually returns an LRESULT ( LONG_PTR ) type.
		// In this case it will be an int ( due to TBM_GETPOS ), so we can cast 
		// it into an int.
        pos = (int) SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
        if (pos == 0)
            pos = 1;

        if (GetDlgItem(m_hWnd, IDC_OPT_REFRESH) == (HWND)lParam)
        {
            ms_refreshIntervalMs = pos * 100;
            piVal = &ms_refreshIntervalMs;
            iCntlID = IDC_OPT_REFRESH_COUNT;
        }
        else if (GetDlgItem(m_hWnd, IDC_OPT_BANDWIDTH) == (HWND)lParam)
        {
            ms_displayBandwidthBits = pos;
            piVal = &ms_displayBandwidthBits;
            iCntlID = IDC_OPT_BANDWIDTH_COUNT;
        }
        else if (GetDlgItem(m_hWnd, IDC_OPT_LATENCY) == (HWND)lParam)
        {
            ms_latencyRangeMs = pos * 10;
            piVal = &ms_latencyRangeMs;
            iCntlID = IDC_OPT_LATENCY_COUNT;
        }
        else if (GetDlgItem(m_hWnd, IDC_OPT_BW_LIMIT) == (HWND)lParam)
        {
            ms_BandwidthLimitkBits = pos;
            piVal = &ms_BandwidthLimitkBits;
            iCntlID = IDC_OPT_BW_LIMIT_COUNT;
        }

        if (iCntlID != 0)
        {
            swprintf_s(rgwchTemp, _countof(rgwchTemp), L"%d", *piVal);
            SetDlgItemText(m_hWnd, iCntlID, rgwchTemp);
        }
    }

    if (wParam == IDC_LOGFILENAME)
    {
        if (GetLogFileName())
        {
           // only update the log filename label if the user selected a
           // file in the fileopen dialog.
           SetDlgItemText(m_hWnd, IDC_LOG_FILENAME_LABEL,ms_szlogFileName);
        }
    }
    else if (wParam == IDC_LOGGING_STATUS)
    {
        // toggle the logging status.
        ms_bLogStatus = !ms_bLogStatus;
        UpdateLogStatus();    
    }
}

///////////////////////////////////////////////////////////////////////
// COptions::GetLogFileName
//
//  This method creates an file Open dialog box that lets the user
//  specify the file that will be used for logs.
//
//  Inputs - None
//
//  Output - TRUE implies sucess i.e. the user has sselected a 
//           a valid file.
bool COptions::GetLogFileName()
{
    WCHAR szFilter[] = L"CSV Files (*.csv)\0*.csv\0" \
                       L"All Files (*.*)\0*.*\0\0";
    WCHAR szFileName[MAX_PATH];
    WCHAR szTitle[MAX_PATH] = L"Select Log File Name";
    OPENFILENAME ofn;
	FILE* hFile = NULL;
    
    memset(&ofn, 0, sizeof(ofn));
    wcscpy_s(szFileName,  ms_szlogFileName); 
    
    ofn.lStructSize       = sizeof (OPENFILENAME) ;
    ofn.hwndOwner         = NULL;//m_hWnd ;
    ofn.hInstance         = NULL ;
    ofn.lpstrFilter       = szFilter ;
    ofn.lpstrCustomFilter = NULL ;
    ofn.nMaxCustFilter    = 0 ;
    ofn.nFilterIndex      = 0 ;
    ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
    ofn.nMaxFile          = MAX_PATH ;
    ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
    ofn.nMaxFileTitle     = MAX_PATH ;
    ofn.lpstrInitialDir   = NULL ;
    ofn.lpstrTitle        = NULL ;
    ofn.Flags             = 0 ;             // Set in Open and Close functions
    ofn.nFileOffset       = 0 ;
    ofn.nFileExtension    = 0 ;
    ofn.lpstrDefExt       = L"txt";
    ofn.lCustData         = 0L ;
    ofn.lpfnHook          = NULL ;
    ofn.lpTemplateName    = NULL ;

    ofn.lpstrFile         = szFileName;
    ofn.lpstrFileTitle    = szTitle;
    ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

   
    if (GetOpenFileName(&ofn))
    {
        wcscpy_s(ms_szlogFileName, MAX_PATH, szFileName);     

		if (!(_wfopen_s(&hFile, szFileName, L"w+b")))
		{
			if (hFile)
				fclose(hFile);
		}
		else
		{
			return FALSE;
		}

        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////
// COptions::UpdateLogStatus
//
//  This method updates the logging status label in the options dialog
//
//  Inputs - None
//
//  Output - None
void COptions::UpdateLogStatus()
{
    WCHAR rgwchTemp[64];
    
    if (ms_bLogStatus)
        LoadString(GetModuleHandle(NULL), IDS_LOGSTATUS_ON, &rgwchTemp[0], 64);
    else
        LoadString(GetModuleHandle(NULL), IDS_LOGSTATUS_OFF, &rgwchTemp[0], 64);
    SetDlgItemText(m_hWnd, IDC_LOG_STATUS_LABEL,rgwchTemp);       
}

