#pragma once

#include "vc_errors.hpp"
#include "vc_exception.hpp"

#include <vdio.h>
#include <vdexports.h>


namespace vcsdk {

/// a C++ RAII wrapper for the VC SDK I/O context structure.
class vc_io_controller {
private:

    VDIOCONTEXT _IoContext; /// I/O context

public:

    /// constructor.
    vc_io_controller(VDIOCONTEXT pIoContext, USHORT usMaxDataSize)
        : _IoContext(pIoContext) {
    }

    /// destructor.
    ~vc_io_controller() {

        // unregister io callback
        (void)VdUnregisterIoCallback(_IoContext);
    }

    /// write some data for the virtual channel.
    void write(MEMORY_SECTION buf) {
        VdWriteData(_IoContext, &buf, 1);
    }

    /// flush any pending outgoing data.
    bool poll() {
        INT rc = VdServiceIo(_IoContext);
        switch (rc) {
        case CLIENT_STATUS_SUCCESS:
            return true;

        case CLIENT_STATUS_NO_DATA:
            return false;

        default:
            throw vc_exception(make_vc_error_code(rc));
        }
    }

    static vc_io_controller create(USHORT usChannelNumber, PVD _vd) {

        // register write hooks for our virtual channel
        USHORT usMaxDataSize = 0;
        VDIOCONTEXT pIoContext = NULL;
        if (auto ec = make_vc_error_code(VdRegisterIoCallback(_vd, usChannelNumber, (PVDWRITEPROCEDURE)&ICADataArrival, &pIoContext, &usMaxDataSize)); ec) {
            throw vc_exception(ec);
        }

        return vc_io_controller(pIoContext, usMaxDataSize);
    }
};

} // namespace vcsdk