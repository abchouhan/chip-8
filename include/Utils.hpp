#pragma once
#include <cstdint>
#include <iomanip>
#include <iostream>

#if false
    #define TRACE(a) std::clog << a << std::endl;
#else
    #define TRACE(a)
#endif

#define RESET   "\033[m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

template <typename T>
std::stringstream stringHex(const T value, unsigned int padding = 0, bool includePrefix = true) {
    std::stringstream ss;

    if (includePrefix) ss << "0x";

    if (padding == 0) {
        ss << std::hex << std::uppercase << value << std::dec;
    } else {
        ss << std::setfill('0') << std::setw(padding) << std::hex
            << std::uppercase << value << std::dec;
    }
    return ss;
}

template <>
std::stringstream stringHex(uint8_t value, unsigned int padding, bool includePrefix);
