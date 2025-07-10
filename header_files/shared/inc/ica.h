/* // begin_binsdk */
/*++ BUILD Version: 0000 ******************************************************
*
*  ICA.H
*     This file contains definitions for the ICA 3.0 structures
*
*     NOTE: this file is shared between the client and the server
*
*  Copyright 2002-2015 Citrix Systems, Inc. 
*
*  This is an unpublished work protected as such under copyright law,
*  containing proprietary and confidential information of Citrix Systems, Inc.
*  Use, disclosure or reproduction without the express written authorization
*  of Citrix Systems, Inc. is prohibited.
*
*******************************************************************************/

#ifndef __ICA_H__
#define __ICA_H__

/*
 * these are done this way (not #if WINFRAME_BUILD) to check to see
 * if we are building on the server side apposed to the client side
 */
#ifdef WINFRAME_BUILD
#ifdef MFIMA
#include <ica30.h>
#else
#include <citrix\ica30.h>
#endif
#endif

/*
 * these are done this way (not #if WINFRAME_BUILD) to check to see
 * if we are building on the server side apposed to the client side
 */
#if defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#ifdef MFIMA
#include <vrtlclss.h>
#else
#include <citrix\vrtlclss.h>
#endif
#endif

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_ENABLE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#pragma pack(1)
#endif

/* // end_binsdk */

#include <icaid.h>

/*=============================================================================
==   Registered Netware Bindery ID  (SAP ID)
=============================================================================*/

/*
 *  SAP ID - Citrix Application Server for NT
 */
#define CITRIX_APPLICATION_SERVER       0x083d
#define CITRIX_APPLICATION_SERVER_SWAP  0x3d08  /* byte swapped */

#ifdef notdef
/* old winview sap id */
#define CITRIX_APPLICATION_SERVER       0x052d
#define CITRIX_APPLICATION_SERVER_SWAP  0x2d05  /* byte swapped */
#endif


/*=============================================================================
==   Citrix AppServer mask (ms networks NetServerEnum type field)
=============================================================================*/

#if defined(notdef) || defined(WINFRAME_BUILD)   /* part of lmserver.h now */
#if WINFRAME_BUILD
#define SV_TYPE_APPSERVER   0x10000000
#endif
#endif

#if WINVER >= 0x500
#define SV_TYPE_APPSERVER   0x10000000
#endif

#ifndef SV_TYPE_TERMINALSERVER   /* part of lmserver.h now */
#define SV_TYPE_TERMINALSERVER   0x02000000
#endif
/* // begin_binsdk */
/* // begin_icasdk */

/*=============================================================================
==   Client Modules
=============================================================================*/

/*
 *  modules classes
 */
typedef enum _MODULECLASS {
    Module_UserInterface,
    Module_UserInterfaceExt, /* Obsolete */
    Module_WinstationDriver,
    Module_VirtualDriver,
    Module_ProtocolDriver,
    Module_TransportDriver,
    Module_NameResolver, /* Obsolete */
    Module_NameEnumerator, /* Obsolete */
    Module_Scripting, /* Obsolete */
    Module_SubDriver, /* Obsolete */
    ModuleClass_Maximum
} MODULECLASS;
/* // end_icasdk */


/*=============================================================================
==   Protocol Drivers - Common data structures
=============================================================================*/

/*
 *  protocol driver classes
 *
 *  NOTE: don't change the order of this structure it will break
 *  NOTE: the Host.  Also, any additions to this structure must
 *  NOTE: be reflected into the SDCLASS in HYDRIX.H or else we're SOL.
 */
typedef enum _PDCLASS {
    PdNone,            /* 0 */
    PdConsole,         /* 1  !Obsolete! no dll */
    PdNetwork,         /* 2  !Obsolete! tdnetb.dll, tdspx.dll, tdftp.dll tdipx.dll */
    PdAsync,           /* 3  !Obsolete! tdasync.dll */
    PdOemTransport,    /* 4  !Obsolete! user transport driver */
    PdISDN,            /* 5  !Obsolete! not implemented */
    PdX25,             /* 6  !Obsolete! not implemented */
    PdModem,           /* 7  !Obsolete! pdmodem.dll */
    PdOemConnect,      /* 8  !Obsolete! user protocol driver */
    PdFrame,           /* 9  PdRFram.dll. Now used for framing reliable transports only. */
    PdReliable,        /* 10 !Obsolete! pdreli.dll */
    PdEncrypt,         /* 11 pdc0.dll, pdcrypt.dll, pdc40.dll, pdc56.dll, pdc128.dll */
    PdCompress,        /* 12 pdcomp.dll */
    PdTelnet,          /* 13 !Obsolete! not implemented */
    PdOemFilter,       /* 14 !Obsolete! user protocol driver */
    PdNasi,            /* 15 !Obsolete! tdnasi.dll */
    PdTapi,            /* 16 !Obsolete! pdtapi.dll */
    PdReserved1,       /* 17 !Obsolete! */
    PdReserved2,       /* 18 !Obsolete! */
    PdReserved3,       /* 19 !Obsolete! */
    PdClass_Maximum    /* 20 must be last */
} PDCLASS;
/* // end_binsdk */


