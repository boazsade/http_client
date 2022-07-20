#pragma once
#include "libs/http_client/http_types.h"
#include "libs/http_client/http_secure_args.h"
#include <boost/asio.hpp>
#include <memory> 


namespace http
{

struct sec_connection_base
{
    using socket_type = https_socket_t;

    sec_connection_base() = default;

    explicit sec_connection_base(int vm);

    void init(int sm);

    int security_method() const;
private:
    int sec_method = 0; 
};

// this class would only deal with the connection aspects of the http stream
// you can use it to build other features, such as the object that would 
// send data or read data or both (a full client)
class secure_connection : public sec_connection_base
{
public:
    secure_connection(boost::asio::io_service& ios, security_args& sa);    

    void init(boost::asio::io_service&, security_args&);    // do nothing operation..

    socket_type* get();

    boost::asio::io_service* service_handle();

    bool is_open() const;

    socket_type::lowest_layer_type* stream_handle();
private:
    socket_type socket_;
};

// this is the same as the above, only in this case we are allowing to copy 
// connection object so that we can use it to pass around,
// note that the constructure allow for empty case as well
class secure_shared_connection : public sec_connection_base
{
public:
    secure_shared_connection() = default;
    secure_shared_connection(secure_shared_connection&&) = default;
    secure_shared_connection(const secure_shared_connection&) = default;
    secure_shared_connection& operator = (const secure_shared_connection&) = default;
    secure_shared_connection& operator = (secure_shared_connection&&) = default;

    explicit secure_shared_connection(boost::asio::io_service& ios, security_args& sa);    

    void init(boost::asio::io_service& ios, security_args& sa);

    socket_type* get();
    const socket_type* get() const;

    boost::asio::io_service* service_handle();

    bool is_open() const;

    socket_type::lowest_layer_type* stream_handle();
private:
    std::unique_ptr<socket_type> socket_;
};

}	// end of namespace http

