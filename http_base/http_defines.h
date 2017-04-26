#pragma once

#include <cstdint>


namespace http
{

struct normal_defines
{
    static constexpr const char* service_name() 
    {
        return "http";
    }

    static constexpr std::uint16_t port()
    {
        return 80;
    }
};

struct secure_defines
{
    static constexpr const char* service_name()
    {
        return "https";
    }
    static constexpr std::uint16_t port()
    {
        return 443;
    }

};


}   // end of namespace http


