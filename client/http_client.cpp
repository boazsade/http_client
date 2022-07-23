#include "http_client.h"
#include "http_stream.h"
#include "http_networking.h"
#include "http_base/http_error.h"
#include "http_base/http_msg_consume.h"
#include "http_send.h"
#include "http_base/http_response.h"
#include "http_read_proxy.h"
//#include "http_client_shared.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <iterator>
#include <stdlib.h>


namespace http
{

client::client(boost::asio::io_service& io_service)
    				: conn(io_service)
{
}

// client::client(client&& other) : conn(*other.conn.service_handle())
// {
//     std::swap(*this, other);
// }

// client& client::operator = (client&& other)
// {
//     if (this != &other) {
//         std::swap(*this, other);
//     }
//     return *this;
// }
bool  client::open(const host_address& server)
{
    return http::open(conn, server);
}

bool client::send(const get& get_request)
{
    return http::send(conn, get_request);
}

bool client::send(const post& post_request)
{
    return http::send(conn, post_request);
}

bool client::send(const put& put_request)
{
    return http::send(conn, put_request);
}

http::response client::read()
{
    return get_response(network_read_proxy<connection_type>::make_proxy(conn));
}

bool client::is_open() const
{
    return conn.is_open();
}

network_read_proxy<client::connection_type> client::start_input()
{
    return network_read_proxy<connection_type>::make_proxy(conn);
}

bool open(client& clt, const host_address& adr)
{
    return clt.open(adr);
}

response read(client& clt)
{
    return clt.read();
}

///////////////////////////////////////////////////////////////////////////////

shared_client::shared_client(boost::asio::io_service& ios) : conn(ios)
{
}


bool  shared_client::open(const host_address& server)
{
    return http::open(conn, server);
}

bool shared_client::send(const http::get& get_request)
{
    return http::send(conn, get_request);
}

bool shared_client::send(const post& post_request)
{
    return http::send(conn, post_request);
}

bool shared_client::send(const put& put_request)
{
    return http::send(conn, put_request);
}

http::response shared_client::read()
{
    return get_response(network_read_proxy<connection_type>::make_proxy(conn));
}

bool shared_client::is_open() const
{
    return conn.is_open();
}

network_read_proxy<shared_client::connection_type> shared_client::start_input()
{
    return network_read_proxy<connection_type>::make_proxy(conn);
}
bool open(shared_client& clt, const host_address& adr)
{
    return clt.open(adr);
}

response read(shared_client& clt)
{
    return clt.read();
}


}   // end of namespace http
