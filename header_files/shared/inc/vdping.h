/**********************************************************************
*
* vdping.h
*
* shared header for ctxping/vdping example program
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

#ifndef __VDPING_H__
#define __VDPING_H__

#include <ica.h>
#include <ica-c2h.h>        /* for VD_C2H structure */

#define CTXPING_VIRTUAL_CHANNEL_NAME  "CTXPING"
#define VC_TIMEOUT_MILLISECONDS     10000L
#define VC_PING_SIGNATURE           0x4950

/*
 * Lowest and highest compatable version.  See DriverInfo().
 */
#define CTXPING_VER_LO      1
#define CTXPING_VER_HI      1

#pragma pack(1)

/* ping packet structure */

typedef struct _PING {
    USHORT  uSign;              /* signature */
    USHORT  uType;              /* type, BEGIN or END, from server */
    USHORT  uLen;               /* packet length from server */
    USHORT  uCounter;           /* sequencer */
    ULONG   ulServerMS;         /* server millisecond clock */
    ULONG   ulClientMS;         /* client millisecond clock */
} PING, * PPING;
#define PING_TYPE_BEGIN     1
#define PING_TYPE_END       2

/* vdping driver info (client to host) structure */
typedef struct _VDPING_C2H
{
    VD_C2H  Header;
    USHORT  usMaxDataSize;      /* maximum data block size */
    USHORT  usPingCount;        /* number of times to ping */
} VDPING_C2H, * PVDPING_C2H;

#pragma pack()

#endif /* __VDPING_H__ */
