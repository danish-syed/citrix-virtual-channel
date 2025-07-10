/**********************************************************************
*
* ctxping.h
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
* $Id$
*
* Copyright (C) Citrix Systems, Inc.  All Rights Reserved.
*
**********************************************************************/

#ifndef _CTXPING_H
#define _CTXPING_H


/*
 * General application definitions.
 */
#define SUCCESS 0
#define FAILURE 1

#define MAX_IDS_LEN   256   // maximum length that the input parm can be
#define MAXNAME 18          // Max allowed for printing.

/*
 * Resource string IDs
 */
#define IDS_ERROR_MALLOC                                100
#define IDS_ERROR_INVALID_PARAMETERS                    101
#define IDS_ERROR_OPEN_FAILED                           104
#define IDS_ERROR_WRITE_FAILED                          105
#define IDS_ERROR_READ_FAILED                           106
#define IDS_ERROR_WRITE_SHORT                           107
#define IDS_ERROR_READ_SHORT                            108
#define IDS_ERROR_CLIENT_PACKET_INVALID                 109
#define IDS_ERROR_CTXPING_FAIL                          110
#define IDS_ERROR_QUERY_FAIL                            111
#define IDS_ERROR_QUERY_SESSION_FAIL                    112
#define IDS_ERROR_NEEDS_SESSION                         113
#define IDS_OPENED_CHANNEL                              120
#define IDS_BEGIN_PACKET                                121
#define IDS_END_PACKET                                  122
#define IDS_SENDPING_SUCCESS                            123
#define IDS_INFO_SENT                                   124
#define IDS_INFO_RCVD                                   125
#define IDS_DATA_SENT                                   126
#define IDS_DATA_RCVD                                   127
#define IDS_QUERY                                       128
#define IDS_QUERY_C2H1                                  129
#define IDS_QUERY_C2H2                                  130
#define IDS_QUERY_C2H3                                  131
#define IDS_QUERY_C2H4                                  132
#define IDS_QUERY_C2H5                                  133
#define IDS_QUERY_C2H6                                  134
#define IDS_QUERY_C2H7                                  135 
#define IDS_QUERY_C2H8                                  136
#define IDS_QUERY_C2H9                                  137
#define IDS_QUERY_C2H10                                 138
#define IDS_ROUND_TRIP                                  139
#define IDS_SEND_FAILED                                 140
/*
 *  Local function prototypes
 */
BOOLEAN SendBeginAndEndPing( HANDLE hVC,
                             PPING pBufSendBegin, PPING pBufRecvBegin,
                             PPING pBufSendEnd, PPING pBufRecvEnd,
                             PULONG pulBeginTime, PULONG pulEndTime );
BOOLEAN SendPing( HANDLE hVC, PPING pSend, PPING pRecv, PULONG pulTime );
VOID WINAPI PrintMessage( int nResourceID, ...);


#endif /* _CTXPING_H */
