/* Copyright 2009 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef CWINSTATION_H
#define CWINSTATION_H

class CWinStation
{
  public:
    static bool StartThread();
    static void StopThread();

  private:
    // ensure that default CTOR etc cannot be called
    // CWinStation();
    CWinStation(const CWinStation&);
    const CWinStation& operator=(const CWinStation&);

    CWinStation();
    ~CWinStation();
    static UINT APIENTRY ThreadProc(PVOID p);

    static HANDLE ms_hThread;
    static bool   ms_fStop;
};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
HANDLE CWinStation::ms_hThread = 0;
bool   CWinStation::ms_fStop = false;
#endif

#endif // CWINSTATION_H
