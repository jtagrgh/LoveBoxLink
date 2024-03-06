#include "PixelBroadcastMessage.hpp"
#include "Message.hpp"

SerialData PixelBroadcastMessage::serializeData() const
{
    SerialData out;
    out.push_back(x);
    out.push_back(y);
    out.push_back(colour);

    return out;
}

void PixelBroadcastMessage::init(SerialData serialData)
{
    serialData.get_at(0, x);
    serialData.get_at(2, y);
    serialData.get_at(4, colour);
}

Message::Type PixelBroadcastMessage::getType() const
{
    return PIXEL_BROADCAST_MSG;
} 

PixelBroadcastMessage::PixelBroadcastMessage(uint16_t x, uint16_t y, uint16_t colour)
    : x(x), y(y), colour(colour)
{

}
