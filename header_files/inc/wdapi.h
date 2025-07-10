/* // begin_binsdk */
/**********************************************************************
*
*  Copyright (C) Citrix Systems, Inc. All Rights Reserved.
*
*  WDAPI.H
*
*  This module contains external winstation driver API
*  defines and structures
*
*  $Id$
*
***********************************************************************/

#ifndef __WDAPI_H__
#define __WDAPI_H__

#pragma once

#if defined(PLATFORM_PRAGMAS) && defined(MKDBG)
#if defined(__DLLAPI_H__) && defined(_MINIDLL_H_)
#pragma MKDBG("DLLAPI and MINIDLL are included")
#elif defined(_MINIDLL_H_)
#pragma MKDBG("MINIDLL is included")
#elif defined(__DLLAPI_H__)
#pragma MKDBG("DLLAPI is included")
#endif
#endif /* PLATFORM_PRAGMAS && MKDBG */

#include <dllapi.h>
#include <vrtlclss.h>
#include <engtypes.h>
#include <ica.h>
#include <Api_HpcCallTable.h>
/* // end_binsdk */

/* for PGRAPH_RECT */
#include <ctxgraph.h>

/* // begin_binsdk */


/*
 * ICA Transport Reliability enum
 */
typedef enum _ICA_TRANSPORT_RELIABILITY_
{
    IcaTransportReliable = 0,           // Reliable Transport
    IcaTransportUnreliable,             // Unreliable Transport
    IcaTransportReliableBasicFec,       // Reliabile with Basic FEC Transport
    /* Reserved for future use
     IcaTransportUnreliableBasicFec,
     IcaTransportReliableAdvFec,
     IcaTransportUnreliableAdvFec,
     */
    IcaTransportCount                   // Must always be last
} ICA_TRANSPORT_RELIABILITY, *PICA_TRANSPORT_RELIABILITY;


/*
 *  Index into WD procedure array
 */

#define WDxQUERYINFORMATION      6
#define WDxSETINFORMATION        7
#define WDxCOUNT                 8  /* number of external wd procedures */

/*
 *  WdInformationClass enum
 * 
 *  IMPORTANT NOTE:   DO NOT REORDER OR DELETE ENTRIES FROM THIS ENUM STRUCTURE.   
 *                    ALWAYS ADD TO THE END OF THE LIST.  
 *                    Customers use this header file and we want to maintain forward compatibility.
 */
