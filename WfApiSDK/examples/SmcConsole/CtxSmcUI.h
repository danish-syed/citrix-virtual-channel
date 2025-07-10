/* Copyright 2009 Citrix Systems, Inc.  */
/* 
 * Copyright (c) 2001 Citrix Systems, Inc. All Rights Reserved.
 */ 
#ifndef CTXSMCUI_H
#define CTXSMCUI_H

/////////////////////////////////////////////////////////////////////
// OS or Compiler specific headers
#ifndef _WINDOWS_
    #include <stdio.h>
    #include <stdlib.h>
    #include <windows.h>
    #include <commctrl.h>
#endif

// OS or compiler specific function decorators
#ifndef OSCALLBACK
    #define OSCALLBACK __stdcall
#endif

// OS specific parameter types
#ifndef OSPARAM
    #define OSPARAM unsigned long
#endif

/////////////////////////////////////////////////////////////////////
// system wide common defines, generally same as in WinDef.h
#ifndef BYTE
    typedef unsigned char BYTE;
#endif

#ifndef UINT16
    typedef unsigned short UINT16;
#endif

#ifndef UINT32
    typedef unsigned int UINT32;
#endif

#ifndef UINT64
    typedef unsigned __int64 UINT64;
#endif

#if !defined( wchar_t) && !defined( _WCHAR_T_DEFINED) && !defined(_NATIVE_WCHAR_T_DEFINED)
    typedef unsigned short wchar_t;
#endif

/////////////////////////////////////////////////////////////////////
// system wide common macros
//
#ifdef _DEBUG
// standard assert does not work in a service
//    #include <assert.h>
//    #define ASSERT assert
#pragma warning (disable : 4127)
    #define ASSERT(x) { if (!(x)) DebugBreak(); }
#else
    #define ASSERT
#endif

#endif // CTXSMCUI_H

