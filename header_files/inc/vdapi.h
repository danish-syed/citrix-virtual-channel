/* // begin_binsdk */
/***************************************************************************
*
*  VDAPI.H
*
*  This module contains external virtual driver API defines and structures
*
*  $Id$
*
*  Copyright 1998-2015 Citrix Systems, Inc.
*
****************************************************************************/

#ifndef __VDAPI_H__
#define __VDAPI_H__

#pragma once

#include <icaconst.h>
#include <dllapi.h>
#include <ica.h>

/*
*  Index into PD procedure array
*/
#define VDxQUERYINFORMATION      6
#define VDxSETINFORMATION        7
#define VDxCOUNT                 8   /* number of external procedures */

//=============================================================================
//   HPC Versions
//=============================================================================

// HPC_VD_API_VERSION_*
//
// These are the version identifiers for the VcSdk HPC engine API.  A VD will specify
// the API version it will use.  The engine will guarantee that VD's using older
// API versions will continue to operate properly.  The version number is ever-increasing.

typedef enum _HPC_VD_API_VERSION
{
	HPC_VD_API_VERSION_LEGACY =				0,				// legacy VDs
	HPC_VD_API_VERSION_V1 =					1,				// VcSdk API Version 1
	HPC_VD_API_VERSION_MAX =				2				// one higher than the largest version supported
} HPC_VD_API_VERSION;

//=============================================================================
//   Enums: For HPC API 1
//=============================================================================

// HPC_VD_OPTIONS
//
// Bit flags for options that a VD can set/request as part of the WDAPI_HPC_PROPERTIES
// WdSetInformation or WdQueryInformation calls. These options will apply to *all*
// VCs opened by a single VD.
//
// NOTE: Define these options so that the default value, corresponding to legacy operation
// of VCs, will be ZERO; i.e., set flag bits must be for new HPC operational features.

typedef enum _HPC_VD_OPTIONS
{
	HPC_VD_OPTIONS_NO_POLLING =		0x0001,		// Flag indicating that channels on this VD do not require send data polling
	HPC_VD_OPTIONS_NO_COMPRESSION =	0x0002		// Flag indicating that channels on this VD send data that does not need reducer compression
} HPC_VD_OPTIONS;

// SENDDATA:  This defines the flags used in Hpc_Comm_ApiSendData, field ulFlags.
// Each of the flags is ORed together in the field.

typedef enum _SENDDATA
{
	SENDDATA_NOTIFY =			0x0001	// If this flag is set, the VD will be notified when it can retry 
										// sending following a CLIENT_ERROR_NO_OUTBUFS error.  The notification occurs via the
										// DriverPoll method.
} SENDDATA;

/*
*  VdOpen structure
*/
typedef struct _VDOPEN
{
	LPVOID pIniSection;
	PDLLLINK pWdLink;
	/* This field can be either a bit mask of supported channels (b0=0) OR 
	* it can be the actual number of a supported channel. 
	* The correct interpretation will be determined by the WD and hence 
	* this field should not be used to set/get any information about the supported channels. */
	ULONG ChannelMask;
	PLIBPROCEDURE pfnWFEngPoll;
	PLIBPROCEDURE pfnStatusMsgProc;
	HND hICAEng;
} VDOPEN, FAR * PVDOPEN;


/*
*  VdInformationClass enum
*   additional information classes can be defined for a given
*   VD, as long as the number used is greater than that generated
*   by this enum list
*/