typedef enum _WDINFOCLASS {
    WdClientData,
    WdStatistics,
    WdLastError,
    WdConnect,
    WdDisconnect,
    WdKillFocus,
    WdSetFocus,
    WdEnablePassThrough,
    WdDisablePassThrough,
    WdVdAddress,
	WdVirtualWriteHook,	//10
    WdAddReadHook,
    WdRemoveReadHook,
    WdAddWriteHook,
    WdRemoveWriteHook,
    WdModemStatus,
    WdXferBufferSize,     /* Get: Size of host and client buffers */
    WdCharCode,
    WdScanCode,
    WdMouseInfo,
	WdInitWindow,		// 20
    WdDestroyWindow,
    WdRedraw,             /* Tell the host to redraw */
    WdThinwireStack,      /* Pass the thinwire stack to the thinwire vd */
    WdHostVersion,        /* get - Host Version information*/
    WdRealizePaletteFG,   /* inform client to realize it's foreground palette */
    WdRealizePaletteBG,   /* inform client to realize it's background palette */
    WdInactivate,         /* client is about to lose input focus */
    WdSetProductID,
    WdGetTerminateInfo,   /* test for legitimate terminate */
	WdRaiseSoftkey,       /* raise the soft keyboard */ // 30
    WdLowerSoftkey,       /* lower the soft keyboard */
    WdIOStatus,           /* IO status */
    WdOpenVirtualChannel, /* get - Open a virtual channel */
    WdCache,              /* persistent caching command set */
    WdGetInfoData,        /* Information from host to client */
    WdWindowSwitch,       /* window has switched back, check keyboard state */
    WdUnicodeCode,        /* window has sent unicode information to wd */
#ifdef PACKET_KEYSYM_SUPPORT
    WdKeysymCode,         /* window has sent keysym information to wd */
#endif
    WdSetNetworkEvent,    /* This is for engine to pass a event handle down to TD if WinSock2 is suported. */
	WdPassThruLogoff,       // 40
    WdClientIdInfo,              /* Get the client identification information */
    WdPartialDisconnect,         /* A PN Refresh connection will disconnect */
    WdDesktopInfo,               /* Get/Set: information about the remote dekstop */
    WdSeamlessHostCommand,       /* Set: Seamless Host Agent command */
    WdSeamlessQueryInformation,  /* Get: Seamless information */
    WdZlRegisterUnicodeHook,     /* Set: Zero Latency Unicode hook registration*/
    WdZLUnRegisterUnicodeHook,   /* Set: Zero Latency Unicode hook unregistration*/
    WdZLRegisterScanCodeHook,    /* Set: Zero Latency scan code hook registration*/
    WdZlUnregisterScanCodeHook,  /* Set: Zero Latency scan code hook unregistration*/
	WdIcmQueryStatus,            /* Get: Ica Channel Monitoring status */ // 50
    WdIcmSendPingRequest,        /* Set: Send ping request and call callback if specified */
    WdSetCursor,                 /* Set: New cursor from TW. Data - PHGCURSOR */
    WdFullScreenMode,            /* Get: Return TRUE if full screen text is enabled */
    WdFullScreenPaint,           /* Set: Full Screen Mode needs redrawn*/
    WdSeamlessInfo,              /* Get: Seamless information/capabilities from WD */
    WdCodePage,                  /* Get: Retrieve server/client-default codepage */
    WdIcaControlCommand,         /* Set: ICA Control Command */
    WdReconnectInfo,             /* Get: Information needed for auto reconnect */
    WdServerSupportBWControl4Printing, /* return TRUE if server suports printer bandwidth control*/
	WdVirtualChannel,            /* Get: Virtual channel information */  // 60
    WdGetLatencyInformation,     /* Get: Latency information */
    WdKeyboardInput,             /* Get/Set: Enable/Disable keyboard input */
    WdMouseInput,                /* Get/Set: Enable/Disable mouse input */
    WdCredentialPassing,         /* Set: Passing Credentials through WD */
    WdRenderingMode,             /* Set: Virtual channel the rendering mode (Headless) */
    WdPauseResume,               /* Get/Set: Pause/Resume virtual channels */
    WdQueryMMWindowInfo,         /* Get: Query the information for the MMVD */
    WdICOSeamlessFunctions,      /* Get/Set: ICO command */
    WdEUKSVersion,               /* Get: Get Server EUKS version. */
	WdSetC2HPriority,            /* Set: Set the virtual channel pritory from client to server */ // 70
                                 /* IMPORTANT: YOU CAN ONLY SET THE PRIOROTY BEFORE YOU HAVE */
                                 /* SENT ANY PROTOCOL FOR THE VC */
    WdPnP,                       /* Set: Send CLPNP_COMMAND commands inbetween the control VC and the */
                                 /*      implmentation VC (such as VIRTUAL_CCM) */
    /* Support for EUEM (Darwin Release For Ohio). This code has been added or
     * changed to support End User Experience Monitoring functionality. Please do
     * not change the code in this section without consulting the EUEM team
     *  Email (at the time of change) "#ENG - Darwin Dev".
     */
    WdEuemEndSLCD,               /* Set endSLCD */
    WdEuemStartupTimes,          /* Get/Set. Set startup times from wfcrun32. Get startup times from VD EUEM  */
    WdEuemTwCallback,            /* Set: register the EUEM ICA roundtrip callback */
                                 /*      function from the thinwire VC to the EUEM VC */
    WdSessionIsReconnected,      /* Get. Get indication if the session was auto reconnected using a token from VD EUEM  */
	WdUserActivity,              /* Get/Set. Get indication if there has been user activity in the session from VD EUEM  */

    /* End of EUEM support section */
	WdLicensedFeatures,           /* Get: Get Licensed Features */
	WdResizeHotBitmapCache,       /* Set: Tell thinwire driver to resize the hot bitmap cache */
	WdLockDisplay,                /* The Server has had its screen locked */
	WdCodePageUTF8Support,        /* Get: CodePage 65001 (UTF-8) support status */  // 80
	WdCreateChannelComms,         /* Get: Provide the driver with a pipe to communicate directly with. */

	WdGetICAWindowInfo,                 /* Get: Get info about the ICA window */
	WdGetClientWindowFromServerWindow,  /* Get: Get the client window corresponding to the given server window */
	WdRegisterWindowChangeCallback,     /* Get: Register a callback for when the ICA window changes */
	WdUnregisterWindowChangeCallback,   /* Get: Unregister the ICA window change callback */

    //added for 64 VC support
	WdQueryVCNumbersForVD,				// Get: a list of virtual channels numbers that were registered since 
	// last call to WdQueryVCNumbersForVD
	WdVirtualWriteHookEx,				// Added 6/19/2009 by LHR for HPC aware channels
	WdInternalFunction,					// Used mostly for debug purposes, params specific to version under test
	WdSendVirtualAck,					// Send virtual ack ICA packet for a given channel
	WdNotifyEuemOfReconnect,			// Send a "set information" to EUEM that a reconnect has occurred (sent by CGP) // 90
	
	WdSecondAppStarts,					// Set: send information to usb vd that a second app from a server has started. Used in case of prelaunch.
	
	WdRequestPoll,						// VD is requesting to be polled to send data (HPC API Version >= 1)
	WdHpcProperties,					// VD setting/getting info about what HPC features it wants to use
	WdAddressInfo,						// Get: retrieve resolved address info (used by VDSSPI)
	WdSetDefaultDisplayVc,				// Set: sets/resets the caller/thinwire to be the default display VC 
    WdQueryHostWindowId,                // Get Host Window ID
    WdRtpSetup,                         // Set: mechanism for sending RTP_SETUP commands.
	WdSeamlessFocusInfo,                // Set: send keyboard focus state of seamless application to Generic USB virtual channel
	WdQueryUdpGatewayAddress,           // Get address of UDP Gateway for tunneling media over UDP.
	WDCheckOutTicket,                   // Check out ticket to authenticate to UDP Gateway // 100
	WDCheckInTicket,                     // Check in new ticket received from UDP Gateway
	WdSendMTCommand,
	WdQueryMTInfo,
	WdSetInputVc,                        // Set: sets the vc for input handling 
	WdSeamlessIMEHotKeySet,
	WdSeamlessSetIMEInformation,
	WdSeamlessQueryIMEInformation,
	WdReceiverAnalytics,
	WdTranslateOverlayWindow,
    WdVirtualWriteHookQos,
    WdQueryEdt,
    WdQueryMaxUnreliablePayload,
    WdQueryEdtStats,
    WdSendToServer,
    WdSetLossTolerant,
    WdDndSetEnabled,
    WdSetShouldTrackMousePosForDnd,
    WdSetDndShouldIgnoreSimulatedMouseEvents,
    WdAppPVCInfo,
    WdEuemLogonInfo,          /* Get/Set. Set logon info from wfcrun32. Get logon info from VD EUEM  */
/* // end_binsdk */
/*
    Please ensure that when you update this enumeration,
    you also update the corresponding version in
	.../SrcVS/Inc/Apis/Api_Wd.h
*/
/* // begin_binsdk */
} WDINFOCLASS;

