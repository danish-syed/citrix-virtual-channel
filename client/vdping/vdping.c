/*************************************************************************
*
* Copyright (C) Citrix Systems, Inc. All Rights Reserved.
*
* VDPING.C
*
* Virtual Driver Example - Ping
*
* $Id: 
*
* The code and comments contained in this file are provided "as is"
* without warranty of any kind, either expressed or implied,
* including, but not limited to, the implied warranties of
* merchantability and/or fitness for a particular purpose.
* You have the right to use, modify, reproduce, and distribute
* this file, and/or any modified version of this file, in any way
* that you find useful, provided that you agree that Citrix Systems
* has no warranty obligations or liability for this or any other
* sample file.
*
* REMARKS:
*	This sample program will work with all clients.  It has been enhanced
*	to send data immediately when working with the HPC client (Version >= 13.0).
*	
*	When the HPC client is detected, this driver works without regular polling.
*	When ICA data arrives from the server, responses are sent immediately.
*	If the client is busy and cannot send the data immediately, the driver
*	is informed of that fact, and the DriverPoll function will be called
*	later when the driver should attempt to send again.
*
*************************************************************************/

#ifdef DOS32
#define CITRIX 1
#include <sys/types.h>
#include "wfc32.h"
#endif

#ifndef unix
#include <windows.h>         /* for far etc. */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "citrix.h"
#include "clib.h"

/* from src/shared/inc/citrix */
#include "ica.h"
#include "ica-c2h.h"
#include "vdping.h"

/* from src/inc */
#include <time.h>

#include "icaconst.h"
#include "clterr.h"
#include "wdapi.h"
#include "vdapi.h"
#include "vd.h"
#include "ctxdebug.h"
#include "logapi.h"
#include "miapi.h"

#include "cwire.h"
#include "cmacro.h"
#include <wchar.h>

//=============================================================================
//==   Definitions
//=============================================================================

#if defined(macintosh) || defined(unix)
#include "pingwire.h"
#endif

#ifdef DEBUG
#pragma optimize ("", off)	// turn off optimization
#endif // DEBUG

#define INI_PING_VDSECTION "Ping"
#define INI_PING_PINGCOUNT "PingCount"
#define DEF_PING_PINGCOUNT 3
#define SIZEOF_CONSOLIDATION_BUFFER 2000		// size of the consolidation buffer to allocate (arbitrary 2000 for the sample).
#define NUMBER_OF_MEMORY_SECTIONS 1             // number of memory buffers to send as a single packet

//=============================================================================
//==   Functions Defined
//=============================================================================

// These are the required driver functions (exported)

int DriverOpen(PVD, PVDOPEN, PUINT16);
int DriverClose(PVD, PDLLCLOSE, PUINT16);
int DriverInfo(PVD, PDLLINFO, PUINT16);
int DriverPoll(PVD, PVOID, PUINT16);
int DriverQueryInformation(PVD, PVDQUERYINFORMATION, PUINT16);
int DriverSetInformation(PVD, PVDSETINFORMATION, PUINT16);
int DriverGetLastError(PVD, PVDLASTERROR);
wchar_t* GetRegKeyTemplate(void);

static void WFCAPI ICADataArrival(PVOID, USHORT, LPBYTE, USHORT);

//=============================================================================
//==   Data
//=============================================================================

PVOID g_pWd = NULL;                         // returned when we register our hook
PQUEUEVIRTUALWRITEPROC g_pQueueVirtualWrite  = NULL;  // returned when we register our hook
BOOL g_fBufferEmpty = TRUE;					// True if the data buffer is empty
PPING g_pPing = NULL;						// pointer to ping data buffer
USHORT g_uLen = 0;							// length of ping data to write
USHORT g_usMaxDataSize = 0;                 // Maximum Data Write Size
USHORT g_usPingCount;						// Number of times to ping us
USHORT g_usVirtualChannelNum = 0;           // Channel number assigned by WD
BOOL g_fIsHpc = FALSE;                      // T: The engine is HPC
PSENDDATAPROC g_pSendData = NULL;           // pointer to the HPC engine SendData function.  Returned when we register our hook.
LPBYTE g_pbaConsolidationBuffer = NULL;     // buffer to consolidate a write that spans the end and beginning of the ring buffer.
MEMORY_SECTION g_MemorySections[NUMBER_OF_MEMORY_SECTIONS];  // memory buffer pointer array
ULONG g_ulUserData = 0xCAACCAAC;      		// sample user data for HPC


