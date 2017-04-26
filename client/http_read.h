#pragma once
#include "http_base/http_error.h"
#include <boost/asio.hpp>
#include <string>
#include <utility>

// this would allow for reading and writing to the remote host
// would would start this object with a object of type connection (either secured or not, and either shared or not)
// then the user can use this to 
//
namespace http
{

struct connection;
struct shared_connection;
struct secure_connection;
struct secure_shared_connection;

struct network_read
{

    network_read() = default;
    network_read(network_read&& other);

    network_read(const network_read&) = default;

    network_read& operator = (const  network_read&) = default;

   network_read& operator = (network_read&& other);

    template<typename CT>
    std::size_t read_all(CT& conn)  // would throw on error
    {
        sanity_check(conn);
        boost::system::error_code error;
        auto r = boost::asio::read(*conn.get(), buffer, boost::asio::transfer_all(), error);
        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }
        return r;
    }

    template<typename CT, typename Term>
    std::size_t read_until(CT& conn, Term terminator)
    {
        sanity_check(conn);
        boost::system::error_code error;
        auto r =  boost::asio::read_until(*conn.get(), buffer, terminator, error);
        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }
        return r;
    }

    template<typename CT>
    std::size_t read_some(CT& conn, std::size_t min_len)
    {
        sanity_check(conn);
        boost::system::error_code error;
        std::size_t n = boost::asio::read(*conn.get(), buffer, boost::asio::transfer_at_least(min_len), error);
        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }
        return n;
    }

    template<typename CT>
    std::size_t read(CT& conn, std::size_t sz = 0)
    {
        sanity_check(conn);
        if (sz == 0) {
            return read_all(conn);
        } else {
            boost::system::error_code error;
            std::size_t n = boost::asio::read(*conn.get(), buffer, boost::asio::transfer_exactly(sz), error);
            if (error && error != boost::asio::error::eof) {
                throw boost::system::system_error(error);
            }
            return n;
        }
    }

    friend std::string string_cast(network_read& from);

    friend std::string string_cast(network_read& from, std::size_t max);

    friend std::string string_cast(network_read& from, std::string::value_type delim, std::string::value_type ignore_char);

    friend std::string sub_string(network_read& from, std::string::value_type delim, std::string::value_type ignore_char);

private:
    static void sanity_check(connection& c);
    static void sanity_check(shared_connection& c);
    static void sanity_check(secure_connection& c);
    static void sanity_check(secure_shared_connection& c);

    template<typename CT>
    static void sanity(CT& conn)
    {
        if (!conn.is_open()) {  // we must initiate this with open connection!!
            throw network_error("you cannot initiate network_read with invalid connection!!");
        }
    }

    boost::asio::streambuf buffer;  // this would be used internally so that we could consume the input
};


}   // end of namespace http

