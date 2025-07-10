/* Copyright 2009 Citrix Systems, Inc.  */
/*
 * Copyright (c) 2001-2004 Citrix Systems, Inc. All Rights Reserved.
 */
#ifndef CVCHANNEL_H
#define CVCHANNEL_H

#ifndef CDISPLAY_H
    #include "CDisplay.h"
#endif

// we only allow for 28 channels, since Citrix only uses 23, that leaves 5 for
// OEMs
#define CVCHANNEL_MAX_VC 28

#define CVCHANNEL_BUTTON   0
#define CVCHANNEL_BWIDTH   1
#define CVCHANNEL_PROGRESS 2
#define CVCHANNEL_PRIORITY 3
#define CVCHANNEL_STEP_ID  4


#define CVCHANNEL_MAX_PRIORITY_STR 16
#define CVCHANNEL_MAX_SESSION_BANDWIDTH_STR 32

typedef struct
{
    int idxVC;
    int idButton;
} SDisplayMapToVC;

class CVChannel : public CDisplay
{
  public:
    CVChannel(HWND hWnd);
    ~CVChannel();

    void SetRanges();
    void NewSession();
    void Update();
    void DoMsg(UINT msg, WPARAM wParam, LPARAM lParam);

  private:
    // ensure that default CTOR etc cannot be called
    CVChannel();
    CVChannel(const CVChannel&);
    const CVChannel& operator=(const CVChannel&);

    void DisplayBandwidth(int);
    void DisplayPriority(int);
    static void ProcessInitDlg(HWND);
    static void ProcessOKBtn(HWND);

    static int APIENTRY SmcDlgChannel(HWND, UINT, WPARAM, LPARAM);
    static CVChannel*   ms_pCVChannel;
    static int          ms_idxChannel;

    SDisplayMapToVC m_rgDisplayMapToVC[CVCHANNEL_MAX_VC];

    int     m_idxFirstBlank;
    WCHAR   m_rgwchPriority[4][CVCHANNEL_MAX_PRIORITY_STR];
    WCHAR   m_rgwchNoLimit[CVCHANNEL_MAX_PRIORITY_STR];
    WCHAR   m_rgwdescriptiveNoLimit[CVCHANNEL_MAX_SESSION_BANDWIDTH_STR];
};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
CVChannel* CVChannel::ms_pCVChannel = 0;
int        CVChannel::ms_idxChannel = 0;
#endif

#endif // CVCHANNEL_H

