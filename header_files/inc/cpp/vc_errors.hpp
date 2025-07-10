#pragma once

#include <clterr.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <system_error>


static auto& client_error_messages() {
    static std::unordered_map<int, std::string> table = {
        {CLIENT_ERROR, "Client error"},
        {CLIENT_ERROR_NO_MEMORY, "Insufficient memory"},
        {CLIENT_ERROR_BAD_OVERLAY, "Bad overlay"},
        {CLIENT_ERROR_BAD_PROCINDEX, "Bad procedure index"},
        {CLIENT_ERROR_BUFFER_TOO_SMALL, "Buffer too small"},
        {CLIENT_ERROR_CORRUPT_ALLOC_HEADER, "Corrupt memory header"},
        {CLIENT_ERROR_CORRUPT_ALLOC_TRAILER, "Corrupt memory trailer"},
        {CLIENT_ERROR_CORRUPT_FREE_HEADER, "Corrupt free header"},
        {CLIENT_ERROR_CORRUPT_SEG_HEADER, "Corrupt segment header"},
        {CLIENT_ERROR_MEM_ALREADY_FREE, "Memory already free"},
        {CLIENT_ERROR_MEM_TYPE_MISMATCH, "Memory type mismatch"},
        {CLIENT_ERROR_NULL_MEM_POINTER, "Null pointer"},
        {CLIENT_ERROR_IO_PENDING, "I/O pending"},
        {CLIENT_ERROR_INVALID_BUFFER_SIZE, "Invalid buffer size"},
        {CLIENT_ERROR_INVALID_MODE, "Invalid mode"},
        {CLIENT_ERROR_NOT_OPEN, "Not open"},
        {CLIENT_ERROR_NO_OUTBUF, "No output buffer"},
        {CLIENT_ERROR_DLL_PROCEDURE_NOT_FOUND, "DLL procedure not found"},
        {CLIENT_ERROR_DLL_LARGER_THAN_256K, "DLL larger than 256K"},
        {CLIENT_ERROR_DLL_BAD_EXEHEADER, "Corrupted DLL"},
        {CLIENT_ERROR_OPEN_FAILED, "Open failed"},
        {CLIENT_ERROR_INVALID_HANDLE, "Invalid handle"},
        {CLIENT_ERROR_VD_NOT_FOUND, "Virtual Driver not found"},
        {CLIENT_ERROR_WD_NAME_NOT_FOUND, "WinStation Driver name not found in MODULE.INI"},
        {CLIENT_ERROR_PD_NAME_NOT_FOUND, "Protocol Driver name not found in MODULE.INI"},
        {CLIENT_ERROR_THINWIRE_OUTOFSYNC, "Thinwire out of sync"},
        {CLIENT_ERROR_NO_MOUSE, "Mouse not available"},
        {CLIENT_ERROR_INVALID_CALL, "Invalid request"},
        {CLIENT_ERROR_QUEUE_FULL, "Queue is full"},
        {CLIENT_ERROR_INVALID_DLL_LOAD, "Invalid DLL load"},
        {CLIENT_ERROR_PD_ERROR, "Protocol Driver error"},
        {CLIENT_ERROR_VD_ERROR, "Virtual Driver error"},
        {CLIENT_ERROR_ALREADY_OPEN, "Already open"},
        {CLIENT_ERROR_PORT_NOT_AVAILABLE, "Port not available"},
        {CLIENT_ERROR_IO_ERROR, "Communication I/O Error"},
        {CLIENT_ERROR_THINWIRE_CACHE_ERROR, "Thinwire cache error"},
        {CLIENT_ERROR_BAD_FILE, "Bad file"},
        {CLIENT_ERROR_CONFIG_NOT_FOUND, "WFCLIENT.INI not found"},
        {CLIENT_ERROR_SERVER_FILE_NOT_FOUND, "APPSRV.INI not found"},
        {CLIENT_ERROR_PROTOCOL_FILE_NOT_FOUND, "MODULE.INI not found"},
        {CLIENT_ERROR_MODEM_FILE_NOT_FOUND, "MODEM.INI not found"},
        {CLIENT_ERROR_LAN_NOT_AVAILABLE, "LAN not available"},
        {CLIENT_ERROR_PD_TRANSPORT_UNAVAILABLE, "Transport not available"},
        {CLIENT_ERROR_INVALID_PARAMETER, "Invalid Parameter"},
        {CLIENT_ERROR_WD_NOT_LOADED, "No WinStation Driver is loaded"},
        {CLIENT_ERROR_NOT_CONNECTED, "Not connected to a Citrix Server"},
        {CLIENT_ERROR_VD_NOT_LOADED, "The Virtual Driver is not loaded"},
        {CLIENT_ERROR_ALREADY_CONNECTED, "Already connected to a Citrix Server"},
        {CLIENT_ERROR_WFENGINE_NOT_FOUND, "Cannot find the Citrix ICA Engine %s"},
        {CLIENT_ERROR_ENTRY_NOT_FOUND, "Entry not found in APPSRV.INI"},
        {CLIENT_ERROR_PD_ENTRY_NOT_FOUND, "Protocol Driver for Entry not found in APPSRV.INI"},
        {CLIENT_ERROR_WD_ENTRY_NOT_FOUND, "WinStation Driver for Entry not found in APPSRV.INI"},
        {CLIENT_ERROR_PD_SECTION_NOT_FOUND, "Protocol Driver section not found in MODULE.INI"},
        {CLIENT_ERROR_WD_SECTION_NOT_FOUND, "WinStation Driver section not found in MODULE.INI"},
        {CLIENT_ERROR_DEVICE_SECTION_NOT_FOUND, "Device section not found in WFCLIENT.INI"},
        {CLIENT_ERROR_VD_SECTION_NOT_FOUND, "Virtual Driver section not found in MODULE.INI"},
        {CLIENT_ERROR_VD_NAME_NOT_FOUND, "Virtual Driver name not found in MODULE.INI"},
        {CLIENT_ERROR_SERVER_CONFIG_NOT_FOUND, "[WFClient] section not found in APPSRV.INI"},
        {CLIENT_ERROR_CONFIG_CONFIG_NOT_FOUND, "[WFClient] section not found in WFCLIENT.INI"},
        {CLIENT_ERROR_HOTKEY_SHIFTSTATES_NOT_FOUND, "[Hotkey Shift States] section not found in MODULE.INI"},
        {CLIENT_ERROR_HOTKEY_KEYS_NOT_FOUND, "[Hotkey Keys] section not found in MODULE.INI"},
        {CLIENT_ERROR_KEYBOARDLAYOUT_NOT_FOUND, "[KeyboardLayout] section not found in MODULE.INI"},
        {CLIENT_ERROR_UI_ENGINE_VER_MISMATCH, "Invalid client window process"},
        {CLIENT_ERROR_IPC_TIMEOUT, "The client window process is not responding."},
        {CLIENT_ERROR_UNENCRYPT_RECEIVED,  "Data error, received unexpected unencrypted data."},
        {CLIENT_ERROR_NENUM_NOT_DEFINED,  "No network enumerator is defined in MODULE.INI"},
        {CLIENT_ERROR_NENUM_NO_SERVERS_FOUND, "No Citrix servers could be found."},
        {CLIENT_ERROR_NENUM_NETWORK_ERROR, "A network error was encountered while searching for Citrix servers."},
        {CLIENT_ERROR_NENUM_WINDOWS95_NOT_SUPPORTED, "This feature is not supported in Windows 95.  Use the Citrix Server's IP address for TCP/IP or the network:node address for IPX/SPX."},
        {CLIENT_ERROR_CONNECTION_TIMEOUT, "Connection dropped because of communication errors."},
        {CLIENT_ERROR_DRIVER_UNSUPPORTED, "Driver unsupported"},
        {CLIENT_ERROR_NO_MASTER_BROWSER, "Unable to contact the Citrix Server Locator.  Either your network is not functional, or you need to check your Server Location settings."},
        {CLIENT_ERROR_TRANSPORT_NOT_AVAILABLE, "The specified Network Protocol is not available."},
        {CLIENT_ERROR_NO_NAME_RESOLVER, "The specified Network Protocol is not available."},
        {CLIENT_ERROR_SEVEN_BIT_DATA_PATH, "A seven bit data path was detected to the Citrix server.  An eight bit data path is required."},
        {CLIENT_ERROR_WIN16_WFENGINE_ALREADY_RUNNING, "Your Citrix ICA Client connection is already running."},
        {CLIENT_ERROR_HOST_NOT_SECURED, "Cannot connect to Citrix server.  The specified server is not secure."},
        {CLIENT_ERROR_ENCRYPT_UNSUPPORT_BYCLIENT, "Your Citrix server requires encryption that your client does not support."},
        {CLIENT_ERROR_ENCRYPT_UNSUPPORT_BYHOST, "Your Citrix server does not support the encryption you required."},
        {CLIENT_ERROR_ENCRYPT_LEVEL_INCORRECTUSE, "This connection entry uses RC5 encryption. For enhanced security, please do not specify a User name and Password in the connection properties or on the command line."},
        {CLIENT_ERROR_BAD_OVERRIDES, "(Internal error - bad connection overrides)"},
        {CLIENT_ERROR_MISSING_CONNECTION_SECTION, "(Internal error - missing connection section)"},
        {CLIENT_ERROR_BAD_COMBINE_ENTRIES, "(Internal error - CombinePrivateProfileEntries failed)"},
        {CLIENT_ERROR_MISSING_WFCLIENT_SECTION, "(Internal error - missing [WFClient] section)"},
        {CLIENT_ERROR_BAD_ENTRY_INSERTION, "(Internal error - AddEntrySection failed)"},
        {CLIENT_ERROR_BAD_HEADER_INSERTION, "(Internal error - AddHeaderSection failed)"},
        {CLIENT_ERROR_BAD_CONCAT_SECTIONS, "(Internal error - ConcatSections failed)"},
        {CLIENT_ERROR_MISSING_SECTION, "(Internal error - missing section)"},
        {CLIENT_ERROR_DUPLICATE_SECTIONS, "(Internal error - duplicate sections)"},
        {CLIENT_ERROR_DRIVER_BAD_CONFIG, "(Internal error - bad driver configuration)"},
        {CLIENT_ERROR_MISMATCHED_ENCRYPTION, "(Internal error - mismatched encryption)"},
        {CLIENT_ERROR_TAPI_NO_INIT, "Failed to initialize the TAPI subsystem."},
        {CLIENT_ERROR_TAPI_VER, "The required TAPI version (1.4) is not available."},
        {CLIENT_ERROR_TAPI_NO_LINES, "There are no TAPI lines defined for use."},
        {CLIENT_ERROR_TAPI_LINE_NO_EXIST, "The TAPI device specified for this dial-in connection is not defined."},
        {CLIENT_ERROR_TAPI_NEGOTIATE_LINE, "The required TAPI line cannot be opened."},
        {CLIENT_ERROR_TAPI_CALL_NOT_MADE, "The TAPI call could not be made."},
        {CLIENT_ERROR_NO_MEMORY_LOAD_AUDIO_VESA, "No memory load audio (VESA)"},
        {CLIENT_ERROR_NO_MEMORY_LOAD_AUDIO, "No memory load audio"},
        {CLIENT_ERROR_NO_MEMORY_LOAD_VESA, "No memory load VESA"},
        {CLIENT_ERROR_NO_MEMORY_LOAD, "No memory load"},
        {CLIENT_ERROR_IPX_CONNECT_TIMEOUT, "The Citrix server has no more connections available at this time."},
        {CLIENT_ERROR_NETWORK_ENUM_FAIL, "Network enumeration failed"},
        {CLIENT_ERROR_PARTIAL_DISCONNECT, "Partial disconnect"},
        {CLIENT_ERROR_CRITICAL_SECTION_IN_USE, "Critical section in use"},
        {CLIENT_ERROR_DISK_FULL, "There is insufficient space in the client directory to launch another ICA session.Contact your system administrator for assistance."},
        {CLIENT_ERROR_DISK_WRITE_PROTECTED, "You need write privileges to the client directory to launch another ICA session. Contact your system administrator for assistance."},
        {CLIENT_ERROR_DLL_NOT_FOUND, "A required DLL file was not found."},
        {CLIENT_ERROR_HOST_REQUEST_FAILED, "The request to the Citrix server has failed"},
#if 0
        {CLIENT_ERROR_SSL_MISSING_CACERTIFICATE, "One of the specified CACerts is missing."},
        {CLIENT_ERROR_SSL_BAD_CACERTIFICATE, "One of the specified CACerts is corrupt."},
        {CLIENT_ERROR_SSL_BAD_CIPHER, "One of the specified ciphers is unsupported."},
        {CLIENT_ERROR_SSL_BAD_PRIVKEY, "The clients private key is corrupt."},
        {CLIENT_ERROR_SSL_UNSET_KEYSTORE, "The keystore location has not been set."},
#endif
        {CLIENT_ERROR_SEAMLESS_HOST_AGENT_NOT_READY, "Seamless Host agent is not ready"},
        {CLIENT_ERROR_SEAMLESS_HOST_BUSY, "Seamless Host agent is busy"},
        {CLIENT_ERROR_NOT_LOGGED_IN, "Not currently logged in"},
        {CLIENT_ERROR_SEAMLESS_CLIENT_BUSY, "Seamless Client agent is busy"},
        {CLIENT_ERROR_SEAMLESS_NOT_COMPATIBLE,"Seamless Host and Client agents are not compatible"}
    };

    return table;
}




namespace { // anonymous namespace

    struct ICAErrCategory : std::error_category {

        const char* name() const noexcept override {
            return "ICA";
        }

        std::string message(int ev) const override {
            auto& tbl = client_error_messages();
            if (auto it = tbl.find(ev); it != tbl.end()) {
                return it->second;
            }
            return "(unknown error)";
        }
    };

    const ICAErrCategory theICAErrCategory{};

} // anonymous namespace

inline std::error_code make_vc_error_code(int e) {
    return { e, theICAErrCategory };
}