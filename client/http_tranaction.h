#pragma once
// this would allow the use of sending a single request while building it
// then retriving the reply all in a single line of code
// note that an exception is the only way to communicate
// any errors that happen in the process. also note that this
// is a sync operation, you would block until you have reply 
// from the server
#include "client/http_client.h"
#include "client/http_stream.h"
#include "client/http_networking.h"
#include "client/http_types.h"
#include "client/http_secure_client.h"
#include "http_base/http_error.h"
#include "client/http_read_proxy.h"
#include "http_base/http_response.h"
#include <string>


namespace http
{

    // for regular client
namespace details {

status_line get_status_line(network_read_proxy<connection>& with);
headers_list read_all_headers(network_read_proxy<connection>& with);
body read_message_body(network_read_proxy<connection>& with, const status_line& sl, const headers_list& hl);

status_line get_status_line(network_read_proxy<shared_connection>& with);
headers_list read_all_headers(network_read_proxy<shared_connection>& with);
body read_message_body(network_read_proxy<shared_connection>& with, const status_line& sl, const headers_list& hl);


status_line get_status_line(network_read_proxy<secure_connection>& with);
headers_list read_all_headers(network_read_proxy<secure_connection>& with);
body read_message_body(network_read_proxy<secure_connection>& with, const status_line& sl, const headers_list& hl);

status_line get_status_line(network_read_proxy<secure_shared_connection>& with);
headers_list read_all_headers(network_read_proxy<secure_shared_connection>& with);
body read_message_body(network_read_proxy<secure_shared_connection>& with, const status_line& sl, const headers_list& hl);

}   // end of namespace details

///////////////////////////////////////////////////////////////////////////////
//
struct read_status_line{};  // placeholder
constexpr read_status_line readsl = read_status_line{};
struct read_msg_headers{};  // placeholder
constexpr read_msg_headers readhdr = read_msg_headers{};
struct read_msg_body{}; // place holder
constexpr read_msg_body readb = read_msg_body{};
struct read_full_message{}; // placeholder
constexpr read_full_message readall = read_full_message{};

template<typename CT>
struct response_istream
{
    using client_type = CT;
    using connection_type = typename client_type::connection_type;

    response_istream(client_type* ct, bool s) : state(s), clt(ct), reader(clt->start_input())
    {
    }

    response_istream& operator >>(read_status_line)
    {
        if (state) {
            result = details::get_status_line(reader);
        }  else {
            throw network_error("failed to read status line");
        }
        return *this;
    }

    response_istream& operator >>(read_msg_headers)
    {
        if (state && result.get_status()) {
            result = details::read_all_headers(reader);
        } else {
            state = false;
        }
        return *this;
    }

    response_istream& operator >> (read_msg_body)
    {
        if (state && result.get_status() && !result.the_headers().empty()) {
            result += details::read_message_body(reader, result.get_status(), result.the_headers());
        } else {
            state = false;
        }
        return *this;
    }

    response_istream& operator >> (read_full_message)
    {
        if (state && result.the_headers().empty() && result.payload().empty()) {
            return *this >> readsl >> readhdr >> readb;
        } 
        return *this;   // we would not allow to re-read, the message was consumed
    }

    const response& answer() const
    {
        return result;
    }

    const status_line& status() const
    {
        return result.get_status();
    }

    const headers_list& the_headers() const
    {
        return result.the_headers();
    }

    const body& msg_body() const
    {
        return result.payload();
    }

    bool good() const
    {
        return state;
    }

    bool bad() const
    {
        return !good();
    }

private:
    bool state{false};
    client_type* clt{nullptr};
    response result;
    network_read_proxy<connection_type> reader;
};

template<typename CT>
struct make_request
{
    using client_type = CT;
    using result_type = response_istream<client_type>; 

    make_request(client_type* c, bool s) : clt(c), state(s)
    {
        if (!clt) {
            throw std::runtime_error("invalid client handle - cannot be null!!");
        }
    }

    result_type operator () (const get& req)
    {
        return send(req);
    }

    result_type send(const get& req)
    {
        return do_it(req, "get");
    }

    result_type operator () (const post& req)
    {
        return send(req);
    }

    result_type send(const post& req)
    {
        return do_it(req, "post");
    }

    result_type operator () (const put& req)
    {
        return send(req);
    }

    result_type send(const put& req)
    {
        return this->do_it(req, "put");
    }

private:

    template<typename T>
    result_type do_it(const T& req, const char* em)
    {
        if (state) {
            state =  http::send(*clt, req);
            if (!state) {
                throw network_error(std::string("failed to send ") + em + "request");
            }
        } else {
            throw network_error(std::string("make request error - invalid state, would not send" )+ em);
        }   
        return response_istream<client_type>(clt, state);
    }

    client_type* clt;
    bool state;
};

template<typename CT>
struct connect
{
    using client_type = CT;
    using result_type = make_request<CT>;
    explicit connect(client_type* ct) : clt(ct)
    {
        if (!clt) {
            throw std::runtime_error("pass invalid client handle - cannot pass null!");
        }
    }

    result_type operator () (const host_address& to)
    {
        bool state = open(*clt, to);
        if (!state) {
            throw network_error("connection error - would not be able to send requests");
        }
        return make_request<client_type>(clt, state);
    }

private:
    client_type* clt;
};
/// the most usefull typedefs
using regular_connect = connect<client>;
using secure_connect = connect<secure_client>;

using regular_request = make_request<client>;
using secure_request = make_request<secure_client>;

using res_istream = response_istream<client>;
using secure_istream = response_istream<secure_client>;
}   // end of namespace http

// and this even more useful from the outside
using http_connect = http::regular_connect;
using https_connect = http::secure_connect;
using http_request = http::regular_request;
using https_request = http::secure_request;
using http_istream = http::res_istream;
using https_istream = http::secure_istream;