/*=============================================================================
==   Winstation Drivers - Common data structures
=============================================================================*/

/*
 *  Valid row/column combinations
 */
typedef struct _WDTEXTMODE {
    BYTE Index;            /* this value is sent by PACKET_SET_VIDEOMODE */
    BYTE Flags;            /* Used by the client */
    USHORT Columns;
    USHORT Rows;
    USHORT ResolutionX;
    USHORT ResolutionY;
    BYTE FontSizeX;
    BYTE FontSizeY;
} WDTEXTMODE, * PWDTEXTMODE;

/*
 * Values for WDTEXTMODE Flags field. This are used by the client for non-standard video adapters
 */
#define VIDEO_ALTERNATE     1

/*
 * Values for WDTEXTMODE Index field
 * These must be common across all WDs in order to support shadow
 * and WinStation reconnect to a different WinStation (WD) type
 */
/*
 * The following are for valid VGA/SVGA text modes.
 * These MUST match the values defined by the client.
 */
#define TEXTINDEX_40x25     0
#define TEXTINDEX_80x25     1
#define TEXTINDEX_80x43     2
#define TEXTINDEX_80x50     3
#define TEXTINDEX_80x60     4
#define TEXTINDEX_132x25    5
#define TEXTINDEX_132x43    6
#define TEXTINDEX_132x50    7
#define TEXTINDEX_132x60    8

/*
 * The following are additional modes supported by Relisys, VT420, and others.
 */
#define TEXTINDEX_80x24     9
#define TEXTINDEX_80x36     10
#define TEXTINDEX_80x48     11
#define TEXTINDEX_80x72     12
#define TEXTINDEX_80x144    13
#define TEXTINDEX_132x24    14
#define TEXTINDEX_132x36    15
#define TEXTINDEX_132x48    16
#define TEXTINDEX_132x72    17
#define TEXTINDEX_132x144   18
/* // begin_binsdk */
/* // begin_icasdk */

/*=============================================================================
==   Virtual Drivers - Common data structures
=============================================================================*/

/*
 *  Virtual Channel Name
 */
#define VIRTUALNAME_LENGTH  7

typedef CHAR VIRTUALNAME[ VIRTUALNAME_LENGTH + 1 ];  /* includes null */
typedef CHAR * PVIRTUALNAME;

/* // end_icasdk */
/* // end_binsdk */

/*
 *  Virtual i/o channel names  (VIRTUALNAME)
 *
 *  name syntax:  xxxyyyy<null>
 *
 *    xxx    - oem id (CTX - Citrix Systems)
 *    yyyy   - virtual channel name
 *    <null> - trailing null
 */

