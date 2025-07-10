#ifndef vdgfxprov_h_included
#define vdgfxprov_h_included

#include <citrix.h>
#include <vdapi.h>
#include <vd.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

#ifndef GFXL_PACKED_STRUCT
#ifdef _WIN32
#define GFXL_PACKED_STRUCT
#else
#define GFXL_PACKED_STRUCT __attribute__((packed))
#endif
#endif

#ifdef _WIN32
#ifdef ALLOW_GFXL_DEPRECATED
#define GFXL_DEPRECATED
#else
#define GFXL_DEPRECATED __declspec(deprecated)
#endif // !ALLOW_GFXL_DEPRECATED
#endif // _WIN32

typedef LPVOID GfxListener, * PGfxListener;
typedef LPVOID GfxSurface, * PGfxSurface;

typedef enum _GfxEvent {
    GfxEvt_Connected, // data type is bool
    GfxEvt_Disconnected, // data type is NULL
    GfxEvt_FrameUpdate, // data type is GfxSurface
    GfxEvt_Subscribed, // data type is GfxSurface
    GfxEvt_Unsubscribed // data type is GfxSurface
} GfxEvent;

typedef enum _GfxSurfaceType {
    GfxSurf_Application,
    GfxSurf_Monitor
} GfxSurfaceType;

typedef enum _GfxPixFormat {
    PixFmt_BGRA,
    PixFmt_RGBA
} GfxPixFormat;

typedef struct _GfxImage {
    UINT stride; // stride image width in bytes
    UINT width; // image width
    UINT height; // image height
    UINT size; // image data size in bytes
    PBYTE data; // pointer to image data that can be serialized bytes or shared memory bytes
    GfxPixFormat pixformat;
} GfxImage GFXL_PACKED_STRUCT;

typedef struct _GfxSurfaceDescriptor { // GfxSubscription
    UINT32 version; // structure version. should be zero.
    UINT32 id; // app bufferid or monitor id
    RECT cropping_rect;
    GfxSurfaceType surface_type; // application or monitor
    GfxSurface surface;
} GfxSurfaceDescriptor GFXL_PACKED_STRUCT;


/* callback type for viewport change notifications */
typedef VOID(WFCAPI* PFN_GFXEVENTCALLBACK) (GfxListener, GfxEvent, PVOID, PVOID);

/**
 * Obtain a graphics listener for a given virtual channel.
 *
 * @param vcName The name of the virtual channel which will receive the graphics data.
 * @param pfnCallback A callback function to be invoked whenever there is graphics data.
 * @param pCallbackData A user-provided context which will be passed to the callback when invoked.
 * @param pListener A pointer to receive the newly created listener.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT GFXL_DEPRECATED WFCAPI VdAcquireGraphicsListener(const char* vcName, PFN_GFXEVENTCALLBACK pfnCallback, PVOID pCallbackData, PGfxListener pListener);

/**
 * Release a graphics listener and free any resources used by it.
 *
 * @param listener The listener to release.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT GFXL_DEPRECATED WFCAPI VdReleaseGraphicsListener(GfxListener listener);

/**
 * Activates an existing graphics listener.
 * 
 * A graphics listener is bound to a virtual channel. This is accomplished by providing the name of the
 * custom virtual channel as an argument when the listener is created. See VdAquireGraphicsListener. The
 * graphics subsystem sends an event to the virtual channel for which the listener was registered. This
 * event contains information needed to complete the initialization of the graphics listener.
 *
 * @param pListener The graphics listener to activate.
 * @param pEvt The event containing initialization information for the graphics listener.
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT GFXL_DEPRECATED WFCAPI VdAuthorizeGraphicsListener(GfxListener listener, GBufferConnectionEvt* pEvt);

/**
 * Lock a frame buffer in order to access image data.
 *
 * @param pListener A handle to the graphics listener from which this buffer originates
 * @param pSurface A pointer to the surface which contains the image data we want to access
 * @param pImage A pointer to receive the image data
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT GFXL_DEPRECATED WFCAPI VdLockFrameBuffer(GfxListener listener, UINT32 bufferid, GfxImage* pImage);

/**
 * Unlocks a buffer that was previously locked.
 *
 * @param pListener A handle to the graphics listener
 * @param pSurface A pointer to the surface which contains the image data we no longer need to access
 *
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT GFXL_DEPRECATED WFCAPI VdUnlockFrameBuffer(GfxListener listener, UINT32 bufferid);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // vdgfxprov_h_included