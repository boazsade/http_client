#include "http_base/http_body.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/iostreams/filtering_streambuf.hpp>
//#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>


namespace http
{

body::body(const std::string& b) : data(b)
{
}

body& body::operator = (const std::string& s)
{
    data = s;
    return *this;
}

std::string::value_type* body::start()
{
    return empty() ? nullptr : &(data[0]);
}

const std::string::value_type* body::start() const
{
    return empty() ? nullptr : &(data[0]);
}

bool inflate(body& payload)
{
    if (!payload.empty()) {
        std::vector<char> result;
        if (unzip(result, (const std::uint8_t*)payload.start(), payload.size())) {
            payload = std::string{result.begin(), result.end()};
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

}   // end of namespace http