/*
 *  WdQueryInformation structure
 */
typedef struct _WDQUERYINFORMATION {
    WDINFOCLASS WdInformationClass;
    LPVOID pWdInformation;
    USHORT WdInformationLength;
    USHORT WdReturnLength;
} WDQUERYINFORMATION, * PWDQUERYINFORMATION;

/*
 *  WdSetInformation structure
 */
typedef struct _WDSETINFORMATION {
    WDINFOCLASS WdInformationClass;
    LPVOID pWdInformation;
    USHORT WdInformationLength;
} WDSETINFORMATION, * PWDSETINFORMATION;

// WDSET_HPC_PROPERITES: The VD passes this structure to Wd_Set(WDAPI_USEHPCAPIVERSION) 
// The VD is informing the engine of the HPC VD API version that the VD will use. 
// Possible return codes from the Wd_Set/Get(WDAPI_USEHPCAPIVERSION): 
//    o CLIENT_STATUS_SUCCESS:  OK 
//    o CLIENT_ERROR_INVALID_PARAMETER: Invalid parameter. 
//    o CLIENT_ERROR_INVALID_CHANNEL: The pWdData structure passed is invalid. 
//    o CLIENT_ERROR_VD_NOT_FOUND: The primary thread for the channel was not found. 

typedef struct _WDSET_HPC_PROPERITES 
{ 
	LPVOID    pWdData;            // pointer to the WdData structure provided by the engine. 
	ULONG    ulVdOptions;        // HPC options pertaining to VD 
	USHORT    usVersion;            // the HPC VD API version that will be used. 
} WDSET_HPC_PROPERITES, *PWDSET_HPC_PROPERITES; 

// WDSET_REQUESTPOLL: The VD passes this structure to Wd_Set(WDAPI_REQUESTPOLL)
// The VD is asking to be polled.
// Possible return codes from the Wd_Set(WDAPI_REQUESTPOLL):
//	o CLIENT_STATUS_SUCCESS:  OK
//	o CLIENT_ERROR_QUEUE_FULL: A request to poll is already queued, either from a
//							prior Wd_Set(WDAPI_REQUESTPOLL), or from a call to
//							Hpc_Comm_ApiSendData with the SENDDATA_NOTIFY flag set that
//							returned with CLIENT_ERROR_NO_OUTBUF.
//	o CLIENT_ERROR_INVALID_PARAMETER: Invalid parameter.
//	o CLIENT_ERROR_INVALID_CHANNEL: The pWdData structure passed is invalid.
//	o CLIENT_ERROR_VD_NOT_FOUND: The primary thread for the channel was not found.
//  o CLIENT_ERROR_VD_STILL_BUSY: A previous requested poll has not yet been delivered.
//  o CLIENT_ERROR_VD_POLLING_NOT_READY: The engine is not yet ready to begin polling.

typedef struct _WDSET_REQUESTPOLL
{
	LPVOID	pWdData;			// pointer to the WdData structure provided by the engine.
	LPVOID pUserData;			// a pointer to arbitrary user that will be returned to the
	// VD via DriverPoll(HPCPOLL_FUNCTION_REQUESTEDPOLL)
} WDSET_REQUESTPOLL, *PWDSET_REQUESTPOLL;

typedef struct _WD_SENDTOSERVER
{
    ICA_TRANSPORT_RELIABILITY qos;
    PUINT8 pBuffer;
    UINT bufSize;
} WD_SENDTOSERVER;

/* // end_binsdk */
/*
 *  WdOpen structure
 */
typedef struct _WDOPEN {
    LPVOID pIniSection;         /* in: pointer to ini file section buffer */
    PDLLLINK pPdLink;           /* in: top most protocol driver */
    PDLLLINK pScriptLink;       /* in: pointer to scripting dll */
    PDLLLINK pDll;              /* in: pointer to a list of all loaded dlls */
    USHORT OutBufHeader;        /* in: number of header bytes to reserve */
    USHORT OutBufTrailer;       /* in: number of trailer bytes to reserve */
    USHORT OutBufParam;         /* in: number of parameter bytes to reserve */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(disable : 4214)
#endif
    USHORT fOutBufCopy: 1;      /* in: pd copies data into new outbuf */
    USHORT fOutBufFrame: 1;     /* in: framing protocol driver is loaded */
    USHORT fAsync: 1;           /* in: Async connection or not */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(default : 4214)
#endif
    USHORT MaxVirtualChannels;  /* out: maximum virtual channels supported */
    PLIBPROCEDURE pfnStatusMsgProc; /* in: status message procedure */
    HND hICAEng;                /* in: handle to ICA engine */
} WDOPEN, * PWDOPEN;

/*
 *  WdConnect structure
 */
typedef struct _WDCONNECT {
    USHORT NotUsed;
} WDCONNECT, * PWDCONNECT;

/*
 *  WdHostVersion structure
 */
typedef struct _HostVersion {
    UCHAR HostLevel;
} HOSTVERSION, * PHOSTVERSION;

typedef struct _WDCLIENTIDINFO {
    USHORT ProductId;
} WDCLIENTIDINFO, *PWDCLIENTIDINFO;

/*
 * WdHostBufferSize structure
 */
typedef struct _WDXFERBUFFERSIZE
{
    USHORT HostBufferSize;
    USHORT ClientBufferSize;
} WDXFERBUFFERSIZE, * PWDXFERBUFFERSIZE;


/*
 * WdVirtualChannel defines
 */
#define MAX_VC_PACKET_HEADER_SIZE   4               /* BYTE ICACmd, BYTE Channel, WORD length */

#define WDVCINFO_VERSION_ONE        1
#define WDVCINFO_VERSION_CURRENT    WDVCINFO_VERSION_ONE

