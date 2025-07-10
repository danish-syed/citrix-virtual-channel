/**********************************************************************
*
* vdoverlay.h
*
* shared header for ctxoverlay/vdoverlay example program
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

#ifndef __VDOVERLAY_H__
#define __VDOVERLAY_H__

#include <ica.h>
#include <ica-c2h.h>        /* for VD_C2H structure */

#define CTXOVERLAY_VIRTUAL_CHANNEL_NAME  "CTXOVLY"
#define CTXAPPSHARE_VIRTUAL_CHANNEL_NAME  "CTXAPSH"


/*
 * Lowest and highest compatable version.  See DriverInfo().
 */
#define CTXOVERLAY_VER_LO      1
#define CTXOVERLAY_VER_HI      1

#pragma pack(1)

 /* ping packet structure */
typedef enum _command_ids {
	create_window = 0,
	destroy_window = 1,
	reposition_window = 2,
	resize_window = 3,
	reparent_window = 4,
	update_clipping = 5 // clip_window
} command_ids;

typedef struct _position {
	unsigned int x;
	unsigned int y;
} position;

typedef struct _extents {
	unsigned int dx;
	unsigned int dy;
} extents;

typedef struct _rectangle {
	position tl;
	position br;
} rectangle;


typedef unsigned long long window_handle;


typedef struct _window_message {
	command_ids command; /// the command to perform
	unsigned int id; /// the window id
	position pos;
	extents size;
	window_handle parent;
	unsigned int num_clip_rects;
} window_message;



typedef enum _appsh_command_ids {
	start_app_sharing = 1,
	stop_app_sharing = 2,
	app_sharing_status = 3,
	subscribe_app = 4,
	unsubscribe_app = 5,
	subscribe_screen = 6,
	unsubscribe_screen = 7,
	update_screens = 8
} appsh_command_ids;

typedef struct _appshare_message {
	appsh_command_ids command;
	unsigned int app_id;
	unsigned int status;
	unsigned int num_screens;
	unsigned int overlay_id;
} appshare_message;

typedef enum _appshare_status {
	app_sharing_started = 0,
	app_sharing_stopped = 1,
	app_sharing_error = 2
} appshare_status;



typedef struct _VDCWAVERSION {
	unsigned int major;
	unsigned int minor;
	unsigned int revision;
	unsigned int build;
} VDCWAVERSION, *PVDCWAVERSION;


typedef struct _VDOVERLAYINFO {
	unsigned int id;
	position pos;
	extents size;
} VDOVERLAYINFO, *PVDOVERLAYINFO;


/* vdping driver info (client to host) structure */
typedef struct _VDOVERLAY_C2H
{
	VD_C2H  Header;
	VDOVERLAYINFO Overlay;
	VDCWAVERSION CWAVersion;
	USHORT  usMaxDataSize;      /* maximum data block size */
} VDOVERLAY_C2H, * PVDOVERLAY_C2H;

#pragma pack()

#endif /* __VDPING_H__ */
