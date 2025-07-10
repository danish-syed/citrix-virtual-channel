/* // begin_binsdk */
/*++ BUILD Version: 0000 ******************************************************
*
*  ICA-C2H.H   client -> host
*
*     This file contains structures that are sent by the client to
*     the host at connection time.
*
*  Copyright 1998-2015 Citrix Systems, Inc. 
*
*  This is an unpublished work protected as such under copyright law,
*  containing proprietary and confidential information of Citrix Systems, Inc.
*  Use, disclosure or reproduction without the express written authorization
*  of Citrix Systems, Inc. is prohibited.
*
*******************************************************************************/

#ifndef __ICAC2H_H__
#define __ICAC2H_H__

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_ENABLE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD) || (defined(USE_PACK_PRAGMA) && defined(linux)) 
#pragma pack(1)
#endif
/* // end_binsdk */

#if defined(ENABLE_FONT_SMOOTHING) || !defined(linux)
#define VERSION_FONT_SMOOTHING 
#endif



#include <icaid.h>

#define IMEFILENAME_LENGTH 32
/* // begin_binsdk */
/* // begin_icasdk */

/*=============================================================================
==   Common Module Header
=============================================================================*/

/*
 *  Common module header (36 bytes)
 */
typedef struct _MODULE_C2H {
    USHORT ByteCount;               /* length of module data in bytes (<2k) */
    BYTE ModuleCount;               /* number of modules headers left to be sent */
    BYTE ModuleClass;               /* module class (MODULECLASS) */
    BYTE VersionL;                  /* lowest supported version */
    BYTE VersionH;                  /* highest supported version */
    BYTE ModuleName[13];            /* client module name (8.3) */
    BYTE HostModuleName[9];         /* optional host module name (9 characters) */
    USHORT ModuleDate;              /* module date in dos format */
    USHORT ModuleTime;              /* module time in dos format */
    ULONG ModuleSize;               /* module file size in bytes */
} MODULE_C2H, * PMODULE_C2H;
/* // end_icasdk */
/* // end_binsdk */

typedef struct _IMEFILENAME {
      BYTE imeFileName[ IMEFILENAME_LENGTH + 1 ];     /* IME file name (length 32) */
} IMEFileName, * pIMEFileName;

/*=============================================================================
==   User Interfaces
=============================================================================*/

#define VERSION_CLIENTL_UI     1
#define VERSION_CLIENTH_UI_15  2    /* WinFrame 1.5 */
#define VERSION_CLIENTH_UI_16  3    /* WinFrame 1.6 */
#define VERSION_CLIENTH_UI_PICASSO 4 /* picasso server */
#define VERSION_CLIENTH_UI_HUDSON  5 /* Hudson server */
#define VERSION_CLIENTH_UI_UNICODE 5 /* wf17 unicode enabled server */
#define VERSION_CLIENTH_UI_COLORADO 6 /* Colorado server */
#define VERSION_CLIENTH_UI_OHIO     7 /* Ohio server */
#define VERSION_CLIENTH_UI_DELAWARE  8 /* Delaware server */

#ifdef VERSION_FONT_SMOOTHING
#define VERSION_CLIENTH_UI     VERSION_CLIENTH_UI_DELAWARE
#else
#define VERSION_CLIENTH_UI     VERSION_CLIENTH_UI_OHIO
#endif

#define VERSION_CLIENTL_UIEXT  1
#define VERSION_CLIENTH_UIEXT  1

/*
 *  User Interface  (winlink.exe)
 */