#define WDVCINFO_REQUEST_GET_CHANNEL_COUNT          1       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_CHANNEL_INFO_BY_INDEX  2       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_CHANNEL_NUMBER_BY_NAME 3       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_CHANNEL_NAME_BY_NUMBER 4       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_MAX_CHANNEL_COUNT      5       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_MAX_CHANNEL_WRITE      6       /* Ver 1 */
#define WDVCINFO_REQUEST_GET_MAX_CHANNEL_READ       7       /* Ver 1 */

/*
 * WdVirtualChannel structure
 */
typedef struct _WDVIRTUALCHANNELINFO
{
    ULONG        cbSize;
    ULONG        Version;
    ULONG        Request;                   /* Ver 1 - Request type         */
    VIRTUALNAME  ChannelName;               /* Ver 1 - Channel name         */
    USHORT       ChannelNumber;             /* Ver 1 - Channel number       */
    USHORT       usValue;                   /* Ver 1 - Input/output value   */
    ULONG        ulValue;                   /* Ver 1 - Input/output value   */
} WDVIRTUALCHANNELINFO, * PWDVIRTUALCHANNELINFO;

/*
 * WdGetLatencyInformation structure
 */
#define WDLATENCYINFO_VERSION_ONE        1
#define WDLATENCYINFO_VERSION_CURRENT    WDLATENCYINFO_VERSION_ONE

typedef struct _WDLATENCYINFO
{
    ULONG   cbSize;
    ULONG   Version;

    /*
     * Version 1
     */
    UINT32  LastLatency;
    UINT32  AverageLatency;
    INT32   RoundTripDeviation;

} WDLATENCYINFO, * PWDLATENCYINFO;


/*
 *  WdGetInfoData structure
 */
#define INFODATA_ID_SIZE 8
typedef struct _INFODATA {
    UCHAR Id[INFODATA_ID_SIZE];
    /* PUCHAR pData; */
} INFODATA, * PINFODATA;

/* // begin_binsdk */
/*
 *  WdOpenVirtualChannel structure
 */
typedef struct _OPENVIRTUALCHANNEL {
    LPVOID  pVCName;
    USHORT  Channel;
} OPENVIRTUALCHANNEL, * POPENVIRTUALCHANNEL;

/*
 * **** Added for 64 VC support ****
 * WdQueryVCNumbersForVD structure
 */
typedef struct _QUERYVCNUMBERSFORVD {
    USHORT VcCountForVD;
    USHORT VcNumbersForVD[VIRTUAL_MAXIMUM];
} QUERYVCNUMBERSFORVD, *PQUERYVCNUMBERSFORVD;

#define INVALID_VC_HANDLE 0xffff

/* // end_binsdk */

#include "keytypes.h"

/*
 *  WdMouseInfo structure
 */
typedef struct _MOUSEINFO {
    USHORT  cMouseData;
    LPVOID  pMouseData;
} MOUSEINFO, * PMOUSEINFO;


#define WDKEYBOARDINPUT_VERSION_ONE         1
#define WDKEYBOARDINPUT_VERSION_CURRENT     WDKEYBOARDINPUT_VERSION_ONE
/*
 *  WdKeyboardInput structure
 */
typedef struct _WDKEYBOARDINPUT 
{
    USHORT  cbSize;
    USHORT  Version;
    BOOL    bEnable;
} WDKEYBOARDINPUT, * PWDKEYBOARDINPUT;

#define WDMOUSEINPUT_VERSION_ONE         1
#define WDMOUSEINPUT_VERSION_CURRENT     WDMOUSEINPUT_VERSION_ONE
/*
 *  WdMouseInput structure
 */
typedef struct _WDMOUSEINPUT 
{
    USHORT  cbSize;
    USHORT  Version;
    BOOL    bEnable;
} WDMOUSEINPUT, * PWDMOUSEINPUT;

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_ENABLE
#endif

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#endif
/* // begin_binsdk */

/*
 *  Set Info Class enum
 */
typedef enum _SETINFOCLASS {
    CallbackComplete
} SETINFOCLASS, * PSETINFOCLASS;

/*
 *  Query Info Class enum
 */
typedef enum _QUERYINFOCLASS {
    QueryHostVersion,
    OpenVirtualChannel
} QUERYINFOCLASS, * PQUERYINFOCLASS;

/*
 *  Outbuf Buffer data structure
 */
typedef struct _OUTBUF {

    /*
     *  Non-inherited fields
     */
    struct _OUTBUF * pLink;      /* wd/td outbuf linked list */
    LPVOID pParam;               /* pointer to allocated parameter memory */
    LPBYTE pMemory;              /* pointer to allocated buffer memory */
    LPBYTE pBuffer;              /* pointer within buffer memory */
    USHORT MaxByteCount;         /* maximum buffer byte count (static) */
    USHORT ByteCount;            /* byte count pointed to by pBuffer */

    LPBYTE pSaveBuffer;          /* saved outbuf buffer pointer */
    USHORT SaveByteCount;        /* saved outbuf byte count */

    /*
     *  Inherited fields (when pd allocates new outbuf and copies the data)
     */
    ULONG StartTime;            /* pdreli - transmit time (used to measure roundtrip) */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(disable : 4214)
#endif
    USHORT fControl: 1;         /* pdreli - control buffer (ack/nak) */
    USHORT fRetransmit: 1;      /* pdreli - outbuf retransmit */
    USHORT fCompress: 1;        /* pdcomp - compress data */
#ifdef PLATFORM_PRAGMAS
    #pragma warning(default : 4214)
#endif
    BYTE Sequence;              /* pdreli - sequence number */
    BYTE Fragment;              /* pdreli - fragment number */

} OUTBUF, * POUTBUF;

/*
 * WdIcmQueryStatus structure
 */
