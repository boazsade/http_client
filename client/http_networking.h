#pragma once

#include "http_connection.h"
#include "https_connection.h"
#include "http_types.h"
#include <iosfwd>

namespace http
{

struct host_address
{
    host_address(remote_host h, service s);
    host_address() = default;
    host_address(host_address&&) = default;
    host_address(const host_address&) = default;
    host_address& operator = (host_address&&) = default;
    host_address& operator = (const host_address&) = default;

    host_address& operator /= (service s);
    host_address& operator /= (remote_host server);

    const remote_host& server() const;
    const service& service_id() const;
protected:
    service                 tcp_serv;
    remote_host             host;
};

host_address& operator / (host_address& ts, service s);
host_address& operator / (host_address& ts, remote_host s);

host_address operator / (remote_host svr, service srv);

bool open(connection& c, const host_address& to);               // normal http
bool open(secure_connection& c, const host_address& to);        // https
bool open(shared_connection& c, const host_address& to);        // http
bool open(secure_shared_connection& c, const host_address& to); // https

// open connection without blocking - we can have to options
// one is to create a future that can run later in other thread (deered)
// or though the use of coroutine
struct defered_it {};
struct async_connect {};
constexpr defered_it defered = defered_it{};
constexpr async_connect async = async_connect{};

bool open(connection& c, const host_address& to, defered_it);
bool open(connection& c, const host_address& to, async_connect);

std::ostream& operator << (std::ostream& os, const host_address& ha);

}   // end of namespace http