//=============================================================================
//==   Joseph
//=============================================================================

FILE* fl;
//DWORD WINAPI BackgroundThread(LPVOID lpParam);
HANDLE hThread;
DWORD threadId;
char* str = "Payload Data ";


/*******************************************************************************
 *
 *  DriverOpen
 *
 *    Called once to set up things.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pVdOpen (input/output)
 *       pointer to the structure VDOPEN
 *    puiSize (Output)
 *       size of VDOPEN structure.
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *    CLIENT_ERROR_NO_MEMORY - could not allocate data buffer
 *    On other errors, an error code is returned from lower level functions.
 *
 ******************************************************************************/

int DriverOpen(PVD pVd, PVDOPEN pVdOpen, PUINT16 puiSize)
{
    WDSETINFORMATION   wdsi;
    VDWRITEHOOK        vdwh;
	VDWRITEHOOKEX      vdwhex;					// struct for getting more engine information, used by HPC
    WDQUERYINFORMATION wdqi;
    OPENVIRTUALCHANNEL OpenVirtualChannel;
    int                rc;
    UINT16             uiSize;

    TRACE((TC_VD, TT_API1, "VDPING: DriverOpen entered"))

    g_fBufferEmpty = TRUE;

    *puiSize = sizeof(VDOPEN);

    // Get a virtual channel

    wdqi.WdInformationClass = WdOpenVirtualChannel;
    wdqi.pWdInformation = &OpenVirtualChannel;
    wdqi.WdInformationLength = sizeof(OPENVIRTUALCHANNEL);
    OpenVirtualChannel.pVCName = CTXPING_VIRTUAL_CHANNEL_NAME;

    // uiSize will be set  when we return back from VdCallWd.

    uiSize = sizeof(WDQUERYINFORMATION);

    rc = VdCallWd(pVd, WDxQUERYINFORMATION, &wdqi, &uiSize);
    TRACE((TC_VD, TT_API1, "VDPING: opening channel %s", OpenVirtualChannel.pVCName))

    if(rc != CLIENT_STATUS_SUCCESS)
    {
        TRACE((TC_VD, TT_ERROR, "VDPING: Could not open %s. rc=%d.", OpenVirtualChannel.pVCName, rc));
        return(rc);
    }

    g_usVirtualChannelNum = OpenVirtualChannel.Channel;

    pVd->pPrivate   = NULL; /* pointer to private data, if needed */

    // Register write hooks for our virtual channel

    vdwh.Type  = g_usVirtualChannelNum;
    vdwh.pVdData = pVd;
    vdwh.pProc = (PVDWRITEPROCEDURE) ICADataArrival;
    wdsi.WdInformationClass  = WdVirtualWriteHook;
    wdsi.pWdInformation      = &vdwh;
    wdsi.WdInformationLength = sizeof(VDWRITEHOOK);
    uiSize                   = sizeof(WDSETINFORMATION);

    rc = VdCallWd(pVd, WDxSETINFORMATION, &wdsi, &uiSize);
    TRACE((TC_VD, TT_API2, "VDPING: writehook channel number=%u vdwh.pVdData=%x rc=%d", vdwh.Type, vdwh.pVdData, rc));

    if(CLIENT_STATUS_SUCCESS != rc)
    {
        TRACE((TC_VD, TT_ERROR, "VDPING: Could not register write hook. rc %d", rc))
        return(rc);
    }
    g_pWd = vdwh.pWdData;										// get the pointer to the WD data
    g_pQueueVirtualWrite = vdwh.pQueueVirtualWriteProc;			// grab pointer to function to use to send data to the host

    // Do extra initialization to determine if we are talking to an HPC client

    wdsi.WdInformationClass = WdVirtualWriteHookEx;
    wdsi.pWdInformation = &vdwhex;
    wdsi.WdInformationLength = sizeof(VDWRITEHOOKEX);
    vdwhex.usVersion = HPC_VD_API_VERSION_LEGACY;				// Set version to 0; older clients will do nothing
    rc = VdCallWd(pVd, WDxQUERYINFORMATION, &wdsi, &uiSize);
    TRACE((TC_CDM, TT_API2, "VDPING: WriteHookEx Version=%u p=%lx rc=%d", vdwhex.usVersion, vdwhex.pSendDataProc, rc));
    if(CLIENT_STATUS_SUCCESS != rc)
	{
        TRACE((TC_CDM, TT_API1, "VDPING: WD WriteHookEx failed. rc %d", rc));
        return(rc);
    }
    g_fIsHpc = (HPC_VD_API_VERSION_LEGACY != vdwhex.usVersion);	// if version returned, this is HPC or later
    g_pSendData = vdwhex.pSendDataProc;         // save HPC SendData API address
   
    // If it is an HPC client, tell it the highest version of the HPC API we support.

    if(g_fIsHpc)
    {
       WDSET_HPC_PROPERITES hpcProperties;

       hpcProperties.usVersion = HPC_VD_API_VERSION_V1;
       hpcProperties.pWdData = g_pWd;
       hpcProperties.ulVdOptions = HPC_VD_OPTIONS_NO_POLLING;
       wdsi.WdInformationClass = WdHpcProperties;
       wdsi.pWdInformation = &hpcProperties;
       wdsi.WdInformationLength = sizeof(WDSET_HPC_PROPERITES);
       TRACE((TC_CDM, TT_API2, "VDPING: WdSet_HPC_Properties Version=%u ulVdOptions=%lx g_pWd=%lx.", hpcProperties.usVersion, hpcProperties.ulVdOptions, hpcProperties.pWdData));
       rc = VdCallWd(pVd, WDxSETINFORMATION, &wdsi, &uiSize);
       if(CLIENT_STATUS_SUCCESS != rc)
       {
           TRACE((TC_CDM, TT_API1, "VDPING: WdSet_HPC_Properties failed. rc %d", rc));
           return(rc);
       }
    }

    TRACE((TC_VD, TT_API1, "VDPING: Registered"))

    // ALL MEMORY MUST BE ALLOCATED DURING INITIALIZATION.
    // Allocate a single buffer to respond to the mix request.
    // This example shows use of the MaximumWriteSize returned via
    // the previous call.
    // Subtract one because the first byte is used internally by the
    // WD for the channel number.  We are given a pointer to the
    // second byte.

    g_usMaxDataSize = vdwh.MaximumWriteSize - 1;

    // This is the main buffer which is going to be exchanged
    // between the client and the server. We allocate memory
    // of size g_usMaxDataSize rather than sizeof(MIXHEAD)
    // as the same buffer is used to send back additional data
    // to the server if required.

    if(NULL == (g_pPing = (PPING)malloc(g_usMaxDataSize)))
	{
        return(CLIENT_ERROR_NO_MEMORY);
    }

	// Get tunable parameters from the Ping section of the MODULE.INI file.

    g_usPingCount = (USHORT)miGetPrivateProfileInt(INI_PING_VDSECTION, INI_PING_PINGCOUNT, DEF_PING_PINGCOUNT);


    fl = fopen("c:/temp/ChannelStatus.txt", "w");

    if (fl == NULL) {
        return EXIT_FAILURE;
    }

    fprintf(fl, "Hello, World!\n");

    //hThread = CreateThread(
    //    NULL,                   // Default security attributes
    //    0,                      // Default stack size
    //    BackgroundThread,       // Thread function
    //    NULL,                   // Argument to thread function
    //    0,                      // Default creation flags
    //    &threadId               // Returns the thread identifier
    //);

    //if (hThread == NULL) {
    //    return 1;
    //}

    wchar_t* resKey = GetRegKeyTemplate();
    fwprintf(fl, L"Final Value: %ls\n", resKey);
    fflush(fl);

    return(CLIENT_STATUS_SUCCESS);
}

