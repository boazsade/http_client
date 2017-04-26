#include "http_context.h"
#include "http_networking.h"

http::make_request<http_c::client_type> operator | (http::connect<http_c::client_type> connection, http::host_address adr)
{
        return connection(adr);
}

http::make_request<http_c::client_type>::result_type 
     operator | (http::make_request<http_c::client_type> clt, const http::get& r)
{
        return clt.send(r);
}

http::make_request<http_c::client_type>::result_type 
    operator | (http::make_request<http_c::client_type> clt, const http::put& r)
{
        return clt.send(r);
}

http::make_request<http_c::client_type>::result_type 
    operator | (http::make_request<http_c::client_type> clt, const http::post& r)
{
        return clt.send(r);
}

///////////////////////////////////////////////////////////////////////////////
http::make_request<https_c::client_type> operator | (http::connect<https_c::client_type> connection, http::host_address adr)
{
        return connection(adr);
}

http::make_request<https_c::client_type>::result_type 
     operator | (http::make_request<https_c::client_type> clt, const http::get& r)
{
        return clt.send(r);
}

http::make_request<https_c::client_type>::result_type 
    operator | (http::make_request<https_c::client_type> clt, const http::put& r)
{
        return clt.send(r);
}

http::make_request<https_c::client_type>::result_type 
    operator | (http::make_request<https_c::client_type> clt, const http::post& r)
{
        return clt.send(r);
}

