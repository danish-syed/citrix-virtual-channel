/********************************************************************
*
* testall.c
*
* Citrix Server SDK WFAPI Component Example Program
*
*
* Citrix hereby permits you to use, reproduce, modify, display, compile,
* sublicense and distribute all or any portion of the sample code contained
* in this file, provided that you remove this Copyright block from any
* in this file, provided that you remove this Modifications Copyright 2005-2010 Citrix Systems, Inc.
* redistributed versions of this file.
*
* THIS SAMPLE CODE IS PROVIDED BY CITRIX "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
*
**********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <wchar.h>    // for _getws()
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <lmaccess.h> // for NetGet[Any]DCName()
#include <lmerr.h>    // for NERR_Success 
#include <lmwksta.h>  // for WKSTA_INFO_100
#include "wfapi.h"
#include <wtsapi32.h>
#include <winsock2.h>
#include <Inaddr.h>
#include <In6addr.h>

#define TEST_LOCAL  0
#define TEST_DOMAIN 1

#define CTXWS_GUIDSTRLENGTH          40

#define SMC_STRUCTURE_VERSION_V1 1
#define SMC_STRUCTURE_VERSION_V2 2

#define MEMORY_ALLOCATION_SIZE 256

typedef LONG (*L_RTLIPV4ADDRESSTOSTRINGEXW)(const IN_ADDR *, USHORT, LPWSTR, PULONG);
typedef LONG (*L_RTLIPV6ADDRESSTOSTRINGEXW)(const IN6_ADDR *, ULONG, USHORT, LPWSTR, PULONG);


static L_RTLIPV4ADDRESSTOSTRINGEXW gpfnRtlIpv4AddressToStringExW = NULL;
static L_RTLIPV6ADDRESSTOSTRINGEXW gpfnRtlIpv6AddressToStringExW = NULL;


/********************************************************************
 *
 *  EnumerateServers
 *
 *    Display a list of Citrix servers within the specified Windows
 *    NT domain
 *
 * ENTRY:
 *    pDomainName (input)
 *       Pointer to Windows NT domain name (or NULL for current
 *       domain)
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/

void
EnumerateServers( LPTSTR pDomainName )
{
    PWF_SERVER_INFO pServerInfo;
    DWORD Count;
    DWORD i;

    _tprintf( TEXT("\nWFEnumerateServers: domain %s\n"), pDomainName );

    if ( !WFEnumerateServers( pDomainName,
                              0,    // reserved
                              1,    // version 1
                              &pServerInfo,
                              &Count ) ) {

        _tprintf( TEXT("*** WFEnumerateServers failed, error %u\n"),
                  GetLastError() );
        return;
    }

    _tprintf( TEXT("WFEnumerateServers: count %u\n"), Count );

    for ( i=0; i < Count; i++ ) {
        _tprintf( TEXT("%s "), pServerInfo[i].pServerName );
    }
    _tprintf( TEXT("\n") );

    WFFreeMemory( pServerInfo );
} // EnumerateServers()



/********************************************************************
 *
 *  WFWindowPositionSDKTest
 *
 *    Verify that the Window Position SDK is working correctly
 *
 *
 *******************************************************************/
void WFWindowPositionSDKTest()
{
    TCHAR    consoleTitle[MAX_PATH];
    TCHAR    origFirstChar;
    DWORD    retVal;
    HWND    hwnd;

    WF_RESPONSE_METHOD    commMethod;
    HANDLE                myMailSlot;
    UINT                myId;

    DWORD nextSize = 0;
    DWORD messageCount = 0;
    DWORD numRead = 0;
    OVERLAPPED ov = {0};
    WF_WINDOW_DATA message;

    WINDOWINFO    windowInfo;

    _tprintf( TEXT("\nStarting Window Positioning SDK Test\n") );

    // We need a window to test, so we'll use the console window
    retVal = GetConsoleTitle(consoleTitle, MAX_PATH);
    if(retVal <= 0)
    {
         _tprintf( TEXT("Failed to get console window title\n") );
         goto done;
    }
    
    // Change the console title temporarily so we can find it easier
    origFirstChar = consoleTitle[0];
    consoleTitle[0] = L'$';
    if(!SetConsoleTitle(consoleTitle))
    {
         _tprintf( TEXT("Failed to set console window title to unique identifier\n") );
         goto done;
    }

    // get console window handle
    Sleep(40);
    hwnd = FindWindow(NULL, consoleTitle);
    if(!hwnd)
    {
         _tprintf( TEXT("Failed to find console window\n") );
         consoleTitle[0] = origFirstChar;
         SetConsoleTitle(consoleTitle);
         goto done;
    }
    
    // Restore console window
    consoleTitle[0] = origFirstChar;
    SetConsoleTitle(consoleTitle);

    /*Actual test starts here*/
    commMethod.Type = WFMailslotResponse;
    commMethod.Params.MailSlot.WaitTime = MAILSLOT_WAIT_FOREVER; 
    commMethod.Params.MailSlot.pSecurityAttributes = NULL;
    if(!WFSubscribe(&myId, &commMethod))
    {
        _tprintf( TEXT("Failed to subscribe Error = %u\n"), GetLastError() );
        goto done;
    }
    myMailSlot = commMethod.Params.MailSlot.hSlot;

    if(!WFStartTrackingWindow(myId, (UINT32)PtrToLong(hwnd)))
    {
        _tprintf( TEXT("Failed to track window %d: Error = %u\n"), (UINT32)PtrToLong(hwnd), GetLastError() );
        goto unsubscribe;
    }

    Sleep(1000);    // Wait a second just in case mailslot message is delayed

    // Get the initial window data that was sent
    if(!GetMailslotInfo( myMailSlot, NULL, &nextSize, &messageCount, NULL))
    {
        _tprintf( TEXT("Failed to get initial mailslot info\n") );
        goto unsubscribe;
    }
    if(messageCount == 0)
    {
        _tprintf( TEXT("No message received\n") );
        goto unsubscribe;
    }
    if(nextSize != sizeof(WF_WINDOW_DATA))
    {
        _tprintf( TEXT("Message received wrong size: is %d should be %d\n"), nextSize, sizeof(WF_WINDOW_DATA) );
        goto unsubscribe;
    }

    if(FAILED( ReadFile(myMailSlot, (LPVOID)&message, sizeof(WF_WINDOW_DATA), &numRead, &ov) ))
    {
        _tprintf( TEXT("Failed to read message from mailslot\n") );
        goto unsubscribe;
    }
        
    /* Compare received data to data gathered from GetWindowInfo */
    windowInfo.cbSize = sizeof(WINDOWINFO);
    if(!GetWindowInfo(hwnd, &windowInfo))
    {
        _tprintf( TEXT("Failed to get window info\n") );
        goto unsubscribe;
    }

    // Basic test: compare the window rectangles and their client area rectangles
    if(!EqualRect(&windowInfo.rcWindow, (RECT*)&message.RclBounds))
    {
        _tprintf( TEXT("Window rectangles don't match: Actual (%l, %l) - (%l, %l) != Data (%l, %l) - (%l, %l)\n"), 
            windowInfo.rcWindow.left, windowInfo.rcWindow.top, windowInfo.rcWindow.right, windowInfo.rcWindow.bottom,
            message.RclBounds.left, message.RclBounds.top, message.RclBounds.right, message.RclBounds.bottom);
        goto unsubscribe;
    }
    if(!EqualRect(&windowInfo.rcClient, (RECT*)&message.RclClient))
    {
        _tprintf( TEXT("Window client rectangles don't match: Actual (%l, %l) - (%l, %l) != Data (%l, %l) - (%l, %l)\n"), 
            windowInfo.rcClient.left, windowInfo.rcClient.top, windowInfo.rcClient.right, windowInfo.rcClient.bottom,
            message.RclClient.left, message.RclClient.top, message.RclClient.right, message.RclClient.bottom);
        goto unsubscribe;
    }

    /* Stop tracking the window */
    if(!WFStopTrackingWindow(myId, (UINT32)PtrToLong(hwnd)))
    {
        _tprintf( TEXT("Failed to stop tracking window %d: Error = %u\n"), (UINT32)PtrToLong(hwnd), GetLastError());
        goto unsubscribe;
    }

    
unsubscribe:
    if(!WFUnsubscribe(myId))
    {
        _tprintf( TEXT("Failed to unsubscribe user %u: Error = %u\n"), myId, GetLastError() );
    }
    CloseHandle(myMailSlot);
done:
    _tprintf( TEXT("End of Window Positioning SDK Test\n") );
} // WFWindowPositionSDKTest()

/********************************************************************
 *
 *  EnumerateSessions
 *
 *    Display a list of ICA sessions on the specfied server
 *
 * ENTRY:
 *    hServer (input)
 *       handle to server
 *
 * EXIT:
 *    nothing
 *
 ********************************************************************/

void
EnumerateSessions( LPTSTR lpServerName, HANDLE hServer )
{
    PWF_SESSION_INFO pSessionInfo;
    DWORD Count;
    DWORD i;

    _tprintf( TEXT("\nWFEnumerateSessions: server %s (handle 0x%x)\n"),
              lpServerName,(INT_PTR) hServer );

    if ( !WFEnumerateSessions( hServer,
                               0,    // reserved
                               1,    // version 1
                               &pSessionInfo,
                               &Count ) ) {

        _tprintf( TEXT("*** WFEnumerateSessions failed, error %u\n"),
                  GetLastError() );
        return;
    }

    _tprintf( TEXT("WFEnumerateSessions: count %u\n"), Count );

    for ( i=0; i < Count; i++ ) {
        _tprintf( TEXT("%-5u  %-20s  %u\n"),
                  pSessionInfo[i].SessionId,
                  pSessionInfo[i].pWinStationName,
                  pSessionInfo[i].State );
    }

    WFFreeMemory( pSessionInfo );
} // EnumerateSessions()

/********************************************************************
 *
 *  FormatTime
 *
 *    Convert a FILETIME to readable time.
 *
 * ENTRY:
 *    time (input)
 *       Time to be converted.
 *    s (output)
 *       Readable time format.
 *
 *******************************************************************/
// HDX-8669: CID 15213 (#1 of 2)
void
FormatTime( LARGE_INTEGER time, TCHAR* s, int buffsize )
{
    FILETIME lt;
    SYSTEMTIME st;

    FileTimeToLocalFileTime((FILETIME*)&time, &lt);
    FileTimeToSystemTime(&lt, &st);
    _stprintf_s(s, buffsize, TEXT("%02d/%02d/%04d %02d:%02d:%02d.%04d"),
        st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond,
        st.wMilliseconds);
}

