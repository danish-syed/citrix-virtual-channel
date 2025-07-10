#pragma once


/// a simple wrapper for WIN32 event objects
class wfevent {
private:

    HANDLE _hndl; /// the underlying event handle
    bool _should_delete;

    /// constructor.
    wfevent(HANDLE hndl)
        : _hndl(hndl)
        , _should_delete(false) {
    }

public:

    /// return a non-owning reference to the event
    wfevent ref() const { return wfevent(_hndl); }

    /// constructor.
    wfevent(bool auto_reset, bool initial_state)
        : _hndl(CreateEvent(NULL, auto_reset ? FALSE : TRUE, initial_state ? TRUE : FALSE, NULL))
        , _should_delete(true) {
    }

    /// constructor. copy construction is now allowed.
    wfevent(const wfevent&) = delete;

    /// constructor. move constructor.
    wfevent(wfevent&& other) noexcept
        : _hndl(std::exchange(other._hndl, INVALID_HANDLE_VALUE))
        , _should_delete(std::exchange(other._should_delete, false)) {
    }

    /// destructor.
    ~wfevent() {
        if (_should_delete) {
            CloseHandle(_hndl);
            _hndl = NULL;
        }
    }

    /// HANDLE conversion operator. enables seamless integration with WIN32 APIs.
    operator HANDLE() { return _hndl; }

    /// set the event
    bool set() { return (SetEvent(_hndl) == TRUE); }

    /// wait a specified amount of time for the event to become signalled.
    bool waitFor(DWORD dwMilliseconds) {
        switch (WaitForSingleObject(_hndl, dwMilliseconds)) {
        case WAIT_OBJECT_0:
        case WAIT_FAILED:
            return true;
            break;

        default:
            break;
        }

        return false;
    }

    /// wait indefinitely until the event is signalled.
    bool wait() { return waitFor(INFINITE); }
};