typedef struct _UI_C2H {

    /* version 1 */
    MODULE_C2H Header;
    ULONG Flags;		/* Flag definitions follow. */

#define UI_C2H_DISABLE_SOUND        0x00000001 /* don't send sound to client */
#define UI_C2H_RECONNECT            0x00000002 /* request to reconnect to existing session */
#define UI_C2H_DISABLE_CAD          0x00000004	/* disable ctrl-alt-del on host for this client */
#define UI_C2H_PROMPT_PASSWD        0x00000008	/* force server to prompt for password */
#define UI_C2H_SEAMLESS             0x00000010	/* Seamless mode or not */
#define UI_C2H_WIN_KEYS             0x00000020	/* Client can send left and right windows keys through */
#define UI_C2H_SGCONNECTION	        0x00000040 /* connection is using Secure Gateway */
#define UI_C2H_CT_SMARTCARD         0x00000800 /* Password is a smartcard PIN (added in version 6) */
#define UI_C2H_CT_AADLOGONTICKET    0x00001000 /* Password is a AzureAD Logon Ticket */
#define UI_C2H_CT_RCG               0x00002000 /* Single Sign On is using remote credential guard */

    ULONG KeyboardLayout;       /* e.g. 409 */
    BYTE EncryptionLevel;       /* encryption level of password (0=plain text) */
    BYTE EncryptionSeed;        /* seed for encryption */
    USHORT oDomain;             /* offset - domain of user database */
    USHORT oUserName;           /* offset - user's logon name */
	// VANPORT_SECURITY_REVIEW_CHECK_VARIABLE_DONE
    USHORT oPassword;           /* offset - user's password */
    USHORT oClientDirectory;    /* offset - directory containing winlink.exe */
    USHORT oWorkDirectory;      /* offset - user's working directory (initial dir) */
    USHORT oInitialProgram;     /* offset - user's initial program */
    BYTE Lpt1;                  /* connect this host lpt to (1=lpt1 .. 8=lpt8) */
    BYTE Port1;                 /* ... this client port (1=lpt1 .. 8=com4) */
    BYTE Lpt2;                  /* connect this host lpt to (1=lpt1 .. 8=lpt8) */
    BYTE Port2;                 /* ... this client port (1=lpt1 .. 8=com4) */
    BYTE Lpt3;                  /* connect this host lpt to (1=lpt1 .. 8=lpt8) */
    BYTE Port3;                 /* ... this client port (1=lpt1 .. 8=com4) */

    /* version 2
     * I collapsed 158 2 and 3 down to 2 because the VERSION_CLIENTH_UI
     * was set to 2 even though the comments below included version 2 and
     * version 3 prior to my version 3.
     */
    USHORT oCloudName;          /* offset - appserver cloud name for load-balance */
    USHORT ClientBuildNumber;   /* client build number */
    USHORT oClientName;         /* offset - name of client */
    USHORT FixedLength;         /* sizeof this structure */
    USHORT OffsetLength;        /* sizeof any following data */

    /* version 3 */
    USHORT oClientLicense;      /* offset - client license number */

    /* version 4 */
    USHORT EncodingType;        /* Encoding type */
                                /* - 0: ANSI/ASCII */
                                /* - 1: UNICODE */
    USHORT EncodingData;        /* Encoding data ( encoding type dependent ) */
                                /* - for ANSI/ASCII - code page */
                                /*                    0: system default CP */
                                /*                    1: system OEM CP */
	 USHORT KeyboardType;        /* Keyboard type (PC/XT, PC/AT, Japanese...) */
	 BYTE KeyboardSubType;       /* Keyboard subtype (US 101, JPN 106) */
	 BYTE KeyboardFunctionKey;   /* Number of function keys */
    USHORT oimeFileName;        /* Offset to IME file name */
    USHORT KeyboardSubtype2;    /* keep the structure at multiple of 4 bytes */

	/* version 5 */
	USHORT oLauncher;    /* offset to client launcher. values: PNAgent, WI, PN Classic/Others, etc. */
	USHORT oVersion;      /* offset to client version. values ex: 7.00.12345 */

    /* version 6 */             /* Added new flags in version 6 */

    /* version 7 */
#define UI_C2H_TWI_ICON_4BPP	 (BYTE)0x01
#define UI_C2H_TWI_ICON_8BPP     (BYTE)0x02
#define UI_C2H_TWI_ICON_16BPP	 (BYTE)0x04
#define UI_C2H_TWI_ICON_24BPP    (BYTE)0x08
#define UI_C2H_TWI_ICON_32BPP    (BYTE)0x10

    BYTE PreferredIconBPP; /*preferred icon color depth, in bitsPerPixel; normal values: 4, 8, 16, 24, 32*/
    USHORT oLaunchReference;   /* offset to the launch reference string, max 256 */

#ifdef VERSION_FONT_SMOOTHING
    /* version 8 */

    USHORT oFontSmoothing;   /* Offset to font smoothing structure */
#endif
} UI_C2H, * PUI_C2H;

