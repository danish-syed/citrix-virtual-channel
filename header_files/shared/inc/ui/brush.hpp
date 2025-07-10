#pragma once


class brush {
private:

	HBRUSH _hndl{NULL};

public:

	brush(HBRUSH hndl) : _hndl(hndl) {}
	brush(const brush&) = delete;
	brush(brush&&) noexcept = default;
	~brush() {
		if (_hndl) {
			DeleteObject(_hndl);
		}
	}

	brush& operator =(const brush&) = delete;
	brush& operator =(brush&&) noexcept = default;

	static brush solid_brush(COLORREF color) {
		return CreateSolidBrush(color);
	}

	operator HBRUSH() { return _hndl; }
};