typedef enum _VDINFOCLASS
{
#ifndef unix
	VdLastError,
#endif /* unix */
	VdKillFocus,
	VdSetFocus,
#ifndef unix
	VdMousePosition,
#endif /* unix */
	VdThinWireCache,
	VdWinCEClipboardCheck,		//  Used by WinCE to check for clipboard changes
	VdDisableModule,
	VdFlush,
	VdInitWindow,
	VdDestroyWindow,
#ifndef unix
	VdPaint,
#endif /* unix */
	VdThinwireStack,
	VdRealizePaletteFG,			// inform client to realize it's foreground palette
	VdRealizePaletteBG,			// inform client to realize it's background palette
	VdInactivate,				// client is about to lose input focus
#ifndef unix
	VdGetSecurityAccess,		// cdm security info
	VdSetSecurityAccess,		// cdm security info
	VdGetAudioSecurityAccess,	// cdm audio security info
	VdSetAudioSecurityAccess,	// cdm audio security info
#endif /* unix */
	VdGetPDASecurityAccess,		// cdm PDA security info
	VdSetPDASecurityAccess,		// cdm PDA security info
#ifndef unix
	VdGetTWNSecurityAccess,		// cdm TWN security info
	VdSetTWNSecurityAccess,		// cdm TWN security info
#endif /* unix */
	VdSendLogoff,
	VdCCShutdown,
	VdSeamlessHostCommand,		// Seamless command call
	VdSeamlessQueryInformation,	// Seamless query call
	VdWindowGotFocus,
	VdSetCursor,				// Set: New cursor from TW. Data - PHGCURSOR
	VdSetCursorPos,				// Set: New cursor position. Data - VPPOINT
	VdEnableState,				// Set/Get driver state (enabled/disabled)
	VdIcaControlCommand,
#ifndef unix
	VdVirtualChannel,			// Set/Get virtual channel data
	VdWorkArea,					// Set the work area of the application
#endif /* unix */
	VdSupportHighThroughput,
#ifndef unix
	VdRenderingMode,			// Set/Get the rendering mode (headless client)
#endif /* unix */
	VdPauseResume,				// Pause/Resume commands
#ifdef BLT_IS_EXPENSIVE
	VdRedrawNotify,				// Overdrawing suppression.
#endif
	VdDisplayCaps,				// Get/Set display capabilities and/or preferred mode
	VdICOSeamlessFunctions,		// Get seamless functions for ICO
	VdPnP,						// Set: Send CLPNP_COMMAND commands inbetween the control VC and the implementation VC (such as VIRTUAL_CCM)
	/* Support for EUEM (Darwin Release For Ohio). This code has been added or
	* changed to support End User Experience Monitoring functionality. Please do
	* not change the code in this section without consulting the EUEM team
	*  Email (at the time of change) "#ENG - Darwin Dev" 
	*/
	VdEuemStartupTimes,			// Set: EUEM: pass the startup times of shared sessions to the EUEM VD
	VdEuemTwCallback,			// Set: register the EUEM ICA roundtrip callback
								// function from the thinwire VC to the EUEM VC
								// End of EUEM support section
	VdResizeHotBitmapCache,		// Set: Tell thinwire driver to resize the hot bitmap cache
	VdSetMonitorLayout,			// Set: Tell thinwire driver to update monitor layout info
	VdGUSBGainFocus,			// Set: Tell Generic USB driver that engine has gained keyboard focus
	VdGUSBLoseFocus,			// Set: Tell Generic USB driver that engine has lost keyboard focus
	VdCreateChannelComms,		// Query: Provide the driver with a pipe to communicate directly with
	VdGetPNPSecurityAccess,		// usb PNP security info
	VdSetPNPSecurityAccess,		// usb PNP security info
	VdReverseSeamless,			// For use with RS specific calls
	VdInfoRequest,				// Used to request information from a VD
    VdReverseSeamlessPartial,   // partial RS packet data used to form a complete RS VC packet
	VdEuemNotifyReconnect,		// Notify EUEM about a reconnect
    VdCHAEnabled,                // Notify Drivers about Enabling/Disabling CHA based on CST recommendation
	VdMTCommand,
    VdSendMouseData,            // Mouse data packets to be sent to host when using VC for mouse data
    VdSendKeyboardData,          // Keyboard type and codes to be sent to host when using VC for keyboard data
	VdSendTouchData,
	VdGUSBSecondAppStarts,
	VdCTXIMEHotKeySetIMEModeInApp,
	VdCTXIMEHotKeySetIMEModeInCDSBar,
  	VdCTXIMEQueryInformation,
  	VdCTXIMESeamlessQueryInformation,
  	VdCTXIMESetDispWMInfo,
  	VdCTXIMESetSeamlessWMInfo,
	VdSeamlessResumeLaterCapEnabled,
    VdDndMouseLeave,
    VdDimRequest,               // enable / disable session dim based on SR mode
    VdPaintEvent,               // paint notification to virtual channels
    VdGraphParam,               // graphics param change notification 
    VdSeamlessInfo,             // seamless head info notification
#ifndef unix
    VdSeamlessMode,             // enable / disable semaless mode
    VdGetWIASecurityAccess,	    // cdm WIA security info
    VdSetWIASecurityAccess, 	    // cdm WIA security info
#else
    VdSeamlessMode,              // enable / disable semaless mode
#endif // unix
    VdGBufferValidateConnection,  // validate GBuffer connection using secrets. Event Data type is GBufferConnectionEvt
	VdSeamlessWindowLifecycle, // indicate seamless window is created/destroyed
	VdSeamlessWindowState, // seamless window position change
    VdRenderSurfaceInfo,    // RenderSurfaceInfo notification to virtual channels
    VdRefreshRenderSurfaceInfo, // Refresh RenderSurfaceInfo to virtual channels
    VdPerWindowLVBInfo, // PerWindowLVBInfo notification to virtual channels
    VdEuemLogonInfo, // Set: EUEM: pass the logon info of sessions to the EUEM VD
	VdRegisterSeamlessWindowNotification // register virtual channel to get notified on VdSeamlessWindowLifecycle and VdSeamlessWindowState events
} VDINFOCLASS;

