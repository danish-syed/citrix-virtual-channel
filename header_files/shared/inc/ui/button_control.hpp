#pragma once

#include "window.hpp"


class button_control : public window {
public:

	/// constructor.
	button_control(HWND hwnd)
		: window(hwnd) {
	}

	static button_control create(const window& parent, int nID, const std::wstring& text, RECT rc) {
		return CreateWindowW(L"BUTTON", text.c_str(), WS_CHILD, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, parent, (HMENU)(UINT_PTR)nID, parent.get_app_instance(), nullptr);
	}

	void set_image(HBITMAP hBitmap) { send_message(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }
	HBITMAP get_image() { return (HBITMAP)send_message(BM_GETIMAGE, IMAGE_BITMAP, 0L); }
};