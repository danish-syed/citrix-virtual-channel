#pragma once

#include "window_class.hpp"
#include "device_context.hpp"

#include <tuple>


/// an application window
class window {
protected:

    HWND _hwnd{}; /// the WIN32 window handle

public:

    /// constructor. default constructor.
    window() = default;

    /// constructor. construct with underlying window handle.
    window(HWND hwnd)
        : _hwnd(hwnd) {
    }

    /// constructor. copy construction is not allowed.
    window(const window& other) = default;

    /// constructor. move constructor.
    window(window&& other) noexcept
        : _hwnd(std::exchange(other._hwnd, nullptr)) {
    }

    /// destructor.
    virtual ~window() {}

    /// move assignment operator.
    window& operator =(window&& other) noexcept {
        if (this != &other) {
            std::swap(_hwnd, other._hwnd);
        }
        return *this;
    }

    window& operator =(const window&) = default;

    /// destroy the window
    void destroy() { DestroyWindow(_hwnd); }

    /// retrieve a pointer to an offset within the window data.
    LONG_PTR get_window_data(int index) const { return GetWindowLongPtr(_hwnd, index); }

    /// store a value at an offset within the window data.
    void set_window_data(int index, LONG_PTR lpData) { SetWindowLongPtr(_hwnd, index, lpData); }

    /// retrieve the application instance from the window.
    HINSTANCE get_app_instance() const { return (HINSTANCE)get_window_data(GWLP_HINSTANCE); }

    /// retrieve user data from the window.
    LONG_PTR get_user_data() const { return get_window_data(GWLP_USERDATA); }

    /// store user data for the window.
    void set_user_data(LONG_PTR lpData) { set_window_data(GWLP_USERDATA, lpData); }

    /// redraw the window
    void redraw() { RedrawWindow(_hwnd, NULL, NULL, RDW_INVALIDATE); }

    /// update the window by immediately repainting it.
    void update() { UpdateWindow(_hwnd); }

    /// make the window visible.
    void show() { ShowWindow(_hwnd, SW_SHOW); }

    /// hide the window so that it is not shown to the user.
    void hide() { ShowWindow(_hwnd, SW_HIDE); }

    /// enable the window so that it accepts mouse or keyboard input.
    void enable() { EnableWindow(_hwnd, TRUE); }

    /// disable the window so that it does not accept mouse or keyboard input.
    void disable() { EnableWindow(_hwnd, FALSE); }

    /// test if the window is visible.
    bool visible() const { return (IsWindowVisible(_hwnd) == TRUE); }

    /// test if the window is minimized.
    bool iconic() const { return (IsIconic(_hwnd) == TRUE); }

    /// retrieve window styles
    LONG get_style() { return GetWindowLong(_hwnd, GWL_STYLE); }

    /// set window styles
    void set_style(LONG new_style) { SetWindowLong(_hwnd, GWL_STYLE, new_style); }

    /// add and remove window styles
    void modify_style(LONG add_styles, LONG remove_styles) {
        LONG lStyle = get_style();
        lStyle |= add_styles;
        lStyle &= ~remove_styles;
        set_style(lStyle);
    }

    /// retrieve extended window styles
    LONG get_extended_style() { return GetWindowLong(_hwnd, GWL_EXSTYLE); }

    /// set extended window styles
    void set_extended_style(LONG new_style) { SetWindowLong(_hwnd, GWL_EXSTYLE, new_style); }

    /// specify the window region to clip the window to.
    void clip_to_region(HRGN hrgn) { SetWindowRgn(_hwnd, hrgn, TRUE); }

    /// clear the window clip region.
    void clear_region() { SetWindowRgn(_hwnd, NULL, TRUE); }

    /// reposition the window
    void move_window(RECT rc) { MoveWindow(_hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE); }

    /// get the client area rectangle.
    void get_client_rect(LPRECT lprc) const { GetClientRect(_hwnd, lprc); }

    /// get the window rectangle in screen coordinates.
    void get_window_rect(LPRECT lprc) const { GetWindowRect(_hwnd, lprc); }

    /// convert a point in the window's client area to screen coordinates.
    void client_to_screen(LPPOINT lppt) const { ClientToScreen(_hwnd, lppt); }

    /// get the client area rectangle in screen coordinates.
    void client_rect_to_screen(LPRECT lprc) const {
        POINT pt = {};
        client_to_screen(&pt);
        get_client_rect(lprc);
        OffsetRect(lprc, pt.x, pt.y);
    }

    /// get the dimensions of the window.
    SIZE get_dimensions() const {
        RECT rc = {};
        get_client_rect(&rc);

        SIZE dims = {};
        dims.cx = rc.right - rc.left;
        dims.cy = rc.bottom - rc.top;

        return dims;
    }

    /// resize the window so that the client area of the window matches the requested size. the window itself will be bigger to account for the border and caption.
    void resize_client_area(SIZE size) {
        RECT rcWindow = {}, rcClient = {};
        get_window_rect(&rcWindow);
        get_client_rect(&rcClient);

        POINT ptDiff{};
        ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
        ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

        rcWindow.right = rcWindow.left + size.cx + ptDiff.x;
        rcWindow.bottom = rcWindow.top + size.cy + ptDiff.y;
        move_window(rcWindow);
    }

    /// test if the window has a valid window handle.
    bool valid() const { return (IsWindow(_hwnd) == TRUE); }

    /// set the text or title of the window.
    void set_text(const std::wstring& text) { SetWindowTextW(_hwnd, text.c_str()); }