/********************************************************************
 *
 *  QuerySession
 *
 *    Query and display session data
 *
 * ENTRY:
 *    hServer (input)
 *       handle to server
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/
void QuerySession( HANDLE hServer, DWORD SessionId, WF_INFO_CLASS InfoClass )
{
    WF_CONNECTSTATE_CLASS * pConnectState;
    LPTSTR pSessionInfo;
    DWORD ByteCount;
    LPOSVERSIONINFO pVerInfo;
    PWF_CLIENT_DISPLAY pClientDisplay;
    PWF_CLIENT_CACHE   pClientCache;
    PWF_CLIENT_ADDRESS pClientAddress;
    PWF_CLIENT_DRIVES pClientDrives;
    PWF_CLIENT_INFO pClientInfo;
    PWF_USER_INFO pUserInfo;
    PWF_APP_INFO pAppInfo;
    PWF_CLIENT_LATENCY pLatency;
    DWORD * pDword;
    WORD * pWord;
    BYTE * pByte;
    ULONG i;
    PWF_SESSION_TIME pSessionTime;
    TCHAR TimeStr[80];

    _tprintf( TEXT("\nWFQuerySessionInformation: Server 0x%x, Session %d, Class %u\n"), (ULONG_PTR)hServer, SessionId, InfoClass );



    if ( !WFQuerySessionInformation( hServer,
                                     SessionId,
                                     InfoClass,
                                     &pSessionInfo,
                                     &ByteCount ) ) {

        _tprintf( TEXT("*** WFQuerySessionInformation failed, error %u\n"),
                  GetLastError() );
        return;
    }

    switch ( InfoClass ) {
        case WFVersion :
            pVerInfo = (LPOSVERSIONINFO) pSessionInfo;
            _tprintf( TEXT("MFVersion: major %u, minor %u, build %u, CSD Version: %s\n"),
                      pVerInfo->dwMajorVersion, pVerInfo->dwMinorVersion,
                      pVerInfo->dwBuildNumber, pVerInfo->szCSDVersion);
            break;
       
        case WFInitialProgram :
            _tprintf( TEXT("WFInitialProgram: %s\n"), pSessionInfo );
            break;
        case WFSessionId :
            pDword = (DWORD *) pSessionInfo;
            _tprintf( TEXT("WFSessionId: %u\n"), *pDword );
            break;
        case WFOEMId :
            _tprintf( TEXT("WFOEMId: %s\n"), pSessionInfo );
            break;
        case WFConnectState :
            pConnectState = (WF_CONNECTSTATE_CLASS*) pSessionInfo;
            _tprintf( TEXT("WFConnectState: %u\n"), *pConnectState );
            _tprintf( TEXT("\n\
States are: 0 = WFActive        User logged on to WinStation\n\
            1 = WFConnected     WinStation connected to client\n\
            2 = WFConnectQuery  In the process of connecting to client\n\
            3 = WFShadow        Shadowing another WinStation\n\
            4 = WFDisconnected  WinStation logged on without client\n\
            5 = WFIdle          Waiting for client to connect\n\
            6 = WFListen        WinStation is listening for connection\n\
            7 = WFReset         WinStation is being reset\n\
            8 = WFDown          WinStation is down due to error\n\
            9 = WFInit          WinStation in initialization\n") );
            break;
        case WFWorkingDirectory :
            _tprintf( TEXT("WFWorkingDirectory: %s\n"), pSessionInfo );
            break;

        case WFWinStationName :
            _tprintf( TEXT("WFWinStationName: %s\n"), pSessionInfo );
            break;

        case WFUserName :
            _tprintf( TEXT("WFUserName: %s\n"), pSessionInfo );
            break;

        case WFDomainName :
            _tprintf( TEXT("WFDomainName: %s\n"), pSessionInfo );
            break;
        case WFClientBuildNumber :
            pWord = (WORD *) pSessionInfo;
            _tprintf( TEXT("WFClientBuildNumber: %u\n"), *pWord );
            break;
        case WFClientName :
            _tprintf( TEXT("WFClientName: %s\n"), pSessionInfo );
            break;

        case WFClientDirectory :
            _tprintf( TEXT("WFClientDirectory: %s\n"), pSessionInfo );
            break;
        case WFClientAddress :
            pClientAddress = (PWF_CLIENT_ADDRESS) pSessionInfo;
            _tprintf( TEXT("WFClientAddress: (%u) "),
                      pClientAddress->AddressFamily );
            switch ( pClientAddress->AddressFamily ) {
                case AF_UNSPEC :
                    printf( "%s", pClientAddress->Address );
                    break;
                case AF_INET :
                    if (gpfnRtlIpv4AddressToStringExW == NULL)
                    {
                        HMODULE  hNtDll = NULL;

                        if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
                        {
                            gpfnRtlIpv4AddressToStringExW = (L_RTLIPV4ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv4AddressToStringExW");
                        }
                    }

                    if (gpfnRtlIpv4AddressToStringExW != NULL)
                    {
                        WCHAR  ClientAddress[64] = { L'\0' };
                        ULONG  ClientAddressLength = 64;
                        USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

                        // The first two bytes are reserved for port. The next 4 bytes are the IP Address
                        (*gpfnRtlIpv4AddressToStringExW)( (IN_ADDR *)&pClientAddress->Address[2], ClientPort, ClientAddress, &ClientAddressLength );
                        wprintf( L"%s", ClientAddress );
                    }
                    break;
                case AF_INET6:
                    if (gpfnRtlIpv6AddressToStringExW == NULL)
                    {
                        HMODULE  hNtDll = NULL;

                        if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
                        {
                            gpfnRtlIpv6AddressToStringExW = (L_RTLIPV6ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv6AddressToStringExW");
                        }
                    }

                    if (gpfnRtlIpv6AddressToStringExW != NULL)
                    {
                        WCHAR  ClientAddress[64] = { L'\0' };
                        ULONG  ClientAddressLength = 64;
                        USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

                        // The first two bytes are reserved for port. The next 4 bytes are the IP Address
                        (*gpfnRtlIpv6AddressToStringExW)( (IN6_ADDR *)&pClientAddress->Address[2], 0, ClientPort, ClientAddress, &ClientAddressLength );
                        wprintf( L"%s", ClientAddress );
                    }
                    break;
                case AF_IPX :
                    pByte = pClientAddress->Address;
                    for ( i=0; i<4; i++ )
                        _tprintf( TEXT("%02x"), pByte[i] );
                    _tprintf( TEXT(":") );
                    for ( i=4; i<10; i++ )
                        _tprintf( TEXT("%02x"), pByte[i] );
                    break;
            }
            _tprintf( TEXT("\n") );
            break;

       case WFClientDisplay :
            pClientDisplay = (PWF_CLIENT_DISPLAY) pSessionInfo;
            _tprintf( TEXT("WFClientDisplay: hres %u, vres %u, depth %u\n"),
                      pClientDisplay->HorizontalResolution,
                      pClientDisplay->VerticalResolution,
                      pClientDisplay->ColorDepth );
            break;

    case WFClientInfo :
            pClientInfo = (PWF_CLIENT_INFO)pSessionInfo;
            _tprintf( TEXT("WFClientInfo:\n") );
            _tprintf( TEXT("    Name: %s\n"), pClientInfo->Name);
            _tprintf( TEXT("    Directory: %s\n"), pClientInfo->Directory);
            _tprintf( TEXT("    BuildNumber: %d\n"), pClientInfo->BuildNumber);
            _tprintf( TEXT("    ProductID: 0x%x\n"), pClientInfo->ProductID);
            _tprintf( TEXT("    HardwareID: 0x%x\n"), pClientInfo->HardwareID);
            pClientAddress = &pClientInfo->Address;
            _tprintf( TEXT("    WFClientAddress: (%u) "), pClientAddress->AddressFamily );
            switch ( pClientAddress->AddressFamily ) {
                case AF_UNSPEC :
                    printf( "%s", pClientAddress->Address );
                    break;
                case AF_INET :
                    if (gpfnRtlIpv4AddressToStringExW == NULL)
                    {
                        HMODULE  hNtDll = NULL;

                        if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
                        {
                            gpfnRtlIpv4AddressToStringExW = (L_RTLIPV4ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv4AddressToStringExW");
                        }
                    }

                    if (gpfnRtlIpv4AddressToStringExW != NULL)
                    {
                        WCHAR  ClientAddress[64] = { L'\0' };
                        ULONG  ClientAddressLength = 64;
                        USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

                        // The first two bytes are reserved for port. The next 4 bytes are the IP Address
                        (*gpfnRtlIpv4AddressToStringExW)( (IN_ADDR *)&pClientAddress->Address[2], ClientPort, ClientAddress, &ClientAddressLength );
                        wprintf( L"%s", ClientAddress );
                    }
                    break;
                case AF_INET6:
                    if (gpfnRtlIpv6AddressToStringExW == NULL)
                    {
                        HMODULE  hNtDll = NULL;

                        if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
                        {
                            gpfnRtlIpv6AddressToStringExW = (L_RTLIPV6ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv6AddressToStringExW");
                        }
                    }

                    if (gpfnRtlIpv6AddressToStringExW != NULL)
                    {
                        WCHAR  ClientAddress[64] = { L'\0' };
                        ULONG  ClientAddressLength = 64;
                        USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

                        // The first two bytes are reserved for port. The next 4 bytes are the IP Address
                        (*gpfnRtlIpv6AddressToStringExW)( (IN6_ADDR *)&pClientAddress->Address[2], 0, ClientPort, ClientAddress, &ClientAddressLength );
                        wprintf( L"%s", ClientAddress );
                    }
                    break;
                case AF_IPX :
                    pByte = pClientAddress->Address;
                    for ( i=0; i<4; i++ )
                        _tprintf( TEXT("%02x"), pByte[i] );
                    _tprintf( TEXT(":") );
                    for ( i=4; i<10; i++ )
                        _tprintf( TEXT("%02x"), pByte[i] );
                    break;
            }
            _tprintf( TEXT("\n") );
            break;
   // HDX-8669: CID 15213 (#2 of 2)
   case WFSessionTime :
            pSessionTime = (PWF_SESSION_TIME)pSessionInfo;
            _tprintf( TEXT("WFSessionTime:\n") );
            FormatTime(pSessionTime->DisconnectTime, TimeStr, ARRAYSIZE(TimeStr));
            _tprintf( TEXT("    Disconnect time: %016I64X (%s)\n"),
                pSessionTime->DisconnectTime.QuadPart, TimeStr);
            FormatTime(pSessionTime->ConnectTime, TimeStr, ARRAYSIZE(TimeStr));
            _tprintf( TEXT("    Connect time:    %016I64X (%s)\n"),
                pSessionTime->ConnectTime.QuadPart, TimeStr);
            FormatTime(pSessionTime->LastInputTime, TimeStr, ARRAYSIZE(TimeStr));
            _tprintf( TEXT("    Last input time: %016I64X (%s)\n"),
                pSessionTime->LastInputTime.QuadPart, TimeStr);
            FormatTime(pSessionTime->LogonTime, TimeStr, ARRAYSIZE(TimeStr));
            _tprintf( TEXT("    Logon time:      %016I64X (%s)\n"),
                pSessionTime->LogonTime.QuadPart, TimeStr);
            FormatTime(pSessionTime->CurrentTime, TimeStr, ARRAYSIZE(TimeStr));
            _tprintf( TEXT("    Current time:    %016I64X (%s)\n"),
                pSessionTime->CurrentTime.QuadPart, TimeStr);
            break;
      case WFUserInfo :
            pUserInfo = (PWF_USER_INFO)pSessionInfo;
            _tprintf( TEXT("WFUserInfo:\n") );
            _tprintf( TEXT("    User Name: %s\n"), pUserInfo->UserName);
            _tprintf( TEXT("    Domain Name: %s\n"), pUserInfo->DomainName);
            _tprintf( TEXT("    Connection Name: %s\n"),
                pUserInfo->ConnectionName);
            break;
      case WFAppInfo :
            pAppInfo = (PWF_APP_INFO)pSessionInfo;
            _tprintf( TEXT("WFAppInfo:\n") );
            _tprintf( TEXT("    Initial Program: %s\n"),
                pAppInfo->InitialProgram);
            _tprintf( TEXT("    Working Directory: %s\n"),
                pAppInfo->WorkingDirectory);
            _tprintf( TEXT("    Application Name: %s\n"),
                pAppInfo->ApplicationName);
            break;
     case WFClientProductId :
            pWord = (WORD *) pSessionInfo;
            _tprintf( TEXT("WFClientProductId: 0x%x\n"), *pWord );
            break;
     case WFClientLatency :
            pLatency = (PWF_CLIENT_LATENCY)pSessionInfo;
            _tprintf( TEXT("WFClientLatency:\n") );
            _tprintf( TEXT("    Last latency: %d ms\n"), pLatency->Last);
            _tprintf( TEXT("    Average latency: %d ms\n"),
                pLatency->Average);
            _tprintf( TEXT("    Latency deviation: %d ms\n"),
                pLatency->Deviation);
            break;
        case WFClientCache :
            pClientCache = (PWF_CLIENT_CACHE) pSessionInfo;
            _tprintf( TEXT("WFClientCache: (mem) tiny %u, low %u, xms %u, disk %u\n"),
                      pClientCache->CacheTiny,
                      pClientCache->CacheLowMem,
                      pClientCache->CacheXms,
                      pClientCache->CacheDisk );
            _tprintf( TEXT("WFClientCache: (dim) size %u, min %u, level %u, overhead %u\n"),
                      pClientCache->DimCacheSize,
                      pClientCache->DimBitmapMin,
                      pClientCache->DimSignatureLevel,
                      pClientCache->DimFilesysOverhead );
            break;

        case WFClientDrives :
            pClientDrives = (PWF_CLIENT_DRIVES) pSessionInfo;
            _tprintf( TEXT("WFClientDrives: fAuto %u, count %u\n"),
                      pClientDrives->fAutoClientDrives,
                      pClientDrives->Count );
            for ( i=0; i < pClientDrives->Count; i++ ) {
                _tprintf( TEXT(" %c:%02x"),
                          pClientDrives->Drives[i].DriveLetter,
                          pClientDrives->Drives[i].Flags );
            }
            _tprintf( TEXT("\n\n\
Drives are: WF_DRIVE_REMOVEABLE      0x0001\n\
            WF_DRIVE_FIXED           0x0002\n\
            WF_DRIVE_REMOTE          0x0004\n\
            WF_DRIVE_CDROM           0x0008\n") );
            break;

        case WFICABufferLength :
            pDword = (DWORD *) pSessionInfo;
            _tprintf( TEXT("WFICABufferLength: %u\n"), *pDword );
            break;

        case WFLicensingModel:
            _tprintf( TEXT("WFLicensingModel: %s\n"), pSessionInfo );
            break;


    }
}
//void
//QuerySession( HANDLE hServer, DWORD SessionId, WF_INFO_CLASS InfoClass )
//{
//    WF_CONNECTSTATE_CLASS * pConnectState;
//    LPTSTR pSessionInfo;
//    DWORD ByteCount;
//    LPOSVERSIONINFO pVerInfo;
//    PWF_CLIENT_DISPLAY pClientDisplay;
//    PWF_CLIENT_CACHE   pClientCache;
//    PWF_CLIENT_ADDRESS pClientAddress;
//    PWF_CLIENT_DRIVES pClientDrives;
//    PWF_CLIENT_INFO pClientInfo;
//    PWF_USER_INFO pUserInfo;
//    PWF_APP_INFO pAppInfo;
//    PWF_CLIENT_LATENCY pLatency;
//    DWORD * pDword;
//    WORD * pWord;
//    BYTE * pByte;
//    ULONG i;
//    PWF_SESSION_TIME pSessionTime;
//    TCHAR TimeStr[80];
//
//    _tprintf( TEXT("\nWFQuerySessionInformation: Server 0x%x, Session %d, Class %u\n"),
//              (unsigned int)hServer, SessionId, InfoClass );
//
//    if ( !WFQuerySessionInformation( hServer,
//                                     SessionId,
//                                     InfoClass,
//                                     &pSessionInfo,
//                                     &ByteCount ) ) {
//
//        _tprintf( TEXT("*** WFQuerySessionInformation failed, error %u\n"),
//                  GetLastError() );
//        return;
//    }
//
//    _tprintf( TEXT("WFQuerySessionInformation: ByteCount %u\n"), ByteCount );
//
//    switch ( InfoClass ) {
//
//        case WFVersion :
//            pVerInfo = (LPOSVERSIONINFO) pSessionInfo;
//            _tprintf( TEXT("MFVersion: major %u, minor %u, build %u, CSD Version: %s\n"),
//                      pVerInfo->dwMajorVersion, pVerInfo->dwMinorVersion,
//                      pVerInfo->dwBuildNumber, pVerInfo->szCSDVersion);
//            break;
//
//        case WFInitialProgram :
//            _tprintf( TEXT("WFInitialProgram: %s\n"), pSessionInfo );
//            break;
//
//        case WFWorkingDirectory :
//            _tprintf( TEXT("WFWorkingDirectory: %s\n"), pSessionInfo );
//            break;
//
//        case WFOEMId :
//            _tprintf( TEXT("WFOEMId: %s\n"), pSessionInfo );
//            break;
//
//        case WFSessionId :
//            pDword = (DWORD *) pSessionInfo;
//            _tprintf( TEXT("WFSessionId: %u\n"), *pDword );
//            break;
//
//        case WFConnectState :
//            pConnectState = (WF_CONNECTSTATE_CLASS*) pSessionInfo;
//            _tprintf( TEXT("WFConnectState: %u\n"), *pConnectState );
//            _tprintf( TEXT("\n\
//States are: 0 = WFActive        User logged on to WinStation\n\
//            1 = WFConnected     WinStation connected to client\n\
//            2 = WFConnectQuery  In the process of connecting to client\n\
//            3 = WFShadow        Shadowing another WinStation\n\
//            4 = WFDisconnected  WinStation logged on without client\n\
//            5 = WFIdle          Waiting for client to connect\n\
//            6 = WFListen        WinStation is listening for connection\n\
//            7 = WFReset         WinStation is being reset\n\
//            8 = WFDown          WinStation is down due to error\n\
//            9 = WFInit          WinStation in initialization\n") );
//            break;
//
//        case WFWinStationName :
//            _tprintf( TEXT("WFWinStationName: %s\n"), pSessionInfo );
//            break;
//
//        case WFUserName :
//            _tprintf( TEXT("WFUserName: %s\n"), pSessionInfo );
//            break;
//
//        case WFDomainName :
//            _tprintf( TEXT("WFDomainName: %s\n"), pSessionInfo );
//            break;
//
//        case WFClientBuildNumber :
//            pWord = (WORD *) pSessionInfo;
//            _tprintf( TEXT("WFClientBuildNumber: %u\n"), *pWord );
//            break;
//
//        case WFClientName :
//            _tprintf( TEXT("WFClientName: %s\n"), pSessionInfo );
//            break;
//
//        case WFClientDirectory :
//            _tprintf( TEXT("WFClientDirectory: %s\n"), pSessionInfo );
//            break;
//
//        case WFClientProductId :
//            pWord = (WORD *) pSessionInfo;
//            _tprintf( TEXT("WFClientProductId: 0x%x\n"), *pWord );
//            break;
//
//        case WFClientAddress :
//            pClientAddress = (PWF_CLIENT_ADDRESS) pSessionInfo;
//            _tprintf( TEXT("WFClientAddress: (%u) "),
//                      pClientAddress->AddressFamily );
//            switch ( pClientAddress->AddressFamily ) {
//                case AF_UNSPEC :
//                    printf( "%s", pClientAddress->Address );
//                    break;
//                case AF_INET :
//                    pByte = &pClientAddress->Address[2];
//                    _tprintf( TEXT("%u.%u.%u.%u"),
//                              pByte[0], pByte[1], pByte[2], pByte[3] );
//                    break;
//                case AF_IPX :
//                    pByte = pClientAddress->Address;
//                    for ( i=0; i<4; i++ )
//                        _tprintf( TEXT("%02x"), pByte[i] );
//                    _tprintf( TEXT(":") );
//                    for ( i=4; i<10; i++ )
//                        _tprintf( TEXT("%02x"), pByte[i] );
//                    break;
//            }
//            _tprintf( TEXT("\n") );
//            break;
//
//        case WFClientDisplay :
//            pClientDisplay = (PWF_CLIENT_DISPLAY) pSessionInfo;
//            _tprintf( TEXT("WFClientDisplay: hres %u, vres %u, depth %u\n"),
//                      pClientDisplay->HorizontalResolution,
//                      pClientDisplay->VerticalResolution,
//                      pClientDisplay->ColorDepth );
//            break;
//
//        case WFClientCache :
//            pClientCache = (PWF_CLIENT_CACHE) pSessionInfo;
//            _tprintf( TEXT("WFClientCache: (mem) tiny %u, low %u, xms %u, disk %u\n"),
//                      pClientCache->CacheTiny,
//                      pClientCache->CacheLowMem,
//                      pClientCache->CacheXms,
//                      pClientCache->CacheDisk );
//            _tprintf( TEXT("WFClientCache: (dim) size %u, min %u, level %u, overhead %u\n"),
//                      pClientCache->DimCacheSize,
//                      pClientCache->DimBitmapMin,
//                      pClientCache->DimSignatureLevel,
//                      pClientCache->DimFilesysOverhead );
//            break;
//
//        case WFClientDrives :
//            pClientDrives = (PWF_CLIENT_DRIVES) pSessionInfo;
//            _tprintf( TEXT("WFClientDrives: fAuto %u, count %u\n"),
//                      pClientDrives->fAutoClientDrives,
//                      pClientDrives->Count );
//            for ( i=0; i < pClientDrives->Count; i++ ) {
//                _tprintf( TEXT(" %c:%02x"),
//                          pClientDrives->Drives[i].DriveLetter,
//                          pClientDrives->Drives[i].Flags );
//            }
//            _tprintf( TEXT("\n\n\
//Drives are: WF_DRIVE_REMOVEABLE      0x0001\n\
//            WF_DRIVE_FIXED           0x0002\n\
//            WF_DRIVE_REMOTE          0x0004\n\
//            WF_DRIVE_CDROM           0x0008\n") );
//            break;
//
//        case WFICABufferLength :
//            pDword = (DWORD *) pSessionInfo;
//            _tprintf( TEXT("WFICABufferLength: %u\n"), *pDword );
//            break;
//
//        case WFApplicationName :
//            _tprintf( TEXT("WFApplicationName: %s\n"), pSessionInfo );
//            break;
//        case WFClientInfo :
//            pClientInfo = (PWF_CLIENT_INFO)pSessionInfo;
//            _tprintf( TEXT("WFClientInfo:\n") );
//            _tprintf( TEXT("    Name: %s\n"), pClientInfo->Name);
//            _tprintf( TEXT("    Directory: %s\n"), pClientInfo->Directory);
//            _tprintf( TEXT("    BuildNumber: %d\n"), pClientInfo->BuildNumber);
//            _tprintf( TEXT("    ProductID: 0x%x\n"), pClientInfo->ProductID);
//            _tprintf( TEXT("    HardwareID: 0x%x\n"), pClientInfo->HardwareID);
//            pClientAddress = &pClientInfo->Address;
//            _tprintf( TEXT("    WFClientAddress: (%u) "), pClientAddress->AddressFamily );
//            switch ( pClientAddress->AddressFamily ) {
//                case AF_UNSPEC :
//                    printf( "%s", pClientAddress->Address );
//                    break;
//                case AF_INET :
//                    pByte = &pClientAddress->Address[2];
//                    _tprintf( TEXT("%u.%u.%u.%u"),
//                              pByte[0], pByte[1], pByte[2], pByte[3] );
//                    break;
//                case AF_IPX :
//                    pByte = pClientAddress->Address;
//                    for ( i=0; i<4; i++ )
//                        _tprintf( TEXT("%02x"), pByte[i] );
//                    _tprintf( TEXT(":") );
//                    for ( i=4; i<10; i++ )
//                        _tprintf( TEXT("%02x"), pByte[i] );
//                    break;
//            }
//            _tprintf( TEXT("\n") );
//            break;
//        case WFUserInfo :
//            pUserInfo = (PWF_USER_INFO)pSessionInfo;
//            _tprintf( TEXT("WFUserInfo:\n") );
//            _tprintf( TEXT("    User Name: %s\n"), pUserInfo->UserName);
//            _tprintf( TEXT("    Domain Name: %s\n"), pUserInfo->DomainName);
//            _tprintf( TEXT("    Connection Name: %s\n"),
//                pUserInfo->ConnectionName);
//            break;
//        case WFAppInfo :
//            pAppInfo = (PWF_APP_INFO)pSessionInfo;
//            _tprintf( TEXT("WFAppInfo:\n") );
//            _tprintf( TEXT("    Initial Program: %s\n"),
//                pAppInfo->InitialProgram);
//            _tprintf( TEXT("    Working Directory: %s\n"),
//                pAppInfo->WorkingDirectory);
//            _tprintf( TEXT("    Application Name: %s\n"),
//                pAppInfo->ApplicationName);
//            break;
//        case WFClientLatency :
//            pLatency = (PWF_CLIENT_LATENCY)pSessionInfo;
//            _tprintf( TEXT("WFClientLatency:\n") );
//            _tprintf( TEXT("    Last latency: %d ms\n"), pLatency->Last);
//            _tprintf( TEXT("    Average latency: %d ms\n"),
//                pLatency->Average);
//            _tprintf( TEXT("    Latency deviation: %d ms\n"),
//                pLatency->Deviation);
//            break;
//        case WFSessionTime :
//            pSessionTime = (PWF_SESSION_TIME)pSessionInfo;
//            _tprintf( TEXT("WFSessionTime:\n") );
//            FormatTime(pSessionTime->DisconnectTime, TimeStr);
//            _tprintf( TEXT("    Disconnect time: %016I64X (%s)\n"),
//                pSessionTime->DisconnectTime.QuadPart, TimeStr);
//            FormatTime(pSessionTime->ConnectTime, TimeStr);
//            _tprintf( TEXT("    Connect time:    %016I64X (%s)\n"),
//                pSessionTime->ConnectTime.QuadPart, TimeStr);
//            FormatTime(pSessionTime->LastInputTime, TimeStr);
//            _tprintf( TEXT("    Last input time: %016I64X (%s)\n"),
//                pSessionTime->LastInputTime.QuadPart, TimeStr);
//            FormatTime(pSessionTime->LogonTime, TimeStr);
//            _tprintf( TEXT("    Logon time:      %016I64X (%s)\n"),
//                pSessionTime->LogonTime.QuadPart, TimeStr);
//            FormatTime(pSessionTime->CurrentTime, TimeStr);
//            _tprintf( TEXT("    Current time:    %016I64X (%s)\n"),
//                pSessionTime->CurrentTime.QuadPart, TimeStr);
//    }
//
//    WFFreeMemory( pSessionInfo );
//} // QuerySession()
//

/********************************************************************
 *
 *  GetUserNameFromSid
 *
 *  Attempts to retrieve the user (login) name of the process by
 *  looking up the SID (Security Identifier) in the SAM database
 *
 *
 * ENTRY:
 *    pUserSid (input)
 *       pointer to SID
 *    pUserBuffer (output)
 *       pointer to buffer to return user name to
 *    ByteCount (input)
 *       length of buffer
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/

void
GetUserNameFromSid( PVOID pUserSid,
                    LPTSTR pUserBuffer,
                    DWORD UserLength )
{
    TCHAR DomainBuffer[257];
    DWORD DomainLength = sizeof(DomainBuffer) / sizeof(TCHAR);
    SID_NAME_USE SidNameUse;

    if ( !LookupAccountSid( NULL,
                            pUserSid,
                            pUserBuffer,
                            &UserLength,
                            DomainBuffer,
                            &DomainLength,
                            &SidNameUse ) ) {
        *pUserBuffer = '\0';
    }
} // GetUserNameFromSid()


/********************************************************************
 *
 *  EnumerateProcesses
 *
 *    Display a list of processes on the specfied server
 *
 * ENTRY:
 *    hServer (input)
 *       handle to server
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/

void
EnumerateProcesses( LPTSTR lpServerName, HANDLE hServer )
{
    PWF_PROCESS_INFO pProcessInfo;
    TCHAR UserName[257];
    DWORD Count;
    DWORD i;

    _tprintf( TEXT("\nWFEnumerateProcesses: server %s (handle 0x%x)\n"),
              lpServerName, (INT_PTR)hServer );

    if ( !WFEnumerateProcesses( hServer,
                                0,    // reserved
                                1,    // version 1
                                &pProcessInfo,
                                &Count ) ) {

        _tprintf( TEXT("*** WFEnumerateProcesses failed, error %u\n"),
                  GetLastError() );
        return;
    }

    _tprintf( TEXT("WFEnumerateProcesses: count %u\n"), Count );

    for ( i=0; i < Count; i++ ) {
            if(IsValidSid(pProcessInfo[i].pUserSid))
            {
                 GetUserNameFromSid( pProcessInfo[i].pUserSid, UserName, 257 );
            }
        
        _tprintf( TEXT("%-5u  %-20s  %-5u  %s\n"),
                  pProcessInfo[i].SessionId,
                  UserName,
                  pProcessInfo[i].ProcessId,
                  pProcessInfo[i].pProcessName );
    }

    WFFreeMemory( pProcessInfo );
} // EnumerateProcesses()

/********************************************************************
 *
 *  EnumerateProcessesEx
 *
 *    Display a list of processes on the specfied server.  This call
 *    displays more extensive information about the processes.
 *
 * ENTRY:
 *    hServer (input)
 *       handle to server
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/

void
EnumerateProcessesEx( LPTSTR lpServerName, HANDLE hServer )
{
    PWF_PROCESS_INFOEX pProcessInfo;
    TCHAR UserName[257];
    DWORD Count=0;
    DWORD i=0;
    DWORD BufSize=0;
    BOOL br;

    _tprintf( TEXT("\nWFEnumerateProcessesEx: server %s (handle 0x%x)\n"),
              lpServerName, (INT_PTR)hServer );

    br = WFEnumerateProcessesEx(hServer, 0, 1, &pProcessInfo, &Count);
    if (br == FALSE) {
        _tprintf( TEXT("*** WFEnumerateProcessesEx failed, error %u\n"),
                  GetLastError() );
        return;
    }

    _tprintf( TEXT("WFEnumerateProcessesEx: count %u\n"), Count );

    for ( i=0; i < Count; i++ ) {
       if(IsValidSid(pProcessInfo[i].pUserSid))
            {
                 GetUserNameFromSid( pProcessInfo[i].pUserSid, UserName, 257 );
            }
        _tprintf( TEXT("\n\tProcessID: %-5u\t\tProcessName: %s\n"),
            pProcessInfo[i].ProcessId, pProcessInfo[i].pProcessName);
        _tprintf( TEXT("\tSessionID: %-5u\t\tUserName:    %s\n"),
            pProcessInfo[i].SessionId, UserName);
        _tprintf( TEXT("\tUser Time: %-16X\tKernel Time: %-16X\n"),
            pProcessInfo[i].UserTime.QuadPart, pProcessInfo[i].KernelTime.QuadPart);
        _tprintf( TEXT("\tWorkingSetSize: %-8X\tPeakWorkingSetSize: %-8X\n"),
            pProcessInfo[i].WorkingSetSize,
            pProcessInfo[i].PeakWorkingSetSize);
        _tprintf( TEXT("\tVirtualSize:%-8X\t\tPeakVirtualSize %-8X\n"),
            pProcessInfo[i].VirtualSize,
            pProcessInfo[i].PeakVirtualSize);
        _tprintf( TEXT("\tNumThreads: %-8X\t\tNumHandles: %-8X\n"),
            pProcessInfo[i].NumThreads, pProcessInfo[i].NumHandles);
    }

    WFFreeMemory(pProcessInfo);

} // EnumerateProcessesEx()

void
EnumerateProcessesExA( LPTSTR lpServerName, HANDLE hServer )
{
    PWF_PROCESS_INFOEXA pProcessInfo;
    TCHAR UserName[257];
    DWORD Count=0;
    DWORD i=0;
    DWORD BufSize=0;
    BOOL br;

    _tprintf( TEXT("\nWFEnumerateProcessesEx: server %s (handle 0x%x)\n"),
              lpServerName, (INT_PTR)hServer );

    br = WFEnumerateProcessesExA(hServer, 0, 1, &pProcessInfo, &Count);
    if (br == FALSE) {
        _tprintf( TEXT("*** WFEnumerateProcessesEx failed, error %u\n"),
                  GetLastError() );
        return;
    }

    _tprintf( TEXT("WFEnumerateProcessesEx: count %u\n"), Count );

    for ( i=0; i < Count; i++ ) {
       if(IsValidSid(pProcessInfo[i].pUserSid))
       {
           GetUserNameFromSid( pProcessInfo[i].pUserSid, UserName, 257 );
       }
        _tprintf(TEXT( "\n\tProcessID: %-5u\t\tProcessName: %s\n"),
            pProcessInfo[i].ProcessId, pProcessInfo[i].pProcessName);
        _tprintf( TEXT("\tSessionID: %-5u\t\tUserName:    %s\n"),
            pProcessInfo[i].SessionId, UserName);
        _tprintf( TEXT("\tUser Time: %-16X\tKernel Time: %-16X\n"),
            pProcessInfo[i].UserTime.QuadPart, pProcessInfo[i].KernelTime.QuadPart);
        _tprintf( TEXT("\tWorkingSetSize: %-8X\tPeakWorkingSetSize: %-8X\n"),
            pProcessInfo[i].WorkingSetSize,
            pProcessInfo[i].PeakWorkingSetSize);
        _tprintf( TEXT("\tVirtualSize:%-8X\t\tPeakVirtualSize %-8X\n"),
            pProcessInfo[i].VirtualSize,
            pProcessInfo[i].PeakVirtualSize);
        _tprintf( TEXT("\tNumThreads: %-8X\t\tNumHandles: %-8X\n"),
            pProcessInfo[i].NumThreads, pProcessInfo[i].NumHandles);
    }

    WFFreeMemory(pProcessInfo);

} // EnumerateProcessesEx()

void
GetCtxSessionKeyList(LPTSTR lpServerName, HANDLE hServer )
{
    PWCHAR SessionKeys = 0;
    PWCHAR pSessionKeys = 0;
    DWORD Keybufferlen = 0;
    DWORD KeyCount = 0;
    DWORD Keystrlen = 0;
    DWORD x = 0;
    WFGetCtxSessionKeyList( hServer, &SessionKeys, &KeyCount, &Keybufferlen, &Keystrlen);
    _tprintf( TEXT("\tNumSessionKeys: %d\n"), KeyCount);
    pSessionKeys = SessionKeys;
    for(x = 0; x < KeyCount; x++)
        {
            pSessionKeys += x * Keystrlen;
            _tprintf( TEXT("\tSessionKeys: %s \n"),pSessionKeys);
        }
    LocalFree(SessionKeys);
}

char *VCName[] = {
    "CTXTW  ",
    "CTXLPT1",
    "CTXCPM ",
    "CTXCDM ",
    "CTXCLIP",
    NULL,
};

void TestSendMessage()
{
    //LPWSTR message;
//    LPWSTR title;
    DWORD dwMsgLen;
    DWORD titleLen;
    DWORD response;
    TCHAR  message[200] = TEXT("This is a message");
    TCHAR  title[50] = TEXT("Hello");
//        message=_T("This is a message");
//        title=_T("Hello");
        #ifdef UNICODE
           dwMsgLen = 200; // lstrlen(message)+1 * sizeof(WCHAR);
          titleLen =50; // lstrlen(title)+1 * sizeof(WCHAR);
        #else 
          dwMsgLen = 200; //lstrlen(message)+1 * sizeof(CHAR);
          titleLen =50; //lstrlen(title)+1 * sizeof(CHAR);
        #endif


        

        if(WFSendMessage(WF_CURRENT_SERVER_HANDLE,WF_CURRENT_SESSION,title,titleLen,message,dwMsgLen,MB_OK,200,&response,0))
        {
            _tprintf(_T("Message Send"));
        }


        
} //TestSendMessage

/********************************************************************
 *
 *  Virtual Channel Test
 *
 *    Verify the ability to open/close virtual channel by name.
 *
 * ENTRY:
 *    None.
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/
void VirtualChannelTest(VOID)
{
    #define CTXPING_VIRTUAL_CHANNEL_NAME  "CTXPING"

    HANDLE VCHandle;
    int i, j, k;
    PVOID DataBuf;
    ULONG BytesReturned;
    int Words;
    PULONG pw;

    _tprintf( TEXT("\nStarting Virtual Channel Test\n") );
    for (i=0;VCName[i];i++) {
        printf("String length of %s is %d\n",VCName[i],strlen(VCName[i]));
        if (VCHandle = WFVirtualChannelOpen(WF_CURRENT_SERVER_HANDLE,
                                            WF_CURRENT_SESSION,
                                            VCName[i])) {
            if (WFVirtualChannelQuery(VCHandle,
                                      WFVirtualClientData,
                                      &DataBuf,
                                      &BytesReturned)) {
                _tprintf( TEXT("WFVirtualChannelQuery(WFVirtualClientData) Passed Bytes Returned: %d\n"),
                          BytesReturned);
                Words = (BytesReturned + sizeof(ULONG) - 1) / sizeof(ULONG);
                for (j=0,pw=(PULONG)DataBuf;j<Words;) {
                    _tprintf( TEXT("%04x: "),j*4);
                    for(k=0;((k < 4) && (j<Words));j++,k++,pw++) {
                        _tprintf( TEXT("%08x  "),*pw);
                    }
                    _tprintf( TEXT("\n") );
                }
                WFFreeMemory(DataBuf);
            } else {
                _tprintf( TEXT("*** WFVirtualChannelQuery(WFVirtualClientData) failed - error: %d\n"),
                       GetLastError());
            }

            if (WFVirtualChannelPurgeInput(VCHandle)) {
                _tprintf( TEXT("WFVirtualChannelPurgeInput Passed\n") );
            } else {
                _tprintf( TEXT("*** WFVirtualChannelPurgeInput failed - error: %d\n"),
                       GetLastError());
            }

            if (WFVirtualChannelPurgeOutput(VCHandle)) {
                _tprintf( TEXT("WFVirtualChannelPurgeOutput Passed\n") );
            } else {
                _tprintf( TEXT("*** WFVirtualChannelPurgeOutput failed - error: %d\n"),
                       GetLastError());
            }

            if (WFVirtualChannelClose(VCHandle)) {
                printf("Open/Close channel %s Passed\n", VCName[i]);
            } else {
                printf("*** Close channel %s failed - error: %d\n",
                         VCName[i], GetLastError());
            }
        } else {
            printf("*** Open channel %s failed - error: %d\n",
                     VCName[i], GetLastError());
        }
    }
    _tprintf( TEXT("Ending Virtual Channel Test\n") );

} // VirtualChannelTest()


/********************************************************************
 *
 *  WFQueryUserConfigTest
 *
 *    Verify the ability to query Citrix server-specific user
 *    information.
 *
 * ENTRY:
 *    dwTest - TEST_LOCAL = local accounts, TEST_DOMAIN = find Domain
 *             Controller
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/
void WFQueryUserConfigTest( BOOL dwTest, LPTSTR lpUserName )
{
    LPTSTR pBuffer = NULL;
    DWORD  dwBytes;
    LPWSTR lpDCName = NULL;
#ifndef UNICODE
    /*  NOTE: We need the following ANSI variable because
     *        NetGetAnyDCName() always returned Unicode, and we need
     *        to pass ANSI to WFQueryUserConfig(), so we need to
     *        convert it.
     */
    LPSTR lpDCNameA = NULL;
    size_t i;
