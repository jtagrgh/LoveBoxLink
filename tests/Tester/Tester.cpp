#include <iostream>
#include <vector>
#include <memory>
#include "Test.hpp"
#include "TestSerialData.hpp"
#include "TestPixelBroadcastMessage.hpp"

int main()
{
	std::vector<std::unique_ptr<Test>> tests;
	tests.push_back(std::unique_ptr<Test>(new TestSerialData()));
	tests.push_back(std::unique_ptr<Test>(new TestPixelBroadcastMessage()));

	for (auto& test : tests)
	{
		Test::Result result = test->run();
		if (result.passed == false)
		{
			std::cout << "[x] " << test->getName() << " failed. Reason: " << result.reason << std::endl;
		}
		else
		{
			std::cout << "[o] " << test->getName() << " passed." << std::endl;
		}
	}

	return 0;
}