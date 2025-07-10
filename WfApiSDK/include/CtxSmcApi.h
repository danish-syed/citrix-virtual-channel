/* Copyright 2001-2010 Citrix Systems, Inc.  */

/************************************************************************
 * ctxsmcapi.h
 *
 * Header file for SMC API DLL.
 ***********************************************************************/

#ifndef CTXSMCAPI_H
#define CTXSMCAPI_H

 /************************************************************************
  * Defines the dll export of the functions in the code
  * Set it to CTX_DLL_EXPORT if you do not use a .DEF file
  * Else, set it to nothing
  ***********************************************************************/
#define CTX_DLL_EXPORT 


  /************************************************************************
   * Current version number of the API
   ***********************************************************************/
#define CTXSMCAPI_VERSION_NUMBER         1
#define CTXSMCAPI_VERSION_NUMBER_V2      2
#define CTXSMCAPI_VERSION_NUMBER_V3      3


   /************************************************************************
    * Compatibility flags. These are used by callers of the SMC API to
    * determine the supported functionality level, each of the APIs exposed
    * by SMC depends on the compatibility level, which is usually
    * determined by a licensing requirement.
    *
    * Add any new compatibility flags here, these flags are used to form a
    * 32 bit mask
    ***********************************************************************/
    // Functions compatible with XP (when XPe with FR1 license not installed)
#define SMC_API_COMPATIBILITY_XP         0x00000001

// Functions compatible with XPe with FR1 installed
#define SMC_API_COMPATIBILITY_XPE_FR1    0x00000002


/************************************************************************
 * Quality Of Service (QOS) values
 ***********************************************************************/
#define CTX_SESSION_QOS_ENTRIES          4
#define CTX_SESSION_QOS_INVALID          (-1)


 /************************************************************************
  * Virtual Channel Priority values
  ***********************************************************************/
#define CTX_SESSION_VC_PRIORITY_VERYHIGH 0
#define CTX_SESSION_VC_PRIORITY_HIGH     1
#define CTX_SESSION_VC_PRIORITY_NORMAL   2
#define CTX_SESSION_VC_PRIORITY_LOW      3


  /************************************************************************
   * Data Structures
   ***********************************************************************/

   /////////////////////////////////////////////////////////////////////////
   // Ctx_Smc_Server_Data
typedef struct
{
    UINT32      VersionNumber;

    UINT64      TotalBytesSentPreCompression;
    UINT64      TotalBytesSentPostCompression;

    UINT64      TotalBytesReceivedPreExpansion;
    UINT64      TotalBytesReceivedPostExpansion;

    UINT32      BandwidthSentBitsPerSec;
    UINT32      BandwidthReceivedBitsPerSec;

    UINT32      CompressionNumeratorSent;
    UINT32      CompressionDenominatorSent;

    UINT32      CompressionNumeratorReceived;
    UINT32      CompressionDenominatorReceived;
} Ctx_Smc_Server_Data, *pCtx_Smc_Server_Data;

/////////////////////////////////////////////////////////////////////////
// Ctx_Smc_Server_VC_Data
typedef struct
{
    UINT32      VersionNumber;

    UINT32      InUseCount;

    UINT32      BandwidthSentBitsPerSecPostCompressionEstimate;
    UINT64      BytesSent;

    UINT32      BandwidthReceivedBitsPerSecPreExpansionEstimate;
    UINT64      BytesReceived;

    WCHAR       rgwchName[8];

} Ctx_Smc_Server_VC_Data, *pCtx_Smc_Server_VC_Data;

/////////////////////////////////////////////////////////////////////////
// Ctx_Smc_Session_VC_Name
typedef struct
{
    UINT32      VersionNumber;

    UINT32      idVC;

    WCHAR       rgwchName[8];

} Ctx_Smc_Session_VC_Name, *pCtx_Smc_Session_VC_Name;

/////////////////////////////////////////////////////////////////////////
// Ctx_Smc_Session_Data
typedef struct
{
    UINT32      VersionNumber;

    // session totals
    UINT32      BandwidthSentBitsPerSecond;
    UINT32      BytesSentPreCompression;
    UINT32      BytesSentPostCompression;

    UINT32      BandwidthReceivedBitsPerSecond;
    UINT32      BytesReceivedPreExpansion;
    UINT32      BytesReceivedPostExpansion;

    // compression counters
    UINT32      CompressionNumeratorSent;
    UINT32      CompressionDenominatorSent;
    UINT32      CompressionNumeratorReceived;
    UINT32      CompressionDenominatorReceived;

    // latency data
    UINT32      LastClientLatency;
    UINT32      AverageClientLatency;
    UINT32      RoundTripDeviation;
    UINT32      LastUpdateTime;

    // Bandwidth cap
    UINT32      SessionBandwidthLimitKBitsPerSecond;

    // Line speed
    UINT32      OutputSpeedInBitsPerSecond;
    UINT32      InputSpeedInBitsPerSecond;

} Ctx_Smc_Session_Data, *pCtx_Smc_Session_Data;