#endif // !UNICODE
    DWORD  rc;

    /*
     *  To reduce the number of lines of code in the following (both
     *  this routine and WFSetUserConfigTest() ), we'll define an
     *  all-caps LPDCNAME variable to be the UNICODE string if
     *  UNICODE is defined, or the regular string if it isn't.  This
     *  is what we'll pass down to WFQueryUserConfig() and
     *  WFSetUserConfig().
     */
#ifdef UNICODE
#define LPDCNAME lpDCName
#else  // UNICODE
#define LPDCNAME lpDCNameA
#endif // UNICODE

    _tprintf( TEXT("\nStarting Query User Config Test\n") );

    /*
     *  If not local, then get Domain Controller name.  If it is local, then
     *  we leave lpDCName == NULL, which will query local accounts.
     */
    if ( TEST_DOMAIN == dwTest ) {
        rc = NetGetAnyDCName( NULL, NULL, (LPBYTE *)&lpDCName );
        if ( rc != NERR_Success ) {
            _tprintf( TEXT("WARNING: Cannot get Domain Controller name; will continue with NULL...\n") );
        }
#ifndef UNICODE
        if ( lpDCName ) {
            i = wcstombs( NULL, lpDCName, 256 );
            lpDCNameA = LocalAlloc( LPTR, i+1 );
            i = wcstombs( lpDCNameA, lpDCName, 256 );
        }
#endif // !UNICODE
    }

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigInitialProgram,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, initial program is %s\n"),
                  lpUserName, pBuffer );
    } else {
         _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                   lpUserName, GetLastError());
         goto done;
    }

    WFFreeMemory( pBuffer );
    pBuffer = NULL;

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigWorkingDirectory,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, working directory is %s\n"),
                  lpUserName, pBuffer );
    } else {
         _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                   lpUserName, GetLastError());
         goto done;
    }

    WFFreeMemory( pBuffer );
    pBuffer = NULL;

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigfInheritInitialProgram,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, fInheritInitialProgram is %ld\n"),
                  lpUserName, *pBuffer );
    } else {
         _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                   lpUserName, GetLastError());
         goto done;
    }