/*
*  VdQueryInformation structure
*/
typedef struct _VDQUERYINFORMATION
{
	VDINFOCLASS VdInformationClass;
	LPVOID pVdInformation;
	int VdInformationLength;
	int VdReturnLength;
} VDQUERYINFORMATION, * PVDQUERYINFORMATION;

/*
*  VdSetInformation structure
*/
typedef struct _VDSETINFORMATION
{
	VDINFOCLASS VdInformationClass;
	LPVOID pVdInformation;
	int VdInformationLength;
} VDSETINFORMATION, * PVDSETINFORMATION;

/*
*  VdLastError
*/
typedef struct _VDLASTERROR
{
	int Error;
	char Message[ MAX_ERRORMESSAGE ];
} VDLASTERROR, * PVDLASTERROR;

/*
* VD Flush
*/
typedef struct _VDFLUSH
{
	UCHAR Channel;
	UCHAR Mask;
} VDFLUSH, * PVDFLUSH;

#define  VDFLUSH_MASK_PURGEINPUT    0x01
#define  VDFLUSH_MASK_PURGEOUTPUT   0x02

/* // end_binsdk */

//////////////////////////////////
// VdInfoRequest 
//

typedef struct _VDTWI_INFOREQUEST
{
	DWORD dwEnginePid;
	UINT nNumMonitors;
	USHORT cbBuffer;
	PBYTE abBuffer[1];
} VDTWI_INFOREQUEST;


//////////////////////////////////
// VD - Reverse Seamless Support
//

#pragma pack(push, 1)

#define VDRSINFO_VERSION_1              (1)
#define VDRSINFO_CURRENT_VERSION        VDRSINFO_VERSION_1

typedef struct _VDRSINFO
{
	UCHAR ucVersion;
	UCHAR ucPacketType;
	UINT  cbPacketSize;
	UCHAR abPacketBuf[1];
} VDRSINFO;

#pragma pack(pop)

//////////////////////////////////

/*
* VD Thinwire Stack
*/
typedef struct _VDTWSTACK
{
	LPBYTE pTop;
	LPBYTE pMiddle;
	LPBYTE pBottom;
} VDTWSTACK, * PVDTWSTACK;

/*
*  VdThinWireCache
*/
typedef struct _VDTWCACHE
{
	ULONG ulXms;
	ULONG ulLowMem;
	ULONG ulDASD;
	ULONG ulTiny;
	char  pszCachePath[MAXPATH+1];
} VDTWCACHE, * PVDTWCACHE;

typedef struct _MOUSEPOSITION
{
	USHORT X;
	USHORT Y;
} MOUSEPOSITION, FAR * PMOUSEPOSITION;


/*
* VdVirtualChannel
*/
#define VDVCINFO_MAX_CHANNELNAME  (VIRTUALNAME_LENGTH + 1)
#define VDVCINFO_MAX_CHANNELNUM   VIRTUAL_MAXIMUM

#define VDVCINFO_VERSION_ONE      1
#define VDVCINFO_VERSION_CURRENT  VDVCINFO_VERSION_ONE

#define VDVCINFO_REQUEST_QUERY_CHANNEL_NUMBER   1       /* query */
#define VDVCINFO_REQUEST_QUERY_CHANNEL_NAME     2       /* query */
#define VDVCINFO_REQUEST_QUERY_CHANNEL_COUNT    3       /* query */
#define VDVCINFO_REQUEST_QUERY_READ_TYPE        4       /* query */
#define VDVCINFO_REQUEST_QUERY_READ_SIZE        5       /* query */
#define VDVCINFO_REQUEST_READ_CHANNEL           6       /* query */
#define VDVCINFO_REQUEST_WRITE_CHANNEL          7       /* set */
#define VDVCINFO_REQUEST_CHANNEL_FLAGS          8       /* query/set */

