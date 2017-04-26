#include "http_read.h"
#include "http_connection.h"
#include "https_connection.h"
#include <iostream>

namespace http
{

namespace
{
std::string string_cast(boost::asio::streambuf& from);
std::string string_cast(boost::asio::streambuf& from, std::size_t max);
std::string string_cast(boost::asio::streambuf& from, std::string::value_type delim, 
                        std::string::value_type ignore_char = '\0');

std::string string_cast(boost::asio::streambuf& from, std::size_t max)
{
    auto bufs = from.data();
    std::string str(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + max);
    return str;
}

std::string string_cast(boost::asio::streambuf& from)
{
    std::string s((std::istreambuf_iterator<char>(&from)), std::istreambuf_iterator<char>());
    return s;
}

std::string string_cast(boost::asio::streambuf& from, std::string::value_type delim, 
                        std::string::value_type ignore_char)
{
    // we assume that we would like to "break" and the boundries for the input and not the all input
    std::istream extractor(&from);
    std::string result;
    std::getline(extractor, result, delim);
    if (ignore_char != '\0' && !result.empty() && result[result.size()-1] == ignore_char) {  // we want to remove the last entry..
        result.erase(result.size()-1);
    }
    return result;
}

}   // end of local namespace

network_read::network_read(network_read&& other)  
{
        std::swap(*this, other);
}

network_read& network_read::operator = (network_read&& other) 
{
        if (&other != this) {
            std::swap(*this, other);
        }
        return *this;
}

void network_read::sanity_check(connection& c)
{
    sanity(c);
}

void network_read::sanity_check(shared_connection& c)
{
    sanity(c);
}

void network_read::sanity_check(secure_connection& c)
{
    sanity(c);
}
void network_read::sanity_check(secure_shared_connection& c)
{
    sanity(c);
}

std::string string_cast(network_read& from)
{
    return string_cast(from.buffer);
}

std::string string_cast(network_read& from, std::size_t max)
{
    return string_cast(from.buffer, max);
}

std::string string_cast(network_read& from, std::string::value_type delim, std::string::value_type ignore_char)
{
    return string_cast(from.buffer, delim, ignore_char);
}

std::string sub_string(network_read& from, std::string::value_type delim, 
                        std::string::value_type ignore_char)
{
    return string_cast(from, delim, ignore_char);
}


}   // end of namespace http

