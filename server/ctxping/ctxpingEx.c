/**********************************************************************
*
* ctxping.c
*
* Example Virtual Channel application
*
* Send and receive packets to/from the VDPING virtual channel
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

/*
 *  Include files
 */
#include <windows.h> // Base headers
#include <stdio.h>
#include <tchar.h>

#include <wfapi.h>   // Citrix Server SDK

#include <vdping.h>  // Citrix shared header

#include "ctxping.h" // local header for CTXPING


 /*
  *  Global variables
  */
BOOLEAN vfDebug = FALSE;


/****************************************************************************
 *
 *  main
 *
 ***************************************************************************/

int __cdecl
main(INT argc, CHAR** argv)
{
    int         exitcode = SUCCESS;
    BOOLEAN     rc;
    HANDLE      hVC = NULL;
    LPTSTR      pSessionInfo = NULL;
    PPING       pBufBeginSend = NULL;
    PPING       pBufBeginRecv = NULL;
    PPING       pBufEndSend = NULL;
    PPING       pBufEndRecv = NULL;
    PVDPING_C2H pVdQbuf = NULL;
    PMODULE_C2H pmc2h = NULL;
    PVD_C2H     pvdc2h = NULL;
    DWORD       dwSize;
    DWORD       i;
    ULONG       ulBeginTime;
    ULONG       ulEndTime;
    ULONG       ulTotalTime = 0;
    DWORD       ByteCount;
    USHORT      usPingCount = 0;

    /*
     *  Determine if we are running in a session.
     */
    if (WFQuerySessionInformation(WF_CURRENT_SERVER_HANDLE,
        WF_CURRENT_SESSION,
        WFSessionId,
        &pSessionInfo,
        &ByteCount) == FALSE) {
        PrintMessage(IDS_ERROR_QUERY_SESSION_FAIL, GetLastError());
        exitcode = FAILURE;
        goto ExitMain;
    }
    else if ((INT)(*pSessionInfo) == 0) {     // session 0 is console
        PrintMessage(IDS_ERROR_NEEDS_SESSION);
        exitcode = FAILURE;
        goto ExitMain;
    }

    /*
     *  Look for a /d or -d to turn on debugging output.
     */
    if (argc > 1 && (argv[1][0] == '/' || argv[1][0] == '-') &&
        tolower(argv[1][1]) == 'd') {
        vfDebug = TRUE;
    }

    /*
     *  Allocate data buffers.
     */
    if ((pBufBeginSend = (PPING)calloc(1, sizeof(PING))) == NULL) {
        PrintMessage(IDS_ERROR_MALLOC);
        exitcode = FAILURE;
        goto ExitMain;
    }
    if ((pBufBeginRecv = (PPING)calloc(1, sizeof(PING))) == NULL) {
        PrintMessage(IDS_ERROR_MALLOC);
        exitcode = FAILURE;
        goto ExitMain;
    }
    if ((pBufEndSend = (PPING)calloc(1, sizeof(PING))) == NULL) {
        PrintMessage(IDS_ERROR_MALLOC);
        exitcode = FAILURE;
        goto ExitMain;
    }
    if ((pBufEndRecv = (PPING)calloc(1, sizeof(PING))) == NULL) {
        PrintMessage(IDS_ERROR_MALLOC);
        exitcode = FAILURE;
        goto ExitMain;
    }

    /*
     *  First, we want to open the channel and report the handle.
     *  If failure, print the error number.
     */
    hVC = WFVirtualChannelOpen(WF_CURRENT_SERVER,
        WF_CURRENT_SESSION,
        CTXPING_VIRTUAL_CHANNEL_NAME);
    if (hVC) {
        PrintMessage(IDS_OPENED_CHANNEL, hVC);
    }
    else {
        PrintMessage(IDS_ERROR_OPEN_FAILED, GetLastError());
        exitcode = FAILURE;
        goto ExitMain;
    }

    /*
     *  Now, query the channel to get data from the client.
     */
    rc = WFVirtualChannelQuery(hVC,
        WFVirtualClientData,
        &pVdQbuf,
        &dwSize);

    if (rc != TRUE) {
        PrintMessage(IDS_ERROR_QUERY_FAIL, GetLastError());
        exitcode = FAILURE;
        goto ExitMain;
    }

    /*
     *  Get the number of pings from the query results.
     */
    usPingCount = pVdQbuf->usPingCount;
    if (vfDebug) {
        printf("usPingCount = %d\n", usPingCount);
    }

    /*
     *  If debug, report the queried data.  First, dump the data in byte form,
     *  then break it apart to show the fields in the VDPING_C2H structure.
     */
    if (vfDebug) {
        USHORT day, month, year, second, minute, hour;

        PrintMessage(IDS_QUERY);
        printf("    ");
        for (i = 0; i < dwSize; i++) {
            if ((i + 1) % 26 == 0) {
                printf("\n    ");
            }
            printf("%02x ", *(((PBYTE)pVdQbuf) + i));
        }
        printf("\n\n");

        pvdc2h = &pVdQbuf->Header;
        pmc2h = &pvdc2h->Header;
        day = pmc2h->ModuleDate & 0x1f;           // bits 0-4
        month = (pmc2h->ModuleDate >> 5) & 0xf;   // bits 5-8
        year = (pmc2h->ModuleDate >> 9) & 0x7f;   // bits 9-15
        year += 1980;                             // offset from 1980
        second = pmc2h->ModuleTime & 0x1f;        // bits 0-4
        second *= 2;                              // packed format is sec/2
        minute = (pmc2h->ModuleTime >> 5) & 0x3f; // bits 5-10
        hour = (pmc2h->ModuleTime >> 11) & 0x1f;  // bits 11-15

        /*
         *  Display the results.  Note that ModuleName and HostModuleName are
         *  always ANSI strings.  We display them using printf() because if
         *  UNICODE is defined, a %s in the resource string means to plug in
         *  a UNICODE string.
         */

        PrintMessage(IDS_QUERY_C2H1, pmc2h->ByteCount);
        PrintMessage(IDS_QUERY_C2H2, pmc2h->ModuleCount);
        PrintMessage(IDS_QUERY_C2H3, pmc2h->ModuleClass);
        PrintMessage(IDS_QUERY_C2H4, pmc2h->VersionL);
        PrintMessage(IDS_QUERY_C2H5, pmc2h->VersionH);
        PrintMessage(IDS_QUERY_C2H6);
        printf("%s\n", pmc2h->ModuleName);
        PrintMessage(IDS_QUERY_C2H7);
        printf("%s\n", pmc2h->HostModuleName);
        PrintMessage(IDS_QUERY_C2H8, month, day, year);
        PrintMessage(IDS_QUERY_C2H9, hour, minute, second);
        PrintMessage(IDS_QUERY_C2H10, pmc2h->ModuleSize);
    }

    /*
     *  Next, let's send some data.  Initialize the counters.
     */
    pBufBeginSend->uCounter = 0;
    pBufEndSend->uCounter = 0;

    /*
     *  Ping a number of times times.  If one ping fails, stop pinging.
     */
    for (i = 0; i < usPingCount; i++) {

        rc = SendBeginAndEndPing(hVC, pBufBeginSend, pBufBeginRecv, pBufEndSend, pBufEndRecv, &ulBeginTime, &ulEndTime);
        if (rc != TRUE) {
            PrintMessage(IDS_SEND_FAILED, rc, GetLastError());
            break;
        }
        ulTotalTime += ulBeginTime + ulEndTime;
    }

    /*
     *  Report average round trip time.
     */
    if (i != 0) {
        PrintMessage(IDS_ROUND_TRIP, ulTotalTime, ulTotalTime / (i * 2));
    }

    /*
     *  Finally, close the channel and free allocated memory.
     */
ExitMain:
    if (pSessionInfo) {
        WFFreeMemory(pSessionInfo);
    }
    if (pVdQbuf) {
        WFFreeMemory(pVdQbuf);
    }
    if (hVC) {
        WFVirtualChannelClose(hVC);
    }
    if (pBufBeginSend) {
        free(pBufBeginSend);
    }
    if (pBufBeginRecv) {
        free(pBufBeginRecv);
    }
    if (pBufEndSend) {
        free(pBufEndSend);
    }
    if (pBufEndRecv) {
        free(pBufEndRecv);
    }
    if (exitcode == FAILURE) {
        PrintMessage(IDS_ERROR_CTXPING_FAIL);
    }


    return(exitcode);

} /* main() */


