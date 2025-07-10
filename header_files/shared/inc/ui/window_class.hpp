#pragma once

#include <string>


/// manages the lifecycle of a window class
template <typename window_type>
class window_class {
private:

    HINSTANCE _inst; /// the application instance
    std::wstring _name; /// the name of the window class
    ATOM _class; /// a handle to the window class

    /// constructor. construct with an app instance, class name and handle
    window_class(HINSTANCE hInstance, const std::wstring& name, ATOM cls)
        : _inst(hInstance)
        , _name(name)
        , _class(cls) {
    }

public:

    /// registers a new window class.
    static window_class register_class(HINSTANCE hInstance, const std::wstring& szWindowClass, HBRUSH hbrBackground) {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = window_type::dispatch_message;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = hbrBackground;
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_APP_MENU);
        wcex.lpszClassName = szWindowClass.c_str();
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL_ICON));
        return window_class(hInstance, szWindowClass, RegisterClassExW(&wcex));
    }

    static window_class register_class(HINSTANCE hInstance, const std::wstring& szWindowClass) {
        return register_class(hInstance, szWindowClass, (HBRUSH)COLOR_WINDOW);
    }

    /// unregisters a previously registered window class.
    ~window_class() {
        if (_class != NULL) {
            UnregisterClassW(_name.c_str(), _inst);
            _class = NULL;
        }
    }

    /// retrieve the window class name
    const std::wstring& name() const { return _name; }

    /// retrieve the application instance
    HINSTANCE app_instance() { return _inst; }
};