/* Copyright 2009-2010 Citrix Systems, Inc. */
#ifndef CDataLogger_H
#define CDataLogger_H

#ifndef WFAPI_H
    #include "wfapi.h"
#endif

class CDataLogger
{
public:

static    void StartLog(WCHAR *szLogFileName);
static    void StopLog(void);
static    void WriteServerData(pCtx_Smc_Server_Data, UINT);
static    void WriteSessionHeader(UINT, UINT, pCtx_Smc_Session_VC_Data_V2);
static    void WriteSessionData(UINT, pCtx_Smc_Session_Data_V2, UINT, pCtx_Smc_Session_VC_Data_V2);
static    bool IsLoggingActive();
static    void LogNewSession();

private:
    // ensure that default CTOR etc cannot be called
    CDataLogger();
    const CDataLogger& operator=(const CDataLogger&);
    ~CDataLogger();
    static    void FormatToString(UINT64 nBytes, UINT32 buffSize, CHAR *pchString);
    static    void FormatLatencyString(UINT32 LatencyTickCount, UINT32 buffSize, CHAR* pchString);
    static    void WriteSessionHeaderLabel(UINT numVCs, pCtx_Smc_Session_VC_Data_V2 pVCData);
    static    void WriteServerHeaderLabel();    
    static FILE*   m_hFile;
    static WCHAR   ms_szLogFileName[256];
    static CHAR    ms_sztimeStamp[64];
    static bool    m_bSessionHeaderWritten;
    static bool    m_bNewSession;

};

// define class statics
#ifdef CTXSMC_INSTANTIATE_STATICS
FILE* CDataLogger::m_hFile = NULL;
WCHAR CDataLogger::ms_szLogFileName[256]=L"";
CHAR CDataLogger::ms_sztimeStamp[64];
bool CDataLogger::m_bSessionHeaderWritten=FALSE;
bool CDataLogger::m_bNewSession=FALSE;
#endif

#endif // CDataLogger_H

