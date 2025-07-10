#pragma once

#include "wfbuffer.hpp"

#include <string>


class wfappinfo {
private:

	wfbuffer _data;

public:

	wfappinfo(wfbuffer&& buf) : _data(std::move(buf)) {}

	std::wstring initialProgram() {
		return std::wstring(_data.as<WF_APP_INFO>()->InitialProgram);
	}

	std::wstring workingDirectory() {
		return std::wstring(_data.as<WF_APP_INFO>()->WorkingDirectory);
	}

	std::wstring applicationName() {
		return std::wstring(_data.as<WF_APP_INFO>()->ApplicationName);
	}
};