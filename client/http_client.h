#pragma once
#include "http_base/http_defines.h"
#include "client/http_connection.h"
#include "client/http_networking.h"
#include "client/http_types.h"
#include "client/http_client_fwd.h"
#include "client/http_read_proxy.h"
#include "client/http_stream.h"
#include "http_base/http_response.h"
#include "http_base/http_request.h"
#include <boost/asio.hpp>
#include <cstdint>


namespace http 
{

class client : public normal_defines
{
public:
    using connection_type = connection; 

    explicit client(boost::asio::io_service& io_service);
    
    client(client&& other) = default;
    client& operator = (client&&) = default;

    client(const client& ) = default;

    bool open(const host_address& server);

    bool send(const get& get_request);
    bool send(const post& post_request);
    bool send(const put&  put_request);

	// call this function to get the payload from the server. return false on 
	// error - either failed to connect (in the send function above) or that
	// we failed to read from the server - if this function is successfull it
	// would allow the user to use the function get_replay to get any payload
    http::response read();

    bool is_open() const;

    network_read_proxy<connection_type> start_input();
private:
    connection conn;
};

class shared_client : public normal_defines
{
public:
    using connection_type = shared_connection; 
    shared_client() = default;
    shared_client(shared_client&&) = default;
    shared_client(const shared_client&) = default;
    shared_client& operator = (shared_client&&) = default;
    shared_client& operator = (const shared_client&) = default;

    explicit shared_client(boost::asio::io_service& io_service);

    bool open(const host_address& server);

    bool send(const get& get_request);
    bool send(const post& post_request);
    bool send(const put&  put_request);

	// call this function to get the payload from the server. return false on 
	// error - either failed to connect (in the send function above) or that
	// we failed to read from the server - if this function is successfull it
	// would allow the user to use the function get_replay to get any payload
    http::response read();

    bool is_open() const;

    network_read_proxy<connection_type> start_input();
private:
    shared_connection conn;
};

}   // end pf namespace http

