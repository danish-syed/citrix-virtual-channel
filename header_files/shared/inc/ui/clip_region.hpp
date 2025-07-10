#pragma once

#include <vector>


/// represents a GDI clipping region.
class clip_region {
private:

	/// The native region object
	HRGN _region;

private:

	/// constructor. construct with native region object handle.
	clip_region(HRGN region)
		: _region(region) {
	}

	/// copy constructor. copy construction not allowed.
	clip_region(const clip_region&) = delete;

	/// compute a bounding rectangle for the specified list of rectangles.
	static RECT compute_bounding_rectangle(std::vector<RECT>& rectList) {

		if (rectList.size()) {

			RECT bounding_rect;
			bounding_rect.left = rectList[0].left;
			bounding_rect.top = rectList[0].top;
			bounding_rect.right = rectList[0].right;
			bounding_rect.bottom = rectList[0].bottom;

			for (std::size_t i = 0; i < rectList.size(); i++) {

				bounding_rect.left = std::min(bounding_rect.left, rectList[i].left);
				bounding_rect.top = std::min(bounding_rect.top, rectList[i].top);
				bounding_rect.right = std::max(bounding_rect.right, rectList[i].right);
				bounding_rect.bottom = std::max(bounding_rect.bottom, rectList[i].bottom);
			}

			return bounding_rect;
		}

		return {};
	}

public:

	static clip_region from_window(HWND hwnd) {
		HDC hDC = GetDC(hwnd);
		clip_region clip;
		(void)GetRandomRgn(hDC, clip, SYSRGN);
		ReleaseDC(hwnd, hDC);
		return clip;
	}

	/// create a clip region from a rectangle.
	static clip_region from_rectangle(RECT& rect) {
		return clip_region(CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom));
	}

	/// create a clip region from a list of rectangles.
	static clip_region from_rectangle_list(std::vector<RECT>& rectList) {

		if (rectList.size() == 1) {

			// simple rectangle region.
			return from_rectangle(rectList[0]);
		}
		else if (rectList.size() > 1) {

			// complex shape region.
			DWORD data_size = sizeof(RGNDATAHEADER) + ((DWORD)rectList.size() * sizeof(RECT));
			std::vector<BYTE> data_array(data_size);
			PRGNDATA data = reinterpret_cast<PRGNDATA>(data_array.data());

			// setup region data header
			data->rdh.dwSize = sizeof(RGNDATAHEADER);
			data->rdh.iType = RDH_RECTANGLES;
			data->rdh.nCount = (DWORD)rectList.size();
			data->rdh.nRgnSize = (DWORD)rectList.size() * sizeof(RECT);
			data->rdh.rcBound = compute_bounding_rectangle(rectList);

			// copy rectangles to region data.
			memcpy(&data->Buffer[0], rectList.data(), data->rdh.nRgnSize);

			// create the region.
			HRGN rgn = ExtCreateRegion(NULL, data_size, data);

			if (rgn) {
				return clip_region(rgn);
			}
		}

		return clip_region();
	}

	/// constructor. default constructor.
	clip_region()
		: _region(CreateRectRgn(0, 0, 0, 0)) {
	}

	/// move constructor.
	clip_region(clip_region&& source)
		: _region(NULL) {

		std::swap(source._region, _region);
	}

	/// Destructor.
	~clip_region() {
		if (_region) {
			DeleteObject(_region);
		}
	}

	/// conversion operator.
	operator HRGN() {
		return _region;
	}

	/// move assignment operator.
	clip_region& operator = (clip_region&& source) {
		std::swap(_region, source._region);
		return *this;
	}

	/// overloaded bitwise-and operator. returns a new clip_region consisting of the intersection of the current clip region and the specified clip region.
	clip_region operator & (const clip_region& clip) const {
		clip_region new_clip;
		(void)CombineRgn(new_clip, _region, clip._region, RGN_AND);
		return std::move(new_clip);
	}

	/// overloaded bitwise-or operator. returns a new clip_region consisting of the union of the current clip region and the specified clip region.
	clip_region operator | (const clip_region& clip) const {
		clip_region new_clip;
		(void)CombineRgn(new_clip, _region, clip._region, RGN_OR);
		return std::move(new_clip);
	}

	/// overloaded bitwise-xor operator. returns a new clip_region consisting of the union of the current clip region and the specified clip region except overlapping areas.
	clip_region operator ^ (const clip_region& clip) const {
		clip_region new_clip;
		(void)CombineRgn(new_clip, _region, clip._region, RGN_XOR);
		return std::move(new_clip);
	}

	/// overloaded difference operator. returns a new clip_region consisting of the difference between the current clip region and the specified clip region.
	clip_region operator - (const clip_region& clip) const {
		clip_region new_clip;
		(void)CombineRgn(new_clip, _region, clip._region, RGN_DIFF);
		return std::move(new_clip);
	}

	/// creates a list of rectangles that correspond to the clip_region.
	std::vector<RECT> to_rectangle_list() {
		std::vector<RECT> rect_list;
		DWORD dwByteCount = GetRegionData(_region, 0, NULL);
		if (dwByteCount > 0) {

			std::vector<BYTE> data(dwByteCount);
			RGNDATA* region_data = reinterpret_cast<RGNDATA*>(data.data());
			if (GetRegionData(_region, dwByteCount, region_data)) {

				RECT* rect_data = reinterpret_cast<RECT*>(region_data->Buffer);
				rect_list.assign(rect_data, rect_data + region_data->rdh.nCount);
			}
		}

		return rect_list;
	}
};
