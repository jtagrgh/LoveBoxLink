#include "TestPixelBroadcastMessage.hpp"
#include "PixelBroadcastMessage.hpp"

std::string TestPixelBroadcastMessage::getName() const
{
	return "TestPixelBroadcastMessage";
}

Test::Result TestPixelBroadcastMessage::run()
{
	const PixelBroadcastMessage sendPixel(0x1234, 0x8888, 0xFFFF);
	const SerialData sendSerial = sendPixel.serialize();

	const SerialData recvSerial = sendSerial;
	const Message::Info recvInfo = Message::deserialize(recvSerial);

	if (recvInfo.type != sendPixel.getType())
	{
		RESET_REASON();
		reason << "Sent message of type " << (int)sendPixel.getType() << ". But received " << (int)recvInfo.type;
		return Test::Result{false, reason.str()};
	}

	const PixelBroadcastMessage recvPixel = Message::construct<PixelBroadcastMessage>(recvInfo.data);

	if (sendPixel.x != recvPixel.x)
	{
		RESET_REASON();
		reason << "sendPixel.x is " << sendPixel.x << " but recvPixel.x is " << recvPixel.x;
		return Test::Result{false, reason.str()};
	}

	if (sendPixel.y != recvPixel.y)
	{
		RESET_REASON();
		reason << "sendPixel.y is " << sendPixel.y << " but recvPixel.y is " << recvPixel.y;
		return Test::Result{false, reason.str()};
	}

	if (sendPixel.colour != recvPixel.colour)
	{
		RESET_REASON();
		reason << "sendPixel.colour is " << sendPixel.colour << " but recvPixel.colour is " << recvPixel.colour;
		return Test::Result{false, reason.str()};
	}

	return Test::Result{true, ""};
}