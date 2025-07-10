#pragma once

#include "window.hpp"


class static_control : public window {
public:

	/// constructor. default constructor.
	static_control()
		: window(NULL) {
	}

	/// constructor.
	static_control(HWND hwnd)
		: window(hwnd) {
	}

	static static_control create(const window& parent, int nID, const std::wstring& text, RECT rc) {
		return CreateWindowW(L"STATIC", text.c_str(), WS_CHILD, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, parent, (HMENU)(UINT_PTR)nID, parent.get_app_instance(), nullptr);
	}

	/// set the icon to display on the static control.
	void set_icon(HICON hIcon) { Static_SetIcon(_hwnd, hIcon); }

	/// retrieve a handle to the current icon for the static control.
	HICON get_icon() { return Static_GetIcon(_hwnd, unused); }

	/// set the bitmap to display on the static control.
	void set_image(HBITMAP hBitmap) { send_message(STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }

	/// retrieve a handle to the current bitmap for the static control.
	HBITMAP get_image() { return (HBITMAP)send_message(STM_GETIMAGE, IMAGE_BITMAP, 0L); }
};