/*
 *  User Interface extension  (uicwin.dll)
 */
typedef struct _UIEXT_C2H {
    MODULE_C2H Header;
} UIEXT_C2H, * PUIEXT_C2H;

#define FONT_SMOOTHING_INITIAL_VERSION	1
#define FONT_SMOOTHING_NONE			    0x0000
#define FONT_SMOOTHING_STANDARD 	    0x0001
#define FONT_SMOOTHING_CLEARTYPE	    0x0002
#define FONT_SMOOTHING_SERVER_DEFAULT   0x0003


typedef struct _FONT_SMOOTHING {
	USHORT ByteCount;	/*Length of the structure and any associated data in bytes*/
	USHORT Version;	/*Version of the structure*/
	USHORT Type;	/*Smoothing type (only pertinent if Font Smoothing is allowed) */
} FONT_SMOOTHING, * PFONT_SMOOTHING;

/*=============================================================================
==   Winstation Driver
=============================================================================*/

/*
 *  Version 1 - SouthBeach 1.29
 *  Version 3 - WinFrame 1.5 (build 158)
 *  Version 4 - Internet Client (new)
 *  Version 5 - WinFrame 2.0 beta
 *  Version 6 - WinFrame 2.0
 *  Version 7 - Picasso with WD reducer
 *  Version 8 - WinFrame with WD reducer / unicode support
 *  Version 9 - Capability lists
 */
 
#define VERSION_CLIENTL_WD   1 
#define MIN_WD_REDUCER_CLIENT_VERSION  8
#define MIN_WF_WD_REDUCER_CLIENT_VERSION 8
#define VERSION_CLIENTH_WD   9

/*
 *  The following defines set out the minmum Module header versions 
 *  for each PD/virtual channel which supports capability negotiation
 */
#define WD_MIN_C2H_MODULE_CAPABILITIES_VERSION  9

/*
 *  ica 3.0 (wdica30.dll)
 */
typedef struct _WD_C2H {

    /* version 1,2,3,4 */
    MODULE_C2H Header;
    ULONG SerialNumber;         /* client computer unique serial number */
    struct {
        BYTE day;               /* 1-31 */
        BYTE month;             /* 1-12 */
        USHORT year;            /* 1980-2099 */
    } CurrentDate;              /* client computer date */
    struct {
        BYTE hour;              /* 0-23 */
        BYTE minute;            /* 0-59 */
        BYTE second;            /* 0-59 */
        BYTE hsecond;           /* 0-99 */
    } CurrentTime;              /* client computer time */
    ULONG Flags;		/* Flag definitions follow */

#define WD_C2H_GRAPHICS         0x00000001 /* graphics are supported */
#define WD_C2H_MOUSE            0x00000002 /* mouse is available */
#define WD_C2H_DOUBLE_CLICK     0x00000004 /* client will detect double clicks (ver 6) */
#define WD_C2H_WINDOWS_KEY      0x00000008 /* Windows key will be supported by the server */
#define WD_C2H_AUTO_RECONNECT   0x00000010 /* This is an auto reconnect attempt */
#define WD_C2H_PASSTHROUGH      0x00000020 /* This is a pass-through session */

    USHORT TextModeCount;       /* number of supported text modes */
    USHORT oTextMode;           /* offset - array of WDTEXTMODE structures */
    USHORT ICABufferLength;     /* maximum size of one ica packet */
    USHORT OutBufCountHost;     /* number of outbufs on host */
    USHORT OutBufCountClient;   /* number of outbufs on client */
    USHORT OutBufDelayHost;     /* host outbuf write delay (1/1000 seconds) */
    USHORT OutBufDelayClient;   /* client outbuf write delay (1/1000 seconds) */
    USHORT ClientProductId;     /* client software product id */
    ULONG  ClientSerialNumber;  /* client software serial number */

    /* version 5,6 */
    USHORT VcBindCount;         /* number of WDVCBIND structures in array */
    USHORT oVcBind;             /* offset - array of WDVCBIND structures */

    /* version 7 */
    BYTE C2H_PowerOf2Wanted;    /* reduction buffer requested */
    BYTE H2C_PowerOf2Wanted;    /* expansion buffer requested */
    USHORT H2C_MaxNewData;      /* maximum expanded packet size */
    USHORT ReducerVersion;      /* version of reducer */

    /* version 8 */
    USHORT capListLength;       /* Length of the capability list */
    USHORT oCapList;		/* offset to the list of capability blocks */

    /* All future extensions should be negotiated via capability blocks. */
} WD_C2H, * PWD_C2H;

