#pragma once

#include "window.hpp"
#include "window_class.hpp"

#include <functional>
#include <optional>
#include <string>


/// a custom, user-defined window class
class managed_window : public window/*, public window_event_listener*/ {
public:

	/// delete callback type
	using DeleterFn = std::function<void(managed_window*)>; // cleanup func

	/// window emplacement callback type
	using EmplaceFn = std::function<bool(HWND)>;

	/// a deleter which does nothing.
	static constexpr auto null_deleter = [](managed_window*) {};

	/// a deleter which informs the application to quit.
	static constexpr auto post_quit_deleter = [](managed_window*) { PostQuitMessage(0); };

private:

	DeleterFn _deleter;
	bool _closed;

	/// embed the managed window class instance pointer into the user data for the window handle, so that messages can be routed to the right class instance.
	void register_with_window() {
		set_user_data((LONG_PTR)this);
	}

public:

	/// constructor.
	managed_window(HWND hndl, DeleterFn&& deleter)
		: window(hndl)
		, _deleter(std::move(deleter))
		, _closed(false) {

		register_with_window();
	}

	/// move constructor.
	managed_window(managed_window&& other) noexcept
		: window(std::move(other))
		, _deleter(std::move(other._deleter))
		, _closed(std::exchange(other._closed, false)) {

		register_with_window();
		other.register_with_window();
	}

	/// destructor.
	~managed_window() {
		_closed = true;
		_deleter = null_deleter; // no need to inform owner since the owner initiated the destructor.
		destroy(); // should trigger WM_DESTROY

		// be careful not to pass any window messages, including WM_DESTROY on to the derived class.
		// otherwise, a pure virtual function call error will result.
	}

	/// move assignment operator.
	managed_window& operator =(managed_window&& other) noexcept {
		if (this != &other) {
			window::operator=(std::move(other));
			std::swap(_deleter, other._deleter);
			std::swap(_closed, other._closed);

			register_with_window();
			other.register_with_window();
		}
		return *this;
	}

	/// extract the managed window class instance pointer from the given window handle and invoke a callback with it.
	template <typename Fn>
	static LRESULT with_instance(HWND hwnd, Fn&& fn) {
		if (managed_window* win = reinterpret_cast<managed_window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); win) {
			return fn(win);
		}

		return 0;
	}

	/// dispatch window messages to the managed window class instance.
	static LRESULT dispatch_message(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		LRESULT lResult = -1;
		bool skip_default_processing = false;
		switch (message) {
		case WM_CREATE:
			if (LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam); lpcs != NULL) {
				if (EmplaceFn* emplace = reinterpret_cast<EmplaceFn*>(lpcs->lpCreateParams); emplace != NULL) {
					if ((*emplace)(hwnd)) { // create a new class instance with the given window handle
						lResult = 0;
					}
				}
			}
			break;

		case WM_DESTROY: // destroy can be initiated by the owner or the user
			lResult = with_instance(hwnd, [](managed_window* win) {
				win->set_user_data(NULL);
				win->_hwnd = NULL; // set the window handle to nullptr in order to prevent a double delete scenario
				win->_deleter(win); // inform the owner that the user closed the window, so that it can clean up
				return 0;
				});
			break;

		default:
			lResult = with_instance(hwnd, [&](managed_window* win) {
				return (win->_closed) ? -1 : win->on_message(message, wparam, lparam, skip_default_processing);
				});
			break;
		}

		if (lResult != 0 || skip_default_processing) {
			return lResult;
		}

		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	/// process a window message/event
	virtual LRESULT on_message(UINT message, WPARAM wparam, LPARAM lparam, bool& skip_default_processing) = 0;



	/// emplace new window into an associative container.
	template <typename Container, typename... Args>
	static bool emplace_new(Container& c, const typename Container::key_type& key, window_class<typename Container::value_type::second_type>& cls, const std::wstring& title, Args&&... args) {
		return emplace_new(cls, title,
			[&c, key](auto&&... args2) { c.emplace(key, typename Container::value_type::second_type(std::forward<decltype(args2)>(args2)...)); return true; }, // map_inserter
			[&c, key](managed_window*) { c.erase(key); }, // map_deleter
			std::forward<Args>(args)...);
	}

	/// emplace new window instance using custom inserter.
	template <typename T, typename Inserter, typename Deleter, typename... Args>
	static bool emplace_new(window_class<T>& cls, const std::wstring& title, Inserter&& inserter, Deleter&& deleter, Args&&... args) {
		EmplaceFn emplace = [&](HWND hwnd) { return inserter(hwnd, std::move(deleter), std::forward<Args>(args)...); }; // Inserter to EmplaceFn adapter
		return create_window(cls, title, &emplace); // should trigger WM_CREATE, which will then invoke emplace
	}

	template <typename T, typename... Args>
	static T create(window_class<T>& cls, const std::wstring& title, DeleterFn&& deleter, Args&&... args) {
		T proto; // the prototype window. an initial object which will received the newly constructed window. requires T to be default constructible.
		auto inserter = [&proto](auto&&... args2) mutable { std::move(proto) = T(std::forward<decltype(args2)>(args2)...); return true; };
		if (!emplace_new(cls, title, std::move(inserter), std::move(deleter), std::forward<Args>(args)...)) {
			deleter(&proto);
		}
		return std::move(proto);
	}
};