    /// retrieve the text or title from the window.
    std::wstring get_text() const {
        std::wstring text(GetWindowTextLengthW(_hwnd) + 1, L'\0');
        text.resize(GetWindowTextW(_hwnd, text.data(), static_cast<int>(text.size())));
        return text;
    }

    /// retrieve the thread that created/owns the window.
    DWORD get_thread_id() const { return GetWindowThreadProcessId(_hwnd, NULL); }

    /// post a message to the window.
    bool post_message(UINT message, WPARAM wparam, LPARAM lparam) const { return (PostMessage(_hwnd, message, wparam, lparam) == TRUE); }

    /// send a message to the window.
    LRESULT send_message(UINT message, WPARAM wparam, LPARAM lparam) const { return SendMessage(_hwnd, message, wparam, lparam); }

    /// process a message on the thread that created/owns the window.
    void send_message_to_main_thread(UINT message, WPARAM wparam, LPARAM lparam) const {
        if (get_thread_id() != GetCurrentThreadId()) {
            post_message(message, wparam, lparam);
            return;
        }

        send_message(message, wparam, lparam);
    }

    /// get a related window.
    window get_window(UINT cmd) const { return GetWindow(_hwnd, cmd); }

    /// get the window that owns this one.
    window get_owner() const { return get_window(GW_OWNER); }

    /// get the parent window.
    window get_parent() const { return GetParent(_hwnd); }

    /// set the parent window.
    void set_parent(HWND hwndParent) { SetParent(_hwnd, hwndParent); }

    /// embed this window into another by making it a child window.
    void embed_into(HWND hwndParent) {
        set_parent(hwndParent);
        modify_style(WS_CHILDWINDOW, WS_OVERLAPPEDWINDOW);
    }

    /// HWND conversion operator.
    operator HWND() const { return _hwnd; }

    /// get the device context for the window.
    device_context get_device_context() const { return device_context(GetDC(_hwnd), [hwnd = _hwnd](HDC dc) { ReleaseDC(hwnd, dc); }); }

    /// print the contents of the window into the specified device context.
    bool print(device_context& dc) { return (PrintWindow(_hwnd, dc, PW_RENDERFULLCONTENT) == TRUE); }

    /// return a compatible bitmap for the window.
    bitmap compatible_bitmap(SIZE size) const { return get_device_context().create_compatible_bitmap(size); }
    bitmap compatible_bitmap() const { return compatible_bitmap(get_dimensions()); }

private:

    /// prepare the window for painting and run the paint callback function.
    void paint_internal(std::function<void(device_context&, PAINTSTRUCT&)> fn) {
        PAINTSTRUCT ps{};
        device_context dc(BeginPaint(_hwnd, &ps), device_context::NullDeleter);
        fn(dc, ps);
        EndPaint(_hwnd, &ps);
    }

public:

    using PaintHandler = std::function<void(device_context&, RECT)>;

    /// repaint invalidated rectangle
    void paint(PaintHandler fn) {
        paint_internal([fn = std::move(fn)](device_context& dc, PAINTSTRUCT& ps) { fn(dc, ps.rcPaint); });
    }

    /// repaint the entire client area.
    void paint_all(PaintHandler fn) {
        paint_internal([this, fn = std::move(fn)](device_context& dc, PAINTSTRUCT& ps) {
            RECT rc{};
            get_client_rect(&rc);
            fn(dc, rc);
        });
    }



#if 1 // experimental
    // scrolling stuff
    void update_scrollbar(int bar, int size, int pos) {
        SCROLLINFO si = { 0 };
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = size;
        si.nPage = pos;
        SetScrollInfo(_hwnd, bar, &si, TRUE);
    }

    void update_horizontal_scrollbar(int width, int pos) { update_scrollbar(SB_HORZ, width, pos); }
    void update_vertical_scollbar(int height, int pos) { update_scrollbar(SB_VERT, height, pos); }
    void enable_horizontal_scrollbar() { modify_style(WS_HSCROLL, 0); }
    void enable_vertical_scrollbar() { modify_style(WS_VSCROLL, 0); }
    void enable_scrollbars() { modify_style(WS_HSCROLL | WS_VSCROLL, 0); }
    void scroll(int dx, int dy) { ScrollWindow(_hwnd, dx, dy, NULL, NULL); }


    std::tuple<int, int, int> get_scroll_info(int nBar) {
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(_hwnd, nBar, &si);
        return std::tuple(si.nPage, si.nPos, si.nTrackPos);
    }

    int get_scroll_position_x() { return get_scroll_position(SB_HORZ); }
    int get_scroll_position_y() { return get_scroll_position(SB_VERT); }
    int get_scroll_position(int nBar) {
        int pos = 0;
        std::tie(std::ignore, pos, std::ignore) = get_scroll_info(nBar);
        return pos;
    }

    void set_scroll_position_x(int pos) { return set_scroll_position(SB_HORZ, pos); }
    void set_scroll_position_y(int pos) { return set_scroll_position(SB_VERT, pos); }
    void set_scroll_position(int nBar, int pos) {
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        si.nPos = pos;
        SetScrollInfo(_hwnd, nBar, &si, TRUE);
    }
#endif


    /// create the window
    template <typename T>
    static bool create_window(window_class<T>& cls, const std::wstring& name, LPVOID pInstance) {

        // WM_CREATE will be dispatched before this returns
        HWND hwnd = CreateWindowW(cls.name().c_str(), name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, cls.app_instance(), (LPVOID)pInstance);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        return (hwnd != NULL);
    }
};