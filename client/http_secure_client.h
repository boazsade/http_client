#pragma once
//#include "libs/http_client/http_types.h"
#include "http_base/http_defines.h"
#include "client/https_connection.h"
#include "client/http_client_fwd.h"
#include "client/http_secure_args.h"
#include "client/http_stream.h"
#include "client/http_read_proxy.h"
#include "http_base/http_response.h"
#include "http_base/http_request.h"
#include <cstdint>
#include <boost/asio.hpp>


namespace http
{

// implements support for http client over ssl
class secure_client : public secure_defines
{
public:

    using connection_type = secure_connection;

    secure_client(boost::asio::io_service& ios, security_args& initiator);

    secure_client(secure_client&& other);
    secure_client& operator = (secure_client&& other);

    bool open(const host_address& server);

    bool send(const get& get_request);
    bool send(const post& post_request);
    bool send(const put&  put_request);

	// call this function to get the payload from the server. return false on 
	// error - either failed to connect (in the send function above) or that
	// we failed to read from the server - if this function is successfull it
	// would allow the user to use the function get_replay to get any payload
    response read();

    bool is_open() const;

    network_read_proxy<connection_type> start_input();
private:
    connection_type  conn;
};  

class shared_secured_client  : public secure_defines
{
public:
    using connection_type = secure_shared_connection;

    shared_secured_client() = default;
    shared_secured_client(shared_secured_client&&) = default;
    shared_secured_client(const shared_secured_client&) = default;
    shared_secured_client& operator = (shared_secured_client&&) = default;
    shared_secured_client& operator = (const shared_secured_client&) = default;
    shared_secured_client(boost::asio::io_service& ios,  security_args& initiator);

    bool open(const host_address& server);

    bool send(const get& get_request);
    bool send(const post& post_request);
    bool send(const put&  put_request);

	// call this function to get the payload from the server. return false on 
	// error - either failed to connect (in the send function above) or that
	// we failed to read from the server - if this function is successfull it
	// would allow the user to use the function get_replay to get any payload
    response read();

    bool is_open() const;
    network_read_proxy<connection_type> start_input();

private:
    connection_type conn;
};

}   // end of namespace http

