#pragma once

#include "static_control.hpp"


class dialog : public window {
public:

	/// constructor.
	dialog(HWND hDlg)
		: window(hDlg) {
	}

	window get_item(int item) { return GetDlgItem(_hwnd, item); }
	static_control get_static(int nID) { return GetDlgItem(_hwnd, nID); }
	void end(INT_PTR nResult) { EndDialog(_hwnd, nResult); }
};