#pragma once
#include <string>
#include <iterator>
#include <algorithm>


namespace http
{

// the client can send a body in the message
struct body
{
    body() = default;
    body(body&&) = default;
    body(const body&) = default;
    body& operator = (const body&) = default;
    body& operator = (body&) = default;

    explicit body(const std::string& b);

    body& operator = (const std::string& b);

    template<typename It>
    void assign(It from, It to)
    {
        std::copy(from, to, std::back_inserter(data));
    }

    const std::string& value() const;

    std::string::size_type size() const;

    bool empty() const;

    const std::string::value_type* start() const;
    std::string::value_type* start();
    // would uncompress the body, don't call this function if it was not compress!!
    // note that we do  pass it by reference, since we are changing it
    friend bool inflate(body& payload); 
private:
    std::string data;
};

}	// end of namespace http

