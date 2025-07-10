#ifndef driver_h_included
#define driver_h_included

#include <vd.h>
#include <vdapi.h>


#ifdef __cplusplus
extern "C" {
#endif

	int DriverOpen(PVD, PVDOPEN, PUINT16);
	int DriverClose(PVD, PDLLCLOSE, PUINT16);
	int DriverInfo(PVD, PDLLINFO, PUINT16);
	int DriverPoll(PVD, PDLLPOLL, PUINT16);
	int DriverQueryInformation(PVD, PVDQUERYINFORMATION, PUINT16);
	int DriverSetInformation(PVD, PVDSETINFORMATION, PUINT16);
	int DriverGetLastError(PVD, PVDLASTERROR);

	void WFCAPI ICADataArrival(PVD, USHORT, LPBYTE, USHORT);
	void WFCAPI ICAViewportChanged(PVD);

#ifdef __cplusplus
}
#endif

#endif