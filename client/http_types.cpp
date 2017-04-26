#include "http_types.h"

namespace http
{   


remote_host::remote_host(std::string h) : name(std::move(h))
{
}

const std::string& remote_host::get() const
{
    return name;
}

remote_host operator "" _hn(const char* s)
{
    return remote_host(std::string(s));
}

remote_host operator "" _hn(const char* s, std::size_t sz)
{
    return remote_host(std::string(s, sz));
}


service::service(std::string s) : value(std::move(s))
{
}

service::service(std::uint16_t p) : value(std::to_string(p))
{
}

const std::string& service::get() const
{
    return value;
}
service operator "" _srv(const char* s)
{
    return service(std::string(s));
}
service operator "" _srv(const char* s, std::size_t sz)
{
    return service(std::string(s, sz));
}

service operator "" _port(unsigned long long port)
{
    return service(port);
}


}   // end of namespace http