done:
    // HDX-7659: WFFreeMemory will check for NULL pointer, thus eliminating the redundant check. 
    WFFreeMemory( pBuffer );

    _tprintf( TEXT("End of Query User Config Test\n") );

#ifndef UNICODE
    if ( lpDCNameA ) {
        LocalFree( lpDCNameA );
    }
#endif // UNICODE

} // WFQueryUserConfigTest()

/********************************************************************
 *
 *  WaitSystemEventTest
 *
 *    Verify the ability to wait on system events like Connect/Disconnect/LogOFF/LogON/Shutdown;
 *    
 *
 * ENTRY:
 *    nothing
 *           
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/
void WaitSystemEventTest()
{
    DWORD eventFlags;
    if(WFWaitSystemEvent(WF_CURRENT_SERVER,WF_EVENT_CONNECT | WF_EVENT_DISCONNECT, &eventFlags))
       _tprintf( TEXT(" WFWaitSystemEvent passed for connect or disconnect events with event flags : %d\n"),eventFlags);
    else
       _tprintf( TEXT("*** WFWaitSystemEvent failed - error: %d\n"),GetLastError());
}

/********************************************************************
 *
 *  WFSetUserConfigTest
 *
 *    Verify the ability to query Citrix server-specific user
 *    information.
 *
 * ENTRY:
 *    dwTest - TEST_LOCAL = local accounts, TEST_DOMAIN = find Domain
 *             Controller
 *
 * EXIT:
 *    nothing
 *
 *******************************************************************/