typedef struct _WDICMQUERYSTATUSDATA
{
   BOOL    fHostAvailable;  /* Host can support ICM    */
   UINT8   Version;         /* Agreed protocol version */

   BOOL    fPingEnabled;    /* The client can send ping packets      */
   BOOL    fBMEnabled;      /* Background channel monitoring enabled */

   UINT16  uBMRepeatDelay;  /* Background channel monitoring repeat delay, sec. */

   UINT32  LastLatency;
   UINT32  AverageLatency;

} WDICMQUERYSTATUSDATA, *PWDICMQUERYSTATUSDATA;

/*
 * WDSEAMLESSINFO structure
 */
typedef struct _WDSEAMLESSINFO
{
   BOOL     fSeamless20Logon;      /* The server can support seamless 2.0 logon */
   BOOL     fSynchronousSeamless;  /* MF20 synchronous seamless */

   BOOL     fEnabled;              /* Seamless mode enabled for this session */
   BOOL     fPaused;               /* Host agent paused (simulated full screen */

} WDSEAMLESSINFO, *PWDSEAMLESSINFO;

/*
 * WdIcmSendPingRequest structure
 */
typedef struct _WDICMSENDPINGREQUEST
{
   PVOID    pfnCallback;        /* Callback function. Can be NULL */
   PVOID    Context;            /* Callback context. Can be NULL  */
   BOOL     fUrgent;            /* if TRUE then flush on the next poll */

} WDICMSENDPINGREQUEST, *PWDICMSENDPINGREQUEST;

/*
 *  WdReconnectInfo structure. This is now selectively enabled since it was updated to support IPV6.
 *  We have to include winsock2.h for the SOCKADDR_STORAGE type, however this breaks the build for
 *  a number of existing components. By having it disabled by default existing components will build
 *  correctly, while components that need it can enable it as required.
 */
#ifdef USE_IPV6_RECONNECTINFO

#include <winsock2.h>

typedef struct _WDRECONNECTINFO
{
	UINT32       ReconnectManagerId;
	BOOL         bAddressValid; // Name, ResolvedAddress, ClientName, and PortNum are valid
	ADDRESSW     ConnectionFriendlyNameW;
	ADDRESS      Name;
	SOCKADDR_STORAGE ResolvedAddress;
	ADDRESS      IntermediateAddress;
	CLIENTNAMEW  ClientNameW;
	USHORT       PortNum;
	ADDRESS      SSLGatewayName;
	USHORT       SSLGatewayPortNum;
	USHORT       CGPPortNum;
	USHORT       CGPBrPortNum;
	UINT16       cbCookie;	 // Input zero to query for cookie size
	PVOID        cookie[1];  // Placeholder - cookie begins here in the buffer.
	                         // This should always be the last field in this structure.
	                         // since cookies will overwrite any further fields.
} WDRECONNECTINFO, *PWDRECONNECTINFO;

#endif // USE_IPV6_RECONNECTINFO

typedef int  (PWFCAPI POUTBUFALLOCPROC)( LPVOID, POUTBUF * );
typedef void (PWFCAPI POUTBUFFREEPROC)( LPVOID, POUTBUF );
typedef int  (PWFCAPI PPROCESSINPUTPROC)( LPVOID, LPBYTE, USHORT );
typedef int  (PWFCAPI PSETINFOPROC)( LPVOID, SETINFOCLASS, LPBYTE, USHORT );
typedef void (PWFCAPI PIOHOOKPROC)( LPBYTE, USHORT );
/* // end_binsdk */
typedef int  (PWFCAPI PAPPENDICAPKTPROC)( LPVOID, USHORT, LPBYTE, USHORT );
/* // begin_binsdk */
typedef int  (PWFCAPI PQUERYINFOPROC)( LPVOID, QUERYINFOCLASS, LPBYTE, USHORT );
typedef int  (PWFCAPI POUTBUFRESERVEPROC)( LPVOID, USHORT );
typedef int  (PWFCAPI POUTBUFAPPENDPROC)( LPVOID, LPBYTE, USHORT );
typedef int  (PWFCAPI POUTBUFWRITEPROC)( LPVOID );
typedef int  (PWFCAPI PAPPENDVDHEADERPROC)( LPVOID, USHORT, USHORT );

#define FLUSH_IMMEDIATELY                   0

typedef struct _MEMORY_SECTION {
    UINT    length;
    LPBYTE  pSection;
} MEMORY_SECTION, far * LPMEMORY_SECTION;

typedef INT (PWFCAPI PQUEUEVIRTUALWRITEPROC) (LPVOID, USHORT, LPMEMORY_SECTION, USHORT, UINT32);
typedef INT (PWFCAPI PQUEUEVIRTUALWRITEPROCQOS) (LPVOID, USHORT, LPMEMORY_SECTION, USHORT, UINT32, ICA_TRANSPORT_RELIABILITY, UINT32 *);
typedef INT (PWFCAPI PSENDDATAPROC) (DWORD, USHORT usChannel, LPBYTE pData, USHORT usLen, LPVOID pUserData, UINT32 uiFlags);
typedef INT (PWFCAPI PSENDDATAPROCQOS) (DWORD, USHORT usChannel, LPBYTE pData, USHORT usLen, LPVOID pUserData, UINT32 uiFlags, ICA_TRANSPORT_RELIABILITY qos, UINT32 *Ticket);


/*=============================================================================
 ==   Virtual driver hook structures
 ============================================================================*/

/*
 *  Virtual driver write hook structure
 */
typedef void (PWFCAPI PVDWRITEPROCEDURE)( LPVOID, USHORT, LPBYTE, USHORT );
typedef void (PWFCAPI PVDWRITEPROCEDUREQOS)(LPVOID, USHORT, LPBYTE, USHORT, DWORD, PVOID);

/*
 *  Used when registering virtual write hook  (WdVirtualWriteHook)
 */
