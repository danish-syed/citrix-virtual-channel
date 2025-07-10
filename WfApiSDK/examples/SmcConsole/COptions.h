/* Copyright 2009 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef COPTIONS_H
#define COPTIONS_H

#ifndef CDISPLAY_H
    #include "CDisplay.h"
#endif

#define COPTIONS_DEFAULT_REFESH_MS           500
#define COPTIONS_DEFAULT_LATENCY_MS          1000
#define COPTIONS_DEFAULT_BANDWIDTH_BITS      1
#define COPTIONS_DEFAULT_VCLIMIT_KBITS       100
#define COPTIONS_MAX_PERCENT                 200
#define COPTIONS_MAX_VCLIMIT_KBITS           10000
#define COPTIONS_MAX_DISPLAY_VCLIMIT_KBITS   10000

class COptions : public CDisplay
{
  public:
    COptions(HWND hWnd);
    ~COptions();

    void Update();
    void DoMsg(UINT msg, WPARAM wParam, LPARAM lParam);

    // following declared as public static so all can see
    static int ms_refreshIntervalMs;
    static int ms_latencyRangeMs;
    static int ms_displayBandwidthBits;
    static int ms_BandwidthLimitkBits;
    static WCHAR ms_szlogFileName[MAX_PATH];
    static boolean ms_bLogStatus;

  private:
    // ensure that default CTOR etc cannot be called
    COptions();
    COptions(const COptions&);
    const COptions& operator=(const COptions&);
    bool GetLogFileName();
    void UpdateLogStatus();
};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
int COptions::ms_refreshIntervalMs    = COPTIONS_DEFAULT_REFESH_MS;
int COptions::ms_latencyRangeMs       = COPTIONS_DEFAULT_LATENCY_MS;
int COptions::ms_displayBandwidthBits = COPTIONS_DEFAULT_BANDWIDTH_BITS;
int COptions::ms_BandwidthLimitkBits  = COPTIONS_DEFAULT_VCLIMIT_KBITS;
WCHAR COptions::ms_szlogFileName[MAX_PATH];
boolean COptions::ms_bLogStatus = FALSE;
#endif

#endif // COPTIONS_H

