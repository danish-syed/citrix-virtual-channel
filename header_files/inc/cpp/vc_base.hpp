#pragma once

#include "vc_controller.hpp"
#include "vc_io_controller.hpp"


namespace vcsdk {

class virtual_channel_base : public virtual_channel {
protected:

    USHORT _usChannelNumber; /// the assigned virtual channel number
    vc_controller* _controller; /// the virtual channel controller which manages this virtual channel
    vc_io_controller _io; /// the I/O controller for writing data to the VC

    /// constructor.
    virtual_channel_base(USHORT usChannelNumber, vc_controller* controller)
        : _usChannelNumber(usChannelNumber)
        , _controller(controller)
        , _io(vc_io_controller::create(usChannelNumber, controller->handle())) {
    }

    /// write outgoing data for the virtual channel.
    void write(MEMORY_SECTION buf) {
        _io.write(buf);
    }

public:

    /// constructor. copy construction is not allowed.
    virtual_channel_base(const virtual_channel_base&) = delete;

    /// copy assignment operator. copy assignment is not allowed.
    virtual_channel_base& operator =(const virtual_channel_base&) = delete;

    /// constructor. move constructor.
    virtual_channel_base(virtual_channel_base&& other) noexcept
        : _usChannelNumber(std::exchange(other._usChannelNumber, 0))
        , _controller(std::exchange(other._controller, nullptr))
        , _io(std::move(other._io)) {
    }

    /// move assignment operator.
    virtual_channel_base& operator =(virtual_channel_base&& other) noexcept {
        if (this != &other) {
            std::swap(_usChannelNumber, other._usChannelNumber);
            std::swap(_controller, other._controller);
            std::swap(_io, other._io);
        }
        return *this;
    }

    /// flush any pending outgoing data for this virtual channel.
    bool poll() override {
        return _io.poll();
    }

    /// return the virtual channel number.
    USHORT channelNumber() const override {
        return _usChannelNumber;
    }

    /// authorize a graphics listener, if this virtual channel has one.
    bool authorize_graphics_listener(GBufferConnectionEvt* pEvt) override { return false; };
};

} // namespace vcsdk