#pragma once

#include "vc.hpp"
#include "vc_exception.hpp"
#include "vc_viewport.hpp"

#include <vdio.h>
#include <ica-c2h.h> // for PVD_C2H
#include <vdapi.h>

#include <map>
#include <memory> // for std::unique_ptr
#include <algorithm> // for std::any_of

#include <sstream>

namespace vcsdk {

/// a virtual channel controller. the VC SDK allows a virtual channel driver to implement/provide more than one virtual channel,
/// although this is not common practice. nevertheless, the virtual channel controller manages the multiple virtual channels.
class vc_controller {
private:

	PVD _vd; /// a handle to the virtual channel driver
	BYTE _versionL;
	BYTE _versionH;
	std::map<USHORT, std::unique_ptr<virtual_channel>> _vcs; /// a collection of virtual channels managed by this controller.

	viewport _view; /// the viewport for this virtual channel.

	/// embed the vc controller class instance in the private virtual driver data.
	void register_with_driver() {
		if (_vd) _vd->pPrivate = this;
	}

	/// notify each virtual channel of display changes.
	void viewport_changed() {
		for (auto& vc : _vcs) {
			vc.second->onviewportchanged();
		}
	}

public:

	/// extract the vc controller instance from a virtual driver's private data member.
	static vc_controller* from_handle(PVD pVd) {
		if (!pVd) throw vc_exception(make_vc_error_code(CLIENT_ERROR_NULL_MEM_POINTER));
		return reinterpret_cast<vc_controller*>(pVd->pPrivate);
	}

	static void WFCAPI ICAViewportChanged2(PVD pVd) {
		try {
			from_handle(pVd)->viewport_changed();
		}
		catch (const vc_exception& ex) {
			UNREFERENCED_PARAMETER(ex);
		}
	}

	/// constructor.
	vc_controller(PVD vd, BYTE versionL, BYTE versionH)
		: _vd(vd)
		, _versionL(versionL)
		, _versionH(versionH)
		, _view(vd, &ICAViewportChanged2) {

		register_with_driver();
	}

	/// constructor. copy construction is not allowed.
	vc_controller(const vc_controller&) = delete;

	/// copy assignment operator. copy assignment is not allowed.
	vc_controller& operator =(const vc_controller&) = delete;

	/// constructor. move constructor.
	vc_controller(vc_controller&& other) noexcept
		: _vd(std::exchange(other._vd, nullptr))
		, _versionL(std::exchange(other._versionL, 0))
		, _versionH(std::exchange(other._versionH, 0))
		, _vcs(std::move(other._vcs))
		, _view(std::move(other._view)) {

		register_with_driver();
		other.register_with_driver();
	}

	/// move assignment operator.
	vc_controller& operator =(vc_controller&& other) noexcept {
		if (this != &other) {
			std::swap(_vd, other._vd);
			std::swap(_versionL, other._versionL);
			std::swap(_versionH, other._versionH);
			std::swap(_vcs, other._vcs);
			std::swap(_view, other._view);
		}
		return *this;
	}

	/// destructor. un-embed the class instance from the virtual driver's private data.
	~vc_controller() {
		if (_vd) _vd->pPrivate = NULL;
	}

	/// return a handle to the virtual channel driver.
	PVD handle() const {
		return _vd;
	}

	viewport& get_viewport() {
		return _view;
	}

	USHORT open_vc(const std::string& name) {

		// Get a virtual channel
		USHORT usChannelNumber = 0;
		if (INT rc = VdOpen(_vd, (PCHAR)name.c_str(), &usChannelNumber); rc != CLIENT_STATUS_SUCCESS) {
			throw vc_exception(make_vc_error_code(rc));
		}

		return usChannelNumber;
	}

	template <typename T, typename... Args>
	void open_vc(const std::string& name, Args&&... args) {
		add_vc(std::unique_ptr<T>(new T(open_vc(name), this, std::forward<Args>(args)...)));
	}

	void add_vc(std::unique_ptr<virtual_channel>&& pvc) {
		_vcs.emplace(pvc->channelNumber(), std::move(pvc));
	}



	void set_info(PVDSETINFORMATION pInfo, PUINT16 puiSize) {
		switch (pInfo->VdInformationClass) {
		case VdGBufferValidateConnection:
			if (GBufferConnectionEvt* pEvt = (GBufferConnectionEvt*)pInfo->pVdInformation; pEvt != NULL /* && *puiSize == sizeof(GBufferConnectionEvt)*/) {
				for (auto& vc : _vcs) {
					if (vc.second->authorize_graphics_listener(pEvt)) {
						break;
					}
				}
			}
			break;

		case VdSeamlessWindowLifecycle:
		{
			SeamlessWindowLifecycleEvt* seamlessWindowLifecycleEvent = (SeamlessWindowLifecycleEvt*)pInfo->pVdInformation;
			std::ostringstream oss;
			oss << "VCSDK-CtxOverlay. vc_controller::set_info. Received seamless window ";
			oss << (seamlessWindowLifecycleEvent->create ? "create" : "destroy");
			oss << " event.\n";
			OutputDebugStringA(oss.str().c_str());
			viewport_changed();
			break;
		}

		case VdSeamlessWindowState:
		{
			SeamlessWindowStateEvt* seamlessWindowStateEvent = (SeamlessWindowStateEvt*)pInfo->pVdInformation;
			std::ostringstream oss;
			oss << "VCSDK-CtxOverlay. vc_controller::set_info. Received VdSeamlessWindowState event: ";
			oss << "Event type = " << seamlessWindowStateEvent->event;
			oss << ", position = (" << seamlessWindowStateEvent->posX << ", " << seamlessWindowStateEvent->posY << ")\n";
			OutputDebugStringA(oss.str().c_str());
			break;
		}
		
		default:
			break;
		}
	}

	int info(LPBYTE lpData, PUSHORT pusLength) {

		USHORT ByteCount = 0;

		// figure out how much data there is. since the data is shared among all VCs, they should all
		// be in agreement.
		for (auto& vc : _vcs) {
			vc.second->writeSharedData(NULL, ByteCount);
		}

		if (*pusLength < ByteCount) {
			*pusLength = ByteCount;
			return CLIENT_ERROR_BUFFER_TOO_SMALL;
		}
		else if (lpData != NULL) {

			PVD_C2H pvdC2H = reinterpret_cast<PVD_C2H>(lpData);

			// initialize module header
			PMODULE_C2H pModule = &pvdC2H->Header;
			pModule->ByteCount = ByteCount;
			pModule->ModuleClass = Module_VirtualDriver;
			pModule->VersionL = _versionL;
			pModule->VersionH = _versionH;

			// initialize vd header
			PVDFLOW pFlow = &pvdC2H->Flow;
			pFlow->BandwidthQuota = 0;
			pFlow->Flow = VirtualFlow_None;

			pvdC2H->ChannelMask = _vd->ChannelMask;

			// allow virtual channels to populate their respective portions of the shared controller data.
			for (auto& vc : _vcs) {
				vc.second->writeSharedData(lpData, ByteCount);
			}
		}

		return CLIENT_STATUS_SUCCESS;
	}

	bool poll() {
		return std::any_of(_vcs.begin(), _vcs.end(), [](auto& entry) { return entry.second->poll(); });
	}

	/// process incoming data for the virtual channel identified by the given virtual channel number.
	void receive_data(USHORT usChannelNumber, MEMORY_SECTION buf) {
		_vcs[usChannelNumber]->ondata(buf.pSection, buf.length);
	}
};

} // namespace vcsdk