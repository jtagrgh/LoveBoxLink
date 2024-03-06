#pragma once

#include <string>
#include <sstream>

#define RESET_REASON() \
	do \
	{ \
		reason.str(""); \
		reason.clear(); \
		reason << "[" << __func__ << "(): " << __LINE__  << "] "; \
	} while (0); \

struct Test
{
	struct Result
	{
		bool passed;
		std::string reason;
	};

	virtual Result run() = 0;
	virtual std::string getName() const = 0;
	virtual ~Test() = default;

	std::ostringstream reason;
};