typedef struct _VDWRITEHOOK {
    USHORT Type;                             /* in: virtual channel id */
    LPVOID pVdData;                          /* in: pointer to virtual driver data */
    union {                                  /* in: pointer to vd write procedure (wd->vd) */
        PVDWRITEPROCEDURE pProc;
        PVDWRITEPROCEDUREQOS pProcQos;
    };
    LPVOID pWdData;                          /* out: pointer to wd structure */
    POUTBUFRESERVEPROC pOutBufReserveProc;   /* out: pointer to OutBufReserve */
    POUTBUFAPPENDPROC pOutBufAppendProc;     /* out: pointer to OutBufAppend */
    POUTBUFWRITEPROC pOutBufWriteProc;       /* out: pointer to OutBufWrite */
    PAPPENDVDHEADERPROC pAppendVdHeaderProc; /* out: pointer to AppendVdHeader */
    USHORT MaximumWriteSize;                 /* out: maximum ica write size */
    union {                                  /* out: pointer to QueueVirtualWrite */
        PQUEUEVIRTUALWRITEPROC pQueueVirtualWriteProc;
        PQUEUEVIRTUALWRITEPROCQOS pQueueVirtualWriteProcQos;
    };
} VDWRITEHOOK, * PVDWRITEHOOK;

// VDWRITEHOOKEX 
// 
// Used when communicating with HPC (client 13.0) or later 
// 

typedef struct _VDWRITEHOOKEX 
{ 
	USHORT usVersion;						// Version number of HPC client; filled in by WdQueryInformation call 
    union {                                 // Pointer to HPC SendData to server API
        PSENDDATAPROC pSendDataProc;            
        PSENDDATAPROCQOS pSendDataProcQos;
    };
	USHORT usExtraLength;					// length of data pointed to by pExtraData 
	LPVOID pExtraData;						// reserved for future expansion 
	HPC_CallTable* pHpcCallTable;			// Address of the HPC Call Table
} VDWRITEHOOKEX, * PVDWRITEHOOKEX; 

/* // end_binsdk */

/*
 *  Used as an internal winstation structure
 */
typedef struct _WDVDWRITEHOOK {
    LPVOID pData;                  /* pointer to virtual driver data */
    PVDWRITEPROCEDURE pProc;       /* pointer to virtual driver write procedure */
} WDVDWRITEHOOK, * PWDVDWRITEHOOK;

typedef struct _WDVDC2HPRIORITY {
    UINT32 VirtualChannelID;       /* pointer to virtual driver data */
    BYTE Priority;       /* pointer to virtual driver write procedure */
} WDVDC2HPRIORITY, * PWDVDC2HPRIORITY;

/*
 *  Used when registering virtual write hook  (WdVirtualWriteHook)
 */
typedef struct _VDWRITEHOOK20 {
    USHORT Type;                             /* in: virtual channel id */
    LPVOID pVdData;                          /* in: pointer to virtual driver data */
    PVDWRITEPROCEDURE pProc;                 /* in: pointer to vd write procedure (wd->vd) */
    LPVOID pWdData;                          /* out: pointer to wd structure */
    POUTBUFRESERVEPROC pOutBufReserveProc;   /* out: pointer to OutBufReserve */
    POUTBUFAPPENDPROC pOutBufAppendProc;     /* out: pointer to OutBufAppend */
    POUTBUFWRITEPROC pOutBufWriteProc;       /* out: pointer to OutBufWrite */
    PAPPENDVDHEADERPROC pAppendVdHeaderProc; /* out: pointer to AppendVdHeader */
    PAPPENDICAPKTPROC pAppendICAPktProc;     /* out: pointer to AppendICAPacket */
    USHORT MaximumWriteSize;                 /* out: maximum ica write size */
} VDWRITEHOOK20, * PVDWRITEHOOK20;

/*
 * Zero Latency structures and types
 */

/*
 * Possible return codes from calls to the scan code hook function
 */

#define ZLC_AUTOREPEAT      1
#define ZLC_NOAUTOREPEAT    2

/*
 * Define a callback function for scan code events
 * This is passed a pointer to a SCANCODE structure
 */

typedef UINT32 (PWFCAPI PFN_ZLSCANCODEHOOK)   (PSCANCODE,PSCANCODE *);

/*
 * A handle to a scan code hook is a HND
 */

typedef HND     HSCANCODEHOOK;

/*
 * Define a structure that Zero Latency uses to register scan code hooking information to WD
 */

typedef struct _ZLSCANCODEREGISTERHOOKINFORMATION
{
    PFN_ZLSCANCODEHOOK  pfnScanCodeHook;    /*(IN) points at Hook function*/
    PVOID               pContext;           /*(IN) context pointer*/
    HSCANCODEHOOK       hHook;              /*(OUT) Handle to this hook*/
} ZLSCANCODEREGISTERHOOKINFORMATION, FAR * PZLSCANCODEREGISTERHOOKINFORMATION;

/*
 * Structure that ZLC uses to unregister scan code hook with WD
 */

typedef struct _ZLSCANCODEUNREGISTERHOOKINFORMATION
{
    HSCANCODEHOOK       hHook;              /*(IN) Handle of hook to unregister with WD*/
    BOOL                fError;             /*(OUT)Used to indicate success of unregistration, TRUE == ERROR, FALSE == SUCCESS*/
} ZLSCANCODEUNREGISTERHOOKINFORMATION, FAR * PZLSCANCODEUNREGISTERHOOKINFORMATION;

/*
 * Define an element in a linked list that is used by WD to maintain information about scan code hook functions
 * that were registered with it by ZL
 */

typedef struct _WDZLSCANCODEHOOKINFORMATIONENTRY
{
    PFN_ZLSCANCODEHOOK  pfnScanCodeHook;    /*hook function pointer*/
    PVOID               pContext;           /*context pointer*/
    struct _WDZLSCANCODEHOOKINFORMATIONENTRY * pNext;   /*next element in list*/
} WDZLSCANCODEHOOKINFORMATIONENTRY, FAR * PWDZLSCANCODEHOOKINFORMATIONENTRY;

