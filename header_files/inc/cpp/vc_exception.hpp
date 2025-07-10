#pragma once

#include "vc_errors.hpp"


class vc_exception : public std::system_error {
public:

	/// constructor.
	vc_exception(const std::error_code& ec)
		: std::system_error(ec) {
	}
};