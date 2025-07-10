#pragma once

#include <button_control.hpp>

#include <array>


template <std::size_t N> // N denotes the number of states for the image button
class image_button : public button_control {
private:
	std::array<bitmap, N> _imgs; /// the images which represent the states of the image button.

public:

	/// constructor. construct with window handle and images.
	image_button(HWND hwnd, std::array<bitmap, N>&& imgs)
		: button_control(hwnd)
		, _imgs(std::move(imgs)) {

		set_state(0);
	}

	/// obtain a new window handle by creating an underlying button window.
	static HWND create(const window& parent, int nID, const RECT& rc) {
		return CreateWindowW(L"BUTTON", L"", WS_CHILD | BS_BITMAP | BS_FLAT, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, parent, (HMENU)(UINT_PTR)nID, parent.get_app_instance(), nullptr);
	}

	/// change the state of the image button.
	void set_state(std::size_t state) {
		if (state < N) {
			set_image(_imgs[state]);
		}
	}
};