#pragma once
#include <stdexcept>
#include <string>


namespace http
{

struct format_error : std::runtime_error
{
    explicit format_error(const std::string& err);
    explicit format_error(const char* err);
};

using network_error  = format_error;    // just so that we would know that this is network issue

}   // end of namespace http



