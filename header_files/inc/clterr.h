/* Copyright 2002-2011 Citrix Systems, Inc. */
/***************************************************************************
*
*  CLTERR.H
*
*  This module contains the client error and status codes.
*
*  Version: $Id: //icaclientcore/main/src/inc/clterr.h#19 
*
*  Copyright 1995-2015 Citrix Systems, Inc. All Rights Reserved.
*
***************************************************************************/
#ifndef _CLTERR_H_
#define _CLTERR_H_

#include "errorcodes.h"



// this is duplicated with CLIENT_STATUS_RETRIEVE_ICA_AUTH_DECISION in errorcodes.h
#define CLIENT_STATUS_TWI_CREATED_APP_WINDOW        74
#define CLIENT_STATUS_CL_STATUS                     83   /* connection lease status */
// this is duplicated with CLIENT_ERROR_APP_PROTECTION_FAILED in client_errors.h
#define CLIENT_ERROR_SENDMSG_TO_CCMREDIRECTINFO   1123







/* prototypes for functions to make strings from codes or print such */
void display_ICA_status(char *string, int code);
BOOL status_code_to_message(char *string, int code, char *message);



// Range of 3000 to 3200 is reserved for connection lease error
#define CLIENT_ERROR_NO_LEASEFILE_PRESENT             3000
#define CLIENT_ERROR_LEASEFILE_VERSION_MISMATCH       3001
#define CLIENT_ERROR_COULD_NOT_READ_LEASEFILE         3002
#define CLIENT_ERROR_NO_ZONEFOUND                     3003
#define CLIENT_ERROR_CONNECTIONDETAIL_MISSING         3004
#define CLIENT_ERROR_ICAFILE_EMPTY                    3005
#define CLIENT_ERROR_CONNECTIONLEASE_EXPIRED          3006
#define CLIENT_ERROR_CONNECTIONLEASE_INVALID          3007
#define CLIENT_ERROR_NO_VSR                           3008
#define CLIENT_ERROR_VSR_INVALID                      3009
#define CLIENT_ERROR_PARAMETER_MISSING                3010
#define CLIENT_ERROR_CLIENT_SIGNATURE_NOTFOUND        3011
#define CLIENT_ERROR_CLISPUBKEYFILE_NOTFOUND          3012
#define CLIENT_ERROR_PUBKEYFILE_EMPTY                 3013
#define CLIENT_ERROR_CLIS_SIGNATURE_NOTFOUND          3014
#define CLIENT_ERROR_ROTPUBKEYFILE_NOTFOUND           3015
#define CLIENT_ERROR_CLIS_SIGNATUREVALIDATION_FAILED  3016
#define CLIENT_ERROR_ROT_SIGNATUREVALIDATION_FAILED   3017
#define CLIENT_ERROR_SIGNATUREVALIDATION_FAILED       3018
#define CLIENT_ERROR_DECRYPTION_FAILED                3019
#define CLIENT_ERROR_LEASEVALIDATION_FAILED           3020
#define CLIENT_ERROR_RUNOUTOF_HOST_INALLZONE          3021

#endif // _CLTERR_H_