/////////////////////////////////////////////////////////////////////////
// Ctx_Smc_Session_VC_Data
typedef struct
{
    UINT32      VersionNumber;

    UINT32      idVC;

    UINT32      PreReducerBitsPerSecond;
    UINT32      BandwidthSentPostCompressionEstimate;
    UINT32      BytesSentTotal;

    UINT32      PostExpanderBitsPerSecond;
    UINT32      BandwidthReceivedPreExpansionEstimate;
    UINT32      BytesReceivedTotal;
} Ctx_Smc_Session_VC_Data, *pCtx_Smc_Session_VC_Data;


/************************************************************************
 * Version 2 Data Structures
 ***********************************************************************/

 /////////////////////////////////////////////////////////////////////////
 // Ctx_Smc_Session_Data_V2
typedef struct
{
    UINT32      VersionNumber;

    // QoS level
    UINT32      QosLevel;

    // session totals
    UINT32      BandwidthSentBitsPerSecond;
    UINT32      BytesSentPreCompression;
    UINT32      BytesSentPostCompression;

    UINT32      BandwidthReceivedBitsPerSecond;
    UINT32      BytesReceivedPreExpansion;
    UINT32      BytesReceivedPostExpansion;

    // compression counters
    UINT32      CompressionNumeratorSent;
    UINT32      CompressionDenominatorSent;
    UINT32      CompressionNumeratorReceived;
    UINT32      CompressionDenominatorReceived;

    // latency data
    UINT32      LastClientLatency;
    UINT32      AverageClientLatency;
    UINT32      RoundTripDeviation;
    UINT32      LastUpdateTime;

    // Bandwidth cap
    UINT32      SessionBandwidthLimitKBitsPerSecond;

    // Line speed
    UINT32      OutputSpeedInBitsPerSecond;
    UINT32      InputSpeedInBitsPerSecond;
    UINT32      IsConnectionActive;
    UINT32		IsPrimaryStream;

} Ctx_Smc_Session_Data_V2, *pCtx_Smc_Session_Data_V2;

/////////////////////////////////////////////////////////////////////////
// Ctx_Smc_Session_VC_Data_V2
typedef struct
{
    UINT32      VersionNumber;

    UINT32      idVC;
    WCHAR       rgwchName[8];

    UINT32      QosLevel;

    UINT32      PreReducerBitsPerSecond;
    UINT32      BandwidthSentPostCompressionEstimate;
    UINT32      BytesSentTotal;

    UINT32      PostExpanderBitsPerSecond;
    UINT32      BandwidthReceivedPreExpansionEstimate;
    UINT32      BytesReceivedTotal;

    UINT32      BandwidthLimitKBitsPerSecond;
} Ctx_Smc_Session_VC_Data_V2, *pCtx_Smc_Session_VC_Data_V2;