/*=============================================================================
==   Virtual Drivers
=============================================================================*/

/*
 *  Version 1 - SouthBeach 1.29
 *  Version 2 - WinFrame 1.6    (256 color)
 *  Version 3 - WinFrame 2.0    (dim support)
 *  Version 4 - MetaFrame 2.0   
          (thinwire 1.5 : 
                    16bpp, 24bpp connetion support
                    high resolution support up to 64K *64K
          )
 */

#define VERSION_CLIENTL_VDTW   1
#define VERSION_CLIENTH_VDTW   5 /* changed to include new TW2 capabilities */

#define VERSION_CLIENTL_VDCDM  1
#define VERSION_CLIENTH_VDCDM  8 /* Must track cdmwire.h CDM_VERSIONx */

#define VERSION_CLIENTL_VDCPM  1
#define VERSION_CLIENTH_VDCPM  1

#define VERSION_CLIENTL_VDCCM  4
#define VERSION_CLIENTH_VDCCM  4

#define VERSION_CLIENTL_VDSPL  4 /* Do not set lower, means old client */
#define VERSION_CLIENTH_VDSPL  7 /* Must track cpmwire.h CPM_MAX_VERSION */

#define VERSION_CLIENTL_VDCOM  4 /* Do not set lower, means old client */
#define VERSION_CLIENTH_VDCOM  4 /* Must track ccmwire.h CCM_MAX_VERSION */

/*
 * Lowest and highest versions supported by the Smart Card Virtual Channel.
 */
#define VERSION_CLIENTL_VDSCARD  1 /* Lowest supported version */
#define VERSION_CLIENTH_VDSCARD  1 /* Hishest supported version */

/*
 * Lowest and highest versions supported by the Multimedia Virtual Channel.
 */
#define VERSION_CLIENTL_VDMM  1 /* Lowest supported version */
#define VERSION_CLIENTH_VDMM  2 /* Hishest supported version */

/*
 * Lowest and highest versions supported by the MultiTouch Virtual Channel.
 */
#define VERSION_CLIENTL_VDMT  1 /* Lowest supported version */
#define VERSION_CLIENTH_VDMT  2 /* Hishest supported version */

 /*
 * Lowest and highest versions supported by the NetScaler AppFlow Virtual Channel.
 */
#define VERSION_CLIENTL_VDNSAP  1
#define VERSION_CLIENTH_VDNSAP  1

/*
 * Lowest and highest versions supported by the GDT Virtual Channel.
 */
#define VERSION_CLIENTL_VDGDT  1
#define VERSION_CLIENTH_VDGDT  1

/*
 * Lowest and highest versions supported by the DND Virtual Channel.
 */
#define VERSION_CLIENTL_VDDND  1
#define VERSION_CLIENTH_VDDND  1

/*
 * Lowest and highest compatable versions supported by the Port Forward Virtual Channel.
 */
#define VERSION_CLIENTL_VDPORTFORWARD  1
#define VERSION_CLIENTH_VDPORTFORWARD  1

