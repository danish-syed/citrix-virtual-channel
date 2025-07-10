#pragma once

#include "wfbuffer.hpp"

#include <string>


class wfuserinfo {
private:

	wfbuffer _data;

public:

	/// constructor.
	wfuserinfo(wfbuffer&& buf)
		: _data(std::move(buf)) {
	}

	/// retrieve the user name from the data buffer.
	std::wstring name() const {
		return std::wstring(_data.as<WF_USER_INFO>()->UserName);
	}

	/// retrieve the domain name from the data buffer.
	std::wstring domain() const {
		return std::wstring(_data.as<WF_USER_INFO>()->DomainName);
	}

	/// retrieve the connection name from the data buffer.
	std::wstring connectionName() const {
		return std::wstring(_data.as<WF_USER_INFO>()->ConnectionName);
	}
};