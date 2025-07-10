// Copyright 2009-2010 Citrix Systems, Inc.
#include "CtxSmcUI.h"
#include "CtxSmcDlg.h"
//#include "CMonitor.h"
#include "CSelect.h"
#include "CDataLogger.h"

///////////////////////////////////////////////////////////////////////
// CDataLogger::StartLog
//
//  This method, given a log filename, checks to see if it is different
//  from the current log file.  If it is different it will close the 
//  current file and open a handle to the new file.  If the filename was
//  unchanged then it will just return.
//
//  Inputs - szLogFileName = file to log to.
//
//  Output - None.
void CDataLogger::StartLog(WCHAR *szLogFileName)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    errno_t  eRet;

    sprintf_s(ms_sztimeStamp, _countof(ms_sztimeStamp), "%d:%d:%d:%d",sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
    
    if (m_hFile != NULL) 
    {
        // we have an exiting file so if the file name has changed then
        // close the exiting file and open a new one.
        if ( wcscmp(szLogFileName, ms_szLogFileName ) != 0 )
        {
            fclose(m_hFile);
        }
        else
        {
            return;
        }
    }
    m_bSessionHeaderWritten = FALSE;
    wcscpy_s(ms_szLogFileName, _countof(ms_szLogFileName),szLogFileName);
    eRet = _wfopen_s(&m_hFile,ms_szLogFileName, L"w+b");

    if (m_hFile)
    {
        fprintf(m_hFile, "Log Started on %d\\%d\\%d @ %s\r\n",sysTime.wMonth,sysTime.wDay,sysTime.wYear, ms_sztimeStamp);
        WriteServerHeaderLabel();
    }

}