void WFSetUserConfigTest( DWORD dwTest, LPTSTR lpUserName )
{
     LPTSTR pBuffer  = NULL;
    DWORD  dwBytes;
    LPTSTR pBuffer2 = NULL;
    PDWORD pdw;
    DWORD  dwInheritInitialProgram;
    LPWSTR lpDCName = NULL;
    size_t lent = 0;
#ifndef UNICODE
    /*  NOTE: We need the following ANSI variable because
     *        NetGetAnyDCName() always returned Unicode, and we need
     *        to pass ANSI to WFQueryUserConfig(), so we need to
     *        convert it.
     */
    LPSTR lpDCNameA = NULL;
    size_t i;
#endif // !UNICODE
    DWORD  rc;

#ifdef UNICODE
#define TESTSTRING TEXT("This is a test")

#else
#define TESTSTRING "This is a test"

#endif // !UNICODE
    #define TESTSTRINGLEN 11

    _tprintf( TEXT("\nStarting Set User Config Test\n") );

    /*
     *  If not local, then get Domain Controller name.  If it is
     *  local, we leave lpDCName == NULL, which will query local
     *  accounts.
     */
    if ( TEST_DOMAIN == dwTest ) {
        rc = NetGetAnyDCName( NULL, NULL, (LPBYTE *)&lpDCName );
        if ( rc != NERR_Success ) {
            _tprintf( TEXT("WARNING: Cannot get Domain Controller name; will continue with NULL...\n") );
        }
#ifndef UNICODE
        if ( lpDCName ) {
            i = wcstombs( NULL, lpDCName, 256 );
            lpDCNameA = LocalAlloc( LPTR, i );
            i = wcstombs( lpDCNameA, lpDCName, 256 );
        }
#endif // !UNICODE
    }

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigInitialProgram,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, initial program is %s\n"),
                  lpUserName, pBuffer );
    } else {
        _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                  lpUserName, GetLastError());
        goto done;
    }
    if (WFSetUserConfig( LPDCNAME,
                         lpUserName,
                         WFUserConfigInitialProgram,
                         TESTSTRING,
                         _tcsnlen(TESTSTRING,TESTSTRINGLEN))
                       ) {

        lent = (_tcsnlen(pBuffer,TESTSTRINGLEN)+1) * sizeof(TCHAR);
        _tprintf( TEXT("WFSetUserConfig  (%s) Passed, initial program set to %s\n"),
                  lpUserName, TESTSTRING );
        pBuffer2 = LocalAlloc( LPTR, lent );
        if ( pBuffer2 == NULL ) {
            _tprintf( TEXT("WFSetUserConfig  (%s) failed - LocalAlloc() failed\n"),
                      lpUserName );
            goto done;
        }

        _tcscpy_s( pBuffer2,lent, pBuffer );
        
        // HDX-7659: CID 37251 (#1 of 2)
        // Freeing the pBuffer before reusing it.
        WFFreeMemory( pBuffer );
        pBuffer = NULL;
        
        if (WFQueryUserConfig( LPDCNAME,
                               lpUserName,
                               WFUserConfigInitialProgram,
                               &pBuffer,
                               &dwBytes)
                             ) {
            _tprintf( TEXT("WFQueryUserConfig(%s) Passed, initial program is now %s\n"),
                      lpUserName, pBuffer );
            if (WFSetUserConfig( LPDCNAME,
                                 lpUserName,
                                 WFUserConfigInitialProgram,
                                 pBuffer2,
                                 _tcsnlen(pBuffer2,lent))
                               ) {
                _tprintf( TEXT("WFSetUserConfig  (%s) Passed, initial program set back to %s\n"),
                          lpUserName, pBuffer2 );
            } else {
                _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                          lpUserName, GetLastError());
                goto done;
            }
        } else {
            _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                      lpUserName, GetLastError());
            goto done;
        }
    } else {
        _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                  lpUserName, GetLastError());
        goto done;
    }

    // HDX-7659: CID 37251 (#1 of 2)
    // The if-check is redundant
    WFFreeMemory( pBuffer );
    pBuffer = NULL;

    WFFreeMemory( pBuffer2 );
    pBuffer2 = NULL;

    /*
     *  =====================================================
     *  Now, repeat the above but with the working directory.
     *  =====================================================
     */

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigWorkingDirectory,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, work directory is %s\n"),
                  lpUserName, pBuffer );
    } else {
        _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                  lpUserName, GetLastError());
        goto done;
    }
    if (WFSetUserConfig( LPDCNAME,
                         lpUserName,
                         WFUserConfigWorkingDirectory,
                         TESTSTRING,
                         _tcsnlen(TESTSTRING,TESTSTRINGLEN))
                       ) {
            lent = (_tcsnlen(pBuffer,TESTSTRINGLEN)+1) * sizeof(TCHAR);
        _tprintf( TEXT("WFSetUserConfig  (%s) Passed, work directory set to %s\n"),
                  lpUserName, TESTSTRING );
        pBuffer2 = LocalAlloc( LPTR, lent );
        if ( pBuffer2 == NULL ) {
            _tprintf( TEXT("WFSetUserConfig  (%s) failed - LocalAlloc() failed\n"),
                      lpUserName );
            goto done;
        }
        _tcscpy_s( pBuffer2,lent ,pBuffer );
        
        // HDX-7659: CID 37251 (#2 of 2)
        // Freeing the pBuffer before reusing it.
        WFFreeMemory( pBuffer );
        pBuffer = NULL;
        
        if (WFQueryUserConfig( LPDCNAME,
                               lpUserName,
                               WFUserConfigWorkingDirectory,
                               &pBuffer,
                               &dwBytes)
                             ) {
            _tprintf( TEXT("WFQueryUserConfig(%s) Passed, work directory is now %s\n"),
                      lpUserName, pBuffer );
            if (WFSetUserConfig( LPDCNAME,
                                 lpUserName,
                                 WFUserConfigWorkingDirectory,
                                 pBuffer2,
                                 _tcsnlen(pBuffer2,lent))
                               ) {
                _tprintf( TEXT("WFSetUserConfig  (%s) Passed, work directory set back to %s\n"),
                          lpUserName, pBuffer2 );
            } else {
                _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                          lpUserName, GetLastError());
                goto done;
            }
        } else {
            _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                      lpUserName, GetLastError());
            goto done;
        }
    } else {
        _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                  lpUserName, GetLastError());
        goto done;
    }
    
    // HDX-7659: CID 37251 (#2 of 2)
    // The if-check is redundant
    WFFreeMemory( pBuffer );
    pBuffer = NULL;

    WFFreeMemory( pBuffer2 );
    pBuffer2 = NULL;

    /*
     *  ======================================================
     *  Now, repeat the above but with fInheritInitialProgram.
     *  ======================================================
     */

    if (WFQueryUserConfig( LPDCNAME,
                           lpUserName,
                           WFUserConfigfInheritInitialProgram,
                           &pBuffer,
                           &dwBytes)
                         ) {
        _tprintf( TEXT("WFQueryUserConfig(%s) Passed, fInheritInitialProgram is %ld\n"),
                  lpUserName, *pBuffer );
        dwInheritInitialProgram = (DWORD) (*pBuffer);
        pdw = (DWORD *) pBuffer;
        *pdw = !dwInheritInitialProgram;
        if (WFSetUserConfig( LPDCNAME,
                             lpUserName,
                             WFUserConfigfInheritInitialProgram,
                             pBuffer,
                             dwBytes)
                           ) {
            _tprintf( TEXT("WFSetUserConfig  (%s) Passed, fInheritInitialProgram set to %ld\n"),
                      lpUserName, *pBuffer );
            /*
             *  Have to free pBuffer here because Query allocates a new one.
             */
            WFFreeMemory( pBuffer );
            pBuffer = NULL;
            if (WFQueryUserConfig( LPDCNAME,
                                   lpUserName,
                                   WFUserConfigfInheritInitialProgram,
                                   &pBuffer,
                                   &dwBytes)
                                 ) {
                _tprintf( TEXT("WFQueryUserConfig(%s) Passed, fInheritInitialProgram is now %ld\n"),
                          lpUserName, *pBuffer );
                pdw = (DWORD *) pBuffer;
                *pdw = dwInheritInitialProgram;
                if (WFSetUserConfig( LPDCNAME,
                                     lpUserName,
                                     WFUserConfigfInheritInitialProgram,
                                     pBuffer,
                                     dwBytes)
                                   ) {
                    _tprintf( TEXT("WFSetUserConfig  (%s) Passed, fInheritInitialProgram set back to %ld\n"),
                              lpUserName, *pBuffer );
                } else {
                     _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                               lpUserName, GetLastError());
                     goto done;
                }
            } else {
                 _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                           lpUserName, GetLastError());
                 goto done;
            }
        } else {
             _tprintf( TEXT("*** WFSetUserConfig  (%s) failed - error: %d\n"),
                       lpUserName, GetLastError());
             goto done;
        }
    } else {
         _tprintf( TEXT("*** WFQueryUserConfig(%s) failed - error: %d\n"),
                   lpUserName, GetLastError());
         goto done;
    }

done:
    // HDX-7659: WFFreeMemory will check for NULL pointer, thus eliminating the redundant check. 
    WFFreeMemory( pBuffer );
    WFFreeMemory( pBuffer2 );

    _tprintf( TEXT("End of Set User Config Test\n") );
} // WFSetUserConfigTest()

BOOL TestDisconnectSessionEx(ULONG Flags)
{
    return WFDisconnectSessionEx(WF_CURRENT_SERVER_HANDLE,WF_CURRENT_SESSION,Flags,NULL,0);
}

BOOL TestDisconnectSession()
{
    return WFDisconnectSession(WF_CURRENT_SERVER_HANDLE,WF_CURRENT_SESSION,FALSE);
}

BOOL TestLogOffSession()
{
    return WFLogoffSession(WF_CURRENT_SERVER_HANDLE,WF_CURRENT_SESSION,TRUE );
}
void TerminateAProcess()
{

    STARTUPINFO startup_info;
    PROCESS_INFORMATION pi;
    DWORD processID;
    DWORD exitCode =0;
    ZeroMemory( &startup_info, sizeof(startup_info) );
    startup_info.cb = sizeof(startup_info);
    ZeroMemory( &pi, sizeof(pi) );

    CreateProcess(_T("c:\\Windows\\System32\\Notepad.exe"),_T(""),NULL,NULL,FALSE,0,NULL,NULL,&startup_info,&pi);
    Sleep(3000);
    processID = GetProcessId(pi.hProcess);
    if(WFTerminateProcess(WF_CURRENT_SERVER_HANDLE,processID,exitCode))
    _tprintf( TEXT("WFTerminateProcess Passed with Exit code %d\n"),exitCode );
    else
    _tprintf( TEXT("*** WFTerminateProcess failed - error: %d and exit code %d\n"), GetLastError(),exitCode);
    
    
    _tprintf( TEXT("End of terminate a process Test\n") );


}// TerminateAProcess

void ShutdownSystemTest()
{
    //WF_WSD_SHUTDOWN
    //WF_WSD_REBOOT
    //WF_WSD_POWEROFF
    //WF_WSD_FASTREBOOT

    if(WFShutdownSystem(WF_CURRENT_SERVER,WF_WSD_REBOOT))
        _tprintf( TEXT("WFShutdownSystem passed"));
    else
        _tprintf( TEXT("*** WFShutdownSystem failed - error: %d\n"),GetLastError());
    
}

