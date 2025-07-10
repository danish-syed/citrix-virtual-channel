#pragma once

#include "wfserver.hpp"
#include "wfvcreader.hpp"
#include "wfclientinfo.hpp"
#include "wfappinfo.hpp"
#include "wfuserinfo.hpp"
#include "wfversioninfo.hpp"

#include <optional>


/// A Citrix session
class wfsession {
private:

    wfserver _server; /// the server instance that the session belongs to
    DWORD _id; /// the underlying session identifier

public:

    /// retrieve the current session.
    static wfsession current_session() {
        return wfsession(wfserver::current_server(), WF_CURRENT_SESSION);
    }

    /// constructor. default constructor.
    wfsession() = default;

    /// constructor. construct with underlying session identifier.
    wfsession(const wfserver& server, DWORD session_id)
        : _server(server)
        , _id(session_id) {
    }

    /// retrieve the session id
    int query_session_id() {
        if (wfbuffer buf; WFQuerySessionInformation(_server, _id, WFSessionId, (LPWSTR*)&buf, &buf.mutable_size())) {
            return *buf.as<int>();
        }

        return 0;
    }

    std::optional<wfclientinfo> query_client_info() {
        if (wfbuffer buf; WFQuerySessionInformation(_server, _id, WFClientInfo, (LPWSTR*)&buf, &buf.mutable_size())) {
            return wfclientinfo(std::move(buf));
        }

        return {};
    }

    std::optional<wfappinfo> query_app_info() {
        if (wfbuffer buf([](void*) { /* temporary work-around while I investigate a bug. */ }); WFQuerySessionInformation(_server, _id, WFAppInfo, (LPWSTR*)&buf, &buf.mutable_size()) && buf.size() > 0) {
            return wfappinfo(std::move(buf));
        }

        return {};
    }

    std::optional<wfuserinfo> query_user_info() {
        if (wfbuffer buf; WFQuerySessionInformation(_server, _id, WFUserInfo, (LPWSTR*)&buf, &buf.mutable_size())) {
            return wfuserinfo(std::move(buf));
        }

        return {};
    }

    std::optional<wfversioninfo> query_version_info() {
        if (wfbuffer buf; WFQuerySessionInformation(_server, _id, WFVersionEx, (LPWSTR*)&buf, &buf.mutable_size()) && buf.size() > 0) {
            return wfversioninfo(std::move(buf));
        }

        return {};
    }

    std::pair<SIZE, DWORD> query_client_display() {
        if (wfbuffer buf; WFQuerySessionInformation(_server, _id, WFClientDisplay, (LPWSTR*)&buf, &buf.mutable_size())) {
            PWF_CLIENT_DISPLAY disp = buf.as<WF_CLIENT_DISPLAY>();
            SIZE sz = {};
            sz.cx = (LONG)disp->HorizontalResolution;
            sz.cy = (LONG)disp->VerticalResolution;

            return std::pair(sz, disp->ColorDepth);
        }

        return {};
    }

    /// open a virtual channel with a given name for the session.
    wfvirtualchannel::unique_resource open_virtual_channel(const std::string& name) {
        return wfvirtualchannel::unique_resource{ wfvirtualchannel(WFVirtualChannelOpen(_server, _id, const_cast<char*>(name.c_str()))) };
    }

    /// returns the active protocol for this session.
    WF_PROTOCOL get_active_protocol() { return WFGetActiveProtocol(_id); }

#if 0
    /// disconnect the user session.
    bool disconnect() { return (WFDisconnectSessionEx(_server, _id, WF_DISCONNECT_NO_NOTIFY_CLIENT, NULL, 0) == TRUE); }
#else
    /// disconnect the user session.
    bool disconnect() { return (WFDisconnectSession(_server, _id, FALSE) == TRUE); }
#endif

    // logoff the user session
    bool logoff() { return (WFLogoffSession(_server, _id, FALSE) == TRUE); }

    // send message
    bool send_message(const std::wstring& title, const std::wstring& message, DWORD dwTimeout) {
        return (WFSendMessage(_server, _id, const_cast<LPWSTR>(title.c_str()), (DWORD)title.size(), const_cast<LPWSTR>(message.c_str()), (DWORD)message.size(), MB_OK, dwTimeout, NULL, FALSE) == TRUE);
    }
};