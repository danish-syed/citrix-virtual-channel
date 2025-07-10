#pragma once


class font {
private:

	HFONT _hndl;

public:

	/// constructor.
	font(HFONT hndl)
		: _hndl(hndl) {
	}

	/// constructor. copy construction is not allowed.
	font(const font&) = delete;

	/// constructor. move constructor.
	font(font&& other)
		: _hndl(std::exchange(other._hndl, nullptr)) {
	}

	/// destructor.
	virtual ~font() {
		if (_hndl) DeleteObject(_hndl);
	}

	/// move assignment operator.
	font& operator =(font&& other) {
		if (this != &other) {
			std::swap(_hndl, other._hndl);
		}
		return *this;
	}

	static font create(HDC dc, int point_size, int weight, bool italic, const std::wstring& face) {
		int pixels = MulDiv(point_size, GetDeviceCaps(dc, LOGPIXELSY), 72);
		return CreateFontW(-pixels, 0, 0, 0, weight, italic ? TRUE : FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY, FF_DONTCARE, face.c_str());
	}

	static font create(PLOGFONT logical_font) {
		return CreateFontIndirect(logical_font);
	}

	bool valid() const { return (_hndl != NULL); }

	operator HFONT() const { return _hndl; }
};