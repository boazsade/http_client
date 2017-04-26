#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

    
using http_socket_t = boost::asio::ip::tcp::socket; 
using https_socket_t= boost::asio::ssl::stream<http_socket_t>;

namespace http
{

struct remote_host
{
    remote_host() = default;
    remote_host(std::string h);

    remote_host(const remote_host&) = default;
    remote_host(remote_host&&) = default;
    remote_host& operator = (const remote_host&) = default;
    remote_host& operator = (remote_host&&) = default;

    const std::string& get() const;

private:
    std::string name;
};

remote_host operator "" _hn(const char* s);
remote_host operator "" _hn(const char* s, std::size_t sz);

struct service
{
    service() = default;
    service(std::string s);
    service(std::uint16_t p);   // init with port
    service(service&&) = default;
    service(const service&) = default;
    service& operator = (service&&) = default;
    service& operator = (const service&) = default;

    const std::string& get() const;

private:
    std::string value;
};

service operator "" _srv(const char* s);
service operator "" _srv(const char* s, std::size_t sz);
service operator "" _port(unsigned long long port);
}   // end of namespace http