/*
 * Unicode support for zero latency
 */

#if defined(UNICODESUPPORT) || defined(USE_EUKS)

/*
 * Define a hook function that is used to pass UNICODE information to ZL.  This is passed in the form
 * of a pointer to a UNICODE structure
 */

typedef VOID  (PWFCAPI PFN_ZLUNICODEHOOK)  (PUNICODECODE);

/*
 * Define a handle to a Unicode hook
 */

typedef HND HUNICODEHOOK;

/*
 * Define a struct that Zero Latency uses to pass Unicode hook information to WD
 */

typedef struct _ZLREGISTERUNICODEHOOKINFORMATION
{
    PFN_ZLUNICODEHOOK   pfnUnicodeHook;     /*(IN)  hook function pointer*/
    PVOID               pContext;           /*(IN)  flags, reserved for future use*/
    HUNICODEHOOK        hHook;              /*(OUT) handle to this hook entry*/
} ZLREGISTERUNICODEHOOKINFORMATION, *PZLREGISTERUNICODEHOOKINFORMATION;

/*
 * Structure that ZLC uses to unhook a hook that was previously set with WD
 */

typedef struct _ZLUNREGISTERUNICODEHOOKINFORMATION
{
    HUNICODEHOOK        hHook;          /*(IN)  handle to hook to unregister*/
    BOOL                fError;         /*(OUT) TRUE == Hook unregistered successfully, FALSE == Hook unregistered unsuccessfully*/
} ZLUNREGISTERUNICODEHOOKINFORMATION, FAR * PZLUNREGISTERUNICODEHOOKINFORMATION;

/*
 * Define an element in a linked list that is used by WD to maintain information about Unicode hook functions
 * that were registered with it with ZL
 */

typedef struct _WDZLUNICODEHOOKINFORMATIONENTRY
{
    PFN_ZLUNICODEHOOK   pfnUnicodeHook;     /*hook function pointer*/
    PVOID               pContext;           /*flags, reserved for future use*/
    struct _WDZLUNICODEHOOKINFORMATIONENTRY * pNext;    /*points to next element in list*/
} WDZLUNICODEHOOKINFORMATIONENTRY, *PWDZLUNICODEHOOKINFORMATIONENTRY;

#endif /*UNICODESUPPORT*/

#define INPUT_DATA_TYPE_MOUSE    0x1
#define INPUT_DATA_TYPE_KEYBOARD 0x2
#define INPUT_DATA_TYPE_MULTITOUCH	0x4
#define INPUT_DATA_RESET         0x80000000

/*
 * WdRedraw structure
 */
#ifdef PLATFORM_PRAGMAS
#pragma warning(disable : 4214)
#endif
typedef struct _WDRCL {         /* SetFocusProcedure parameter (via PACKET_REDRAW) */
   ULONG x:12, /* X coordinate */
         y:12; /* Y coordinate */
} WDRCL, *PWDRCL;
#ifdef PLATFORM_PRAGMAS
#pragma warning(default : 4214)
#endif

#define WD_CALL_INDEX  254  /* index for WDSETINFO calls using external VdInfo calls */

#define WDPAUSERESUME_VERSION_ONE      1
#define WDPAUSERESUME_VERSION_CURRENT  WDPAUSERESUME_VERSION_ONE

#define WDPAUSERESUME_REQUEST_PAUSE     0
#define WDPAUSERESUME_REQUEST_RESUME    1

typedef struct _WDPAUSERESUME 
{
    UINT16  cbSize;             /* size of WDPAUSERESUME structure with data   */
    UINT16  Version;            /* Version of request structure             */

    UINT32  uiRequestType;      /* Ver 1 - Type of Request (Pause or Resume)*/
    UINT32  uiFlags;            /* Ver 1 - Flags for operation              */

} WDPAUSERESUME, * PWDPAUSERESUME;

/* The opaque handle to the WD itself. */

typedef struct _WD * PWD;

/* QueueVirtualWrite() is now the preferred virtual driver write function. */

int WFCAPI QueueVirtualWrite(PWD, USHORT, LPMEMORY_SECTION, USHORT, USHORT);

typedef struct _WDLICENSEDFEATURES {
    ULONG LicensedFeatures;        /* LicensedFeatures Mask set by WD */
} WDLICENSEDFEATURES, * PWDLICENSEDFEATURES;

/*
 * WdCreateChannelComms
 */
#define WDCREATECCOMMS_VERSION_CURRENT  (VDCREATECCOMMS_VERSION_CURRENT)

/*
 * This is defined in vdapi.h as its the same as _VDCREATECCOMMS
 *
typedef struct _WDCREATECCOMMS
{
} WDCREATECCOMMS, * PWDCREATECCOMMS;

*/

/* Client side window monitoring sdk */
#ifdef WIN32
/* Function header for ICA window change callback */
typedef VOID (__cdecl * PFNWD_WINDOWCHANGED) (UINT32);

/* // begin_binsdk */

/* The current mode of the ICA window */
typedef union _WDICAWINDOWMODE
{
    struct
    {
        UINT Reserved : 1;
        UINT Seamless : 1;
        UINT Panning : 1;
        UINT Scaling : 1;
    } Flags;
    UINT Value;
} WDICAWINDOWMODE;

/* Populated by WdGetICAWindowInfo call */
typedef struct _WDICAWINDOWINFO
{
    HWND hwnd;										/* The ica window handle */
    WDICAWINDOWMODE mode;							/* The current mode of the window */
	UINT32 xWinWidth, yWinHeight, xViewWidth, yViewHeight; /* ICA window dimensions */
    INT xViewOffset, yViewOffset;					/* The window's view offset (i.e panning) */
} WDICAWINDOWINFO, * PWDICAWINDOWINFO;

