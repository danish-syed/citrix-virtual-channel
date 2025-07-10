#pragma once

#include "wfbuffer.hpp"

#include <string>
#include <algorithm>


class wfversioninfo {
private:

	wfbuffer _data;

public:

	/// constructor.
	wfversioninfo(wfbuffer&& buf)
		: _data(std::move(buf)) {
	}

	std::size_t major() const {
		return _data.as<WF_VERSIONEX_INFO>()->dwMajorVersion;
	}

	std::size_t minor() const {
		return _data.as<WF_VERSIONEX_INFO>()->dwMinorVersion;
	}

	std::size_t buildNumber() const {
		return _data.as<WF_VERSIONEX_INFO>()->dwBuildNumber;
	}

	std::size_t platformId() const {
		return _data.as<WF_VERSIONEX_INFO>()->dwPlatformId;
	}

	std::wstring csd() const {
		WCHAR* pstr = _data.as<WF_VERSIONEX_INFO>()->szCSDVersion;
		return std::wstring(pstr, std::min((std::size_t)128, wcslen(pstr)));
	}
};