static wchar_t* GetRegKeyTemplate()
{
    DWORD dataType;
    DWORD dataSize = 0;
    LPDWORD  nullptr;
    wchar_t* data = NULL;
    LPCWSTR subKey = L"SOFTWARE\\TecUnify";
    HKEY hKey;

    LSTATUS keyRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        subKey,
        0,
        KEY_READ | KEY_WOW64_64KEY | KEY_WRITE,
        &hKey);

    if (keyRet == ERROR_SUCCESS) {
	    LPCWSTR valueName = L"XFER";
	    LONG result = RegQueryValueExW(hKey, valueName, NULL, &dataType, NULL, &dataSize);

        data = (wchar_t*)malloc(dataSize);

        result = RegQueryValueExW(hKey, valueName, NULL, &dataType, (LPBYTE)data, &dataSize);

        if (result == ERROR_SUCCESS)
        {
	        LPCWSTR valueData = "Hello";

            result = RegSetValueExW(
                hKey,               // Handle to the key
                valueName,          // Name of the value
                0,                  // Reserved (must be 0)
                REG_SZ,             // Type of the value (string)
                (const BYTE *)valueData, // Data to set
                (DWORD)(wcslen(valueData) + 1)); // Size of the data (including null terminator)

        	fprintf(fl, "Result: %ld\n", result);
        }
        else {
            fprintf(fl, "Error: Failed to query registry value. Error code: %ld\n", result);
        }
        // Close the registry key
        RegCloseKey(hKey);
    }
    else {
        fprintf(fl, "Failed to open registry key. %li\n", keyRet);
    }

    fflush(fl);
    return data;
}



