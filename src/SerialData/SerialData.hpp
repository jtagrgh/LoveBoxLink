#pragma once

#include <vector>
#include <string>
#include <stdint.h>

struct SerialData : std::vector<uint8_t>
{
    typedef std::vector<uint8_t> Super;

    using Super::Super;
    using Super::push_back;

    void push_back(uint16_t);
    void get_at(int, uint16_t&) const;
    void get_at(int, uint8_t&) const;
    std::string toString() const;
};