#define VIRTUAL_SCREEN    "CTXSCRN"   /* reserved channel for screen data */
#define VIRTUAL_LPT1      "CTXLPT1"   /* old client printer mapping */
#define VIRTUAL_LPT2      "CTXLPT2"   /* old client printer mapping */
#define VIRTUAL_RESERVED3 "CTXRES3"   /* reserved channel */
#define VIRTUAL_COM1      "CTXCOM1"   /* old client printer mapping */
#define VIRTUAL_COM2      "CTXCOM2"   /* old client printer mapping */
#define VIRTUAL_CPM       "CTXCPM "   /* new client printer mapping (spooling) */
#define VIRTUAL_RESERVED4 "CTXRES4"   /* reserved channel */
#define VIRTUAL_CCM       "CTXCCM "   /* client com mapping */
#define VIRTUAL_CDM       "CTXCDM "   /* client drive mapping */
#define VIRTUAL_CLIPBOARD "CTXCLIP"   /* clipboard */
#define VIRTUAL_THINWIRE  "CTXTW  "   /* remote windows data */
#define VIRTUAL_PASSTHRU  "CTXPASS"   /* reserved channel for shadowing */
#define VIRTUAL_CAM       "CTXCAM "   /* client audio mapping */
#define VIRTUAL_CM        "CTXCM  "   /* client management */
#define VIRTUAL_LIC       "CTXLIC"    /* license management */
#define VIRTUAL_VFM       "CTXVFM"    /* Video server video (ie. not TW video) */
#define VIRTUAL_PN        "CTXPN  "   /* Program Neighborhood */
#define VIRTUAL_OEM       "OEMOEM "   /* used by oems */
#define VIRTUAL_OEM2      "OEMOEM2"   /* used by oems */
#define VIRTUAL_SEAMLESS  "CTXTWI"    /* Seamless VC */
#define VIRTUAL_ZLFNTKB   "CTXZLFK"   /* Zero latency font and keyboard virtual channel */
#define VIRTUAL_ZLC       "CTXZLC "   /* Zero latency data channel*/
#define VIRTUAL_ICACTL    "CTXCTL "   /* ICA Control Virtual Channel */
#define VIRTUAL_SCARD     "CTXSCRD"   /* Smart Card Virtual Channel Name */
#define VIRTUAL_SBROWSE   "CTXSBR"    /* SpeedBrowse Image Delivery Channel*/
#define VIRTUAL_MM        "CTXMM  "   /* SpeedScreen Multimedia Acceleration,
                                         a.k.a Remoting Audio and Video Extensions
                                         (RAVE), Virtual Channel Name */
#define VIRTUAL_TWN       "CTXTWN "   /* TWAIN Redirector Virtual Channel */
#define VIRTUAL_EUEM      "CTXEUEM"   /* End User Experience Monitoring Virtual Channel */
#define VIRTUAL_FLASH	  "CTXFLSH"	  /* HDX MediaStream for Flash Virtual Channel */
#define VIRTUAL_FLASH2	  "CTXFLS2"	  /* HDX MediaStream for Flash v2 Virtual Channel */
#define VIRTUAL_D3D       "CTXD3D"    /* Direct3D Command Remoting */
#define VIRTUAL_MRVC      "CTXMOB"    /* Mobile Receiver Virtual Channel */
#define VIRTUAL_SENS      "CTXSENS"   /* Sensor and Location Virtual Channel */
#define VIRTUAL_MT        "CTXMTCH"    /* MultiTouch Virtual Channel */
#define VIRTUAL_LFP       "CTXLFP"    /* LFP display protocol Virtual Channel */
#define VIRTUAL_ICAFILE   "CTXFILE"    /* File Transfer */
#define VIRTUAL_MTOP      "CTXMTOP"    /* Webrtc redirection */
#define VIRTUAL_DND       "CTXDND"    /* Drag and Drop */
#define VIRTUAL_FIDO      "CTXFIDO"   /* FIDO2 Redirection Virtual Channel */

/* VIRTUAL_MAXIMUM is defined to 64:
 *   - if its a server side build and USE_TSSDK is NOT defined i.e. a build that uses public API
 *   - OR if its a client side build 
 *
 * If USE_TSSDK is defined then VIRTUAL_MAXIMUM is set to 32 to be compatible with
 * TSSDK definition in hydrix.h.
 */
#if defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD) /**** Server side ****/
#if defined(USE_TSSDK)
#define VIRTUAL_MAXIMUM         32    /* number of virtual channels */
#else
#define VIRTUAL_MAXIMUM         64    /* number of virtual channels */
#endif
#else /**** Client side ****/
/* // begin_binsdk */
#define VIRTUAL_MAXIMUM         64    /* number of virtual channels */
/* // end_binsdk */
#endif

/* Old number of virtual channels needed for backward compatability 
 * with old clients and servers that do not support CAPABILITY_VIRTUAL_MAXIMUM
 * and hence can support a maximum of 32 VCs only.
 */
#define VIRTUAL_MAXIMUM_OLD_32  32    

/* Maximum number of non-VD C2H modules 
 * like UI, WD, TD, etc. that are sent
 * by client as PACKET_INIT_RESPONSE packets
 * during connection setup. This value is 
 * arbitrary for now.
 */
#define MAX_NONVD_C2H_MODULE_CNT 30

/*
 * This is the maximum number of ica modules expected from client to host as 
 * PACKET_INIT_RESPONSE packets during connection setup. The modules sent 
 * are: one for each Virtual Channel, one for each protocol driver, 
 * plus a few others (UI, WD, TD, etc.).
 */