/*******************************************************************************
 *
 *  ICADataArrival
 *
 *   A data PDU arrived over our channel.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *
 *    uChan (input)
 *       ICA channel the data is for.
 *
 *    pBuf (input)
 *       Buffer with arriving data packet
 *
 *    Length (input)
 *       Length of the data packet
 *
 * EXIT:
 *       void
 *
 ******************************************************************************/

static void WFCAPI ICADataArrival(PVOID pVd, USHORT uChan, LPBYTE pBuf, USHORT Length)
{
    WIRE_PTR(PING, pPacket);
    WIRE_READ(PING, pPacket, pBuf);
    fflush(fl);

    TRACE((TC_VD, TT_API3, "VDPING: IcaDataArrival, Len=%d, g_uLen=%d, pData=[%x][%x][%x][%x]",
					Length, pPacket->uLen, ((LPBYTE)pPacket)[0], 
        ((LPBYTE)pPacket)[1], ((LPBYTE)pPacket)[2], ((LPBYTE)pPacket)[3]))
    TRACEBUF((TC_VD, TT_API3, pPacket, 20))

    // This protocol is completely synchronous - host should not send
    // another message with a pending response.

    if(!g_fBufferEmpty)
	{
        TRACE((TC_VD, TT_ERROR, "VDPING: ICADataArrival - Error: not all data was sent"))
        return;
    }

	// Fill in response: echo back everything that server sent us, to a point

    if(pPacket->uLen > g_usMaxDataSize)
	{
        g_pPing->uLen = g_usMaxDataSize;
    }
    else
	{
        g_pPing->uLen = pPacket->uLen;
    }


    DWORD val;
    DWORD dataSize = sizeof(val);
    LPDWORD  nullptr;
    //fprintf(fl, "Get ready for Key!\n");
    //HKEY hKey;

    //LSTATUS keyRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
    //    "SOFTWARE\\TecUnify",
    //    0,
    //    KEY_ALL_ACCESS,
    //    &hKey);

    //fprintf(fl, "Get ready for Key too! %li\n", keyRet);
    //fflush(fl);
    //if (keyRet == ERROR_SUCCESS) {
    //    if (ERROR_SUCCESS == RegGetValueA(HKEY_LOCAL_MACHINE,
    //        "SOFTWARE\\TecUnify",
    //        "XFER",
    //        0,
    //        NULL,
    //        &val,
    //        &dataSize)) {
    //        fprintf(fl, "Value is %i\n", val);
    //        // no CloseKey needed because it is a predefined registry key
    //    }
    //    else {
    //        fprintf(fl, "Error reading.\n");
    //    }
    //    RegCloseKey(hKey);
    //}
    //else {
    //    fprintf(fl, "Failed to open registry key. %li\n", keyRet);
    //}

    //fflush(fl);

    memcpy_s(g_pPing->str,
        sizeof(g_pPing->str),
        "Joseph Data",
        strlen("Joseph Data") + 1);

    //memcpy(g_pPing, pPacket, g_pPing->uLen); (unsafe)
    memcpy_s(g_pPing, g_usMaxDataSize, pPacket, g_pPing->uLen);
    g_pPing->ulClientMS = (ULONG) GetTickCount();
    g_fBufferEmpty = FALSE;
    TRACE((TC_VD, TT_API3, "VDPING: ICADataArrival - data queued up, len %d ",	g_pPing->uLen))
    TRACEBUF((TC_VD, TT_API3, g_pPing, g_pPing->uLen))

	// Save the length, because WIRE_WRITE byte-swaps it

    g_uLen = g_pPing->uLen;
    WIRE_WRITE(PING, g_pPing, g_pPing->uLen);

    // Set up the transmit memory buffer array

    g_MemorySections[0].pSection = (LPBYTE)g_pPing;		// The body of the data to be sent
    g_MemorySections[0].length = (USHORT)g_uLen;		// Its length

	// In the HPC case, drive the outbound data that we just put on the queue.  Note that the HPC version of
	// SendData can be executed at any time on any thread.

	if(g_fIsHpc)
	{
		_SendAvailableData();
	}

    return;
}

