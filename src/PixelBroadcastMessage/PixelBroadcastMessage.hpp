#include <stdint.h>

#include "Message.hpp"
#include "SerialData.hpp"

struct PixelBroadcastMessage : Message
{
    SerialData serializeData() const override;
    void init(SerialData) override;
    Type getType() const override;

    PixelBroadcastMessage(uint16_t, uint16_t, uint16_t);
    PixelBroadcastMessage() = default;

    uint16_t x;
    uint16_t y;
    uint16_t colour;
};