#define MAX_ICA_MODULE_CNT (VIRTUAL_MAXIMUM + MAX_NONVD_C2H_MODULE_CNT)

/* // begin_binsdk */
/* // begin_icasdk */

/*
 *  Types of virtual i/o flow control
 */
typedef enum _VIRTUALFLOWCLASS {
    VirtualFlow_None,
    VirtualFlow_Ack,
    VirtualFlow_Delay,
    VirtualFlow_Cdm
} VIRTUALFLOWCLASS;

/*
 *  Virtual driver flow control - ack
 */
typedef struct _VDFLOWACK {
    USHORT MaxWindowSize;           /* maximum # of bytes we can write */
    USHORT WindowSize;              /* # of bytes we can write before blocking */
} VDFLOWACK, * PVDFLOWACK;

/*
 *  Virtual driver flow control - delay
 */
typedef struct _VDFLOWDELAY {
    ULONG DelayTime;                /* delay time in 1/1000 seconds */
} VDFLOWDELAY, * PVDFLOWDELAY;

/*
 *  Virtual driver flow control - cdm (client drive mapping)
 */
typedef struct _VDFLOWCDM {
    USHORT MaxWindowSize;           /* total # of bytes we can write */
    USHORT MaxByteCount;            /* maximum size of any one write */
} VDFLOWCDM, * PVDFLOWCDM;

/*
 *  Virtual driver flow control structure
 */
typedef struct _VDFLOW {
    BYTE BandwidthQuota;            /* percentage of total bandwidth (0-100) */
    BYTE Flow;                      /* type of flow control */
    BYTE Pad1[2];
    union _VDFLOWU {
        VDFLOWACK Ack;
        VDFLOWDELAY Delay;
        VDFLOWCDM Cdm;
    } VDFLOWU ;
} VDFLOW, * PVDFLOW;
/* // end_icasdk */
/* // end_binsdk */

/*
 *  Structure used to bind virtual channel name to number
 */
typedef struct _WD_VCBIND {
    VIRTUALNAME VirtualName;
    USHORT VirtualClass;
} WD_VCBIND, * PWD_VCBIND;


/*=============================================================================
==   Virtual Driver Header - client drive mapping
=============================================================================*/

typedef struct _VDCLIENTDRIVES {
    BYTE DriveLetter;
    BYTE Pad;
    USHORT Flags;
} VDCLIENTDRIVES, * PVDCLIENTDRIVES;

/*
 *  Client drive mapping flags (Flags)
 */
#define CDM_REMOVEABLE      0x0001
#define CDM_FIXED           0x0002
#define CDM_REMOTE          0x0004
#define CDM_CDROM           0x0008
#define CDM_RAMDISK         0x0010
#define CDM_AUTOCONNECT     0x0020
#define CDM_USBDISK         0x0040
#define CDM_READONLY		0x0080

typedef struct _VDCLIENTDRIVES2 {
    BYTE DriveLetter;
    BYTE NetworkType;
    USHORT oFileServer;
    USHORT oShareName;
    USHORT oCurrentDirectory;
} VDCLIENTDRIVES2, * PVDCLIENTDRIVES2;

/*
 *  Network type (NetworkType)
 */
#define CDM_NONE            0
#define CDM_LANMAN          1
#define CDM_NETWARE_3x      2
#define CDM_NETWARE_4x      3


/*=============================================================================
==   Client Data - Common data structures
=============================================================================*/

/*
 *  Client Data Name
 */
#define CLIENTDATANAME_LENGTH  7

typedef CHAR CLIENTDATANAME[ CLIENTDATANAME_LENGTH + 1 ];  /* includes null */
typedef CHAR * PCLIENTDATANAME;

/*
 *  Client data names  (CLIENTDATANAME)
 *
 *  name syntax:  xxxyyyy<null>
 *
 *    xxx    - oem id (CTX - Citrix Systems)
 *    yyyy   - client data name
 *    <null> - trailing null
 */

#define CLIENTDATA_SERVER      "CTXSRVR"   /* WF Server Name */
#define CLIENTDATA_USERNAME    "CTXUSRN"   /* WF User Name */
#define CLIENTDATA_DOMAIN      "CTXDOMN"   /* WF User Domain Name */
/* // begin_binsdk */

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD)
#pragma pack()
#endif

#endif /* __ICA_H__ */
/* // end_binsdk */