typedef struct // values mirroring UDT_TRACEINFO strucutre
{
    // global measurements
    INT64 msTimeStamp;                 // time since the UDT entity is started, in milliseconds
    INT64 pktSentTotal;                // total number of sent data packets, including retransmissions
    INT64 pktRecvTotal;                // total number of received packets
    UINT64 bytesSentTotal;             // total number of sent data bytes, including retransmissions
    UINT64 bytesRecvTotal;             // total number of received bytes
    INT pktSndLossTotal;               // total number of lost packets (sender side)
    INT pktRcvLossTotal;               // total number of lost packets (receiver side)
    INT pktRcvOOOTotal;                // total number of out-of-order packets (receiver side)
    INT pktRetransTotal;               // total number of retransmitted packets
    INT pktSentACKTotal;               // total number of sent ACK packets
    INT pktRecvACKTotal;               // total number of received ACK packets
    INT pktSentNAKTotal;               // total number of sent NAK packets
    INT pktRecvNAKTotal;               // total number of received NAK packets
    UINT64 usSndDurationTotal;         // total time duration when UDT was sending data (idle time exclusive)

    // local measurements
    INT64 pktSent;                     // number of sent data packets, including retransmissions
    INT64 pktRecv;                     // number of received packets
    UINT64 bytesSent;                  // number of sent data bytes, including retransmissions
    UINT64 bytesRecv;                  // number of received bytes
    INT pktSndLoss;                    // number of lost packets (sender side)
    INT pktRcvLoss;                    // number of lost packets (receiver side)
    INT pktRcvOOO;                     // number of out-of-order packets (receiver side)
    INT pktRetrans;                    // number of retransmitted packets
    INT pktSentACK;                    // number of sent ACK packets
    INT pktRecvACK;                    // number of received ACK packets
    INT pktSentNAK;                    // number of sent NAK packets
    INT pktRecvNAK;                    // number of received NAK packets
    UINT64 bpsSendRate;                // sending rate in b/s
    UINT64 bpsRecvRate;                // receiving rate in b/s
    UINT64 usSndDuration;              // current busy sending time (i.e., idle time exclusive)

    // instant measurements
    INT pktFlowWindow;                 // flow window size, in number of packets
    INT pktCongestionWindow;           // congestion window size, in number of packets
    INT pktFlightSize;                 // number of packets on flight
    INT pktDeliveryRate;               // packet arrival rate at the remote side, in pkts per second
    INT pktAvailSndBuf;                // available UDT sender queue size, in number of packets
    INT pktAvailRcvBuf;                // available UDT receiver queue size, in number of packets
    INT pktUsedSndBuf;                 // size of pending data in the UDT sender queue, in number of packets
    INT pktUsedRcvBuf;                 // size of data available to read in the UDT receiver queue, in number of packets
    UINT64 bpsBandwidth;               // estimated bandwidth, in bits per second
    UINT32 usPktSndPeriod;             // packet sending period, in microseconds
    UINT32 usRTT;
} SMC_UDT_PERFDATA, *PSMC_UDT_PERFDATA;

/************************************************************************
 * Version 3 Data Structures
 ***********************************************************************/

 /////////////////////////////////////////////////////////////////////////
 // Ctx_Smc_Session_Data_V3
