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

auto unzip(auto msg) {
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_decompressor{});
    std::istringstream input{msg.value(), std::ios_base::in | std::ios_base::binary};
    in.push(input);
    std::ostringstream out;
    boost::iostreams::copy(in, out);
    return body{out.str()};
}


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
        payload = unzip(std::move(payload));
    }
    return not payload.empty();
}

}   // end of namespace http