/****************************************************************************
 *
 *  SendBeginAndEndPing
 *
 *      Send a begin and end ping to the client and listen for responses.
 *
 *  ENTRY:
 *      pBufBeginSend (input)  begin packet to send to client
 *      pBufBeginRecv (output) begin packet received from client
 *      pBufEndSend   (input)  end packet to send to client
 *      pBufEndRecv   (output) end packet recieved from client
 *      pulBeginTime  (output) round trip time for BEGIN packet
 *      pulEndTime    (output) round trip time for END packet
 *
 *  EXIT:
 *      returns TRUE if successful, FALSE if not
 *
 ***************************************************************************/

BOOLEAN
SendBeginAndEndPing(HANDLE hVC,
    PPING pBufBeginSend, PPING pBufBeginRecv,
    PPING pBufEndSend, PPING pBufEndRecv,
    PULONG pulBeginTime, PULONG pulEndTime)
{
    BOOLEAN rc;

    /*
     *  Send begin packet and wait for response.
     */
    pBufBeginSend->uType = PING_TYPE_BEGIN;
    pBufBeginSend->uLen = sizeof(PING);
    pBufBeginRecv->uLen = sizeof(PING);

    if (vfDebug) {
        PrintMessage(IDS_BEGIN_PACKET);
    }
    rc = SendPing(hVC, pBufBeginSend, pBufBeginRecv, pulBeginTime);
    if (rc != TRUE) {
        goto Exit;
    }

    /*
     *  Send end packet and wait for response.
     */
    pBufEndSend->uType = PING_TYPE_END;
    pBufEndSend->uLen = sizeof(PING);
    pBufEndRecv->uLen = sizeof(PING);
    if (vfDebug) {
        PrintMessage(IDS_END_PACKET);
    }
    rc = SendPing(hVC, pBufEndSend, pBufEndRecv, pulEndTime);

    PrintMessage(IDS_SENDPING_SUCCESS, pBufEndRecv->uLen, *pulBeginTime,
        *pulEndTime);

Exit:
    return(rc);
} /* SendBeginAndEndPing() */


