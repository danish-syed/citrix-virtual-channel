#pragma once

#include "wfbuffer.hpp"
#include "wftaskqueue.hpp"
#include "wfevent.hpp"

#include <string>
#include <vector>
#include <thread>
#include <functional>


// forward declaration
class wfvcreader;


/// A Citrix virtual channel connection
class wfvirtualchannel {
private:

    HANDLE _hndl{}; // a handle to the virtual channel

    struct release {
        using pointer = wfvirtualchannel;
        void operator ()(pointer vc) const { vc.close(); }
    };

public:

    using DataFn = std::function<void(wfbuffer&)>;

    /// a managed virtual channel resource. closes the virtual channel when the managed resource object goes away.
    using unique_resource = std::unique_ptr<wfvirtualchannel, release>;

    /// virtual channel reader type.
    using reader = std::unique_ptr<wfvcreader>;

    /// constructor. default constructor.
    wfvirtualchannel() = default;

    /// constructor. construct with underlying handle.
    wfvirtualchannel(HANDLE hndl)
        : _hndl(hndl) {
    }

    /// constructor. copy constructor.
    wfvirtualchannel(const wfvirtualchannel& other) = default;

    /// copy assignment operator.
    wfvirtualchannel& operator =(const wfvirtualchannel& other) = default;

    /// constructor. move constructor.
    wfvirtualchannel(wfvirtualchannel&& other) noexcept = default;

    /// move assignment operator.
    wfvirtualchannel& operator =(wfvirtualchannel&& other) noexcept = default;

    /// operator bool
    explicit operator bool() const { return _hndl != NULL; }

    /// gracefully close the virtual channel.
    void close() {
        if (_hndl) {
            WFVirtualChannelClose(_hndl);
            _hndl = NULL;
        }
    }

    /// retrieve client data
    template <typename T>
    bool get_client_data(T& t) {
        if (wfbuffer buf; WFVirtualChannelQuery(_hndl, WFVirtualClientData, &buf, &buf.mutable_size())) {
            memcpy(&t, buf, buf.size());
            return true;
        }

        return false;
    }

    /// write some data to the virtual channel from C-style buffer pointer and length.
    bool write(const BYTE* pData, ULONG len, ULONG& written) {
        return WFVirtualChannelWrite(_hndl, (PCHAR)pData, len, &written) ? true : false;
    }

    /// write data to the virtual channel from a vector of bytes.
    bool write(std::vector<BYTE>& buffer, ULONG& written) {
        return write(buffer.data(), static_cast<ULONG>(buffer.size()), written);
    }

    /// write some structure to the virtual channel.
    template <typename T>
    bool write(const T& data, ULONG& written) {
        return write((BYTE *)&data, sizeof(T), written);
    }

    /// read some data from the virtual channel.
    bool read(HANDLE stopEvent, LPBYTE pData, ULONG len, ULONG& read) {
        return WFVirtualChannelReadEx(_hndl, stopEvent, (PCHAR)pData, len, &read) ? true : false;
    }

    /// kick off an asynchronous thread for reading data from the virtual channel. delete the reader object to stop reading data.
    reader start_receiving(wftaskqueue* task_queue, DataFn&& ondata) {
        return std::make_unique<wfvcreader>(*this, task_queue, std::move(ondata));
    }
};