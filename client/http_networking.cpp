#include "http_networking.h"
//#include "http_client_shared.hpp"
#include <iostream>


namespace http
{

namespace
{

int verification_mode(int ac)
{
    if (ac == security_args::DONT_APPLY) {
        return boost::asio::ssl::verify_none;
    } else {
        int ret = 0;
        if (ac & security_args::CLIENT_ONCE) {
            ret |= boost::asio::ssl::verify_client_once;
        } 
        if (ac & security_args::PEER) {
            ret |= boost::asio::ssl::verify_peer;
        }
        if (ac & security_args::FAIL_CERT_ERR) {
            ret |= boost::asio::ssl::verify_fail_if_no_peer_cert;
        }
        return ret;
    }
}

auto make_connection(const auto& query, auto& stream, auto& resolver) -> bool {
    boost::system::error_code ec;
    boost::asio::connect(stream, resolver.resolve(query), ec);
    return ec.value() == 0;  // we want this to be false, since true means we have an error
}

template<typename C>
inline bool open_connection(C& conn, const host_address& to)
{
    if (!conn.is_open()) {
        if (conn.get()) { // we have a valid client
            boost::asio::ip::tcp::resolver resolver_(*conn.service_handle());
            boost::asio::ip::tcp::resolver::query query(to.server().get(), to.service_id().get());
            return make_connection(query, *conn.stream_handle(), resolver_);
        } else {
            return false;
        }
    }
    return true;
}

bool handshake(https_socket_t& with, const std::string& host_name, int verify_action)
{
    with.set_verify_mode(verification_mode(verify_action));
    with.set_verify_callback(boost::asio::ssl::rfc2818_verification(host_name));
    with.handshake(https_socket_t::client);
    return true;    // otherwise we would get some exception
}

}   // end of local namespace

host_address::host_address(remote_host h, service s) : tcp_serv(s), host(h)
{
}


host_address& host_address::operator /= (service s)
{
    tcp_serv = s;
    return *this;
}

host_address& host_address::operator /= (remote_host s)
{
    host = s;
    return *this;
}

const remote_host& host_address::server() const
{
    return host;
}

const service& host_address::service_id() const
{
    return tcp_serv;
}

host_address& operator / (host_address& ts, service s)
{
    return ts /= s;
}

host_address& operator / (host_address& ts, remote_host s)
{
    return ts /= s;
}

bool open(connection& c, const host_address& to)
{
    return open_connection(c, to);
}

bool open(secure_connection& c, const host_address& to)
{
    return open_connection(c, to) && handshake(*c.get(), to.server().get(), c.security_method());;
}

bool open(shared_connection& c, const host_address& to)
{
    return open_connection(c, to); 
}

bool open(secure_shared_connection& c, const host_address& to)
{
    return open_connection(c, to) && handshake(*c.get(), to.server().get(), c.security_method());
}

host_address operator / (remote_host svr, service srv)
{
    return host_address(svr, srv) ;
}

std::ostream& operator << (std::ostream& os, const host_address& ha)
{
    return os<<"//"<<ha.server().get()<<":"<<ha.service_id().get();
}

}   // end of namespace http