/*******************************************************************************
 *
 *  DriverPoll
 *
 *  The Winstation driver calls DriverPoll
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pVdPoll (input)
 *       pointer to the structure DLLPOLL or DLL_HPC_POLL
 *    puiSize (input)
 *       size of DLLPOOL structure.
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - OK so far.  We will be polled again.
 *    CLIENT_STATUS_NO_DATA - No more data to send.
 *    CLIENT_STATUS_ERROR_RETRY - Could not send the data to the WD's output queue - no space.
 *                                Hopefully, space will be available next time.
 *    Otherwise, a fatal error code is returned from lower level functions.
 *
 *    NOTE:  CLIENT_STATUS_NO_DATA signals the WD that it is OK to stop
 *           polling until another host to client packet comes through.
 *
 * REMARKS:
 *    If polling is enabled (pre-HPC client, or HPC client with polling enabled),
 *    this function is called regularly.  DriverPoll is always called at least once.
 *    Otherwise (HPC with polling disabled), DriverPoll is called only when requested
 *    via WDSET_REQUESTPOLL or SENDDATA_NOTIFY.
 *
 ******************************************************************************/

int DriverPoll(PVD pVd, PDLLPOLL pVdPoll, PUINT16 puiSize)
{
    int rc = CLIENT_STATUS_NO_DATA;
    PDLLPOLL pVdPollLegacy;             // legacy DLLPOLL structure pointer
    PDLL_HPC_POLL pVdPollHpc;           // DLL_HPC_POLL structure pointer
    static BOOL fFirstTimeDebug = TRUE;  /* Only print on first invocation */

    if(fFirstTimeDebug == TRUE)
	{
        TRACE((TC_VD, TT_API2, "VDPING: DriverPoll entered"));
    }

    // Trace the poll information.

	if(g_fIsHpc)
    {
        pVdPollHpc = (PDLL_HPC_POLL)pVdPoll;
        if(fFirstTimeDebug)
        {
            TRACE((TC_VD, TT_API2, "VDPING:DriverPoll. HPC Poll information: ulCurrentTime: %u, nFunction: %d, nSubFunction: %d, pUserData: %x.",
                            pVdPollHpc->ulCurrentTime, pVdPollHpc->nFunction, pVdPollHpc->nSubFunction, pVdPollHpc->pUserData));
        }
    }
    else
    {
        pVdPollLegacy = (PDLLPOLL)pVdPoll;
        if(fFirstTimeDebug)
        {
            TRACE((TC_VD, TT_API2, "VDPING:DriverPoll. Legacy Poll information: ulCurrentTime: %u.", pVdPollLegacy->CurrentTime));
        }
    }
    fFirstTimeDebug = FALSE;                            // No more initial tracing

    // Check for something to write

    if(g_fBufferEmpty)
    {
        rc = CLIENT_STATUS_NO_DATA;
        goto Exit;			
    }

	// Data is available to write.  Send it.  Check for new HPC write API.

    TRACE((TC_VD, TT_API2, "VDPING: Buffer to Write, type=%u", g_pPing->uType));
	if(g_fIsHpc)
	{
        rc = _SendAvailableData();                      // send data via HPC client
	}
	else
	{
		// Use the legacy QueueVirtualWrite interface
		// Note that the FLUSH_IMMEDIATELY control will attempt to put the data onto the wire immediately,
		// causing any existing equal or higher priority data in the queue to be flushed as well.
		// This may result in the use of very small wire packets.  Using the value !FLUSH_IMMEDIATELY
		// may result in the data being delayed for a short while (up to 50 ms?) so it can possibly be combined
		// with other subsequent data to result in fewer and larger packets.
		
		rc = g_pQueueVirtualWrite(g_pWd, g_usVirtualChannelNum, g_MemorySections, NUMBER_OF_MEMORY_SECTIONS, FLUSH_IMMEDIATELY);

		// Normal status returns are CLIENT_STATUS_SUCCESS (it worked) or CLIENT_ERROR_NO_OUTBUF (no room in output queue)

        if(CLIENT_STATUS_SUCCESS == rc)
        {
            TRACE((TC_VD, TT_API2, "VDPING:DriverPoll. g_fBufferEmpty made TRUE"));
            g_fBufferEmpty = TRUE;
        }
        else if(CLIENT_ERROR_NO_OUTBUF == rc)
        {
            rc = CLIENT_STATUS_ERROR_RETRY;            // Try again later
        }
	}
Exit:
    return(rc);
}

