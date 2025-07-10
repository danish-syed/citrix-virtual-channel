/* Copyright 2009-2010 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef CDISPLAY_H
#define CDISPLAY_H

#ifndef WFAPI_H
    #include "wfapi.h"
#endif

class CDisplay
{
  public:
    CDisplay(HWND hWnd);
    virtual ~CDisplay();

    virtual void SetRanges();
    virtual void NewSession();
    virtual void Update();

    virtual void DoMsg(UINT msg, WPARAM wParam, LPARAM lParam);

    HWND GetWnd()
    {
        return m_hWnd;
    }

  protected:
    HWND                        m_hWnd;
    pCtx_Smc_Server_Data        m_pServerData;
    pCtx_Smc_Session_Data_V2    m_pSessionData;
    UINT32*                     m_pSessionLimitKBits;
    UINT32*                     m_pCountVC;
    pCtx_Smc_Session_VC_Data_V2 m_pVCData;
    UINT32*                     m_pBWLimits;
    UINT32*                     m_pPriorities;
 
  private:
    // ensure that default CTOR etc cannot be called
    CDisplay();
    CDisplay(const CDisplay&);
    const CDisplay& operator=(const CDisplay&);
};

#endif // CDISPLAY_H

