#include "TestSerialData.hpp"
#include "SerialData.hpp"
#include <sstream>
#include <ios>

std::string TestSerialData::getName() const
{
	return "TestSerialData";
}


Test::Result TestSerialData::run()
{
	const SerialData empty;
	uint8_t out8;
	uint16_t out16;

	try
	{	
		uint8_t out;
		empty.get_at(0, out);
		RESET_REASON();
		reason << "No exception on accessing empty SerialData for a uint8_t";
		return Test::Result{false, reason.str()};
	}
	catch (std::out_of_range& e)
	{
		// Good
	}

	try
	{	
		uint16_t out;
		empty.get_at(0, out);
		RESET_REASON();
		reason << "No exception on accessing empty SerialData for a uint16_t";
		return Test::Result{false, reason.str()};
	}
	catch (std::out_of_range& e)
	{
		// Good
	}

	const SerialData nonEmpty{0x02, 0x04, 0xFF, 0xFF};

	nonEmpty.get_at(0, out8);
	if (out8 != 0x02)
	{
		RESET_REASON();
		reason << "Got wrong data at index 0, expected " << 0x02 << " got " << (int)out8;
		return Test::Result{false, reason.str()};
	}

	nonEmpty.get_at(3, out8);
	if (out8 != 0xFF)
	{
		RESET_REASON();
		reason << "Got wrong data at index 3, expected " << 0xFF << " got " << (int)out8;
		return Test::Result{false, reason.str()};
	}

	nonEmpty.get_at(0, out16);
	if (out16 != 0x0204)
	{
		RESET_REASON();
		reason << "Got wrong value getting uint16_t from SerialData. Got " << std::hex << std::showbase << out16;
		return Test::Result{false, reason.str()};
	}

	nonEmpty.get_at(2, out16);
	if (out16 != 0xFFFF)
	{
		RESET_REASON();
		reason << "Got wrong value getting uint16_t from SerialData. Got " << std::hex << std::showbase << out16;
		return Test::Result{false, reason.str()};
	}

	try
	{
		nonEmpty.get_at(3, out16);
		RESET_REASON();
		reason << "Expected out_of_range exception getting uint16_t at last index";
		return Test::Result{false, reason.str()};
	}
	catch (std::out_of_range& e)
	{
		// Good
	}


	SerialData toBeFilled;

	toBeFilled.push_back((uint8_t)0x0A);
	toBeFilled.get_at(0, out8);
	if (out8 != 0x0A)
	{
		RESET_REASON();
		reason << "Wrong value pushed. Actually pushed " << std::hex << std::showbase << (int)out8;
		return Test::Result{false, reason.str()};
	}

	toBeFilled.push_back((uint8_t)0xBB);
	toBeFilled.get_at(1, out8);
	if (out8 != 0xBB)
	{
		RESET_REASON();
		reason << "Wrong value pushed. Actually pushed " << std::hex << std::showbase << (int)out8;
		return Test::Result{false, reason.str()};
	}

	SerialData toBePrinted{0xAB, 0xCD, 0xEF, 0x00};
	std::string stringified = toBePrinted.toString();
	if (stringified != "AB CD EF 00")
	{
		RESET_REASON();
		reason << "Didn't convert to to string correctly. Got " << stringified;
		return Test::Result{false, reason.str()};
	}

	stringified = empty.toString();
	if (stringified != "")
	{
		RESET_REASON();
		reason << "Didn't convert to to string correctly. Got " << stringified;
		return Test::Result{false, reason.str()};
	}


	return Test::Result{true, ""};
}






