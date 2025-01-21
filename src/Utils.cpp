#include "../include/Utils.hpp"

template <>
std::stringstream stringHex(uint8_t value, unsigned int padding, bool includePrefix) {
    std::stringstream ss;

    if (includePrefix) ss << "0x";

    if (padding <= 0) {
        ss << std::hex << std::uppercase << +value << std::dec;
    } else {
        ss << std::setfill('0') << std::setw(padding) << std::hex << std::uppercase
            << +value << std::dec;
    }
    return ss;
}