/********************************************************************
*
*  QueryEndpointDetailsInDoubleHop
*
*    Endpoint client details in a double-hop
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/
void QueryEndpointDetailsInDoubleHop(HANDLE hServer, DWORD SessionId)
{
    PWF_CLIENT_INFO pClientInfo = NULL;
    PWF_CLIENT_ADDRESS pClientAddress;  
    DWORD dwSize = 0;
    BOOL bRet = FALSE;
    PBYTE pByte = NULL;

    bRet = WFEndPointClientDataInDoubleHop(hServer, SessionId, &pClientInfo, &dwSize);
    if (!bRet)
    {
        if (GetLastError() == ERROR_INVALID_HANDLE ||
            GetLastError() == WF_CURRENT_SESSION
            )
        {
            _tprintf(TEXT("Invalid API paramters:\n"));
        }
        else
        {
            _tprintf(TEXT("Not running in a double-hopsession\n"));
        }

        _tprintf(TEXT("\n"));
        return;

    }

    if (!pClientInfo || dwSize == 0)
    {
        _tprintf(TEXT("Internal error\n"));
        _tprintf(TEXT("\n"));
        WFFreeMemory(pClientInfo);
        return;
    }

    _tprintf(TEXT("WFClientInfo:\n"));
    _tprintf(TEXT("    Name: %s\n"), pClientInfo->Name);
    _tprintf(TEXT("    Directory: %s\n"), pClientInfo->Directory);
    _tprintf(TEXT("    BuildNumber: %d\n"), pClientInfo->BuildNumber);
    _tprintf(TEXT("    ProductID: 0x%x\n"), pClientInfo->ProductID);
    _tprintf(TEXT("    HardwareID: 0x%x\n"), pClientInfo->HardwareID);
    pClientAddress = &pClientInfo->Address;
    _tprintf(TEXT("    WFClientAddress: (%u) "), pClientAddress->AddressFamily);
    switch (pClientAddress->AddressFamily) {
    case AF_UNSPEC:
        printf("%s", pClientAddress->Address);
        break;
    case AF_INET:
        if (gpfnRtlIpv4AddressToStringExW == NULL)
        {
            HMODULE  hNtDll = NULL;

            if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
            {
                gpfnRtlIpv4AddressToStringExW = (L_RTLIPV4ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv4AddressToStringExW");
            }
        }

        if (gpfnRtlIpv4AddressToStringExW != NULL)
        {
            WCHAR  ClientAddress[64] = { L'\0' };
            ULONG  ClientAddressLength = 64;
            USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

            // The first two bytes are reserved for port. The next 4 bytes are the IP Address
            (*gpfnRtlIpv4AddressToStringExW)((IN_ADDR *)&pClientAddress->Address[2], ClientPort, ClientAddress, &ClientAddressLength);
            wprintf(L"%s", ClientAddress);
        }
        break;
    case AF_INET6:
        if (gpfnRtlIpv6AddressToStringExW == NULL)
        {
            HMODULE  hNtDll = NULL;

            if ((hNtDll = GetModuleHandle(TEXT("ntdll.dll"))) != NULL)
            {
                gpfnRtlIpv6AddressToStringExW = (L_RTLIPV6ADDRESSTOSTRINGEXW)GetProcAddress(hNtDll, "RtlIpv6AddressToStringExW");
            }
        }

        if (gpfnRtlIpv6AddressToStringExW != NULL)
        {
            WCHAR  ClientAddress[64] = { L'\0' };
            ULONG  ClientAddressLength = 64;
            USHORT ClientPort = (pClientAddress->Address[1] << 8) | pClientAddress->Address[0];

            // The first two bytes are reserved for port. The next 4 bytes are the IP Address
            (*gpfnRtlIpv6AddressToStringExW)((IN6_ADDR *)&pClientAddress->Address[2], 0, ClientPort, ClientAddress, &ClientAddressLength);
            wprintf(L"%s", ClientAddress);
        }
        break;
    case AF_IPX:
        pByte = pClientAddress->Address;
        for (int i = 0; i < 4; i++)
            _tprintf(TEXT("%02x"), pByte[i]);
        _tprintf(TEXT(":"));
        for (int i = 4; i < 10; i++)
            _tprintf(TEXT("%02x"), pByte[i]);
        break;

    }
    _tprintf(TEXT("\n"));

    WFFreeMemory(pClientInfo);
}
/********************************************************************
*
*  VerifyDoubleHopSession
*
*    Verify if the session is a double-hop
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void VerifyDoubleHopSession(HANDLE hServer, DWORD SessionId)
{
    BOOL bRet = FALSE;

    bRet = WFIsThisDoubleHopSession(hServer, SessionId);
    if (!bRet)
    {
        if (GetLastError() == ERROR_INVALID_HANDLE ||
            GetLastError() == WF_CURRENT_SESSION
            )
        {
            _tprintf(TEXT("Invalid API paramters:\n"));
        }
        else
        {
            _tprintf(TEXT("Session is not running under a double-hop session\n"));
        }

        _tprintf(TEXT("\n"));
        return;

    }

    _tprintf(TEXT("Session is running under a double-hop session\n"));
    _tprintf(TEXT("\n"));
    return;
}

/********************************************************************
*
*  WaitSystemEventTestInDoublehop
*
*    Verify the ability to wait on endpoint client system events 
*    Connect/Disconnect inside a double-hop session;
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/
void WaitSystemEventTestInDoublehop()
{
    DWORD eventFlags;
    if (WFWaitEndPointClientEventInDoubleHop(WF_CURRENT_SERVER, WF_EVENT_CONNECT | WF_EVENT_DISCONNECT, &eventFlags))
        _tprintf(TEXT(" WFWaitEndPointClientEventInDoubleHop passed for connect or disconnect events with event flags : %d\n"), eventFlags);
    else
        _tprintf(TEXT("*** WFWaitSystemEvent failed - error: %d\n"), GetLastError());
}

#define EPCONNECT L"LOCAL\\EPCONNECT"
#define EPDISCONNECT L"LOCAL\\EPDISCONNECT"
#define ENDTHREAD L"LOCAL\\ENDTHREAD"
DWORD WINAPI EndpointConnectMonitorThread(LPVOID Param)
{
    HANDLE hEvent[2] = { NULL, NULL };
    DWORD dwStatus = 0;

    hEvent[0] = OpenEventW(SYNCHRONIZE, FALSE, L"EPCONNECT");
    hEvent[1] = OpenEventW(SYNCHRONIZE, FALSE, L"ENDTHREAD");

    //Verify both the handle are not null and able to open the events

    while (TRUE)
    {
        dwStatus = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
        switch (dwStatus)
        {
            /* If the hot cache handle is signalled this is a success */
        case WAIT_OBJECT_0:
        {
                              //Connect Activity
        }
            break;
        case WAIT_OBJECT_0 + 1:
        {
                                  CloseHandle(hEvent[0]);
                                  CloseHandle(hEvent[1]);
                                  return 0;

        }
        }

    }

    return 0;
}

DWORD WINAPI EndpointDisConnectMonitorThread(LPVOID Param)
{
    HANDLE hEvent[2] = { NULL, NULL };
    DWORD dwStatus = 0;
    hEvent[0] = OpenEventW(SYNCHRONIZE, FALSE, L"EPDISCONNECT");
    hEvent[1] = OpenEventW(SYNCHRONIZE, FALSE, L"ENDTHREAD");

    //Verify both the handle are not null and able to open the events

    while (TRUE)
    {
        dwStatus = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
        switch (dwStatus)
        {
            /* If the hot cache handle is signalled this is a success */
        case WAIT_OBJECT_0:
        {
                              // Disconnect activity
        }
            break;
        case WAIT_OBJECT_0 + 1:
        {
                                  CloseHandle(hEvent[0]);
                                  CloseHandle(hEvent[1]);
                                  return 0;

        }
        }

    }
    return 0;
}
/********************************************************************
*
*  EndpointSystemEventMonitor
*
*    Verify the ability to wait on endpoint client system events
*    Connect/Disconnect inside a double-hop session;
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/
void EndpointSystemEventMonitor()
{
    DWORD eventFlags = 0;
    HANDLE hConnect = NULL;
    DWORD ConnectThreadid = 0;
    HANDLE hDisConnect = NULL;
    DWORD DisConnectThreadid = 0;
    HANDLE hConnectEvent = NULL;
    HANDLE hDisConnectEvent = NULL;
    HANDLE hEndThreadEvent = NULL;

    hConnectEvent = CreateEventW(NULL, FALSE, FALSE, EPCONNECT);
    if (!hConnectEvent) 
    {
        _tprintf(TEXT("*** CreateEvent failed for EPCONNECT - error: %d\n"), GetLastError());
    }

    hDisConnectEvent = CreateEventW(NULL, FALSE, FALSE, EPDISCONNECT);
    if (!hDisConnectEvent)
    {
        _tprintf(TEXT("*** CreateEvent failed for EPDISCONNECT - error: %d\n"), GetLastError());
    }

    hEndThreadEvent = CreateEventW(NULL, FALSE, FALSE, ENDTHREAD);
    if (!hEndThreadEvent)
    {
        _tprintf(TEXT("*** CreateEvent failed for ENDTHREAD - error: %d\n"), GetLastError());
    }

    hConnect = CreateThread(NULL, 0, EndpointConnectMonitorThread, NULL, 0, &ConnectThreadid);
    if (!hConnect)
    {
        _tprintf(TEXT("*** Creation of EndpointConnectMonitorThread failed - error: %d\n"), GetLastError());
    }

    hDisConnect = CreateThread(NULL, 0, EndpointDisConnectMonitorThread, NULL, 0, &DisConnectThreadid);
    if (!hDisConnect)
    {
        _tprintf(TEXT("*** Creation of EndpointDisConnectMonitorThread failed - error: %d\n"), GetLastError());
    }

    while (TRUE)
    {

        if (WFWaitEndPointClientEventInDoubleHop(WF_CURRENT_SERVER, WF_EVENT_CONNECT | WF_EVENT_DISCONNECT, &eventFlags))
        {
            if ((eventFlags & WF_EVENT_CONNECT) != 0)
            {
                _tprintf(TEXT("\nICA Connect Event...\n"));
                SetEvent(hConnectEvent);
            }

            if ((eventFlags & WF_EVENT_DISCONNECT) != 0)
            {
                _tprintf(TEXT("\nICA Disconnect Event...\n"));
                SetEvent(hDisConnectEvent);
            }
        }
        else
        {
            SetEvent(hEndThreadEvent);
            break;
        }
    }

    if(hConnectEvent) 
    {
        CloseHandle(hConnectEvent);
    }

    if(hDisConnectEvent) 
    {
        CloseHandle(hDisConnectEvent);
    }
    
    if(hEndThreadEvent) 
    {
        CloseHandle(hEndThreadEvent);
    }
    
    return;
}

/********************************************************************
*
*  EnumerateVirtualChannelNames
*
*    Verify the ability to enumerate all the Virtual Channels opened
*    in the session
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void EnumerateVirtualChannelNames()
{
    DWORD SessionId;
    int Status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    _tprintf(TEXT("*** Querying VC names for session id: %d\n"), SessionId);

    HANDLE hSession = NULL;

    int status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        PWF_SMC_SESSION_VC_NAME pVcSessionNames = (PWF_SMC_SESSION_VC_NAME)malloc(sizeof(WF_SMC_SESSION_VC_NAME) * MEMORY_ALLOCATION_SIZE);
        PWF_SMC_SESSION_VC_NAME pOrigVcSessionNames = pVcSessionNames;
        if (pVcSessionNames)
        {
            /*
            We need to define Versionnumber member of the Data Structures used in API Calls.
            PWF_SMC_SESSION_VC_NAME should have the VersionNumber member value = 1.
            Refer WFAPI SDK Documentation.
            */
            pVcSessionNames->VersionNumber = SMC_STRUCTURE_VERSION_V1;

            status = WFSmcGetSessionVirtualChannelNames(WF_CURRENT_SERVER_HANDLE, hSession, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Session_VC_Name), pVcSessionNames, &ret);

            if (status != 0)
            {
                _tprintf(TEXT("\nERROR WFSmcGetSessionVirtualChannelNames %d"), ret);
            }
            else
            {
                _tprintf(TEXT("\Number of VCs %d"), ret);

                for (int i = 0; i < ret; i++)
                {
                    _tprintf(TEXT("\nVC %d Name: %s"), i + 1, pVcSessionNames->rgwchName);
                    pVcSessionNames++;
                }
            }
            if(pOrigVcSessionNames)
                free(pOrigVcSessionNames);
        }
        else
        {
            _tprintf(TEXT("*** ERROR: pVcSessionNames allocation failed\n"));
        }

    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
*
*  CheckSmcCompatibility()
*
*  Checks the SMC version and features supported on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void CheckSmcCompatibility()
{
    UINT32 pVersionSupported = 0, pFeaturesSupported = 0;
    if (!WFSmcGetVersionSupported(WF_CURRENT_SERVER_HANDLE, &pVersionSupported, &pFeaturesSupported))
    {
        printf("pVersionSupported: %d\n", pVersionSupported);
        printf("pFeaturesSupported: %d\n", pFeaturesSupported);
    }
    else
    {
        printf("WFSmcGetVersionSupported Api call failed...\n");
    }
}

/********************************************************************
*
*  GetServerData()
*
*  Gets the statistics of the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void GetServerData()
{
    PWF_SMC_SERVER_DATA    pServerData = (PWF_SMC_SERVER_DATA)malloc(sizeof(WF_SMC_SERVER_DATA));
    PWF_SMC_SERVER_VC_DATA pServerVCData = (PWF_SMC_SERVER_VC_DATA)malloc(sizeof(WF_SMC_SERVER_VC_DATA) * MEMORY_ALLOCATION_SIZE);
    if (pServerData && pServerVCData)
    {
        /*
        We need to define Versionnumber member of the Data Structures used in API Calls.
        PWF_SMC_SERVER_DATA and PWF_SMC_SERVER_VC_DATA should have the VersionNumber member value = 1.
        Refer WFAPI SDK Documentation.
        */ 
        pServerData->VersionNumber = SMC_STRUCTURE_VERSION_V1;
        pServerVCData->VersionNumber = SMC_STRUCTURE_VERSION_V1;
        int ret = 0;
        int status = WFSmcGetServerData(WF_CURRENT_SERVER_HANDLE, sizeof(Ctx_Smc_Server_Data), pServerData, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Server_VC_Data), pServerVCData, &ret);
        if (status == ERROR_SUCCESS)
        {
            printf("TotalBytesSentPreCompression: %llu\n", pServerData->TotalBytesSentPreCompression);
            printf("TotalBytesSentPostCompression: %llu\n", pServerData->TotalBytesSentPostCompression);
            printf("TotalBytesReceivedPreExpansion: %llu\n", pServerData->TotalBytesReceivedPreExpansion);
            printf("TotalBytesReceivedPostExpansion: %llu\n", pServerData->TotalBytesReceivedPostExpansion);
            printf("BandwidthSentBitsPerSec: %lu\n", pServerData->BandwidthSentBitsPerSec);
            printf("BandwidthReceivedBitsPerSec: %lu\n", pServerData->BandwidthReceivedBitsPerSec);
            printf("CompressionNumeratorSent: %lu\n", pServerData->CompressionNumeratorSent);
            printf("CompressionNumeratorReceived: %lu\n", pServerData->CompressionNumeratorReceived);
            printf("CompressionDenominatorSent: %lu\n", pServerData->CompressionDenominatorSent);
            printf("CompressionDenominatorReceived: %lu\n", pServerData->CompressionDenominatorReceived);
        }
        else
        {
            printf("WFSmcGetServerData Api call failed...\n");
        }
    }
    else
    {
        printf("*** ERROR: pServerData or pServerVCData allocation failed\n");
    }
    if (pServerData)
        free(pServerData);
    if (pServerVCData)
        free(pServerVCData);
}

