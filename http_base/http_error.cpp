#include "http_error.h"
namespace http 
{


format_error::format_error(const std::string& msg) : std::runtime_error(msg)
{
}

format_error::format_error(const char* msg) : std::runtime_error(msg)
{
}

}   //end of namespace http

