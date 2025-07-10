#ifndef vdmonitors_h_included
#define vdmonitors_h_included

#include <citrix.h>
#include <vd.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

#ifndef MON_PACKED_STRUCT
#ifdef _WIN32
#define MON_PACKED_STRUCT
#else
#define MON_PACKED_STRUCT __attribute__((packed))
#endif
#endif

// Monitor layout structures
typedef struct _VIRTUAL_MONITOR {
	INT Id;
	VPRECT Rect;
	USHORT DevicePpi;
	USHORT ScaleFactor;
} VIRTUAL_MONITOR MON_PACKED_STRUCT;

typedef struct _VIRTUAL_MONITOR_LAYOUT {
	UINT NumMonitors;
	UINT PrimaryMonitor;
	VIRTUAL_MONITOR* Monitors;
} VIRTUAL_MONITOR_LAYOUT MON_PACKED_STRUCT;


/**
 * Retrieve the monitor layout for the current session.
 *
 * @param pVd A handle to the virtual channel driver.
 * @param ppInfo A pointer to receive the monitor layout data.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdGetMonitorInfo(PVD pVd, VIRTUAL_MONITOR_LAYOUT**ppInfo);

/**
 * Release the memory allocated for the monitor layout data.
 *
 * @param pInfo The monitor layout data to free.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdReleaseMonitorInfo(VIRTUAL_MONITOR_LAYOUT* pInfo);


#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // vdmonitors_h_included