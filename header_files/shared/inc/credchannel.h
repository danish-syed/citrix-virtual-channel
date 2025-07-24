/**********************************************************************
*
* credchannel.h
*
* shared header for credchannel/credserver program
*
* Copyright (C) Credenti LLC.  All Rights Reserved.
*
**********************************************************************/




/* for VD_C2H structure */
#include "ica.h"
#include "ica-c2h.h"

#define CTXCRED_VIRTUAL_CHANNEL_NAME  "CTXCRED"
#define VC_TIMEOUT_MILLISECONDS     10000L
#define MAX_DATA                 4080
#define TEMP_SIZE                512

/*
 * General application definitions.
 */
#define SUCCESS 0
#define FAILURE 1

#define MAX_IDS_LEN   256   // maximum length that the input parm can be


#define CTXCRED_VER_LO      1
#define CTXCRED_VER_HI      1

 /*
  * Resource string IDs
  */
#define IDS_ERROR_MALLOC                                100
#define IDS_ERROR_OPEN_FAILED                           101
#define IDS_ERROR_WRITE_FAILED                          102
#define IDS_ERROR_READ_FAILED                           103
#define IDS_ERROR_WRITE_SHORT                           104
#define IDS_ERROR_READ_SHORT                            105
#define IDS_ERROR_CTXMIX_FAIL                           106
#define IDS_ERROR_NEEDS_SESSION                         107
#define IDS_ERROR_QUERY_SESSION_FAIL                    108
#define IDS_OPENED_CHANNEL                              109



#pragma pack(1)

typedef struct _CREDPARAM
{
	ULONG uLen;
	USHORT Flag;
	USHORT UsernameLen;
	/*USHORT PasswordLen;
	USHORT Domain;*/
} CREDPARAM, * PCREDPARAM;

typedef struct _VDCRED
{
	VD_C2H Header;
	USHORT usMaxDataSize;

} VDCRED, * PVDCRED;

