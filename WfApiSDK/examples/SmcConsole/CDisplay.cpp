// Copyright 2009-2010 Citrix Systems, Inc. 
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
// CDisplay.cpp

#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
#include "CDisplay.h"
#include "CMonitor.h"

///////////////////////////////////////////////////////////////////////
// CDisplay::CDisplay
//
//  This method is the constructor for the CDisplay object
//
//  Inputs - hWnd = Window handle of associated window.
//
CDisplay::CDisplay(HWND hWnd) :
  m_hWnd(hWnd), m_pServerData(0), m_pSessionData(0), m_pSessionLimitKBits(0),
  m_pCountVC(0), m_pVCData(0), m_pBWLimits(0), m_pPriorities(0)
{
}

///////////////////////////////////////////////////////////////////////
// CDisplay::~CDisplay
//
//  This method is the destructor for the CDisplay object.  It destroys
//  the window associated with the object instance.
//
CDisplay::~CDisplay()
{
    if (m_hWnd)
        DestroyWindow(m_hWnd);
}

///////////////////////////////////////////////////////////////////////
// CDisplay::SetRanges
//
void CDisplay::SetRanges()
{
}

///////////////////////////////////////////////////////////////////////
// CDisplay::NewSession
//
void CDisplay::NewSession()
{
}

///////////////////////////////////////////////////////////////////////
// CDisplay::Update
//
//  This method updates the local counters from the CMonitor object.
//
//  Inputs - None.
//
//  Output - None.
//
void CDisplay::Update()
{
    
	m_pServerData        = &CMonitor::ms_ServerData;
    m_pSessionData       = &CMonitor::ms_SessionData;
    m_pSessionLimitKBits = &CMonitor::ms_SessionBWLimitKBits;
    m_pCountVC           = &CMonitor::ms_cSessionVCs;
    m_pVCData            =  CMonitor::ms_rgVCData;
    m_pBWLimits          =  CMonitor::ms_rgBWLimits;
    m_pPriorities        =  CMonitor::ms_rgPriorities;

}

///////////////////////////////////////////////////////////////////////
// CDisplay::DoMsg
//
void CDisplay::DoMsg(UINT /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
}


