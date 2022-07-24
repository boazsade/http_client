#pragma once
#include "client/http_types.h"
#include <boost/asio.hpp>
#include <memory>
#include <optional>


namespace http
{
// this class would only deal with the connection aspects of the http stream
// you can use it to build other features, such as the object that would 
// send data or read data or both (a full client)
class connection 
{
public:
    using socket_type = http_socket_t;

    explicit connection(boost::asio::io_service& ios);    
    void init(boost::asio::io_service& ios);    // do nothing operation..

    socket_type* get();

    std::optional<socket_type::executor_type> service_handle() {
        return {socket_.get_executor()};
    }

    bool is_open() const;

    socket_type::lowest_layer_type* stream_handle();

private:
    socket_type socket_;
};

// this is the same as the above, only in this case we are allowing to copy 
// connection object so that we can use it to pass around,
// note that the constructure allow for empty case as well
class shared_connection
{
public:
    using socket_type = http_socket_t;

    shared_connection() = default;
    shared_connection(shared_connection&&) = default;
    shared_connection(const shared_connection&) = default;
    shared_connection& operator = (const shared_connection&) = default;
    shared_connection& operator = (shared_connection&&) = default;

    explicit shared_connection(boost::asio::io_service& ios);    

    void init(boost::asio::io_service& ios);

    socket_type* get();
    const socket_type* get() const;

    std::optional<socket_type::executor_type> service_handle() {
        return get() ? std::optional<socket_type::executor_type>{get()->get_executor()} : std::nullopt;
    }

    bool is_open() const;

    socket_type::lowest_layer_type* stream_handle();

private:
    std::unique_ptr<socket_type> socket_;
};

}	// end of namespace http