/********************************************************************
*
*  GetSessionData();
*
*  Gets the statistics of the session on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void GetSessionData()
{
    DWORD SessionId = 0;
    int status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    if (!status)
    {
        printf("ProcessIdToSessionId failed\n");
        return;
    }

    HANDLE hSession = NULL;

    status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        PWF_SMC_SESSION_DATA    pSessionData = (PWF_SMC_SESSION_DATA)malloc(sizeof(WF_SMC_SESSION_DATA));
        PWF_SMC_SESSION_VC_DATA pSessionVCData = (PWF_SMC_SESSION_VC_DATA)malloc(sizeof(WF_SMC_SESSION_VC_DATA) * MEMORY_ALLOCATION_SIZE);
        if (pSessionData && pSessionVCData)
        {
            /*
            We need to define Versionnumber member of the Data Structures used in API Calls. 
            PWF_SMC_SESSION_DATA and PWF_SMC_SESSION_VC_DATA should have the VersionNumber member value = 1.
            Refer WFAPI SDK Documentation.
             */
            pSessionData->VersionNumber = SMC_STRUCTURE_VERSION_V1;
            pSessionVCData->VersionNumber = SMC_STRUCTURE_VERSION_V1;
            status = WFSmcGetSessionData(WF_CURRENT_SERVER_HANDLE, hSession, sizeof(Ctx_Smc_Session_Data), pSessionData, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Session_VC_Data), pSessionVCData, &ret);
            if (status == ERROR_SUCCESS)
            {
                printf("BandwidthSentBitsPerSecond: %lu\n", pSessionData->BandwidthSentBitsPerSecond);
                printf("BytesSentPreCompression: %lu\n", pSessionData->BytesSentPreCompression);
                printf("BytesSentPostCompression: %lu\n", pSessionData->BytesSentPostCompression);
                printf("BandwidthReceivedBitsPerSecond: %lu\n", pSessionData->BandwidthReceivedBitsPerSecond);
                printf("BytesReceivedPreExpansion: %lu\n", pSessionData->BytesReceivedPreExpansion);
                printf("BytesReceivedPostExpansion: %lu\n", pSessionData->BytesReceivedPostExpansion);
                printf("Number of VCs %d\n", ret);
            }
            else
            {
                printf("WFSmcGetSessionData API call Failed...%d", status);
            }
        }
        else
        {
            printf("*** ERROR: pSessionData or pSessionVCData allocation failed\n");
        }
        if(pSessionData)
            free(pSessionData);
        if(pSessionVCData)
            free(pSessionVCData);
        if (WFSmcCloseSessionHandle(WF_CURRENT_SERVER_HANDLE, hSession) != ERROR_SUCCESS)
            printf("Failed to close session handle..\n");
    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
*
*  GetSessionDataV2()
*
*  Gets the statistics of the session on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void GetSessionDataV2()
{
    DWORD SessionId = 0;
    int status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    if (!status)
    {
        printf("ProcessIdToSessionId failed\n");
        return;
    }
    HANDLE hSession = NULL;

    status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        pCtx_Smc_Session_Data_V2 pSessionDataV2 = (pCtx_Smc_Session_Data_V2)malloc(sizeof(Ctx_Smc_Session_Data_V2));
        pCtx_Smc_Session_VC_Data_V2 pSessionVCDataV2 = (pCtx_Smc_Session_VC_Data_V2)malloc(sizeof(Ctx_Smc_Session_VC_Data_V2) * MEMORY_ALLOCATION_SIZE);
        if (pSessionDataV2 && pSessionVCDataV2)
        {
            /*
            We need to define Versionnumber member of the Data Structures used in API Calls. 
            pCtx_Smc_Session_Data_V2 and pCtx_Smc_Session_VC_Data_V2 should have the VersionNumber member value = 2.
            Refer WFAPI SDK Documentation. 
            */
            pSessionDataV2->VersionNumber = SMC_STRUCTURE_VERSION_V2;
            pSessionVCDataV2->VersionNumber = SMC_STRUCTURE_VERSION_V2;
            status = WFSmcGetSessionDataV2(WF_CURRENT_SERVER_HANDLE, hSession, sizeof(Ctx_Smc_Session_Data_V2), pSessionDataV2, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Session_VC_Data_V2), pSessionVCDataV2, &ret);
            if (status == ERROR_SUCCESS)
            {
                printf("QosLevel: %lu\n", pSessionDataV2->QosLevel);
                printf("BandwidthSentBitsPerSecond: %lu\n", pSessionDataV2->BandwidthSentBitsPerSecond);
                printf("BytesSentPreCompression: %lu\n", pSessionDataV2->BytesSentPreCompression);
                printf("BytesSentPostCompression: %lu\n", pSessionDataV2->BytesSentPostCompression);
                printf("BandwidthReceivedBitsPerSecond: %lu\n", pSessionDataV2->BandwidthReceivedBitsPerSecond);
                printf("BytesReceivedPreExpansion: %lu\n", pSessionDataV2->BytesReceivedPreExpansion);
                printf("BytesReceivedPostExpansion: %lu\n", pSessionDataV2->BytesReceivedPostExpansion);
                printf("CompressionNumeratorSent: %lu\n", pSessionDataV2->CompressionNumeratorSent);
                printf("LastClientLatency: %lu\n", pSessionDataV2->LastClientLatency);
                printf("AverageClientLatency: %lu\n", pSessionDataV2->AverageClientLatency);
                printf("RoundTripDeviation: %lu\n", pSessionDataV2->RoundTripDeviation);
            }
            else
                printf("WFSmcGetSessionDataV2 API call failed...\n");
            if(pSessionDataV2)
                free(pSessionDataV2);
            if(pSessionVCDataV2)
                free(pSessionVCDataV2);
            if (WFSmcCloseSessionHandle(WF_CURRENT_SERVER_HANDLE, hSession) != ERROR_SUCCESS)
                printf("Failed to close session handle..\n");
        }
        else
        {
            printf("*** ERROR: pSessionDataV2 or pSessionVCDataV2 allocation failed\n");
        }
    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
*
*  GetSessionLatencyData()
*
*  Gets the latency statistics of the session on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void GetSessionLatencyData()
{
    DWORD SessionId = 0;
    int status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    if (!status)
    {
        printf("ProcessIdToSessionId failed\n");
        return;
    }
    HANDLE hSession = NULL;

    status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        UINT32 pLastLatency = 0, pAverageLatency = 0, pRoundTripDeviation = 0, pTickCountLastUpdateTime = 0;
        status = WFSmcGetSessionLatencyData(WF_CURRENT_SERVER_HANDLE, hSession, &pLastLatency, &pAverageLatency, &pRoundTripDeviation, &pTickCountLastUpdateTime);
        if (status == ERROR_SUCCESS)
        {
            printf("pLastLatency: %lu\n", pLastLatency);
            printf("pAverageLatency: %lu\n", pAverageLatency);
            printf("pRoundTripDeviation: %lu\n", pRoundTripDeviation);
            printf("pTickCountLastUpdateTime: %lu\n", pTickCountLastUpdateTime);
        }
        else
        {
            printf("WFSmcGetSessionLatencyData API call failed...\n");
        }
        if (WFSmcCloseSessionHandle(WF_CURRENT_SERVER_HANDLE, hSession) != ERROR_SUCCESS)
            printf("Failed to close session handle..\n");
    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
*
*  SessionPrioritiesTest()
*
*  Gets and sets the priorities of virtual channels for the session on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void SessionPrioritiesTest()
{
    DWORD SessionId;
    int status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    if (!status)
    {
        printf("ProcessIdToSessionId failed\n");
        return;
    }

    HANDLE hSession = NULL;

    status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        PWF_SMC_SESSION_VC_NAME pVcSessionNames = (PWF_SMC_SESSION_VC_NAME)malloc(sizeof(WF_SMC_SESSION_VC_NAME) * MEMORY_ALLOCATION_SIZE);
        PWF_SMC_SESSION_VC_NAME orig_pVcSessionNames = pVcSessionNames;
        if (pVcSessionNames)
        {
            /*
            We need to define Versionnumber member of the Data Structures used in API Calls.
            PWF_SMC_SESSION_VC_NAME should have the VersionNumber member value = 1.
            Refer WFAPI SDK Documentation.
            */
            pVcSessionNames->VersionNumber = SMC_STRUCTURE_VERSION_V1;
            status = WFSmcGetSessionVirtualChannelNames(WF_CURRENT_SERVER_HANDLE, hSession, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Session_VC_Name), pVcSessionNames, &ret);
            UINT32* VCID = (UINT32*)malloc(ret * sizeof(UINT32));
            UINT32* pVCPriorities = (UINT32*)malloc(ret * sizeof(UINT32));
            UINT32* original_pVCPriorities = (UINT32*)malloc(ret * sizeof(UINT32));
            if (VCID && pVCPriorities && original_pVCPriorities)
            {
                for (int i = 0; i < ret; i++)
                {
                    printf("VC NAME: %ls ID: %d\n", pVcSessionNames->rgwchName, pVcSessionNames->idVC);
                    VCID[i] = pVcSessionNames->idVC;
                    pVcSessionNames++;
                }
                status = WFSmcGetSessionPriorities(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCPriorities);

                if (status == ERROR_SUCCESS)
                {
                    //Preserve original values before changing session priorities
                    memcpy(original_pVCPriorities, pVCPriorities, ret * sizeof(UINT32));

                    for (int i = 0; i < ret; i++)
                    {
                        printf("Session priority of VC - %d : %d\n", VCID[i], pVCPriorities[i]);
                        
                        //Set random values for the priorities of the virtual channels for our test
                        pVCPriorities[i] = rand() % 4;
                    }
                    status = WFSmcSetSessionPriorities(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCPriorities);
                    if (status == ERROR_SUCCESS)
                    {
                        memset(pVCPriorities, 0, ret * sizeof(UINT32));
                        status = WFSmcGetSessionPriorities(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCPriorities);
                        if (status == ERROR_SUCCESS)
                        {
                            printf("VC's after setting new session priorities\n");
                            for (int i = 0; i < ret; i++)
                            {
                                printf("New Session Priority of VC - %d : %d\n", VCID[i], pVCPriorities[i]);
                            }
                        }
                        else
                        {
                            printf("WFSmcGetSessionPriorities Api call failed...\n");
                        }
                    }
                    else
                    {
                        printf("WFSmcSetSessionPriorities Api call failed...\n");
                    }
                }
                else
                {
                    printf("WFSmcGetSessionPriorities Api call failed...\n");
                }
                status = WFSmcSetSessionPriorities(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, original_pVCPriorities);
                if (status != ERROR_SUCCESS)
                    printf("Failed to restore original values\n");
                
                //Free allocated memory
                if (VCID) 
                    free(VCID);
                if (pVCPriorities) 
                    free(pVCPriorities);
                if (original_pVCPriorities) 
                    free(original_pVCPriorities);
            }
            else
            {
                printf("*** ERROR: pVCID or pVCPriorities or original_pVCPriorities allocation failed\n");
            }
        }
        else
        {
            printf("*** ERROR: pVcSessionNames allocation failed\n");
        }
        if (orig_pVcSessionNames)
            free(orig_pVcSessionNames);
    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
*
*  SessionBandwidthLimitsTest()
*
*  Gets and sets the bandwidthlimits of virtual channels for the session on the current server
*
*
* ENTRY:
*    nothing
*
*
* EXIT:
*    nothing
*
*******************************************************************/

void SessionBandwidthLimitsTest()
{
    DWORD SessionId;
    int status = ProcessIdToSessionId(GetCurrentProcessId(), &SessionId);
    if (!status)
    {
        printf("ProcessIdToSessionId failed\n");
        return;
    }
    HANDLE hSession = NULL;

    status = WFSmcOpenSessionHandle(WF_CURRENT_SERVER_HANDLE, SessionId, &hSession);
    UINT32 ret = 0;
    if (status == ERROR_SUCCESS && hSession != NULL)
    {
        PWF_SMC_SESSION_VC_NAME pVcSessionNames = (PWF_SMC_SESSION_VC_NAME)malloc(sizeof(WF_SMC_SESSION_VC_NAME) * MEMORY_ALLOCATION_SIZE);
        PWF_SMC_SESSION_VC_NAME orig_pVcSessionNames = pVcSessionNames;
        if (pVcSessionNames)
        {
            /*
            We need to define Versionnumber member of the Data Structures used in API Calls.
            PWF_SMC_SESSION_VC_NAME should have the VersionNumber member value = 1.
            Refer WFAPI SDK Documentation.
            */
            pVcSessionNames->VersionNumber = SMC_STRUCTURE_VERSION_V1;
            status = WFSmcGetSessionVirtualChannelNames(WF_CURRENT_SERVER_HANDLE, hSession, MEMORY_ALLOCATION_SIZE * sizeof(Ctx_Smc_Session_VC_Name), pVcSessionNames, &ret);
            UINT32* VCID = (UINT32*)malloc(ret * sizeof(UINT32));
            UINT32* pVCBandwidthLimits = (UINT32*)malloc(ret * sizeof(UINT32));
            UINT32* original_pVCBandwidthLimits = (UINT32*)malloc(ret * sizeof(UINT32));
            if (VCID && pVCBandwidthLimits && original_pVCBandwidthLimits)
            {
                for (int i = 0; i < ret; i++)
                {
                    printf("VC NAME: %ls ID: %d\n", pVcSessionNames->rgwchName, pVcSessionNames->idVC);
                    VCID[i] = pVcSessionNames->idVC;
                    pVcSessionNames++;
                }
                status = WFSmcGetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCBandwidthLimits);

                if (status == ERROR_SUCCESS)
                {
                    //Preserve original values before changing session priorities
                    memcpy(original_pVCBandwidthLimits, pVCBandwidthLimits, ret * sizeof(UINT32));

                    for (int i = 0; i < ret; i++)
                    {
                        printf("Session Bandwidth Limit of VC - %d : %d\n", VCID[i], pVCBandwidthLimits[i]);
                        
                        //Set random values for the bandwidthlimits of the virtual channels for our test
                        pVCBandwidthLimits[i] = 1000 + i;
                    }
                    status = WFSmcSetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCBandwidthLimits);
                    if (status == ERROR_SUCCESS)
                    {
                        memset(pVCBandwidthLimits, 0, ret * sizeof(UINT32));
                        status = WFSmcGetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, pVCBandwidthLimits);
                        if (status == ERROR_SUCCESS)
                        {
                            printf("VC's after setting new Bandwidth Limits\n");
                            for (int i = 0; i < ret; i++)
                            {
                                printf("Nes Session Bandwidth Limit of VC - %d : %d\n", VCID[i], pVCBandwidthLimits[i]);
                            }
                        }
                        else
                        {
                            printf("WFSmcGetSessionBandwidthLimits Api Call failed...\n");
                        }
                    }
                    else
                    {
                        printf("WFSmcSetSessionBandwidthLimits Api Call failed...\n");
                    }
                }
                else
                {
                    printf("WFSmcGetSessionBandwidthLimits Api Call failed...\n");
                }
                status = WFSmcSetSessionBandwidthLimits(WF_CURRENT_SERVER_HANDLE, hSession, ret, VCID, original_pVCBandwidthLimits);
                if (status != ERROR_SUCCESS)
                    printf("Failed to restore original values\n");               
                if (VCID) 
                    free(VCID);
                if (pVCBandwidthLimits) 
                    free(pVCBandwidthLimits);
                if (original_pVCBandwidthLimits) 
                    free(original_pVCBandwidthLimits);

            }
            else
            {
                printf("*** ERROR: pVCID or pVCBandwidthLimits or original_pVCBandwidthLimits allocation failed\n");
            }
        }
        else
        {
            printf("*** ERROR: pVcSessionNames allocation failed\n");
        }
        if (orig_pVcSessionNames)
            free(orig_pVcSessionNames);
    }
    else
    {
        _tprintf(TEXT("*** ERROR: Session handle failed\n"));
    }
}

