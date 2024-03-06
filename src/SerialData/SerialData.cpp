#include "SerialData.hpp"

#include <iostream>
#include <stdio.h>

void SerialData::push_back(uint16_t x)
{
    Super::push_back((uint8_t)(x >> 8));
    Super::push_back((uint8_t)x);
}

void SerialData::get_at(int i, uint16_t& out) const
{
    out = 0;
    out += (((uint16_t)this->at(i)) << 8);
    out += this->at(i + 1);
}

void SerialData::get_at(int i, uint8_t& out) const
{
    out = this->at(i);
}

std::string SerialData::toString() const
{
    const size_t outSize = this->size()*3;
    if (outSize < 1)
    {
        return "";
    }
    char out[outSize]; // e.g., "FE " is 3 chars
    for (int i = 0; i < this->size(); i++)
    {
        snprintf(out+(i*3), 3, "%02X", this->at(i));
        out[i*3 + 2] = ' '; // b.c. snprintf adds a \n
    }
    out[outSize - 1] = '\0';
    return out;
}