/*
* Lowest and highest compatable versions supported by the Client-Side Browser Virtual Channel.
*/
#define VERSION_CLIENTL_VDBROWSER  1
#define VERSION_CLIENTH_VDBROWSER  1

/*
 * Lowest and highest versions supported by the FIDO Virtual Channel.
 */
#define VERSION_CLIENTL_VDFIDO  1
#define VERSION_CLIENTH_VDFIDO  1

/*
 * Lowest and highest versions supported by the WIA Virtual Channel.
 */
#define VERSION_CLIENTL_VDWIA  1
#define VERSION_CLIENTH_VDWIA  1

/*
 * Lowest and highest compatable versions supported by the MTOP (Teams/WebRTC Redirection) Channel.
 */
#define VERSION_CLIENTL_VDTEAMS  1
#define VERSION_CLIENTH_VDTEAMS  1
/* // begin_binsdk */
/* // begin_icasdk */

/*
 *  common virtual driver header
 */
typedef struct _VD_C2H {
    MODULE_C2H Header;

    /* This field is interpreted as follows:
     * 
     * If the CAPABILITY_VIRTUAL_MAXIMUM is not supported by both the client and the host then:
     *   -  The ChannelMask field is interpreted as a bit mask of Virtual Channels (VCs) 
     *      supported by a client Virtual Driver (VD), where b0=0.
     *   -  Always a single VD_C2H structure is sent per client VD.
     *
     * Otherwise, if the CAPABILITY_VIRTUAL_MAXIMUM is supported by both the client and the host then:
     *   -  The ChannelMask field is interpreted as a single VC number.
     *   -  If a single client VD module handles more than one VC, then for this VD 
     *      multiple VD_C2H structures are sent - one for each VC that the VD supports. 
     *      These modules are sent in separate PACKET_INIT_RESPONSE packets and are 
     *      identical to each other except for the Header.ModuleCount and ChannelMask fields.
     */
    ULONG ChannelMask; 

    VDFLOW Flow;
} VD_C2H, * PVD_C2H;
/* // end_icasdk */
/* // end_binsdk */

/*
 *  client drive mapping (vdcdm30.dll)
 */
typedef struct _VDCDM_C2H {

    /* version 1 */
    VD_C2H Header;
    USHORT ClientDriveCount;    /* number of client drives */
    USHORT oClientDrives;       /* offset - array of VDCLIENTDRIVES structures */

    USHORT CacheFlags;
    USHORT CacheTimeout0;
    USHORT CacheTimeout1;
    USHORT CacheXferSize;

    /* version 2 */
    USHORT oClientDrives2;      /* offset - array of VDCLIENTDRIVES2 structures */

    /* version 3 */
    USHORT EncodingType;        /* Encoding type */
    USHORT EncodingData;        /* Encoding data ( depending on type ) */

    /* version 6 */ 
    USHORT SpecialFoldersCount;  /* Number of SFR element */
    USHORT oSpecialFolders;      /* Offset array of CDM_SPECIAL_FOLDER structures */

	/* version 8 */
	ULONG  HighThroughputWindowSize; /* Bigger Window Size to improve CDM VC performance */ 
} VDCDM_C2H, * PVDCDM_C2H;

/*
 *  client printer mapping (vdcpm30.dll)
 */
typedef struct _VDCPM_C2H {

    /* version 1 */
    VD_C2H Header;
    BYTE LptMask;               /* mask of available client lpt ports (b0=lpt1) */
    BYTE ComMask;               /* mask of available client com ports (b0=com1) */

    /* version 2 */
    USHORT EncodingType;        /* Encoding type */
    USHORT EncodingData;        /* Encoding data ( depending on type ) */

    /* version 3 */
    USHORT Capabilities;        /*  non-zero if VDSPL management commands
                                 * (CREATE PRINTER, DELETE PRINTER) are
                                 * supported.
                                 */

} VDCPM_C2H, * PVDCPM_C2H;


/*
 *  Data structure for the Smart Card Virtual Channel (vdscard[x].dll) passed by
 *  the client to the host at connection time.
 */
