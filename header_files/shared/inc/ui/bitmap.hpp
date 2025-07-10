#ifndef bitmap_hpp_included
#define bitmap_hpp_included


/// represents a GDI bitmap object
class bitmap {
private:

	HBITMAP _hndl; // a handle to the GDI bitmap

public:

	/// constructor.
	bitmap(HBITMAP hndl) : _hndl(hndl) {}

	/// constructor. copy construction is not allowed.
	bitmap(const bitmap& other) = delete;

	/// constructor. move constructor.
	bitmap(bitmap&& other) noexcept : _hndl(std::exchange(other._hndl, (HBITMAP)NULL)) {}

	/// destructor.
	~bitmap() {
		if (_hndl != NULL) {
			DeleteObject(_hndl);
			_hndl = NULL;
		}
	}

	/// move assignment operator.
	bitmap& operator =(bitmap&& other) noexcept {
		std::swap(_hndl, other._hndl);
		return *this;
	}

	/// load a bitmap resource
	static bitmap from_resource(HINSTANCE hAppInstance, USHORT resourceId) {
		return bitmap(LoadBitmap(hAppInstance, MAKEINTRESOURCE(resourceId)));
	}

	/// HBITMAP conversion operator.
	operator HBITMAP() const { return _hndl; }

	/// get the dimensions of the bitmap
	SIZE dimensions() {
		BITMAP bm = { 0 };
		(void)GetObject(_hndl, sizeof(bm), &bm);

		SIZE biSize = {};
		biSize.cx = bm.bmWidth;
		biSize.cy = bm.bmHeight;

		return biSize;
	}

	bool valid() const {
		return _hndl != NULL;
	}

	static bitmap from_pixels(HDC hdc, SIZE dims, PBYTE bits) {

		BITMAPINFO bi{};
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biWidth = dims.cx;
		bi.bmiHeader.biHeight = -dims.cy;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biSizeImage = dims.cx * dims.cy * 4;
		bi.bmiHeader.biCompression = BI_RGB;

		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, dims.cx, dims.cy);

		SetDIBits(hdc, hBitmap, 0, dims.cy, bits, &bi, DIB_RGB_COLORS);

		return hBitmap;
	}
};

#endif // !defined bitmap_hpp_included