/****************************************************************************
 *
 *  SendPing
 *
 *      Send a Ping packet and wait for the response from the client.
 *
 *  ENTRY:
 *      hVC     (input)  handle from the WFVirtualChannelOpen call
 *      pSend   (input)  buffer to send to client
 *      pRecv   (output) buffer received from client
 *      pulTime (output) round trip time for packet
 *
 *  EXIT:
 *      returns TRUE if successful, FALSE if not
 *
 ***************************************************************************/

BOOLEAN
SendPing(HANDLE hVC, PPING pSend, PPING pRecv, PULONG pulTime)
{
    BOOLEAN rc = FALSE;
    ULONG ulen;
    int i;

    pSend->uSign = VC_PING_SIGNATURE;
    pSend->ulServerMS = GetTickCount();
    pSend->uCounter++;

    /*
     *  Send the packet to the client.
     */
    rc = WFVirtualChannelWrite(hVC,
        (PCHAR)pSend,
        pSend->uLen,
        &ulen);

    if (rc != TRUE) {
        PrintMessage(IDS_ERROR_WRITE_FAILED, GetLastError());
        goto ExitSendPing;
    }

    if (pSend->uLen != ulen) {
        PrintMessage(IDS_ERROR_WRITE_SHORT, ulen);
        rc = FALSE;
        goto ExitSendPing;
    }

    /*
     *  Print debugging information -- send ping packet and contents.
     */
    if (vfDebug) {
        PrintMessage(IDS_INFO_SENT);
        PrintMessage(IDS_DATA_SENT, pSend->uSign, pSend->uType,
            pSend->uLen, pSend->ulServerMS);
        printf("          ");
        for (i = 0; i < sizeof(PING); i++) {
            printf("%02x ", *(((PBYTE)pSend) + i));
        }
        printf("\n");
    }

    /*
     *  Read response.
     */
    rc = WFVirtualChannelRead(hVC,
        VC_TIMEOUT_MILLISECONDS,
        (PCHAR)pRecv,
        pRecv->uLen,
        &ulen);

    if (rc != TRUE) {
        PrintMessage(IDS_ERROR_READ_FAILED, GetLastError());
        goto ExitSendPing;
    }

    if (pRecv->uLen != ulen) {
        PrintMessage(IDS_ERROR_READ_SHORT, ulen);
        rc = FALSE;
        goto ExitSendPing;
    }

    *pulTime = GetTickCount() - pRecv->ulServerMS;

    /*
     *  Print debugging information -- received ping packet and contents.
     */
    if (vfDebug) {
        PrintMessage(IDS_INFO_RCVD);
        PrintMessage(IDS_DATA_RCVD, pRecv->uSign, pRecv->uType, pRecv->uLen,
            pRecv->ulServerMS, pRecv->ulClientMS);
        printf("          ");
        for (i = 0; i < sizeof(PING); i++) {
            printf("%02x ", *(((PBYTE)pRecv) + i));
        }
        printf("\n");
    }

    /*
     *  Client echoes back some of buffer; see if it is right.
     */
    if ((pSend->uSign != pRecv->uSign) ||
        (pSend->uType != pRecv->uType) ||
        (pSend->uLen != pRecv->uLen) ||
        (pSend->ulServerMS != pRecv->ulServerMS)) {
        PrintMessage(IDS_ERROR_CLIENT_PACKET_INVALID);
        rc = FALSE;
        goto ExitSendPing;
    }

    rc = TRUE;

ExitSendPing:
    return(rc);

} /* SendPing() */


/*******************************************************************************
 *
 *  PrintMessage
 *      Display a message to stdout with variable arguments.  Message
 *      format string comes from the application resources.
 *
 *  ENTRY:
 *      nResourceID (input)  Resource ID of the format string to use in
 *                           the message.
 *      ...         (input)  Optional additional arguments to be used
 *                           with format string.
 *
 *  EXIT:
 *      none
 *
 ******************************************************************************/

VOID WINAPI
PrintMessage(int nResourceID, ...)
{
    WCHAR str1[MAX_IDS_LEN + 1] = { 0 };
    WCHAR str2[2 * MAX_IDS_LEN + 1] = { 0 };

    va_list args;
    va_start(args, nResourceID);

    if (LoadString(NULL, nResourceID, (LPTSTR)str1, sizeof(str1) / sizeof(*str1))) {

        vswprintf_s(str2, 2 * MAX_IDS_LEN + 1, str1, args);
        _tprintf(str2);

    }
    else {

        _ftprintf(stderr,
            _TEXT("PrintMessage(): LoadString %d failed, Error %ld)\n"),
            nResourceID, GetLastError());
    }

    va_end(args);

}  /* PrintMessage() */