///////////////////////////////////////////////////////////////////////
// CDataLogger::StopLog
//
//  This method closes the current log file if it is open.  
//
//  Inputs - None.
//
//  Output - None.
void CDataLogger::StopLog(void)
{
    if (m_hFile != NULL) 
    {
        fclose(m_hFile);
        m_hFile = NULL;
    }
       
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::StopLog
//
//  This method write the server data counters to the log file.  
//
//  Inputs - pServerdata = pointer to server data block.
//           cbData = size of the server data block
//
//  Output - None.
void CDataLogger::WriteServerData(pCtx_Smc_Server_Data pServerdata, UINT cbData)
{
    int    aveComp;
    UINT64 den, num;
    CHAR  rgwchTemp[64];

    
    if (m_hFile)
    {
        fprintf(m_hFile,"\r\n%s,",ms_sztimeStamp);
        // Log the sent server data postcompression
        FormatToString(pServerdata->TotalBytesSentPostCompression, _countof(rgwchTemp), rgwchTemp);
        fprintf(m_hFile,"%s,",rgwchTemp);


        // scale down to divide for display
        num = pServerdata->TotalBytesSentPostCompression;
        den = pServerdata->TotalBytesSentPreCompression;

        // if we are greater than the largest +ve number that can be
        // represented in 32 bits we scale down
        while ((num >= 0x7FFFFFFF) || (den >= 0x7FFFFFFF))
        {
            num >>= 1;
            den >>= 1;
        }

        if (0 >= den)
            den = 1;

        aveComp = (int)((100 * (float)((int)num) / (float)((int)den)));

        // Log the Average Sent compression
        fprintf(m_hFile,"%d,",aveComp);

        // log the received server data pre expansion
        FormatToString(pServerdata->TotalBytesReceivedPreExpansion, _countof(rgwchTemp), rgwchTemp);
        fprintf(m_hFile,"%s,",rgwchTemp);

        num = pServerdata->TotalBytesReceivedPreExpansion;
        den = pServerdata->TotalBytesReceivedPostExpansion;
        
        while ((0x7FFFFFFF <= num) || (0x7FFFFFFF <= den))
        {
            num >>= 1;
            den >>= 1;
        }
        if (0 >= den)
            den = 1;

        aveComp = (int)((100 * (float)((int)num) /(float)((int)den)));

        // log the average received compression
        fprintf(m_hFile,"%d",aveComp);
    }   
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::WriteSessionHeader
//
//  This method writes the session ID and the row of column headers
//  when the caller decides to log a new session. 
//
//  Inputs - sessionID = ID of the session.
//           numVCs = number of VCs in the session
//           rgVCNames = array containing the VC names
//
//  Output - None.
void CDataLogger::WriteSessionHeader(UINT sessionID, UINT numVCs, pCtx_Smc_Session_VC_Data_V2 pVCData)
{
    if (m_hFile)
    {
        fprintf(m_hFile,"\r\nLogging Session %d.  Column headers as below:\r\n",sessionID);
        WriteServerHeaderLabel();
        WriteSessionHeaderLabel(numVCs, pVCData);
        m_bSessionHeaderWritten = TRUE;
    }
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::WriteSessionData
//
//  This method registers the fact that a new session has started
//
//  Inputs - None.
//
//  Output - None.
void CDataLogger::LogNewSession()
{
    // a new session has started.
    m_bNewSession = TRUE;
 
}


///////////////////////////////////////////////////////////////////////
// CDataLogger::WriteSessionData
//
//  This method writes counters for the current session to the logfile.
//  It is possible for this method to be called by the caller without 
//  a previous call to WriteSessionHeader().  If this is detected then
//  the a call is made to WriteSessionHeader().
//
//  Inputs - sessionID = ID of the session.
//           pSessionData = pointer to data block for the session.
//           numVCs = number of VCs in the session
//           rgVCNames = array containing the VC names
//           pVCData = pointer to block containing the VC data.
//
//  Output - None.
void CDataLogger::WriteSessionData( UINT sessionID, pCtx_Smc_Session_Data_V2 pSessionData, UINT numVCs, pCtx_Smc_Session_VC_Data_V2 pVCData)
{
    CHAR   rgwchTemp[64];
    
    if (m_hFile)
    {
        // if a new session has strated the just write up the column headers
        // and exit.  On the next call to here we have both server and session
        // data.
        if (m_bNewSession)
        {
            m_bNewSession = FALSE;
            WriteSessionHeader(sessionID, numVCs, pVCData);
            // we return here because at this stage we do not have the server data. 
            // the next call will have already had the header written and the server data.
            return;
        }

        if (!m_bSessionHeaderWritten)
        {
            // we don't display results on a session change.  This will allow the
            // display of both server and session data at the same time.
            WriteSessionHeader(sessionID, numVCs, pVCData);
            // we return here because at this stage we do not have the server data. 
            // the next call will have already had the header written and the server data.
            return;
        }       

        // bandwidth in kbits/sec
        FormatToString(pSessionData->BandwidthSentBitsPerSecond/1024, _countof(rgwchTemp), rgwchTemp);
        fprintf(m_hFile,",%s,",rgwchTemp);
        
       
        // bandwidth in kbits/sec
        FormatToString(pSessionData->BandwidthReceivedBitsPerSecond/1024, _countof(rgwchTemp), rgwchTemp);
        fprintf(m_hFile,"%s,",rgwchTemp);


        // output Line speed in kbits/src
        FormatToString(pSessionData->OutputSpeedInBitsPerSecond, _countof(rgwchTemp), rgwchTemp);
        fprintf(m_hFile,"%s,",rgwchTemp);
        
        // average latency      
        fprintf(m_hFile,"%d",pSessionData->AverageClientLatency);

        //log the VC bandwidth in each direction
        for (UINT i = 0; i < numVCs; i++)
        {
            pCtx_Smc_Session_VC_Data_V2 pVCTemp = &pVCData[i];
            int vcBandwidth =  pVCTemp->BandwidthSentPostCompressionEstimate/1024;
            fprintf(m_hFile,",%d", vcBandwidth);
            vcBandwidth =  pVCTemp->BandwidthReceivedPreExpansionEstimate/1024;
            fprintf(m_hFile,",%d", vcBandwidth);
        }
    }         
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::IsLoggingActive
//
//  This method informs the caller if logging  is active
//
//  Inputs - None
//
//  Output - TRUE if logging is active.
bool CDataLogger::IsLoggingActive()
{
    return ((m_hFile==NULL) ? FALSE : TRUE);
}


///////////////////////////////////////////////////////////////////////
// CDataLogger::FormatToString
//
// Formats a 64 bit number and returns it in terms of Kilo, Mega etc
// bytes.
//
// if the byte count is < 1M then we display the raw count
// if it is < 2^42, we display the count in KBytes
// otherwise if it is < 2^52, we display the count in MBytes
// otherwise if it is < 2^62, we display the count in GBytes
// otherwise we display it in Tera bytes.
//
//  Inputs - nBytes = data to stringify
//         - pchString = buffer to contain formatted string
//
//  Output - None.
void CDataLogger::FormatToString(UINT64 nBytes, UINT32 buffSize, CHAR *pchString)
{
    if (nBytes < 1048576L)
        sprintf_s(pchString, buffSize, "%u ", nBytes);
    else if (nBytes < (((UINT64)1) << 42))
        sprintf_s(pchString, buffSize, "%u K", nBytes / 1024L);
    else if (nBytes < (((UINT64)1) << 52))
        sprintf_s(pchString, buffSize, "%u M", nBytes / 1048576L);
    else if (nBytes < (((UINT64)1) << 62))
        sprintf_s(pchString, buffSize, "%u G", nBytes / (((UINT64)1) << 30));
    else
        sprintf_s(pchString, buffSize, "%u T", nBytes / (((UINT64)1) << 40));
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::FormatLatencyString
//
// Formats a 32 bit latency ms count and returns it in terms of  Minutes,
// seconds and milliseconds
//
// if the count is < 1000 then we display the raw count in ms
// otherwise if it is < 60000 (60 s), we display it in seconds, millisec
// otherwise e display it in Minutes, Seconds and Millisecs.
//
//  Inputs - LatencyTickCount = latency count to stringify
//         - pchString = buffer to contain formatted string
//
//  Output - None.
void CDataLogger::FormatLatencyString(UINT32 LatencyTickCount, UINT32 buffSize, CHAR* pchString)
{
    UINT32 LatencyUpdate = GetTickCount() - LatencyTickCount;
    UINT32 LatencyMin;
    UINT32 LatencySec;
    UINT32 LatencyMs;

    if (LatencyUpdate < 1000)
        sprintf_s(pchString, buffSize, "%ums", LatencyUpdate);
    else if (LatencyUpdate < 60000)
    {
        LatencySec = LatencyUpdate / 1000;
        LatencyMs  = LatencyUpdate % 1000;

        sprintf_s(pchString, buffSize, "%us %ums", LatencySec, LatencyMs);
    }
    else
    {
        LatencyMin = LatencyUpdate / 60000;
        LatencySec = LatencyUpdate % 60000;
        LatencyMs  = LatencySec % 1000;
        LatencySec /= 1000;

        sprintf_s(pchString, buffSize, "%um %us %ums", LatencyMin, LatencySec,
                 LatencyMs);
    }
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::WriteServerHeaderLabel
//
// Write the row of column labels that pertain to the server data being
// logged.
//
//  Inputs - None
//
//  Output - None.
void CDataLogger::WriteServerHeaderLabel()
{
    if (m_hFile)
       fprintf(m_hFile,"TimeStamp, Server Bytes Sent (bytes), Sent Data Compression ratio (%%), Server Bytes Received (bytes), Received Data Compression ratio (%%)");
}

///////////////////////////////////////////////////////////////////////
// CDataLogger::WriteSessionHeaderLabel
//
// Write the row of column labels that pertain to the session data being
// logged.  This is session specific because each session can have 
// different VCs.
//
//  Inputs - numVCs = number of VCs
//           rgVCNames = array of the names of VCs.
//
//  Output - None.
void CDataLogger::WriteSessionHeaderLabel(UINT numVCs, pCtx_Smc_Session_VC_Data_V2 pVCData)
{
    size_t CharConv = 0;
    if (m_hFile)
    {
        fprintf(m_hFile,",Session Sent BW (kbps), Session Received BW (kbps), Line Throughput (bps), Avg Latency (ms)");
        for (UINT i = 0; i < numVCs; i++)
        {
            CHAR rgchName[8];
            wcstombs_s(&CharConv, rgchName, 8, pVCData->rgwchName, 8);
            fprintf(m_hFile,",%s, ", rgchName);
            pVCData++;
        }
    }
}

