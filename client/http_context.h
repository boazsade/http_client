#pragma once
#include "client/http_client.h"
#include "client/http_secure_client.h"
#include "client/http_secure_args.h"
#include "client/http_tranaction.h"
#include <boost/asio.hpp>


// use this for the initalization of the http client
template<typename CT>
struct http_context
{
    using client_type = CT;

    http_context() = default;

    client_type make_client()
    {
        return client_type{bsrv};
    }

private:
    boost::asio::io_service bsrv;
};

using http_c = http_context<http::client>;
using http_cp = http_context<http::shared_client>;

template<typename CT>
struct https_context
{
    using client_type = CT;

    https_context(http::security_args args) : sec_args(std::move(args))
    {
    }

    client_type make_client()
    {
        return client_type{bsrv, sec_args};
    }

private:
    boost::asio::io_service bsrv;
    http::security_args sec_args;
};

using https_c = https_context<http::secure_client>;
using https_cp = http_context<http::shared_secured_client>;

template<typename CtxType>
struct client_context
{
    using client_type = typename CtxType::client_type;
    using context_type = CtxType;

    client_context(context_type& ctx) : the_client(ctx.make_client())
    {
    }

    operator client_type& () 
    {
        return the_client;
    }

    client_type* get() 
    {
        return &the_client;
    }

private:
    client_type the_client;
};

struct http_start_tranaction {}; // placeholder
struct http_make_connect_obj{};
constexpr http_start_tranaction http_start = http_start_tranaction{};
constexpr http_make_connect_obj connect_http = http_make_connect_obj{};

inline ups::http_c::client_type operator | (ups::http_c& ctx, const http_start_tranaction)
{
    return ctx.make_client();
}

template<typename CX> inline 
ups::http::connect<typename ups::client_context<CX>::client_type> 
    operator | (ups::client_context<CX>& cx, const http_make_connect_obj)
{
    using client_t = typename ups::client_context<CX>::client_type;
    using result_t = ups::http::connect<client_t>;
    return result_t{cx.get()};
}

//template<typename CX> inline 
//ups::http::make_request<typename ups::client_context<CX>::client_type> 
//     operator | (ups::http::connect<typename ups::client_context<CX>::client_type> connection, ups::http::host_address adr)
ups::http::make_request<ups::http_c::client_type> operator | (ups::http::connect<ups::http_c::client_type> connection, ups::http::host_address adr);
ups::http::make_request<ups::https_c::client_type> operator | (ups::http::connect<ups::https_c::client_type> connection, ups::http::host_address adr);

ups::http::make_request<ups::http_c::client_type>::result_type 
     operator | (ups::http::make_request<ups::http_c::client_type> clt, const ups::http::get& r);
ups::http::make_request<ups::https_c::client_type>::result_type 
     operator | (ups::http::make_request<ups::https_c::client_type> clt, const ups::http::get& r);

ups::http::make_request<ups::https_c::client_type>::result_type 
    operator | (ups::http::make_request<ups::https_c::client_type> clt, const ups::http::put& r);
ups::http::make_request<ups::http_c::client_type>::result_type 
    operator | (ups::http::make_request<ups::http_c::client_type> clt, const ups::http::put& r);

ups::http::make_request<ups::http_c::client_type>::result_type 
    operator | (ups::http::make_request<ups::http_c::client_type> clt, const ups::http::post& r);
ups::http::make_request<ups::https_c::client_type>::result_type 
    operator | (ups::http::make_request<ups::https_c::client_type> clt, const ups::http::post& r);

