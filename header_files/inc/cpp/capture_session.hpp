#pragma once

#include "event_listener.hpp"

#include <vdgfxsink.h>

#include <memory>


namespace vcsdk {

/// an exlusive capture session instance
class capture_session {
private:

	/// a "deleter" type for capture sessions that can be used with std::unique_ptr.
	struct stop_session {
		using pointer = capture_session; /// the "handle" type for std::unique_ptr
		void operator ()(pointer session) { /// the deleter
			if (session._subscription.surface) {
				VdReleaseSurface(session._sink, session._subscription.surface);
			}
		}
	};

	/// start a capture session.
	bool start() {
		return (VdCaptureSurface(_sink, &_subscription) == CLIENT_STATUS_SUCCESS);
	}

public:

	/// callback function type
	using callback_type = std::function<void(GfxEvent, const void*)>;

	/// event listener type
	using event_listener = vcsdk::event_listener<callback_type>;

	/// managed resource type
	using unique_resource_type = std::unique_ptr<capture_session, stop_session>;

	/// start a new capture session.
	static unique_resource_type start(GfxSink sink, GfxSurfaceType type, const UINT32 id, const RECT& cropping_rect, event_listener&& fn) {
		GfxSurfaceDescriptor desc{};
		desc.surface_type = type;
		desc.id = id;
		desc.cropping_rect = cropping_rect;

		if (capture_session session{ sink, desc, std::move(fn) }; session.start()) {
			return unique_resource_type{ session };
		}

		return {};
	}

	/// constructor. default constructor.
	capture_session() = default;

	/// constructor. construct with a handle to a graphics sink, a surface descriptor and an event listener.
	capture_session(GfxSink sink, const GfxSurfaceDescriptor& descriptor, event_listener&& fn)
		: _sink(sink)
		, _subscription(descriptor) {

		add_event_listener(std::move(fn));
	}

	/// constructor. construct from null pointer. provided to meet the NullablePointer requirements so that capture_session can be used with std::unique_ptr.
	capture_session(std::nullptr_t) {}

	/// assignment operator for nullptr_t. provided to meet the NullablePointer requirements so that capture_session can be used with std::unique_ptr.
	capture_session& operator =(std::nullptr_t) {
		capture_session{}.swap(*this);
		return *this;
	}

	/// swap capture sessions.
	void swap(capture_session& other) noexcept {
		std::swap(_sink, other._sink);
		std::swap(_subscription, other._subscription);
		std::swap(_listeners, other._listeners);
	}

	/// dereference operator. used to work around a limitation with std::unique_ptr.
	capture_session& operator *() const noexcept { return const_cast<capture_session&>(*this); }

	/// boolean conversion operator.
	explicit operator bool() const noexcept { return _subscription.surface; }

	/// add an additional event listener. returns true iff the event listener was successfully added. an event listener will not be added if it exists already in the set.
	bool add_event_listener(event_listener&& listener) {
		return _listeners.emplace(std::move(listener)).second;
	}

	/// remove an event listener. returns the number of active listeners that remain after removal.
	std::size_t remove_event_listener(unsigned int id) {
		_listeners.erase({ id, nullptr });
		return _listeners.size();
	}

	/// process an event.
	void on_event(const GfxEvent& evt) const {
		switch (evt) {
		case GfxEvt_Subscribed:
		case GfxEvt_Unsubscribed:
			notify(evt, _subscription);
			break;

		case GfxEvt_FrameUpdate:
			on_capture_frame();
			break;
		}
	}

private:

	/// notify all event listeners that an event has occurred.
	template <typename T>
	void notify(const GfxEvent& evt, const T& data) const {
		std::ranges::for_each(_listeners, [this, evt, &data](const event_listener& listener) { listener.get()(evt, &data); });
	}

	/// handles frame update events. retrieves the image data and notifies event listeners.
	void on_capture_frame() const {
		if (GfxImage image{}; VdLockSurface(_sink, _subscription.surface, &image) == CLIENT_STATUS_SUCCESS) {
			notify(GfxEvt_FrameUpdate, image);
			VdUnlockSurface(_sink, _subscription.surface);
		}
	}


	GfxSink _sink{}; /// the graphics sink handle.
	GfxSurfaceDescriptor _subscription{}; /// the capture surface for the session.
	std::unordered_set<event_listener, event_listener::hash> _listeners; /// capture session event listeners
};

} // namespace vcsdk