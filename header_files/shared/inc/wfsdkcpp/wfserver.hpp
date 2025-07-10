#pragma once

#include <wfapi.h>   // Citrix Server SDK

#include <string>
#include <memory>


/// A Citrix server instance
class wfserver {
private:

    HANDLE _hndl{}; /// a handle to the server instance

    struct release {
        using pointer = wfserver;
        void operator ()(pointer server) const { WFCloseServer(server); }
    };

public:

    using unique_resource = std::unique_ptr<wfserver, release>;

    /// retrieve the current server instance.
    static wfserver current_server() {
        return wfserver(WF_CURRENT_SERVER_HANDLE);
    }

    static unique_resource open(const std::wstring& name) {
        return unique_resource{ wfserver(WFOpenServerW((LPWSTR)name.c_str())) };
    }

    /// constructor. default constructor.
    wfserver() = default;

    /// constructor. construct with underlying server handle.
    wfserver(HANDLE hndl) : _hndl(hndl) {}

    /// constructor. copy constructor.
    wfserver(const wfserver&) = default;

    /// copy assignment operator.
    wfserver& operator =(const wfserver&) = default;

    /// constructor. move constructor.
    wfserver(wfserver&&) noexcept = default;

    /// move assignment operator.
    wfserver& operator =(wfserver&&) noexcept = default;

    /// destructor.
    ~wfserver() = default;

    /// handle conversion operator for convenience.
    operator HANDLE() { return _hndl; }

    /// boolean conversion operator.
    explicit operator bool() const { return _hndl != NULL; }

    /// shut the server down
    bool shutdown() { return (WFShutdownSystem(_hndl, WF_WSD_POWEROFF) == TRUE); }
};