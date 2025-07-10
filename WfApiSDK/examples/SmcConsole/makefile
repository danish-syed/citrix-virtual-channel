# Copyright (c) Citrix Systems, Inc. All Rights Reserved.

TARGET = smcconsole

TARGETTYPE = program

UMTYPE = windows

BUILD_FOR_PORTICA = y

USE_CRT = y
USE_WINDOWS_SDK = y
USE_STATIC_LIBS = n
USE_MFC = n
USE_WINDOWS_LEAN_AND_MEAN = n
WARNING_LEVEL = 3

SOURCES = \
	CtxSmcDlg.rc \
	CDisplay.cpp \
	CMonitor.cpp \
	COptions.cpp \
	CSelect.cpp \
	CSession.cpp \
	CtxSmcDlg.cpp \
	CtxSmcUi.cpp \
	CVChannel.cpp \
	CWinStation.cpp \
	CdataLogger.cpp \


LOCAL_INCLUDE_PATHS = \
	-I..\..\..\ \
	-I..\..\..\..\ \
	-I..\..\..\inc \
	-I..\..\..\..\inc \
	-I$(SDK_ROOT)\CoreFramework\inc \

LOCAL_LINK_LIBS = \
	$(WINDOWS_SDK_LIB_PATH)\comctl32.lib \
	$(DYNAMIC_LIB_PATH)\wfapi$(BITEXTN).lib \
!if ("$(PLATFORM)" == "win32")
	$(SDK_ROOT)\CoreFramework\TS\bin\Release\Win32\CtxWsApi.lib \
!endif
!if ("$(PLATFORM)" == "xp32")
	$(SDK_ROOT)\CoreFramework\WS\bin\Release\Win32\CtxWsApi.lib \
!endif
!if ("$(PLATFORM)" == "win64")
	$(SDK_ROOT)\CoreFramework\TS\bin\Release\x64\ctxwsapi64.lib \
!endif
!if ("$(PLATFORM)" == "xp64")
	$(SDK_ROOT)\CoreFramework\WS\bin\Release\x64\ctxwsapi64.lib \
!endif

# Disable warning: bit field types other than int
# Disable warning: unreferenced local parameter
LOCAL_CXXFLAGS =  /D "_AFXDLL" -wd4214 -wd4100 -wd4714 /EHsc /c -D_MSC_EXTENSIONS /DWIN7TS


!include $(BUILD_ROOT)\build\build.mak