// WDOVERLAYINFO
//
// Populated by WDAPI_TRANSLATEOVERLAYWINDOW call
//
typedef struct _WDOVERLAYINFO
{
	RECT serverRect; //In Server's window rect
	RECT clientRect; //Out Translated client rect 
	DOUBLE scaleFactor; // Out
	BOOL bScaleToFit; //In Flag whether we also account for scale to fit
	UINT32 xViewOffset, yViewOffset; //Out Window's view offset
} WDOVERLAYINFO, * PWDOVERLAYINFO;

/* // end_binsdk */

/* Input/output parameter for WdRegisterWindowChangeCallback */
typedef struct _WDREGISTERWINDOWCALLBACKPARAMS
{
	PFNWD_WINDOWCHANGED pfnCallback; /* Function to call when ICA window changes */
	UINT32 Handle;					 /* Handle is returned that can be used for unregistering the callback */
} WDREGISTERWINDOWCALLBACKPARAMS, *PWDREGISTERWINDOWCALLBACKPARAMS;
#endif //WIN32

/* // begin_binsdk */


//================================================================================
// Transport Quality of Service types as flags
// (used to specify all the QoS types that the current connection has availalbe).
//================================================================================

#define ICA_QOS_RELIABLE              0x0001
#define ICA_QOS_UNRELIABLE            0x0002
#define ICA_QOS_RELIABLE_BASIC_FEC    0x0004
 /* Reserved for future use
 #define ICA_QOS_UNRELIABLE_BASIC_FEC  0x0008
 #define ICA_QOS_RELIABLE_ADV_FEC      0x0010
 #define ICA_QOS_UNRELIABLE_ADV_FEC    0x0020
*/

// EDTTRANSPORTINFO
//
// Populated by WDAPI_QUERYEDT call
//
typedef struct _EDTTRANSPORTINFO
{
    BOOL        useUdt;
    UINT32      Channels;
} EDTTRANSPORTINFO, *PEDTTRANSPORTINFO;

// EDTSTATS
//
// Populated by WDAPI_QUERYEDTSTATS call
//
typedef struct _EDTSTATS
{
    ICA_TRANSPORT_RELIABILITY eReliability;

    BOOL                    bSslInUse;              /**< Flag set when the connection is using SSL */
    ULONG                   dwProtocol;             /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   aiCipher;               /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   dwCipherStrength;       /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   aiHash;                 /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   dwHashStrength;         /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   aiExch;                 /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   dwExchStrength;         /**< See MSDN SecPkgContext_ConnectionInfo structure */
    ULONG                   HeaderLength;           /**< TLS/DTLS Header Size */
    ULONG                   TrailerLength;          /**< TLS/DTLS Trailer Size */
    ULONG                   MaxMessageLength;       /**< TLS/DTLS Max Message Size */

    // global measurements
    INT64 msTimeStamp;                 // time since the UDT entity is started, in milliseconds
    INT64 pktSentTotal;                // total number of sent data packets, including retransmissions
    INT64 pktRecvTotal;                // total number of received packets
    UINT64 bytesSentTotal;             // total number of sent data bytes, including retransmissions
    INT64 bytesRecvTotal;             // total number of received bytes
    int pktSndLossTotal;                 // total number of lost packets (sender side)
    int pktRcvLossTotal;                 // total number of lost packets (receiver side)
    int pktRetransTotal;                 // total number of retransmitted packets
    int pktSentACKTotal;                 // total number of sent ACK packets
    int pktRecvACKTotal;                 // total number of received ACK packets
    int pktSentNAKTotal;                 // total number of sent NAK packets
    int pktRecvNAKTotal;                 // total number of received NAK packets
    INT64 usSndDurationTotal;         // total time duration when UDT was sending data (idle time exclusive)

    // local measurements
    INT64 pktSent;                     // number of sent data packets, including retransmissions
    INT64 pktRecv;                     // number of received packets
    UINT64 bytesSent;                  // number of sent data bytes, including retransmissions
    UINT64 bytesRecv;                  // number of received bytes
    int pktSndLoss;                      // number of lost packets (sender side)
    int pktRcvLoss;                      // number of lost packets (receiver side)
    int pktRetrans;                      // number of retransmitted packets
    int pktSentACK;                      // number of sent ACK packets
    int pktRecvACK;                      // number of received ACK packets
    int pktSentNAK;                      // number of sent NAK packets
    int pktRecvNAK;                      // number of received NAK packets
    UINT64 bpsSendRate;                // sending rate in b/s
    UINT64 bpsRecvRate;                // receiving rate in b/s
    UINT64 usSndDuration;              // current busy sending time (i.e., idle time exclusive)

    // instant measurements
    int pktFlowWindow;                   // flow window size, in number of packets
    int pktCongestionWindow;             // congestion window size, in number of packets
    int pktFlightSize;                   // number of packets on flight
    int pktDeliveryRate;                 // packet arrival rate at the remote side, in pkts per second
    int pktAvailSndBuf;                  // available UDT sender queue size, in number of packets
    int pktAvailRcvBuf;                  // available UDT receiver queue size, in mumber of packets
    int pktUsedSndBuf;                   // size of pending data in the UDT sender queue, in number of packets
    int pktUsedRcvBuf;                   // size of data available to read in the UDT receiver queue, in number of packets
    UINT64 bpsBandwidth;               // estimated bandwidth, in bits per second
    UINT32 usPktSndPeriod;             // packet sending period, in microseconds
    UINT32 usRTT;                      // RTT, in microseconds
} EDTSTATS, * PEDTSTATS;


#endif /* __WDAPI_H__ */
/* // end_binsdk */
