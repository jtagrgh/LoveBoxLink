#pragma once

#include "Test.hpp"

struct TestPixelBroadcastMessage : Test
{
	Result run() override;
	std::string getName() const override;
};