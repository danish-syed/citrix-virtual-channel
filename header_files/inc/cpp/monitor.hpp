#pragma once

#include <vdmonitors.h>


namespace vcsdk {

class monitor {
private:

    int _id;
    RECT _rc;
    USHORT _ppi;
    USHORT _scale;

public:

    /// constructor. construct from underlying VC SDK API object.
    /*explicit*/ monitor(const VIRTUAL_MONITOR& mon)
        : _id(mon.Id)
        , _rc({ mon.Rect.iX, mon.Rect.iY, mon.Rect.iX + static_cast<INT32>(mon.Rect.uiWidth), mon.Rect.iY + static_cast<INT32>(mon.Rect.uiHeight) })
        , _ppi(mon.DevicePpi)
        , _scale(mon.ScaleFactor) {}

    /// constructor. construct with rectangle, pixels-per-inch and scale.
    explicit monitor(int precedence, const RECT& rc, const USHORT& ppi, const USHORT& scale)
        : _id(precedence)
        , _rc(rc)
        , _ppi(ppi)
        , _scale(scale) {
    }

    const RECT& bounds() const { return _rc; }
    const int& id() const { return _id; }
};

} // namespace vcsdk