typedef struct
{
    UINT32      VersionNumber;

    // QoS level
    UINT32      QosLevel;

    // session totals
    UINT32      BandwidthSentBitsPerSecond;
    UINT32      BytesSentPreCompression;
    UINT32      BytesSentPostCompression;

    UINT32      BandwidthReceivedBitsPerSecond;
    UINT32      BytesReceivedPreExpansion;
    UINT32      BytesReceivedPostExpansion;

    // compression counters
    UINT32      CompressionNumeratorSent;
    UINT32      CompressionDenominatorSent;
    UINT32      CompressionNumeratorReceived;
    UINT32      CompressionDenominatorReceived;

    // latency data
    UINT32      LastClientLatency;
    UINT32      AverageClientLatency;
    UINT32      RoundTripDeviation;
    UINT32      LastUpdateTime;

    // Bandwidth cap
    UINT32      SessionBandwidthLimitKBitsPerSecond;

    // Line speed
    UINT32      OutputSpeedInBitsPerSecond;
    UINT32      InputSpeedInBitsPerSecond;
    UINT32      IsConnectionActive;
    UINT32		IsPrimaryStream;

    UINT32      MaxOutBufCount;
    UINT32      BuffersInUse;

    // EDT Performance data
    SMC_UDT_PERFDATA udtPerfStats;
} Ctx_Smc_Session_Data_V3, *pCtx_Smc_Session_Data_V3;
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    /************************************************************************
     * Prototypes of the interface functions
     ***********************************************************************/

     /////////////////////////////////////////////////////////////////////////
     // CtxSmcGetVersionSupported
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetVersionSupported(
            PUINT32                     pVersionSupported,
            PUINT32                     pFeaturesSupported
        );

    typedef INT(WINAPI * PFNCTXSMCGETVERSIONSUPPORTED)
        (UINT32*, UINT32*);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetServerData
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetServerData(
            UINT32                      cbData,
            pCtx_Smc_Server_Data        pData,
            UINT32                      cbVCData,
            pCtx_Smc_Server_VC_Data     pVCData,
            PUINT32                     pcVCs
        );

    typedef INT(WINAPI * PFNCTXSMCGETSERVERDATA)
        (UINT32, pCtx_Smc_Server_Data, UINT32, pCtx_Smc_Server_VC_Data, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcOpenSessionHandle
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcOpenSessionHandle(
            UINT32                      SessionId,
            PHANDLE                     phSession
        );

    typedef INT(WINAPI * PFNCTXSMCOPENSESSIONHANDLE)
        (UINT32, PHANDLE);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcCloseSessionHandle
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcCloseSessionHandle(
            HANDLE                      hSession
        );

    typedef INT(WINAPI * PFNCTXSMCCLOSESESSIONHANDLE)
        (HANDLE);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionData
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionData(
            HANDLE                      hSession,
            UINT32                      cbData,
            pCtx_Smc_Session_Data       pData,
            UINT32                      cbVCData,
            pCtx_Smc_Session_VC_Data    pVCData,
            PUINT32                     pcVC
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONDATA)
        (HANDLE, UINT32, pCtx_Smc_Session_Data, UINT32, pCtx_Smc_Session_VC_Data, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionVirtualChannelNames
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionVirtualChannelNames(
            HANDLE                      hSession,
            UINT32                      cbVCNames,
            pCtx_Smc_Session_VC_Name    pVCNames,
            PUINT32                     pcVCNames
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONVIRTUALCHANNELNAMES)
        (HANDLE, UINT32, pCtx_Smc_Session_VC_Name, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionStartupData
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionStartupData(
            HANDLE                      hSession,
            PUINT32                     pConnectTime
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONSTARTUPDATA)
        (HANDLE, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionLatencyData
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionLatencyData(
            HANDLE                      hSession,
            PUINT32                     pLastLatency,
            PUINT32                     pAverageLatency,
            PUINT32                     pRoundTripDeviation,
            PUINT32                     pTickCountLastUpdateTime);

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONLATENCYDATA)
        (HANDLE, PUINT32, PUINT32, PUINT32, PUINT32);

    //////////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionLatencyData
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionLatencyDataByQos(
            HANDLE   hSession,
            UINT32 * pLastLatency,
            UINT32 * pAverageLatency,
            UINT32 * pRoundTripDeviation,
            UINT32 * pTickCountLastUpdateTime,
            UINT32 qos);

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONLATENCYDATABYQOS)
        (HANDLE, UINT32 *, UINT32 *, UINT32 *, UINT32*, UINT32);

    //////////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionBandwidthLimits
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionBandwidthLimits(
            HANDLE                      hSession,
            UINT32                      cEntries,
            PUINT32                     pVCIDs,
            PUINT32                     pVCLimits
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONBANDWIDTHLIMITS)
        (HANDLE, UINT32, PUINT32, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcSetSessionBandwidthLimits
    INT CTX_DLL_EXPORT WINAPI CtxSmcSetSessionBandwidthLimits(
        HANDLE                      hSession,
        UINT32                      cEntries,
        PUINT32                     pVCIDs,
        PUINT32                     pVCLimits
    );

    typedef INT(WINAPI * PFNCTXSMCSETSESSIONBANDWIDTHLIMITS)
        (HANDLE, UINT32, PUINT32, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionPriorities
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionPriorities(
            HANDLE                      hSession,
            UINT32                      cEntries,
            PUINT32                     pVCIDs,
            PUINT32                     pVCPriorities
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONPRIORITIES)
        (HANDLE, UINT32, PUINT32, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcSetSessionPriorities
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcSetSessionPriorities(
            HANDLE                      hSession,
            UINT32                      cEntries,
            PUINT32                     pVCIDs,
            PUINT32                     pVCPriorities
        );

    typedef INT(WINAPI * PFNCTXSETSESSIONPRIORITIES)
        (HANDLE, UINT32, PUINT32, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionDataV2
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionDataV2(
            HANDLE                      hSession,
            UINT32                      cbData,
            pCtx_Smc_Session_Data_V2    pData,
            UINT32                      cbVCData,
            pCtx_Smc_Session_VC_Data_V2 pVCData,
            PUINT32                     pcVC
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONDATAV2)
        (HANDLE, UINT32, pCtx_Smc_Session_Data_V2, UINT32, pCtx_Smc_Session_VC_Data_V2, PUINT32);

    /////////////////////////////////////////////////////////////////////////
    // CtxSmcGetSessionDataV3
    INT CTX_DLL_EXPORT WINAPI
        CtxSmcGetSessionDataV3(
            HANDLE                      hSession,
            UINT32                      cbData,
            pCtx_Smc_Session_Data_V3    pData,
            UINT32                      cbVCData,
            pCtx_Smc_Session_VC_Data_V2 pVCData,
            PUINT32                     pcVC
        );

    typedef INT(WINAPI * PFNCTXSMCGETSESSIONDATAV3)
        (HANDLE, UINT32, pCtx_Smc_Session_Data_V3, UINT32, pCtx_Smc_Session_VC_Data_V2, PUINT32);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CTXSMCAPI_H
