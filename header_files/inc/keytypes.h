/**********************************************************************
*
*  keytypes.h
*
*  This module contains structures for key press/release storage
*
*  $Id$
*
*  Copyright 2001-2010 Citrix Systems, Inc.  
*
***********************************************************************/

#ifndef __KEYTYPES_H__
#define __KEYTYPES_H__

#pragma once

/* WdScanCode structure */

typedef struct _SCANCODE {
    USHORT  cScanCodes;
    LPVOID  pScanCodes;
    USHORT  uiRepeat;
} SCANCODE, * PSCANCODE;

/* WdCharCode structure */

typedef struct _CHARCODE {
    USHORT  cCharCodes;
    LPVOID  pCharCodes;
    USHORT  uiRepeat;
} CHARCODE, * PCHARCODE;

#ifdef PACKET_KEYSYM_SUPPORT

/* WdKeysymCode structure */

typedef struct _KEYSYMCODE {
    USHORT  cKeysymCodes;
    LPVOID  pKeysymCodes;
    USHORT  uiRepeat;
} KEYSYMCODE, * PKEYSYMCODE;

/*
 *  keysym keyboard packet
 *  structure -- stored in
 *  pKeysymCodes array
 *  in _KEYSYMCODE
 */
#define KEYSYM_DOWN  0x00
#define KEYSYM_UP    0x01

typedef struct _KEYSYMCHAR {
        BYTE  bCharType;          /* KEYSYM_UP/KEYSYM_DOWN */
        BYTE  cKeysymValue[4];      /* keysym value */
} KEYSYMCHAR, * PKEYSYMCHAR;

#endif


#if defined(UNICODESUPPORT) || defined(USE_EUKS)

/* WdUnicodeCode structure */

typedef struct _UNICODECODE {
    USHORT  cUnicodeCodes;
    LPVOID  pUnicodeCodes;
    USHORT  uiRepeat;
} UNICODECODE, * PUNICODECODE;

/*
 *  Unicode keyboard packet
 *  structure -- stored in
 *  pUnicodesCodes array
 *  in _UNICODECODE
 */
#define KEYUP       0x01
#define SPECIALKEY  0x02
#define UCINSERT    0x04

typedef struct _UNICODECHAR {
        BYTE  bCharType;          /* KEYUP, SPECIALKEY & KEYUP, etc. */
        BYTE  cCharValue[2];      /* Unicode or special key value */
} UNICODECHAR, * PUNICODECHAR;
#endif

#endif /* __KEYTYPES_H__ */
