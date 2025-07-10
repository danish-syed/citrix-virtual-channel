#pragma once

#include "wfbuffer.hpp"

#include <string>


enum wfclientproductid {
    wfcpi_windows,
    wfcpi_linux,
    wfcpi_mac,
    wfcpi_ios,
    wfcpi_android,
    wfcpi_html5,
    wfcpi_unknown
};


static constexpr std::wstring to_string(wfclientproductid id) {
    std::wstring str;
    switch (id) {
    case wfcpi_windows:
        str = L"Windows";
        break;

    case wfcpi_linux:
        str = L"unix/linux";
        break;

    case wfcpi_mac:
        str = L"Mac";
        break;

    case wfcpi_ios:
        str = L"iOS";
        break;

    case wfcpi_android:
        str = L"Android";
        break;

    case wfcpi_html5:
        str = L"HTML5/Chrome";
        break;

    case wfcpi_unknown:
    default:
        str = L"Unknown";
        break;
    }
    return str;
}


static constexpr ULONG WindowsClientProductID = 0x0001; /// Windows client product identifier
static constexpr ULONG LinuxClientProductID = 0x0051; /// Unix/Linux client product identifier
static constexpr ULONG MacClientProductID = 0x0052; /// Mac client product identifier
static constexpr ULONG iOSClientProductID = 0x0053; /// iOS client product identifier
static constexpr ULONG AndroidClientProductID = 0x0054; /// Android client product identifier
static constexpr ULONG HTML5ClientProductID = 0x0101; /// HTML5/Chrome client product identifier


/// client information
class wfclientinfo {
private:

	wfbuffer _data; /// the data buffer containing client information

public:

	/// constructor.
	wfclientinfo(wfbuffer&& buf)
		: _data(std::move(buf)) {
	}

    /// retrieve the client product name.
    std::wstring name() const {
        return std::wstring(_data.as<WF_CLIENT_INFO>()->Name);
    }

    /// retrieve the client product directory.
    std::wstring directory() const {
        return std::wstring(_data.as<WF_CLIENT_INFO>()->Directory);
    }

    /// retrieve the client product build number.
    std::size_t buildNumber() const {
        return _data.as<WF_CLIENT_INFO>()->BuildNumber;
    }

    /// retrieve the client product id.
    wfclientproductid productId() const {
        ULONG productId = _data.as<WF_CLIENT_INFO>()->ProductID;
        switch (productId) {
        case WindowsClientProductID:
            return wfcpi_windows;

        case LinuxClientProductID:
            return wfcpi_linux;

        case MacClientProductID:
            return wfcpi_mac;

        case iOSClientProductID:
            return wfcpi_ios;

        case AndroidClientProductID:
            return wfcpi_android;

        case HTML5ClientProductID:
            return wfcpi_html5;

        default:
            break;
        }

        return wfcpi_unknown;
    }

    /// return the client (IP) address.
    std::string address() const {
        WF_CLIENT_ADDRESS addr = _data.as< WF_CLIENT_INFO>()->Address;
        return std::string((char*)addr.Address);
    }
};