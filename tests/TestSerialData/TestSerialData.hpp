#pragma once

#include "Test.hpp"

struct TestSerialData : Test
{
	Result run() override;
	std::string getName() const override;
};