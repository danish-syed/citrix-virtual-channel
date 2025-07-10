#ifndef vdgfxsink_h_included
#define vdgfxsink_h_included

#include <vdgfxlistener.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef LPVOID GfxSink, * PGfxSink;


/* callback type for viewport change notifications */
typedef VOID(WFCAPI* PFN_GFXEVENTCALLBACK2) (GfxSink, GfxEvent, PVOID, PVOID);

/**
 * Obtain a graphics sink for a given virtual channel.
 *
 * @param vcName The name of the virtual channel which will receive the graphics data.
 * @param pfnCallback A callback function to be invoked whenever there is graphics data.
 * @param pCallbackData A user-provided context which will be passed to the callback when invoked.
 * @param pSink A pointer to receive the newly created sink.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdAcquireGraphicsSink(PVD pVd, const char* pVcName, PFN_GFXEVENTCALLBACK2 pfnCallback, PVOID pCallbackData, PGfxSink pSink);

/**
 * Release a graphics sink and free any resources used by it.
 *
 * @param pSink The sink to release.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdReleaseGraphicsSink(GfxSink sink);

/**
 * Activates an existing graphics sink.
 * 
 * A graphics sink is bound to a virtual channel. This is accomplished by providing the name of the
 * custom virtual channel as an argument when the sink is created. See VdAquireGraphicsSink. The
 * graphics subsystem sends an event to the virtual channel for which the sink was registered. This
 * event contains information needed to complete the initialization of the graphics sink.
 *
 * @param pSink The graphics sink to activate.
 * @param pEvt The event containing initialization information for the graphics sink.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdAuthorizeGraphicsSink(GfxSink sink, GBufferConnectionEvt* pEvt);

/**
 * Capture images from an application or monitor surface.
 * 
 * @param sink A handle to the graphics sink.
 * @param surface The surface descriptor which specifies an app or monitor from which to capture images.
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdCaptureSurface(GfxSink sink, GfxSurfaceDescriptor* pSurfaceDescriptor);

/**
 * Stop capturing images from an application or monitor surface.
 * 
 * @param sink A handle to the graphics sink.
 * @param surface The surface from which to stop capturing images.
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdReleaseSurface(GfxSink sink, GfxSurface surface);

/**
 * Lock a frame buffer in order to access image data.
 *
 * @param pSink A handle to the graphics sink from which this buffer originates
 * @param pSurface A pointer to the surface which contains the image data we want to access
 * @param pImage A pointer to receive the image data
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdLockSurface(GfxSink sink, GfxSurface pSurface, GfxImage* pImage);

/**
 * Unlocks a buffer that was previously locked.
 *
 * @param pSink A handle to the graphics sink
 * @param pSurface A pointer to the surface which contains the image data we no longer need to access
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdUnlockSurface(GfxSink sink, GfxSurface pSurface);

#ifdef __cplusplus
}
#endif

#endif // vdgfxsink_h_included