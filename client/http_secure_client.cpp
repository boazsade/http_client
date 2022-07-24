#include "http_secure_client.h"
//#include "http_client_shared.hpp"
#include "http_stream.h"
#include "http_networking.h"
#include "http_networking.h"
#include "http_send.h"
#include "http_base/http_msg_consume.h"
#include <string>


namespace http
{

namespace 
{

// security_args& default_sec_args()   // note that this is not for use, it is just to create fake object
// {
//     static security_args dummy_args = security_args{};
//     return dummy_args;
// }

}   // end of local namespace 

///////////////////////////////////////////////////////////////////////////////
//
secure_client::secure_client(boost::asio::io_service& ios, security_args& initiator) : conn(ios, initiator)
{
}


bool secure_client::open(const host_address& adr)
{
    return http::open(conn, adr);
}

bool secure_client::send(const get& request)
{
    return http::send(conn, request);
}

bool secure_client::send(const post& request)
{
    return http::send(conn, request);
}

bool secure_client::send(const put&  request)
{
    return http::send(conn, request);
}

response secure_client::read()
{
    return http::get_response(network_read_proxy<connection_type>::make_proxy(conn));
}

bool secure_client::is_open() const
{
    return conn.is_open();
}

network_read_proxy<secure_client::connection_type> secure_client::start_input()
{
    return network_read_proxy<connection_type>::make_proxy(conn);
}

bool open(secure_client& c, const host_address& adr)
{
    return c.open(adr);
}
response read(secure_client& clt)
{
    return clt.read();
}

///////////////////////////////////////////////////////////////////////////////
//

shared_secured_client::shared_secured_client(boost::asio::io_service& ios, security_args& initiator) : conn(ios, initiator)
{
}

bool shared_secured_client::open(const host_address& adr)
{
    return http::open(conn, adr);
}

bool shared_secured_client::send(const get& request)
{
    return http::send(conn, request);
}

bool shared_secured_client::send(const post& request)
{
    return http::send(conn, request);
}

bool shared_secured_client::send(const put&  request)
{
    return http::send(conn, request);
}

response shared_secured_client::read()
{
    auto reader{start_input()};
    return http::get_response(network_read_proxy<connection_type>::make_proxy(conn));
}

bool shared_secured_client::is_open() const
{
    return conn.is_open();
}

network_read_proxy<shared_secured_client::connection_type> shared_secured_client::start_input()
{
    return network_read_proxy<connection_type>::make_proxy(conn);
    //return network_read_proxy<shared_secured_client>{conn};
}

bool open(shared_secured_client& c, const host_address& adr)
{
    return c.open(adr);
}
response read(shared_secured_client& clt)
{
    return clt.read();
}

}   // end of namespace http

