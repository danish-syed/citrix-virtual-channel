/**********************************************************************
*
* vdover.h
*
* shared header for ctxping/vdover example program
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

#ifndef __VDOVER_H__
#define __VDOVER_H__

#include <ica.h>
#include <ica-c2h.h>        /* for VD_C2H structure */

#define CTXOVER_VIRTUAL_CHANNEL_NAME  "CTXOVER"
#define VC_TIMEOUT_MILLISECONDS         10000L
#define VC_OVER_SIGNATURE               0x4920
/*
 * Lowest and highest compatable version.  See DriverInfo().
 */
#define CTXOVER_VER_LO      1
#define CTXOVER_VER_HI      1

#define INT_CNT             5
#pragma pack(1)


/* Valid uType fields*/

#define OVER_TYPE_BEGIN     1
#define OVER_TYPE_END       2
#define OVERFLOW_JUMP       3


/* OverFlow packet structure */

typedef struct _OVER {
    USHORT  uSign;              /* signature */
    USHORT  uType;              /* type, BEGIN or END, from server */
    USHORT  uLen;               /* packet length from server */
    USHORT  uCounter;           /* sequencer */
    ULONG   ulServerMS;         /* server millisecond clock */
} OVER, * POVER;

typedef struct _DRVRESP {
    USHORT  uType;              /* type OVERFLOW_JUMP from client */
    USHORT  uLen;               /* packet length from client */
    USHORT  uCounter;           /* sequencer */
} DRVRESP, * PDRVRESP;

/* vdover driver info (client to host) structure */
typedef struct _VDOVER_C2H
{
    VD_C2H  Header;
    USHORT  usMaxDataSize;      /* maximum data block size */
} VDOVER_C2H, * PVDOVER_C2H;

                     

/* Local Functions */

BOOLEAN ProcessDataReceived(POVER );

#pragma pack()

#endif /* __VDOVER_H__ */