typedef struct _SCARDVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                 *  The common virtual driver header, i.e., the
                                 *  usual VD header for PACKET_INIT_RESPONSE ICA
                                 *  packets.
                                 */
    USHORT  cbMaxDataSize;      /*
                                 *  The maximum data packet the client can
                                 *  accept and/or send including the 4-byte
                                 *  packet overhead generated by the ICA data-
                                 *  stream manager.
                                 */
} SCARDVD_C2H, *PSCARDVD_C2H;


/*
 *  Data structure for the Multimedia Virtual Channel (vdmm[x].dll) passed by
 *  the client to the host at connection time.
 */
typedef struct _MMVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                 *  The common virtual driver header, i.e., the
                                 *  usual VD header for PACKET_INIT_RESPONSE ICA
                                 *  packets.
                                 */
    USHORT  cbMaxDataSize;      /*
                                 *  The maximum data packet the client can
                                 *  accept and/or send including the 4-byte
                                 *  packet overhead generated by the ICA data-
                                 *  stream manager.
                                 */
} MMVD_C2H, *PMMVD_C2H;


typedef struct _MTVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                 *  The common virtual driver header, i.e., the
                                 *  usual VD header for PACKET_INIT_RESPONSE ICA
                                 *  packets.
                                 */
    USHORT  cbMaxDataSize;      /*
                                 *  The maximum data packet the client can
                                 *  accept and/or send including the 4-byte
                                 *  packet overhead generated by the ICA data-
                                 *  stream manager.
                                 */
} MTVD_C2H, *PMTVD_C2H;


/*
*  Data structure for the NSAP Virtual Channel (vdnsap[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _NSAPVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                *  The common virtual driver header, i.e., the
                                *  usual VD header for PACKET_INIT_RESPONSE ICA
                                *  packets.
                                */
    UINT16  cbMaxDataSize;      /*
                                *  The maximum data packet the client can
                                *  accept and/or send including the 4-byte
                                *  packet overhead generated by the ICA data-
                                *  stream manager.
                                */
    UINT8   Reserved;
} NSAPVD_C2H, *PNSAPVD_C2H;


/*
*  Data structure for the GDT Virtual Channel (vdgdt[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _GDTVD_C2H
{
	/* Version 1 */
	VD_C2H  Header;             /*
								*  The common virtual driver header, i.e., the
								*  usual VD header for PACKET_INIT_RESPONSE ICA
								*  packets.
								*/
	UINT16  cbMaxDataSize;      /*
								*  The maximum data packet the client can
								*  accept and/or send including the 4-byte
								*  packet overhead generated by the ICA data-
								*  stream manager.
								*/
	UINT8   Reserved;
} GDTVD_C2H, *PGDTVD_C2H;


/*
*  Data structure for the DND Virtual Channel (vddnd[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _DNDVD_C2H
{
	/* Version 1 */
	VD_C2H  Header;             /*
								*  The common virtual driver header, i.e., the
								*  usual VD header for PACKET_INIT_RESPONSE ICA
								*  packets.
								*/
	UINT16  cbMaxDataSize;      /*
								*  The maximum data packet the client can
								*  accept and/or send including the 4-byte
								*  packet overhead generated by the ICA data-
								*  stream manager.
								*/
	UINT8   Reserved;
} DNDVD_C2H, *PDNDVD_C2H;


/*
 *  Data structure for the Port Forward Virtual Channel (vdportfoward[x].dll) passed by
 *  the client to the host at connection time.
 */
typedef struct _VDPORTFORWARD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                 *  The common virtual driver header, i.e., the
                                 *  usual VD header for PACKET_INIT_RESPONSE ICA
                                 *  packets.
                                 */
    USHORT  cbMaxDataSize;      /*
                                 *  The maximum data packet the client can
                                 *  accept and/or send including the 4-byte
                                 *  packet overhead generated by the ICA data-
                                 *  stream manager.
                                 */
    UINT32 Reserved;            /*
                                *  Reserved for future use.  Set to zero.
                                */
} VDPORTFORWARD_C2H, *PVDPORTFORWARD_C2H;