#define VDVCINFO_CHANNEL_FLAGS_FRAGMENT         0x01    /* fragment the request if required */

#define VDVCINFO_PACKET_TYPE_STRING             0       /* no zeroes in data */
#define VDVCINFO_PACKET_TYPE_BINARY             1       /* zeroes in data    */

typedef struct _VDVCINFO
{
	ULONG   cbSize;             /* size of VDVCINFO structure with data     */
	USHORT  Version;            /* Version of request structure             */
	ULONG   ulRequest;          /* Ver 1 - type of Virtual Channel Request  */
	VIRTUALNAME szChannelName;  /* Ver 1 - Channel name                     */
	USHORT      usChannelNum;   /* Ver 1 - Channel number                   */
	ULONG   ulValue;            /* Ver 1 - used for simple requests         */
	ULONG   cbVCInfo;           /* Ver 1 - length of Virtual Channel info   */
	ULONG   oVCInfo;            /* Ver 1 - offset to Virtual Channel info   */
								/* VC info after the structure              */
} VDVCINFO, * PVDVCINFO;

/*
* VdWorkArea
*/
#define VDWORKAREA_VERSION_ONE      1
#define VDWORKAREA_VERSION_CURRENT  VDWORKAREA_VERSION_ONE

typedef struct _VDWORKAREA
{
	UINT16  cbSize;             /* size of VDWORKAREA structure with data   */
	UINT16  Version;            /* Version of request structure             */
	UINT32  uiXPos;             /* Ver 1 - X Position of work area          */
	UINT32  uiYPos;             /* Ver 1 - Y Position of work area          */
	UINT32  uiWidth;            /* Ver 1 - width of work area               */
	UINT32  uiHeight;           /* Ver 1 - height of work area              */
} VDWORKAREA, * PVDWORKAREA;

typedef struct _VDRENDERINGMODE
{
	UINT16  cbSize;             /* size of VDRENDERINGMODE structure        */
	UINT16  Version;            /* Version of request structure             */
	UINT32  uiRenderingMode;    /* Ver 1 - The rendering mode for the client*/
} VDRENDERINGMODE, * PVDRENDERINGMODE;

#define VDPAUSERESUME_VERSION_ONE      1
#define VDPAUSERESUME_VERSION_CURRENT  VDPAUSERESUME_VERSION_ONE

#define VDPAUSERESUME_REQUEST_PAUSE     0
#define VDPAUSERESUME_REQUEST_RESUME    1

typedef struct _VDPAUSERESUME
{
	UINT16  cbSize;             /* size of VDPAUSERESUME structure with data   */
	UINT16  Version;            /* Version of request structure             */
	UINT32  uiRequestType;      /* Ver 1 - Type of Request (Pause or Resume)*/
	UINT32  uiFlags;            /* Ver 1 - Flags for operation              */
} VDPAUSERESUME, * PVDPAUSERESUME;

typedef struct _VDTWXYRES
{
	UINT16  hRes;
	UINT16  vRes;
} VDTWXYRES, *PVDTWXYRES;

#ifndef CCAPS_4_BIT
#define CCAPS_4_BIT         0x0001      /* from twcommon.h */
#endif
#ifndef CCAPS_8_BIT
#define CCAPS_8_BIT         0x0002      /* from twcommon.h */
#endif
#ifndef CCAPS_16_BIT
#define CCAPS_16_BIT        0x0004      /* from twcommon.h */
#endif
#ifndef CCAPS_24_BIT
#define CCAPS_24_BIT        0x0008      /* from twcommon.h */
#endif

typedef struct _VDTWCAPS
{
	UINT16    depths;
#define VDDCAP_4BPP    CCAPS_4_BIT
#define VDDCAP_8BPP    CCAPS_8_BIT
#define VDDCAP_16BPP   CCAPS_16_BIT
#define VDDCAP_24BPP   CCAPS_24_BIT
	UINT16    fixedResCnt;		/* 0    -> variable resolution up to res[0].hRes X res[0].vRes */
								/* 1-10 -> only supports fixed resoltions in res[] array */
	VDTWXYRES res[10];
} VDTWCAPS, * PVDTWCAPS;

typedef struct _VDTWMODE
{
	UINT16    depth;  /* One of VDDCAP_xxx bits */
	VDTWXYRES res;
} VDTWMODE, * PVDTWMODE;

