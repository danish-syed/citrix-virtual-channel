#pragma once

#include "vc_exception.hpp"

#include <vdviewport.h>

#include <sstream>

namespace vcsdk {

/// the viewport through which the contents of the remote desktop or remote application are displayed.
class viewport {
private:

	PVD _vd; /// a handle to the virtual channel driver.
	UINT32 _handle;

	static UINT32 register_for_notifications(PVD pVd, PFN_VIEWPORTCHANGED pCallback) {
		UINT32 handle = 0;
		if (auto ec = make_vc_error_code(VdRegisterViewportChangedCallback(pVd, pCallback, &handle)); ec) {
			throw vc_exception(ec);
		}
		return handle;
	}

public:

	/// constructor. construct with handle to virtual channel driver.
	viewport(PVD vd, PFN_VIEWPORTCHANGED callback)
		: _vd(vd)
		, _handle(register_for_notifications(vd, callback)) {
	}

	/// constructor. copy construction is not allowed.
	viewport(const viewport&) = delete;

	/// copy assignment operator. copy assignment is not allowed.
	viewport& operator =(const viewport&) = delete;

	/// constructor. move constructor.
	viewport(viewport&& other) noexcept
		: _vd(std::exchange(other._vd, nullptr))
		, _handle(std::exchange(other._handle, 0)) {
	}

	/// move assignment operator.
	viewport& operator =(viewport&& other) {
		if (this != &other) {
			std::swap(_vd, other._vd);
			std::swap(_handle, other._handle);
		}
		return *this;
	}

	/// destructor.
	~viewport() {
		VdUnregisterViewportChangedCallback(_vd, _handle);
	}

	/// find the local window handle that maps to the given remote/virtual window handle.
	HND translate_app_window(HND windowHandle) {
		HND localWindow = NULL;
		if (auto ec = make_vc_error_code(VdTranslateAppWindow(_vd, windowHandle, &localWindow)); ec) {
			throw vc_exception(ec);
		}

		std::ostringstream oss;
		oss << "VCSDK-CtxOverlay. viewport::translate_app_window. VDA side window handle = 0x";
		oss << windowHandle;
		oss << " -> client side window handle = 0x";
		oss << localWindow << "\n";
		OutputDebugStringA(oss.str().c_str());

		return localWindow;
	}

	/// translate a rectangle from the VDA to local coordinates.
	RECT translate_virtual_rect(RECT rcVirtual, UINT32 type) {
		RECT rcLocal = {};
		if (auto ec = make_vc_error_code(VdTranslateVirtualRect(_vd, rcVirtual, &rcLocal, type)); ec) {
			throw vc_exception(ec);
		}
		return rcLocal;
	}

	bool is_seamless() const {
		VDVIEWPORTINFO info{};
		if (auto ec = make_vc_error_code(VdGetViewportInformation(_vd, &info)); ec) {
			throw vc_exception(ec);
		}

		return VdViewportIsSeamless(info);
	}
};

} // namespace vcsdk