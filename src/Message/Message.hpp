#pragma once

#include <stdint.h>

#include "SerialData.hpp"

struct Message
{
    enum Type : uint8_t
    {
        PIXEL_BROADCAST_MSG,
        NULL_MSG
    };

    struct Info
    {
        uint8_t totalSize;
        Message::Type type;
        SerialData data;
    };

    SerialData serialize() const;
    static Info deserialize(SerialData);

    virtual SerialData serializeData() const = 0;
    virtual void init(SerialData) = 0;
    virtual Type getType() const = 0;
};
