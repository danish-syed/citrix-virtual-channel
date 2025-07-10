#pragma once

#include "capture_session.hpp"

#include <string>
#include <array>
#include <mutex>


namespace vcsdk {

/// a graphics sink which can receive app-sharing or screen-sharing image frames from the graphics service/provider.
class graphics_sink {
private:

	/// the managed resource type for capture sessions.
	using session_handle_type = typename capture_session::unique_resource_type;

	/// the container type used to store capture sessions.
	using sessions_type = std::unordered_map<unsigned int, session_handle_type>;

	/// a "deleter" type for graphics sinks that can be used with std::unique_ptr.
	struct release {
		using pointer = GfxSink;
		void operator()(pointer sink) const { VdReleaseGraphicsSink(sink); }
	};

	typename capture_session::callback_type _fn; /// the virtual channel callback to receive connected/disconnected events.
	std::recursive_mutex _lock; /// the synchronization lock to protect access to the sessions list.
	std::array<sessions_type, 2> _sessions; /// the list of capture sessions.
	std::unique_ptr<GfxSink, release> _sink; /// the underlying graphics sink managed resource.

public:

	/// constructor.
	graphics_sink(PVD pVd, const std::string& vcName, typename capture_session::callback_type&& fn)
		: _fn(std::move(fn))
		, _sink(acquire(pVd, vcName, this)) {
	}

	/// move constructor. move construction is not allowed because the managed resource holds a pointer to this object instance.
	graphics_sink(graphics_sink&&) = delete;

	/// move assignment. move assignment is not allowed because the managed resource holds a pointer to this object instance.
	graphics_sink& operator =(graphics_sink&&) = delete;

	/// destructor.
	~graphics_sink() {
		_sessions[GfxSurf_Application].clear();
		_sessions[GfxSurf_Monitor].clear();
	}

	/// authorize the graphics sink to receive content.
	bool authorize(GBufferConnectionEvt* pEvt) {
		(void)VdAuthorizeGraphicsSink(_sink.get(), pEvt);
		return pEvt->validClientSecret == TRUE ? true : false;
	}

	/// add a new capture session.
	void add_capture_session(const unsigned int& id, const GfxSurfaceType& type, const unsigned int& overlay_id, capture_session::callback_type&& fn) {
		if (!using_session(id, type, [overlay_id, &fn](capture_session& session) { session.add_event_listener({ overlay_id, std::move(fn) }); return true; })) {
			_sessions[type].emplace(id, capture_session::start(_sink.get(), type, id, {}, { overlay_id, std::move(fn) }));
		}
	}

	/// remove an existing capture session.
	void delete_capture_session(const unsigned int& id, const GfxSurfaceType& type, const unsigned int& overlay_id) {
		if (using_session(id, type, [overlay_id](capture_session& session) { return session.remove_event_listener(overlay_id) == 0; })) {
			_sessions[type].erase(id);
		}
	}

private:

	/// perform an operation on a capture session from the sessions list. returns false if no session is found for the specified id.
	bool using_session(const unsigned int& id, const GfxSurfaceType& type, std::function<bool(capture_session&)>&& fn) {
		std::unique_lock lock{ _lock };
		if (auto it = _sessions[type].find(id); it != _sessions[type].end()) {
			return fn(get_from_managed_resource(it->second));
		}

		return false;
	}

	/// process an event from the graphics sink.
	void on_event(GfxEvent evt, const void* data) {
		switch (evt) {
		case GfxEvt_Connected:
		case GfxEvt_Disconnected:
			forward_event_to_vc(evt, data);
			break;

		case GfxEvt_FrameUpdate:
		case GfxEvt_Subscribed:
		case GfxEvt_Unsubscribed:
			forward_event_to_session(evt, reinterpret_cast<const GfxSurfaceDescriptor*>(data));
			break;

		default:
			break;
		}
	}

	/// relay an event from the graphics sink to the owning virtual channel.
	void forward_event_to_vc(GfxEvent evt, const void* data) {
		if (_fn) _fn(evt, data);
	}

	/// relay an event from the graphics sink to the appropriate capture session.
	void forward_event_to_session(const GfxEvent& evt, const GfxSurfaceDescriptor* surf) {
		std::unique_lock lock{ _lock };
		if (auto it = _sessions[surf->surface_type].find(surf->id); it != _sessions[surf->surface_type].end()) {
			get_from_managed_resource(it->second).on_event(evt);
		}
	}

	capture_session& get_from_managed_resource(typename capture_session::unique_resource_type& resource) const {
		return *resource;
	}

	/// the graphics sink event callback.
	static void WFCAPI EventCallback(GfxSink listener, GfxEvent evt, void* data, void* client) {
		if (graphics_sink* provider = reinterpret_cast<graphics_sink*>(client);  provider != nullptr) {
			provider->on_event(evt, data);
		}
	}

	/// acquire a new graphics sink.
	static GfxSink acquire(PVD pVd, const std::string& vcName, void* client) {
		GfxSink sink{};
		if (auto ec = make_vc_error_code(VdAcquireGraphicsSink(pVd, vcName.c_str(), &EventCallback, client, &sink)); ec) {
			throw vc_exception(ec);
		}
		return sink;
	}
};

} // namespace vcsdk