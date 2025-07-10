#ifndef message_loop_hpp_included
#define message_loop_hpp_included


/// a simple windows message event loop processor
class message_loop final {
private:

	HACCEL _hAccel;

public:

	/// constructor.
	message_loop(HACCEL hAccel)
		: _hAccel(hAccel) {
	}

	/// runs the event loop for a specific window
	int runFor(HWND hwnd) {
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, _hAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (int)msg.wParam;
	}

	/// runs the event loop for all windows
	int run() {
		return runFor(nullptr);
	}
};

#endif // message_loop_hpp_included