/*******************************************************************************
 *
 *  DriverClose
 *
 *  The user interface calls VdClose to close a Vd before unloading it.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to procotol driver data structure
 *    pVdClose (input/output)
 *       pointer to the structure DLLCLOSE
 *    puiSize (input)
 *       size of DLLCLOSE structure.
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *
 ******************************************************************************/

int DriverClose(PVD pVd, PDLLCLOSE pVdClose, PUINT16 puiSize)
{
    TRACE((TC_VD, TT_API1, "VDPING: DriverClose entered"))

	fprintf(fl, "End of Transmission!\n");
    fclose(fl);

	if(NULL != g_pPing)
	{
		free(g_pPing);
		g_pPing = NULL;
	}

	CloseHandle(hThread);
    return(CLIENT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 *  DriverInfo
 *
 *    This routine is called to get module information
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pVdInfo (output)
 *       pointer to the structure DLLINFO
 *    puiSize (output)
 *       size of DLLINFO structure
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *
 ******************************************************************************/

int DriverInfo(PVD pVd, PDLLINFO pVdInfo, PUINT16 puiSize)
{
    USHORT ByteCount;
    PVDPING_C2H pVdData;
    PMODULE_C2H pHeader;
    PVDFLOW pFlow;

    ByteCount = sizeof(VDPING_C2H);

    *puiSize = sizeof(DLLINFO);

    TRACE((TC_VD, TT_API1, "VDPING: DriverInfo entered"));

    // Check if buffer is big enough
    // If not, the caller is probably trying to determine the required
    // buffer size, so return it in ByteCount.

    if(pVdInfo->ByteCount < ByteCount)
	{
        pVdInfo->ByteCount = ByteCount;
        return(CLIENT_ERROR_BUFFER_TOO_SMALL);
    }

    // Initialize default data

    pVdInfo->ByteCount = ByteCount;
    pVdData = (PVDPING_C2H) pVdInfo->pBuffer;

    // Initialize module header

    pHeader = &pVdData->Header.Header;
    pHeader->ByteCount = ByteCount;
    pHeader->ModuleClass = Module_VirtualDriver;

    pHeader->VersionL = CTXPING_VER_LO;
    pHeader->VersionH = CTXPING_VER_HI;

    //strcpy((char*)(pHeader->HostModuleName), "ICA"); // max 8 characters (unsafe)
    strcpy_s((char*)(pHeader->HostModuleName), sizeof(pHeader->HostModuleName), "ICA"); // max 8 characters

    // Initialize virtual driver header

    pFlow = &pVdData->Header.Flow;
    pFlow->BandwidthQuota = 0;
    pFlow->Flow = VirtualFlow_None;

    // add our own data

    pVdData->usMaxDataSize = g_usMaxDataSize;
    pVdData->usPingCount = g_usPingCount;

    pVdInfo->ByteCount = WIRE_WRITE(VDPING_C2H, pVdData, ByteCount);

    return(CLIENT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 *  DriverQueryInformation
 *
 *   Required vd function.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pVdQueryInformation (input/output)
 *       pointer to the structure VDQUERYINFORMATION
 *    puiSize (output)
 *       size of VDQUERYINFORMATION structure
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *
 ******************************************************************************/

int DriverQueryInformation(PVD pVd, PVDQUERYINFORMATION pVdQueryInformation, PUINT16 puiSize)
{
    TRACE((TC_VD, TT_API1, "VDPING: DriverQueryInformation entered"));
    TRACE((TC_VD, TT_API2, "pVdQueryInformation->VdInformationClass = %d",	pVdQueryInformation->VdInformationClass));

    *puiSize = sizeof(VDQUERYINFORMATION);

    return(CLIENT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 *  DriverSetInformation
 *
 *   Required vd function.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pVdSetInformation (input/output)
 *       pointer to the structure VDSETINFORMATION
 *    puiSize (input)
 *       size of VDSETINFORMATION structure
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *
 ******************************************************************************/

int DriverSetInformation(PVD pVd, PVDSETINFORMATION pVdSetInformation, PUINT16 puiSize)
{
    TRACE((TC_VD, TT_API1, "VDPING: DriverSetInformation entered"));
    TRACE((TC_VD, TT_API2, "pVdSetInformation->VdInformationClass = %d", pVdSetInformation->VdInformationClass));

   return(CLIENT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 *  DriverGetLastError
 *
 *   Queries error data.
 *   Required vd function.
 *
 * ENTRY:
 *    pVd (input)
 *       pointer to virtual driver data structure
 *    pLastError (output)
 *       pointer to the error structure to return (message is currently always
 *       NULL)
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - no error
 *
 ******************************************************************************/

int DriverGetLastError(PVD pVd, PVDLASTERROR pLastError)
{
    TRACE((TC_VD, TT_API1, "VDPING: DriverGetLastError entered"));

    // Interpret last error and pass back code and string data

    pLastError->Error = pVd->LastError;
    pLastError->Message[0] = '\0';
    return(CLIENT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 *  int _SendAvailableData(void)
 *
 *  Send any available data to the server on the channel.
 *
 * ENTRY: Nothing
 *
 * EXIT:
 *    CLIENT_STATUS_SUCCESS - OK.
 *    CLIENT_STATUS_NO_DATA - No more data to send.
 *    CLIENT_STATUS_ERROR_RETRY - Could not send the data to the WD's output queue - no space.
 *                                Hopefully, space will be available next time.
 *    Otherwise, a fatal error code is returned from lower level functions.
 *
 * REMARKS:
 *    This function should be called only with the HPC client.
 *
 ******************************************************************************/

int _SendAvailableData(void)
{
    int rc = CLIENT_STATUS_NO_DATA;

    // Check for something to write

    if(g_fBufferEmpty)
    {
        rc = CLIENT_STATUS_NO_DATA;
		return(rc);
    }

    // HPC does not support scatter write.  If there are multiple buffers to write
    // to a single packet, you must consolidate the buffers into a single buffer to 
    // send the data to the engine.

    if(NUMBER_OF_MEMORY_SECTIONS == 1)
    {
        // Send an ICA packet.  Parameters:
        //    g_pWd - A pointer via the WDxSETINFORMATION engine call in DriverOpen().
        //    g_MemorySections[0].pSection - The address of the data to send.
        //    g_MemorySections[0].length - The size of the data to send.
        //    &g_ulUserData - An arbitrary pointer to user data.  This pointer will be returned with a
        //                  notification poll.  In this case, the pointer just points to arbitrary data
        //                  (0xCAACCAAC).
        //    SENDDATA_NOTIFY - Flags.  See vdapi.h:SENDDATA*.

        rc = g_pSendData((DWORD)g_pWd, g_usVirtualChannelNum, g_MemorySections[0].pSection,
            g_MemorySections[0].length, &g_ulUserData, SENDDATA_NOTIFY);
    }
    else
    {
        // Consolidate the buffers into a single buffer.

        LPBYTE pba = g_pbaConsolidationBuffer;              // target buffer
        INT nIndex;
        USHORT usTotalLength = 0;                           // total length to send
        USHORT usSizeOfBufferLeft = SIZEOF_CONSOLIDATION_BUFFER;  // available buffer left to fill

        for(nIndex = 0; nIndex < NUMBER_OF_MEMORY_SECTIONS; ++nIndex)
        {
            memcpy_s(pba, usSizeOfBufferLeft, g_MemorySections[nIndex].pSection, g_MemorySections[nIndex].length);
            pba += g_MemorySections[nIndex].length;           // bump target pointer for next memory section
            usSizeOfBufferLeft -= g_MemorySections[nIndex].length;  // reduce available buffer
            usTotalLength += g_MemorySections[nIndex].length; // accumulate total length sent
        }

        // Now send the single buffer.

        rc = g_pSendData((DWORD)g_pWd, g_usVirtualChannelNum, g_pbaConsolidationBuffer, usTotalLength, &g_ulUserData, SENDDATA_NOTIFY);
    }

    // Normal status returns are CLIENT_STATUS_SUCCESS (it worked) or CLIENT_ERROR_NO_OUTBUF (no room in output queue)

    if(CLIENT_STATUS_SUCCESS == rc)
    {
        TRACE((TC_VD, TT_API2, "VDPING:_SendAvailableData. g_fBufferEmpty made TRUE"))
        g_fBufferEmpty = TRUE;
    }
    else if(CLIENT_ERROR_NO_OUTBUF == rc)
    {
        rc =  CLIENT_STATUS_ERROR_RETRY;                        // Try again later
    }
    else if(CLIENT_STATUS_NO_DATA == rc)
    {
        // There was nothing to do.  Just fall through.
    }
    else
    {
		// We may be waiting on a callback.  This would be indicated by a 
        // CLIENT_ERROR_BUFFER_STILL_BUSY return when we tried
		// to send data.  Just return CLIENT_STATUS_ERROR_RETRY in this case.

		if(CLIENT_ERROR_BUFFER_STILL_BUSY == rc)
		{
			rc =  CLIENT_STATUS_ERROR_RETRY;                        // Try again later
		}
    }
    return(rc);
}

////================================================
//DWORD WINAPI BackgroundThread(LPVOID lpParam) {
//	while (1)
//    {
//        time_t now = time(NULL);
//        struct tm* local_time = localtime(&now);
//        char timestamp[100];
//        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
//        fprintf(fl, "Status Report!: %s\n", timestamp);
//        fflush(fl);
//        Sleep(1500);
//    }
//    return 0;
//}
