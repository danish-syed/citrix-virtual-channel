/**********************************************************************
*
* vdblast.h
*
* shared header for ctxping/vdblst example program
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

#ifndef __VDBLST_H__
#define __VDBLST_H__

#include <ica.h>
#include <ica-c2h.h>        /* for VD_C2H structure */

#define CTXBLST_VIRTUAL_CHANNEL_NAME  "CTXBLST"
#define VC_TIMEOUT_MILLISECONDS         10000L
#define VC_BLST_SIGNATURE               0x4920   /* Not a required field */
#define BIGPACKET                       64000  /* Big Chunk of data to be
                                                    transfered in one time */
/*
 * Lowest and highest compatable version.  See DriverInfo().
 */
#define CTXBLST_VER_LO      1
#define CTXBLST_VER_HI      1

/* Max data being send in each packet */
#define MAX_DATASIZE        2032    /* actual size of the data */
#pragma pack(1)


/* Valid uType fields*/

#define BLST_TYPE_BEGIN     1
#define BLST_TYPE_END       2

/* Blast packet structure */

typedef struct _BLST {
    USHORT  uSign;              /* signature */
    USHORT  uType;              /* type, BEGIN or END */
    USHORT  uLen;               /* packet length */
    USHORT  uCounter;           /* sequencer */
    UCHAR   uDataBuf[MAX_DATASIZE]; /* the buffer to hold the actual data*/
    USHORT  ulDataLen;           /* data length being sent */
} BLST, * PBLST;

/* Note that the total packet size should not be more than 2044 bytes
 * So the size of the data ie MAX_DATASIZE should be 
 * 2044 - (size of the other fields in packet being sent)
 * In this case 2044 - [ ( 4* 2 (for USHORT)) + (1 * 4 (for ULONG))] 
 * 2044 - [ 8 + 4 ]  -> 2044 - 12 = 2032
 */

/* vdblst driver info (client to host) structure */
typedef struct _VDBLST_C2H
{
    VD_C2H  Header;
    USHORT  usMaxDataSize;      /* maximum data block size */
} VDBLST_C2H, * PVDBLST_C2H;

                     

/* Local Functions */

USHORT ProcessDataReceived( );

#pragma pack()

#endif /* __VDBLST_H__ */