/*
*  Data structure for the Client-Side Browser Virtual Channel (vdcsb[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _VDBROWSER_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                *  The common virtual driver header, i.e., the
                                *  usual VD header for PACKET_INIT_RESPONSE ICA
                                *  packets.
                                */
    USHORT  cbMaxDataSize;      /*
                                *  The maximum data packet the client can
                                *  accept and/or send including the 4-byte
                                *  packet overhead generated by the ICA data-
                                *  stream manager, but excluding the 1-byte
                                *  overhead for the virtual channel number.
                                */
    UINT32 Reserved;            /*
                                *  Reserved for future use.  Set to zero.
                                */
} VDBROWSER_C2H, *PVDBROWSER_C2H;


/*
*  Data structure for the Client-Side Teams Virtual Channel (vdteams[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _VDTEAMS_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                *  The common virtual driver header, i.e., the
                                *  usual VD header for PACKET_INIT_RESPONSE ICA
                                *  packets.
                                */
    USHORT  cbMaxDataSize;      /*
                                *  The maximum data packet the client can
                                *  accept and/or send including the 4-byte
                                *  packet overhead generated by the ICA data-
                                *  stream manager, but excluding the 1-byte
                                *  overhead for the virtual channel number.
                                */
    UINT32 Reserved;            /*
                                *  Reserved for future use.  Set to zero.
                                */
} VDTEAMS_C2H, *PVDTEAMS_C2H;

/*
*  Data structure for the Fido Virtual Channel (vdfido[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _FIDOVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                *  The common virtual driver header, i.e., the
                                *  usual VD header for PACKET_INIT_RESPONSE ICA
                                *  packets.
                                */
    UINT16  cbMaxDataSize;      /*
                                *  The maximum data packet the client can
                                *  accept and/or send including the 4-byte
                                *  packet overhead generated by the ICA data-
                                *  stream manager.
                                */
    UINT8   Reserved;
} FIDOVD_C2H, *PFIDOVD_C2H;

/*
*  Data structure for the WIA Virtual Channel (vdwia[x].dll) passed by
*  the client to the host at connection time.
*/
typedef struct _WIAVD_C2H
{
    /* Version 1 */
    VD_C2H  Header;             /*
                                *  The common virtual driver header, i.e., the
                                *  usual VD header for PACKET_INIT_RESPONSE ICA
                                *  packets.
                                */
    UINT16  cbMaxDataSize;      /*
                                *  The maximum data packet the client can
                                *  accept and/or send including the 4-byte
                                *  packet overhead generated by the ICA data-
                                *  stream manager.
                                */
    UINT8   Reserved;
} WIAVD_C2H, *PWIAVD_C2H;
/*=============================================================================
==   Protocol Driver Header
=============================================================================*/

#define VERSION_CLIENTL_PDFRAME     1
#define VERSION_CLIENTH_PDFRAME     1

#define VERSION_CLIENTL_PDRFRAME    1
#define VERSION_CLIENTH_PDRFRAME    2

#define VERSION_CLIENTL_PDRELIABLE  1
#define VERSION_CLIENTH_PDRELIABLE  1

#define VERSION_CLIENTL_PDCOMPRESS  1
#define VERSION_CLIENTH_PDCOMPRESS  1

#define VERSION_CLIENTL_PDCRYPT1    1
#define VERSION_CLIENTH_PDCRYPT1    1

#define VERSION_CLIENTL_PDMODEM     1
#define VERSION_CLIENTH_PDMODEM     1

#define VERSION_CLIENTL_PDTAPI		1
#define VERSION_CLIENTH_PDTAPI		1

/*
 *  common protocol driver header
 */
typedef struct _PD_C2H {
    MODULE_C2H Header;
    BYTE PdClass;                 /* class of protocol driver (PDCLASS) */
    BYTE Pad1[3];
} PD_C2H, * PPD_C2H;

/*
 *  framing protocol driver (pdframe.dll)
 */
typedef struct _PDFRAME_C2H {
    PD_C2H Header;
} PDFRAME_C2H, * PPDFRAME_C2H;

