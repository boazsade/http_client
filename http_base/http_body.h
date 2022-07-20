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

    explicit body(const std::string& b);

    body& operator = (const std::string& b);

    template<typename It>
    void assign(It from, It to)
    {
        std::copy(from, to, std::back_inserter(data));
    }

    constexpr const std::string& value() const {
        return data;
    }

    std::string::size_type size() const {
        return value().size();
    }

    bool empty() const {
        return value().empty();
    }

    const std::string::value_type* start() const;
    std::string::value_type* start();
    // would uncompress the body, don't call this function if it was not compress!!
    // note that we do  pass it by reference, since we are changing it
    friend bool inflate(body& payload); 
private:
    std::string data;
};

}	// end of namespace http