/********************************************************************
 *
 *  main
 *
 *******************************************************************/

void _cdecl
main( INT argc, CHAR **argv )
{
    HANDLE hServer;
    TCHAR  szChoice[256];
    BOOL   fRunAll;
    ULONG usChoice;
    TCHAR  lpUserName  [256];
    TCHAR  lpDomainName[256] = TEXT("DOMAIN");// default in case call fails...
    TCHAR  lpServerName[256];
    DWORD  dwUserName   = 256;
    DWORD  dwDomainName = 256;
    DWORD  dwServerName = 256;
    //TCHAR  serverName [256];
    //TCHAR  szSessionID [356];
    // It seems that lpUserName and lpDomainName might also need the adjustment of 
    // dividing by sizeof(TCHAR), but I am not 100% sure now. At this time I am focusing on
    // calls to GetComputerName.
    WKSTA_INFO_100 *pWorkstationInfo = NULL;

    /*
     *  First, get User, Server, and Domain names.  Note that the domain name
     *  always comes across as UNICODE, so we need to convert it if we're
     *  running the ANSI version.
     */
    GetUserName( lpUserName, &dwUserName );
    GetComputerName( lpServerName, &dwServerName );
    if ( NetWkstaGetInfo( NULL, 100, (LPBYTE *)&pWorkstationInfo ) ==
                          NERR_Success ) {
#ifdef UNICODE
        wcscpy_s( lpDomainName,DOMAIN_LENGTH, pWorkstationInfo->wki100_langroup );
#else  // UNICODE
        wcstombs( lpDomainName, (LPWSTR)(pWorkstationInfo->wki100_langroup),
                  sizeof(lpDomainName) );
#endif // UNICODE
    } else {
        _tprintf( TEXT("*** Error getting domain name; using DOMAIN as default.\n") );
    }

#ifdef UNICODE
    _tprintf( TEXT("WFAPI Test Program (UNICODE)\n") );
#else
    _tprintf( TEXT("WFAPI Test Program (ANSI)\n") );
#endif

print_again:
    _tprintf( TEXT("\n\
1     Resereved (no test)              2     Enumerate Sessions\n\
3     Enumerate Processes              4     WF Version\n\
5     Reserved (no test)               6     WF Working Directory\n\
7     WF OEM Id                        8     WF Session Id\n\
9     WF User Name                     10    WF WinStation Name\n\
11    WF Domain Name                   12    WF Connect State\n\
13(C) WF Client Build Number           14(C) WF Client Name\n\
15(C)WF Client Directory               16(C)WF Client Product Id\n\
17(C)WF Client Address                 18(C)WF Client Display\n\
19(C)WF Client Cache                   20(C)WF Client Drives\n\
21(C) WF ICA Buffer Length             22(C) WF Initial Program\n\
23(C)WF Client Application             24(C)Virtual Channel Test\n\
25(D)Query User Config Domain Test     26(D)Set User Config Domain Test\n\
27(L)Query User Config Local Test      28(L)Set User Config Local Test\n\
29(C) WF Client Information            30(C) WF Application Information\n\
31(C) WF User Information              32(C) Reserved (no test)\n\
33(C) WF Session Time                  34(C) WF Enumerate Processes (more info)\n\
35    WF Wait System Events            36(C) WF Send Message\n\
37(C) WF Disconnect                    38(C) WF Disconnect EX\n\
39(C) WF Log OFF                       40(C) WF Terminate Process\n\
41(C) WF Shut Down System              42(C) WF Window Position\n\
43(C)* WF Licensing Model              44    WF GetCtxSessionKeyList\n\
45(C)* WF Doublehop Session            46(C)WF EndPoint Client information in double - hop\n\
47(C)* WF Wait System Events in  double - hop\n\
48    WF Supported SMC Version         49    WFSmc Get Server Data\n\
50(C) WFSmc Get Virtual Channel Names  51(C) WFSmc Get Session Data\n\
52(C) WFSmc Get Session Data V2        53(C) WFSmc Get Session Latency Data\n\
54(C) WFSmc Session Priorities Test    55(C) WFSmc Session BandwidthLimits Test"
));

#define MAX_CHOICE 55

    _tprintf( TEXT("\n\
NOTES: C = valid only from client.  D = must be member of a Domain.\n\
       L = uses local accounts.  (Both D and L use the User specified below,\n\
       and require you to be logged into an administrator account.)\n\
       * = Specific to XenDesktop environment\n\
\n\
A.  Run all tests\n\
Q.  Exit\n\
\n\
Current defaults (use letter to change):\n\
  U. User  : %s\n\
  D. Domain: %s\n\
  S. Server: %s\n\
\n\
Your choice: "), lpUserName, lpDomainName, lpServerName );

    (void)_fgetts( szChoice, 256, stdin );

    fRunAll = (szChoice[0] == 'A' || szChoice[0] == 'a');
    if ( szChoice[0] == 'Q' || szChoice[0] == 'q' )
    {
        goto done;
    }
    if ( szChoice[0] == 'U' || szChoice[0] == 'u' ) 
    {
        _tprintf( TEXT("\nUser name to use: ") );
        (void)_fgetts( lpUserName, 256, stdin );
        goto print_again;
    }
    if ( szChoice[0] == 'D' || szChoice[0] == 'd' ) 
    {
        _tprintf( TEXT("\nDomain name to use: ") );
        (void)_fgetts( lpDomainName, 256, stdin );
        goto print_again;
    }
    if ( szChoice[0] == 'S' || szChoice[0] == 's' ) 
    {
        _tprintf( TEXT("\nServer name to use: ") );
        (void)_fgetts( lpServerName, 256, stdin );
        goto print_again;
    }

    usChoice = _ttoi( szChoice );
    if ( !fRunAll && (usChoice < 1 || usChoice > MAX_CHOICE) ) 
    {
        goto print_again;
    }
    if (fRunAll)
        usChoice = 1;

    //if ( fRunAll || usChoice == 1 ) 
    //{
    //    /* EnumerateServers( NULL );
    //    EnumerateServers( lpDomainName );*/
    //}

    if ( fRunAll || usChoice == 2 )
    {
        EnumerateSessions( TEXT("CURRENT SERVER"), WF_CURRENT_SERVER_HANDLE );
        hServer = WFOpenServer( lpServerName );
        if ( hServer )
        {
           EnumerateSessions( lpServerName, hServer );
           WFCloseServer( hServer );
        }
        else 
        {
            _tprintf( TEXT("*** ERROR: cannot open server %s.\n"),
                      lpServerName );
        }
    }

    if ( fRunAll || usChoice == 3 ) 
    {
        EnumerateProcesses( TEXT("CURRENT SERVER"), WF_CURRENT_SERVER_HANDLE );
        hServer = WFOpenServer( lpServerName );
        if ( hServer ) 
        {
           EnumerateProcesses( lpServerName, hServer );
           WFCloseServer( hServer );
        }
        else
        {
            _tprintf( TEXT("*** ERROR: cannot open server %s.\n"),
                      lpServerName );
        }
    }

    if ( fRunAll || usChoice == 4 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFVersion );
    }
    if ( fRunAll || usChoice == 6 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFWorkingDirectory );
    }
    if ( fRunAll || usChoice == 7 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFOEMId );
    }
    if ( fRunAll || usChoice == 8 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFSessionId );
    }
    if ( fRunAll || usChoice == 9 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFUserName );
    }
    if ( fRunAll || usChoice == 10 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFWinStationName );
    }
    if ( fRunAll || usChoice == 11 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFDomainName );
    }
    if ( fRunAll || usChoice == 12 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFConnectState );
    }
    if ( fRunAll || usChoice == 13 ) 
    {
         QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientBuildNumber );
    }
    if ( fRunAll || usChoice == 14 ) 
    {
         QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientName );
    }
    if ( fRunAll || usChoice == 15 ) 
    {
         QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientDirectory );
    }
    if ( fRunAll || usChoice == 16 ) 
    {
         QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientProductId );
    }
    if ( fRunAll || usChoice == 17 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientAddress );
    }
    if ( fRunAll || usChoice == 18 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientDisplay );
    }
    if ( fRunAll || usChoice == 19 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientCache );
    }
    if ( fRunAll || usChoice == 20 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientDrives );
    }
    if ( fRunAll || usChoice == 21 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFICABufferLength  );
    }
    if ( fRunAll || usChoice == 22 ) 
    {
            QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFInitialProgram );
    }

    if ( fRunAll || usChoice == 23 ) 
    {
            QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFApplicationName );
    }
    if ( fRunAll || usChoice == 24 ) 
    {
            VirtualChannelTest();
    }
    if ( fRunAll || usChoice == 25 ) 
    {
        WFQueryUserConfigTest( TEST_DOMAIN, lpUserName );
    }
    if ( fRunAll || usChoice == 26 ) 
    {
        WFSetUserConfigTest( TEST_DOMAIN, lpUserName );
    }
    if ( fRunAll || usChoice == 27 ) 
    {
        WFQueryUserConfigTest( TEST_LOCAL, lpUserName );
    }
    if ( fRunAll || usChoice == 28 ) 
    {
        WFSetUserConfigTest( TEST_LOCAL, lpUserName );
    }

    if ( fRunAll || usChoice == 29 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientInfo );
    }

    if ( fRunAll || usChoice == 30 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFAppInfo );
    }

    if ( fRunAll || usChoice == 31 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFUserInfo );
    }

    //if ( fRunAll || usChoice == 32 ) 
    //{
    //    //QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFClientLatency );
    //}

    if ( fRunAll || usChoice == 33 ) 
    {
            QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFSessionTime );
    }

    if ( fRunAll || usChoice == 34 ) 
    {
#ifdef UNICODE
        EnumerateProcessesEx( TEXT("CURRENT SERVER"), WF_CURRENT_SERVER_HANDLE );
#else

        EnumerateProcessesExA( TEXT("CURRENT SERVER"), WF_CURRENT_SERVER_HANDLE );
#endif

    }

    if( usChoice == 35 )
    {
        WaitSystemEventTest();
    }
    if( fRunAll || usChoice == 36 )
    {
        TestSendMessage();
    }
    if(fRunAll || usChoice == 37)
    {
        TestDisconnectSession();
    }
    if(fRunAll || usChoice == 38 )
    {
        TestDisconnectSessionEx(0xffff);
    }
    if( fRunAll || usChoice == 39)
    { 
        TestLogOffSession();
    }
    if(fRunAll || usChoice == 40)
    {
        TerminateAProcess();
    }
    if(usChoice == 41)// dont want to shutdown the system during a run all
    {
        ShutdownSystemTest();
    }
    if(fRunAll || usChoice == 42)
    {
        WFWindowPositionSDKTest();
    }
    if ( fRunAll || usChoice == 43 ) 
    {
        QuerySession( WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION, WFLicensingModel );
    }
    if ( fRunAll || usChoice == 44 ) 
    {
        GetCtxSessionKeyList(TEXT("CURRENT SERVER"), WF_CURRENT_SERVER_HANDLE);
    }
    if (fRunAll || usChoice == 45)
    {
        VerifyDoubleHopSession(WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION);
    }
    if (fRunAll || usChoice == 46)
    {
        QueryEndpointDetailsInDoubleHop(WF_CURRENT_SERVER_HANDLE, WF_CURRENT_SESSION);
    }
    if (fRunAll || usChoice == 47)
    {
        WaitSystemEventTestInDoublehop();
    }
    if (fRunAll || usChoice == 48)
    {
        CheckSmcCompatibility();
    }
    if (fRunAll || usChoice == 49)
    {
        GetServerData();
    }
    if (fRunAll || usChoice == 50)
    {
        EnumerateVirtualChannelNames();
    }
    if (fRunAll || usChoice == 51)
    {
        GetSessionData();
    }
    if (fRunAll || usChoice == 52)
    {
        GetSessionDataV2();
    }
    if (fRunAll || usChoice == 53)
    {
        GetSessionLatencyData();
    }
    if (fRunAll || usChoice == 54)
    {
        SessionPrioritiesTest();
    }
    if (fRunAll || usChoice == 55)
    {
        SessionBandwidthLimitsTest();
    }
    
    _tprintf( TEXT("\nHit Enter to continue...") );
    (void)_fgetts( szChoice, 256, stdin );
    goto print_again;

done: ;

} // main()




