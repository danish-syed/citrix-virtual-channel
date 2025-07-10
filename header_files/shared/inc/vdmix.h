/**********************************************************************
*
* vdmix.h
*
* shared header for vdmix\ctxmix example program
*
* Copyright (C) Citrix Systems, Inc.  All Rights Reserved.
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
**********************************************************************/

#ifndef __VDMIX_H__
#define __VDMIX_H__

#include <ica.h>
#include <ica-c2h.h>        // for VD_C2H structure

#define CTXMIX_VIRTUAL_CHANNEL_NAME "Mix"
#define VC_TIMEOUT_MILLISECONDS  10000L
#define MAX_DATA                 2040
#define TEMP_SIZE                512

/*
 *  Enumeration of the functions that are implemented
 */
enum FUNCTIONS {ADD, DISP, TIME };

/*
 *   Functions used
 */

VOID AddNo(LPBYTE pBufIp);
VOID DispStr(LPBYTE pBufIp);
VOID LocTime(LPBYTE pBufIp);

/*
 * Lowest and highest compatable version.  See DriverInfo().
 */
#define CTXMIX_VER_LO 1
#define CTXMIX_VER_HI 1

#pragma pack(1)

/*
 * Mix data header structure
 */
typedef struct _MIXHEAD {
    USHORT      uType;              /* packet type */
    USHORT      uFunc;              /* Index of Function */
    ULONG       uLen;               /* Length of data */
    USHORT      fRetReq;            /* True if return value required */
    USHORT      dwLen1;             /* length of data for pointer arg #1 */
    USHORT   	dwLen2;             /* length of data for pointer arg #2  */
    ULONG       dwRetVal;           /* Return Value from client */
}  MIXHEAD, *PMIXHEAD;

/*
 * The arguments are located immediately following the above structure
 * The function being called knows how to interpret the data being 
 * received. For example if uFunc specifies ADD then the client 
 * interprets the next 8 bytes as 2 DWORDS. This cannot be extended for
 * a function receiving a Pointer argument. So dwLen1 and dwLen2 give
 * the length of the array for the pointer arguments. Example Disp_Str
 * knows that the length of the valid string following MIXHEAD is the
 * value in dwLen1. 
 */


/*
 * Vdmix driver info (client to host) structure
 */

typedef struct _VDMIX_C2H
{
    VD_C2H  Header;
    USHORT  usMaxDataSize;      // maximum data block size
} VDMIX_C2H, *PVDMIX_C2H;

/*
 *   This structure is added for compatability with WIN16
 */
typedef struct _vdtm {
      ULONG tm_sec;     /* seconds after the minute - [0,59] */
      ULONG tm_min;     /* minutes after the hour - [0,59] */
      ULONG tm_hour;    /* hours since midnight - [0,23] */
      ULONG tm_mday;    /* day of the month - [1,31] */
      ULONG tm_mon;     /* months since January - [0,11] */
      ULONG tm_year;    /* years since 1900 */
      ULONG tm_wday;    /* days since Sunday - [0,6] */
      ULONG tm_yday;    /* days since January 1 - [0,365] */
     ULONG tm_isdst;    /* daylight savings time flag */
} VDTM, *PVDTM;


#pragma pack()

#endif //__VDMIX_H__