typedef struct _VDTWDISPLAYCAPS
{
	UINT32   Flags;
#define VDTW_PREF_UPDATE 0x1
#define VDTW_CAP_UPDATE  0x2
	VDTWMODE Pref;
	VDTWCAPS Caps;
} VDTWDISPLAYCAPS, * PVDTWDISPLAYCAPS;

typedef enum _CCMSECURITYCLASS
{
	evSerialDevices,     /* Virtual serial devices over COM */
	evCOMDevices         /* Actual COM devices */
}CCMSECURITYCLASS;


/*
* VdCreateChannelComms & WdCreateChannelComms data
*/
#define VDCREATECCOMMS_VERSION_ONE      1
#define VDCREATECCOMMS_VERSION_CURRENT  VDCREATECCOMMS_VERSION_ONE

typedef struct _VDCREATECCOMMS /* & _WDCREATECCOMMS */
{
	ULONG       cbSize;             /* size of V/WDCREATECCOMMS structure with data */
	USHORT      Version;            /* Version of request structure                 */
	USHORT      channelNumber;      /* Ver 1 - Which channel number to pass the     */
									/*           VdCreateComms request to           */
	UINT32      bSuccess;           /* Ver 1 - Return status                        */
	char        szPipeName[1];      /* Ver 1 - Pipe name (starts here)              */
} VDCREATECCOMMS, * PVDCREATECCOMMS, WDCREATECCOMMS, * PWDCREATECCOMMS;

typedef enum _KBDDATATYPE
{
    ScancodeKeyboardData = 0,
    UnicodeKeyboardData = 1,
    XKeyKeyboardData = 2,
} KBDDATATYPE;

/* VdDIMRequest */
typedef struct _DIMRequest
{
    BOOL bEnableDim;
    USHORT usDimPercentage;
}DIMRequest;

typedef struct _PaintEvt
{
    HND hWnd; /* handle to window, type of HWND */
    void* hgRegion; /* type of HGREGION */
}PaintEvt;

typedef struct _SeamlessModeEvt
{
    BOOL bSeamlessStatus; /* seamless status - 1: enabled, 0: disabled*/
}SeamlessModeEvt;

typedef struct _GBufferConnectionEvt // Graphics Buffer connection event for AppSharing
{
    UINT32 clientId;            /* client Id, which can be client process id*/
    char clientSecret[128];     /* client secret known to server */
    char serverSecret[128];     /* server secret known to client */
    BOOL validClientSecret;     /* client secret validated by wfica */
}GBufferConnectionEvt;

typedef struct _SeamlessWindowLifecycleEvt
{
	BOOL create; /* true if window created, false if window destroyed*/
} SeamlessWindowLifecycleEvt;

typedef enum _SeamlessWindowStateEventType
{
	PositionChanged = 0,
} SeamlessWindowStateEventType;

typedef struct _SeamlessWindowStateEvt
{
	UINT32 event;
	INT posX;
	INT posY;
	INT width;
	INT height;
} SeamlessWindowStateEvt;

/* Seamless RenderSurfaceInfoEvt notification message type */
typedef enum _RenderSurfaceInfoMsgType
{
    AddWindow = 1,
    RefreshWindow = 2,
    RemoveWindow = 3
} RenderSurfaceInfoMsgType;

/* Seamless RenderSurfaceInfoEvt to notify Graphics engine */
typedef struct _RenderSurfaceInfoEvt
{
    RenderSurfaceInfoMsgType messageType; /* Specifies whether message handles Add/Delete/Refresh window */
    HND hWnd; /* handle to window, type of HWND */
    UINT32 RenderSurfaceID; /* RenderSurfaceID received as part of TWI_PACKET_RENDER_SURFACE_INFO */
} RenderSurfaceInfoEvt;

/* Graphics to notify PerWindowLVBInfoEvt to Seamless or other virtual channel drivers */
typedef struct _PerWindowLVBInfoEvt
{
    BOOL enabled; /* Indicates whether PerWindowLVB is enabled on Graphics virtual channel */
} PerWindowLVBInfoEvt, *PPerWindowLVBInfoEvt;

typedef struct _SeamlessWindowNotificationRegistration
{
	USHORT channelNumber;
} SeamlessWindowNotificationRegistration;

/* // begin_binsdk */
#endif /* __VDAPI_H__ */
/* // end_binsdk */
