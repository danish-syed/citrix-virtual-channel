#ifndef viewport_h_included
#define viewport_h_included

#include <vd.h> // for definition of PVD


#ifdef __cplusplus
extern "C" {
#endif

/* callback type for viewport change notifications */
typedef VOID(WFCAPI* PFN_VIEWPORTCHANGED) (PVD);


/* dimensional properties */
typedef struct _VDVIEWPORTDIMS {
	UINT32 x;
	UINT32 y;
} VDVIEWPORTDIMS, *PVDVIEWPORTDIMS;


/**
 * A viewport represents a local window in which a remote desktop session is displayed. This
 * structure contains various information about a viewport, such as the viewport mode or type,
 * the window handle, dimensions, etc.
 * 
 * There are two basic types of viewports: Windowed mode and seamless apps. In a windowed session,
 * all application windows (and their contents) within the session are composited and rendered in
 * a single window on the local endpoint device. Seamless applications get their own window/viewport
 * on the local endpoint device.
 * 
 * There are three main display modes for a remote desktop session:
 * - Resolution matching. The display resolution in the session is made to match the display resolution
                          on the local endpoint device. Scaling and panning are not necessary.
 * - Scale to fit.        The graphics for the remote desktop session are scaled to account for the
 *                        different display resolutions. Any overlays need to also be scaled.
 * - Actual size.         The graphics for the remote desktop session are rendered in the remote desktop's
 *                        display resolution. Since this may not fit in the local endpoint viewport, scroll
 *                        bars are activated and panning/scrolling offsets are maintained. Any overlays need
 *                        to be repositioned according to these panning offsets.
 */
typedef struct _VDVIEWPORTINFO {
	UINT32 flags; /* indicates the viewport type or mode (windowed or seamless app) and display modes (scaling/panning) */
	HWND hwnd; /* the window handle of the viewport */
	VDVIEWPORTDIMS size; /* the dimensions of the viewport */
	VDVIEWPORTDIMS sessionSize; /* the dimensions of the session displayed in the viewport */
	VDVIEWPORTDIMS panningOffset; /* the scroll or panning offset of the session */
} VDVIEWPORTINFO, *PVDVIEWPORTINFO;

#define VDVI_SEAMLESS 1
#define VDVI_ZOOM 2
#define VDVI_PANNING 4


#define VdViewportIsSeamless(info) \
	(((info).flags & VDVI_SEAMLESS) == VDVI_SEAMLESS)

#define VdViewportIsWindowed(info) \
	!VdViewportIsSeamless(info)

#define VdViewportZoomEnabled(info) \
	(((info).flags & VDVI_ZOOM) == VDVI_ZOOM)

#define VdViewportPanningEnabled(info) \
	(((info).flags & VDVI_PANNING) == VDVI_PANNING)


#define VDTF_WINDOW_RECT 0 /* window rect in screen coordinates */
#define VDTF_CLIENT_AREA_RECT 1 /* rectangle relative to the upper left corner of the drawing area */


/**
 * Retrieve basic information about the local viewport window.
 *
 * See VDVIEWPORTINFO to get an idea of what viewport information is provided by this function.
 * 
 * @param pVd A handle to the virtual channel driver
 * @param pInfo A pointer to receive the viewport information
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdGetViewportInformation(PVD pVd, PVDVIEWPORTINFO pInfo);

/**
 * Retrieve the session scale factors from the viewport information
 *
 * @param pInfo A pointer to the viewport information
 * @param pScaleX A pointer which receives the x scale factor
 * @param pScaleY A pointer which receives the y scale factor
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdGetViewportScaleFactors(PVDVIEWPORTINFO pInfo, DOUBLE* pScaleX, DOUBLE* pScaleY);

/**
 * Register for viewport change notifications.
 *
 * The callback will be invoked whenever viewport parameters change, such as the viewport mode, the viewport
 * window handle, the session scaling parameters, or the session panning offsets. The caller can retrieve the
 * updated viewport parameters by calling VdGetViewportInformation in the callback function. Typically called
 * from DriverOpen.
 * 
 * @param pVd A handle to the virtual channel driver
 * @param pfnCallback The callback to be invoked when the viewport changes
 * @param puiHandle Receives an identifier that can be used to unregister the callback
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdRegisterViewportChangedCallback(PVD pVd, PFN_VIEWPORTCHANGED pfnCallback, PUINT32 puiHandle);

/**
 * Unregister for viewport change notifications.
 * 
 * This function unregisters a previously registered callback for viewport change notifications. Typically
 * called from DriverClose
 * 
 * @param pVd A handle to the virtual channel driver
 * @param Handle The handle returned by VdRegisterViewportChangeNotifications
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdUnregisterViewportChangedCallback(PVD pVd, UINT32 Handle);

/**
 * Find the local window handle that maps to the given remote/virtual window handle.
 * 
 * Most of the time this function is used to set the correct parent window for overlays created on the local
 * endpoint device. Virtual channels that create local overlays will typically send the window handle of the
 * parent window on the remote desktop (within the remote session) to the endpoint device. This window handle
 * then needs to be looked up in a mapping table on the local endpoint to find the corresponding local window
 * handle.
 * 
 * @param pVd A handle to the virtual channel driver
 * @param pVirtualWindow The window handle from the remote desktop session
 * @param pLocalWindow Receives the window handle on the local endpoint device which corresponds to the given
                       remote desktop window
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdTranslateAppWindow(PVD pVd, HND pVirtualWindow, PHND pLocalWindow);

/**
 * Translate a rectangle from the VDA to local coordinates.
 * 
 * Performs a combination of:
 * - DPI scaling for remote desktops which dont natively support DPI (mostly a legacy feature)
 * - Scaling the rectangle according to the viewport session scale factors
 * - Offseting the rectangle according to the viewport session panning offsets
 * 
 * Typically, in that order. There are different types of rectangles which need to be scaled differently. Most
 * of the time this function will be used to translate window rectangles in order to correctly size and position
 * overlay windows on the local endpoint device. In such cases, the rectangle is assumed to be in screen coordinates.
 * However, this function can also be used to translate client area rectangles or clipping rectangles. In such cases,
 * the panning offsets will be ignored.
 * 
 * @param pVd A handle to the virtual channel driver
 * @param pVirtualRect The input rectangle to translate
 * @param pLocalRect Receives the translated rectangle
 * @param flags Indicates the type of rectangle being translated
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdTranslateVirtualRect(PVD pVd, RECT pVirtualRect, LPRECT pLocalRect, UINT32 type);

/**
 * Register a virtual channel to receive seamless window events. The events and
 * associated data are defined in VDINFOCLASS in vdapi.h. Examples are
 * VdSeamlessWindowLifecycle and VdSeamlessWindowState.
 * A successful registration is good for the duration of the ICA session.
 *
 * Calling this function before all virtual drivers have been loaded will fail.
 * Therefore don't call this function from DriverOpen.
 *
 * @param pVd A handle to the virtual channel driver.
 * @param vcNumber The virtual channel number to register.
 * @return CLIENT_STATUS_SUCCESS if successful, or error code. In particular,
 *         CLIENT_ERROR_DLL_PROCEDURE_NOT_FOUND may indicate that the VCSDK
 *         version is too old to support this operation.
 */
INT WFCAPI VdRegisterForSeamlessWindowEvents(PVD pVd, USHORT vcNumber);

#ifdef __cplusplus
}
#endif

#endif