/*
 *  compression protocol driver (pdcomp.dll)
 */
typedef struct _PDCOMPRESS_C2H {
    PD_C2H Header;
    USHORT MaxCompressDisable;  /* disable compression for this many outbufs */
    USHORT Pad;
} PDCOMPRESS_C2H, * PPDCOMPRESS_C2H;

/*
 *  compression protocol driver (pdcrypt1.dll)
 */
typedef struct _PDCRYPT1_C2H {
    PD_C2H Header;
    BYTE EncryptionLevel;       /* encryption level */
} PDCRYPT1_C2H, * PPDCRYPT1_C2H;

/*=============================================================================
==   Transport Driver Header
=============================================================================*/

#define VERSION_CLIENTL_TDTCPVSL  1
#define VERSION_CLIENTH_TDTCPVSL  2

/*
 * NOTE: Currently, none of the ICA Clients have added code for the HDX-24767 feature
 * and do not fill in the version 3 fields of the TD_C2H structure. So they will send 
 * the TD_C2H structure with a verion number of 2 (VERSION_CLIENTH_TDTCPVSL) - so the 
 * VDA will not look at the version 3 fields.
 *
 * When an ICA Client gets around to adding code for the HDX-24767 feature, it will
 * fill in the version 3 fields and can send VERSION_CLIENTH_TD_VER3 to the VDA.
 */
#define VERSION_CLIENTH_TD_VER3   3

/*
 *  transport driver
 */
typedef struct _TD_C2H {

    /* version 1 */
    MODULE_C2H Header;
    USHORT OutBufLength;        /* length of outbufs in bytes */

    /* version 2 */
    USHORT ClientAddressFamily; /* address family of ClientAddress */
    BYTE ClientAddress[20];     /* client network address */

    /* version 3 */
    USHORT EncryptionProtocol;          /* Encryption Protocol */
    USHORT EncryptionCipher;            /* Encryption Cipher (per https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-4) */
    USHORT EncryptionCipherStrength;    /* Encryption Cipher Strength, in bits */
} TD_C2H, *PTD_C2H;

#ifndef MacOrUnix		/* In socket.h on these systems. */
/*
 * Address families.
 */
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#undef AF_IPX   // there are different defines for this in winsock.h and winsock2.h
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_INET6        23              /* Internetwork Version 6 */
#endif /* MacOrUnix */

/* Encryption Protocol */
#define TD_C2H_ENCRYPT_NONE     0x0000   /* No Transport-Level encryption */
#define TD_C2H_ENCRYPT_SSL30    0x0300   /* SSL 3.0 */
#define TD_C2H_ENCRYPT_TLS10    0x0301   /* TLS 1.0 */
#define TD_C2H_ENCRYPT_TLS11    0x0302   /* TLS 1.1 */
#define TD_C2H_ENCRYPT_TLS12    0x0303   /* TLS 1.2 */
#define TD_C2H_ENCRYPT_TLS13    0x0304   /* TLS 1.3 */
#define TD_C2H_ENCRYPT_DTLS10   0xFEFF   /* DTLS 1.0 */
#define TD_C2H_ENCRYPT_DTLS12   0XFEFD   /* DTLS 1.2 */

/* // end_binsdk */


/*=============================================================================
==   Protocol Resolver Header
=============================================================================*/

typedef struct _PR_C2H {
    MODULE_C2H Header;
} PR_C2H, * PPR_C2H;


/*=============================================================================
==   Scripting Header
=============================================================================*/

typedef struct _SCRIPT_C2H {
    MODULE_C2H Header;
} SCRIPT_C2H, * PSCRIPT_C2H;

/* // begin_binsdk */

#ifdef PLATFORM_PRAGMAS
#pragma ICA_PACKING_RESTORE
#elif defined(WINFRAME_BUILD) || defined(METAFRAME_BUILD) || (defined(USE_PACK_PRAGMA) && defined(linux))
#pragma pack()
#endif

#endif /* __ICAC2H_H__ */
/* // end_binsdk */
