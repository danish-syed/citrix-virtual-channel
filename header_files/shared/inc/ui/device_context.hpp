#pragma once

#include "bitmap.hpp"

#include <functional>


class device_context {
public:

	using DeleterFn = std::function<void(HDC)>;

	static constexpr auto DeleteDC = [](HDC hdc) { ::DeleteDC(hdc); };
	static constexpr auto NullDeleter = [](HDC hdc) {};

private:

	HDC _hndl;
	DeleterFn _deleter;

public:

	/// constructor.
	device_context(HDC hndl)
		: _hndl(hndl)
		, _deleter(DeleteDC) {
	}

	/// constructor.
	device_context(HDC hndl, DeleterFn&& deleter)
		: _hndl(hndl)
		, _deleter(std::move(deleter)) {
	}

	/// constructor. copy construction is not allowed.
	device_context(const device_context&) = delete;

	/// copy assignment operator. copy assignment is not allowed.
	device_context& operator =(const device_context&) = delete;

	/// constructor. move constructor.
	device_context(device_context&& other) noexcept
		: _hndl(std::exchange(other._hndl, nullptr))
		, _deleter(std::move(other._deleter)) {
	}

	/// move assignment operator.
	device_context& operator =(device_context&& other) noexcept {
		if (this != &other) {
			std::swap(_hndl, other._hndl);
			std::swap(_deleter, other._deleter);
		}
		return *this;
	}

	/// destructor.
	~device_context() {
		if (_deleter) _deleter(_hndl);
	}

	/// HDC conversion operator.
	operator HDC() const { return _hndl; }


	static device_context new_memory_dc() { return CreateCompatibleDC(NULL); }

	device_context new_compatible_dc() { return CreateCompatibleDC(_hndl); }

	void set_stretch_mode(int mode) { SetStretchBltMode(_hndl, mode); }
	void set_stretch_halftone() { set_stretch_mode(HALFTONE); }

	void stretch_blt(const device_context& source, RECT rcSource, RECT rcTarget) {
		StretchBlt(_hndl, rcTarget.left, rcTarget.top, rcTarget.right - rcTarget.left, rcTarget.bottom - rcTarget.top,
			source, rcSource.left, rcSource.top, rcSource.right - rcSource.left, rcSource.bottom - rcSource.top, SRCCOPY);
	}

	void blt(const device_context& source, POINT ptTarget, POINT ptSource, SIZE sz) {
		BitBlt(_hndl, ptTarget.x, ptTarget.y, sz.cx, sz.cy, source, ptSource.x, ptSource.y, SRCCOPY);
	}

	bitmap create_compatible_bitmap(SIZE sz) {
		return CreateCompatibleBitmap(_hndl, sz.cx, sz.cy);
	}

	void using_object(HGDIOBJ hNewObj, std::function<void(device_context&)> fn) {
		HGDIOBJ hOldObj = SelectObject(_hndl, hNewObj);
		fn(*this);
		(void)SelectObject(_hndl, hOldObj);
	}

	void using_bitmap(bitmap& img, std::function<void(device_context&, bitmap&)> fn) {
		using_object(img, [&img, &fn](device_context& dc) { fn(dc, img); });
	}

	void using_bitmap(bitmap&& img, std::function<void(device_context&, bitmap&)> fn) {
		using_object(img, [&img, &fn](device_context& dc) { fn(dc, img); });
	}

	static constexpr RECT from_size(SIZE sz) {
		RECT rc = {};
		rc.right = sz.cx;
		rc.bottom = sz.cy;
		return rc;
	}

	void draw(bitmap& image, RECT rcTarget) {
		new_compatible_dc().using_bitmap(image, [this, rcTarget](device_context& dc, bitmap& img) {
			stretch_blt(dc, from_size(img.dimensions()), rcTarget);
			});
	}

	void draw(bitmap& image, POINT ptTarget) {
		new_compatible_dc().using_bitmap(image, [this, ptTarget](device_context& dc, bitmap& img) { blt(dc, ptTarget, {0, 0}, img.dimensions()); });
	}

	void frame_rect(const RECT& rc, HBRUSH hBrush) {
		FrameRect(_hndl, &rc, hBrush);
	}
};