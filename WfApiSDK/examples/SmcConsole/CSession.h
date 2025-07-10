/* Copyright 2009 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef CSESSION_H
#define CSESSION_H

#ifndef CDISPLAY_H
    #include "CDisplay.h"
#endif

class CSession : public CDisplay
{
  public:
    CSession(HWND hWnd);
    ~CSession();

    void SetRanges();
    void NewSession();
    void Update();

  private:
    // ensure that default CTOR etc cannot be called
    CSession();
    CSession(const CSession&);
    const CSession& operator=(const CSession&);
    void GetFormattedString(UINT64, UINT32, WCHAR*);
    void GetFormattedLatencyString(UINT32, UINT32, WCHAR*);
};

#endif // CSESSION_H

