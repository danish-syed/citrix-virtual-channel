#pragma once

#include <button_control.hpp>


class slider_control : public button_control {
public:

	/// constructor. default constructor.
	slider_control()
		: button_control(NULL) {
	}

	/// constructor. construct with window handle and images.
	slider_control(HWND hwnd)
		: button_control(hwnd) {
	}

	/// obtain a new window handle by creating an underlying button window.
	static HWND create(const window& parent, int nID, const RECT& rc) {
		return CreateWindowW(L"BUTTON", L"", WS_CHILD | BS_CHECKBOX /*| BS_OWNERDRAW*/, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, parent, (HMENU)(UINT_PTR)nID, parent.get_app_instance(), nullptr);
	}

	void turn_on() {
		send_message(BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}

	void turn_off() {
		send_message(BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
	}

	bool is_on() {
		return (send_message(BM_GETCHECK, 0, 0) == BST_CHECKED);
	}

	bool toggle() {
		if (bool prev_state = is_on(); prev_state) {
			turn_off();
			return prev_state;
		}

		turn_on();
		return false;
	}
};