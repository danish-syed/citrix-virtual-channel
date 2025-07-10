/* Copyright 2009-2010 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef CMONITOR_H
#define CMONITOR_H

#ifndef WFAPI_H
    #include "wfapi.h"
#endif

#ifndef COPTIONS_H
    #include "COptions.h"
#endif

#define CMONITOR_MSG_REFRESH        (WM_APP + 1)
#define CMONITOR_MSG_DISPLAY        (WM_APP + 2)
#define CMONITOR_MSG_CHANGESESSION  (WM_APP + 3)
#define CMONITOR_MSG_SESSION  (WM_APP + 4)

#define CMONITOR_MAX_VC         32

#define CMONITOR_DISPLAY_NONE       0
#define CMONITOR_DISPLAY_SESSION    1
#define CMONITOR_DISPLAY_VCS        2

#define MSG_BUFFER                400
#define SMC_QOS_LOW        0
#define SMC_QOS_MED        1
#define SMC_QOS_HIGH       2
#define SMC_QOS_REAL       3
#define SMC_QOS_MAXIMUM    (SMC_QOS_REAL + 1)

#define QOSLOW L"Connection: Priority Very High"
#define QOSNORMAL L"Connection: Priority High"
#define QOSHIGH L"Connection: Priority Normal"
#define QOSVERYHIGH L"Connection: Priority Low"

class CMonitor
{
  public:
    static bool StartThread();
    static void StopThread();
    static bool IsSessionSelected();
    static void SetSession(int id);
    static void SetDisplay(int type);
    static bool SetBWLimit(int idxVC, int val);
    static bool SetPriority(int idxVC, int val);
    static void DoRefresh(DWORD);
    static bool IsMultiStreamActive();
    static void ChangeSelection(HWND hCombo);
    static UINT32 GetPrimaryStream();

  private:
    // ensure that default CTOR etc cannot be called
    // CMonitor();
    CMonitor(const CMonitor&);
    const CMonitor& operator=(const CMonitor&);

    CMonitor();
    ~CMonitor();
    static UINT APIENTRY ThreadProc(PVOID p);

    static HANDLE                   ms_hThread;
    static bool                     ms_fStop;
    static bool                     ms_fNeedRefresh;
    static HANDLE                   ms_hEvent[2];
    static int                      ms_Display;
    static UINT32                   ms_SessionID;
    static HANDLE                   ms_hSession;
    static Ctx_Smc_Server_Data      ms_ServerData;
    static Ctx_Smc_Session_Data_V2  ms_SessionData;
    static Ctx_Smc_Session_Data_V2  ms_SessionDataMsi[SMC_QOS_MAXIMUM];
    static UINT32                   ms_SessionBWLimitKBits;
    static UINT32                   ms_cSessionVCs;
    static Ctx_Smc_Session_VC_Data_V2  ms_rgVCData[CMONITOR_MAX_VC];
    static UINT32                   ms_rgBWLimits[CMONITOR_MAX_VC];
    static UINT32                   ms_rgPriorities[CMONITOR_MAX_VC];
    static DWORD                    ms_dwWFEvent;
    static volatile UINT32			ms_selectedQos;

    // let the display classes see the data
    friend class CDisplay;
};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
HANDLE                   CMonitor::ms_hThread = 0;
bool                     CMonitor::ms_fStop = false;
bool                     CMonitor::ms_fNeedRefresh = false;
HANDLE                   CMonitor::ms_hEvent[2];
int                      CMonitor::ms_Display = CMONITOR_DISPLAY_NONE;
UINT32                   CMonitor::ms_SessionID = 0;
HANDLE                   CMonitor::ms_hSession = 0;
Ctx_Smc_Server_Data      CMonitor::ms_ServerData;
Ctx_Smc_Session_Data_V2  CMonitor::ms_SessionData;
Ctx_Smc_Session_Data_V2     CMonitor::ms_SessionDataMsi[SMC_QOS_MAXIMUM];
UINT32                   CMonitor::ms_SessionBWLimitKBits;
UINT32                   CMonitor::ms_cSessionVCs = CMONITOR_MAX_VC;
Ctx_Smc_Session_VC_Data_V2 CMonitor::ms_rgVCData[CMONITOR_MAX_VC];
UINT32                   CMonitor::ms_rgBWLimits[CMONITOR_MAX_VC];
UINT32                   CMonitor::ms_rgPriorities[CMONITOR_MAX_VC];
DWORD                    CMonitor::ms_dwWFEvent = 0;
volatile UINT32					 CMonitor::ms_selectedQos = 0;

#endif

#endif // CMONITOR_H
