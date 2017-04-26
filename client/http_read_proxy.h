#pragma once

#include "client/http_read.h"

namespace http
{

template<typename CT>
struct network_read_proxy
{
    using connection_type = CT;

    network_read_proxy() = delete;

    static network_read_proxy<CT> make_proxy(CT& conn)
    {
        return network_read_proxy<CT>(conn);
    }

private:
    network_read_proxy(connection_type& ct) : conn(ct)
    {
    }

public:

    network_read_proxy(network_read_proxy&&) = default;
    network_read_proxy(const network_read_proxy&) = default;
    network_read_proxy& operator = (const network_read_proxy&) = default;
    network_read_proxy& operator = (network_read_proxy&&) = default;

    std::size_t read_all() 
    {
        return reader.read_all(conn);
    }

    template<typename Term>
    std::size_t read_until(Term terminator)
    {
        return reader.read_until(conn, terminator);
    }

    std::size_t read(const std::string& sstr)
    {
        return reader.read_until(conn, sstr);
    }

    std::size_t read(std::string::value_type ch)
    {
        return reader.read_until(conn, ch);
    }

    std::size_t read_some(std::size_t min_len)
    {
        return reader.read_some(conn, min_len);
    }

    std::size_t read(std::size_t sz = 0)
    {
        return reader.read(conn, sz);
    }

    network_read& read_data()
    {
        return reader;
    }   
    void commit() {}    // no op
private:
    connection_type& conn;
    network_read     reader;
};

template<typename T> inline 
std::string string_cast(network_read_proxy<T>& from)
{
    return string_cast(from.read_data());
}

template<typename T> inline 
std::string string_cast(network_read_proxy<T>& from, std::string::value_type d, std::string::value_type igr)
{
    return string_cast(from.read_data(), d, igr);
}

template<typename T> inline 
std::string string_cast(network_read_proxy<T>& from, std::size_t s)
{
    return string_cast(from.read_data(), s);
}


}   // end of namespace http

