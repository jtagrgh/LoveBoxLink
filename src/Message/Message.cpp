#include <vector>

#include "Message.hpp"
#include "SerialData.hpp"

SerialData Message::serialize() const
{
    SerialData out;
    SerialData data = serializeData();

    const uint8_t totalMessageSize = 1 + 1 + data.size(); // size, type, data...

    out.push_back(totalMessageSize);
    out.push_back((uint8_t)getType());

    for (uint8_t byte : data)
    {
        out.push_back(byte);
    }

    return out;
}

Message::Info Message::deserialize(SerialData serialData)
{
    Info out;
    out.totalSize = serialData[0];
    out.type = static_cast<Type>(serialData[1]);
    out.data = SerialData(serialData.begin() + 2, serialData.end());

    return out;
}
