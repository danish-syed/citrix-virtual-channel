/* Copyright 2009 Citrix Systems, Inc.  */
/*
 * Copyright (c) 2001-2004 Citrix Systems, Inc. All Rights Reserved.
 */
#ifndef CSELECT_H
#define CSELECT_H

// string representation in the combobox is
// "WinstationName (UserName)"
#define WINSTATIONNAME_LENGTH 32
#define USERNAME_LENGTH       20
// in the statement below, 3 == (Space and Two Brackets)
#define STATIONNAME_LENGTH (WINSTATIONNAME_LENGTH + USERNAME_LENGTH + 3)

#define CSELECT_NO_SESSION 0xFFFFFFFF

class CSelect
{
 public:
    static void Init();
    static void Cleanup();
    static void Refresh(HWND hCombo, LPARAM WFEvent);
    static void ChangeSelection(HWND hCombo);

 private:
    // ensure that default CTOR etc cannot be called
    CSelect();
    CSelect(const CSelect&);
    const CSelect& operator=(const CSelect&);

    CSelect(CSelect* pPrev, UINT32 SessionID, WCHAR* pwchszName);
    ~CSelect();

    CSelect*        m_pNext;
    UINT32          m_SessionID ;
    WCHAR           m_rgWinStationName[STATIONNAME_LENGTH +1];

    static UINT32   ms_SelectedSession;
    static CSelect* ms_pRoot;
    static WCHAR    ms_rgNoneSelected[STATIONNAME_LENGTH + 1];
};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
UINT32          CSelect::ms_SelectedSession = CSELECT_NO_SESSION;
CSelect*        CSelect::ms_pRoot           = 0;
WCHAR           CSelect::ms_rgNoneSelected[STATIONNAME_LENGTH + 1];
#endif

#endif // CSELECT_H

