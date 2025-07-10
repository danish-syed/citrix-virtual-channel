#ifndef vdio_h_included
#define vdio_h_included

#include <vd.h>
#include <wdapi.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef LPVOID VDIOCONTEXT, * PVDIOCONTEXT;


/**
 * Retrieve channel number for virtual channel with the given name
 * 
 * @param pVd A handle to the virtual channel driver
 * @param pName The name of the virtual channel to open
 * @param pChannelNumber A pointer to receive the allocated channel number
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdOpen(PVD pVd, PCHAR pName, PUSHORT pChannelNumber); // VdOpenVc

/**
 * Release resources acquired by opening the virtual channel.
 * 
 * @param pVd A handle to the virtual channel driver
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdClose(PVD pVd);

/**
 * Registers an IO callback which is used to receive data for the virtual channel driver
 * 
 * @param pVd A handle to the virtual channel driver
 * @param usChannelNumber The number which identifies the virtual channel from which to receive data
 * @param pProc The callback function to be invoked when data is received from the virtual channel
 * @param pIoContext A pointer to receive an I/O context through which data can be written to the virtual channel
 * @param pusMaxWriteSize A pointer to receive the maximum packet size which can be written to the virtual channel
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdRegisterIoCallback(PVD pVd, USHORT usChannelNumber, PVDWRITEPROCEDURE pProc, PVDIOCONTEXT pIoContext, PUSHORT pusMaxWriteSize);

/**
 * Unregisters an IO callback and releases the associated IO context.
 * 
 * @param pIoContext The I/O context to release
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdUnregisterIoCallback(VDIOCONTEXT pIoContext);

/**
 * Write data to the virtual channel.
 * 
 * If the client supports sending data immediately, then the data is immediately sent. Otherwise, the data
 * will be queued until VdServiceIo is called.
 * 
 * @param pIoContext The I/O context for the virtual channel to write data to
 * @param pBuffers An array of buffers containing the data to write
 * @param count The number of buffers to write
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdWriteData(VDIOCONTEXT pIoContext, MEMORY_SECTION* pBuffers, UINT32 count);

/**
 * Sends any pending outgoing data that has been queued for the virtual channel.
 * 
 * This method should be called by the virtual driver Poll method.
 * 
 * @param pIoContext The I/O context for the virtual channel to send data for
 * 
 * @return CLIENT_STATUS_SUCCESS if successful
 */
INT WFCAPI VdServiceIo(VDIOCONTEXT pIoContext);


#ifdef __cplusplus
}
#endif

#endif