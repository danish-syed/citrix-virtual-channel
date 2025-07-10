#pragma once

#include <vdapi.h>


namespace vcsdk {

/// virtual channel interface.
class virtual_channel {
public:

    /// destructor. to be overridden by derived classes.
    virtual ~virtual_channel() {};

    /// return the channel number assigned/allocated for this virtual channel.
    virtual USHORT channelNumber() const PURE;

    /// writes some shared data for the virtual channel controller.
    virtual void writeSharedData(LPBYTE lpData, USHORT& usLen) PURE;

    /// flush any pending outgoing data for this virtual channel.
    virtual bool poll() PURE;

    /// process incoming data for this virtual channel.
    virtual INT ondata(LPBYTE pBuf, USHORT Length) PURE;

    /// handle display changes for this virtual channel.
    virtual void onviewportchanged() PURE;

    /// authorize a graphics listener, if this virtual channel has one.
    virtual bool authorize_graphics_listener(GBufferConnectionEvt* pEvt) PURE;
};

} // namespace vcsdk