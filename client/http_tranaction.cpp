#include "http_tranaction.h"
#include "http_client_shared.hpp"

namespace http
{

namespace details {

status_line get_status_line(network_read_proxy<connection>& with)
{
    return http::read_status(with);
}

headers_list read_all_headers(network_read_proxy<connection>& with)
{
    return http::read_headers(with);
}

body read_message_body(network_read_proxy<connection>& with, const status_line& sl, const headers_list& hl)
{
    return http::message_body(with, hl, sl);
}

status_line get_status_line(network_read_proxy<shared_connection>& with)
{
    return http::read_status(with);
}

headers_list read_all_headers(network_read_proxy<shared_connection>& with)
{
    return http::read_headers(with);
}

body read_message_body(network_read_proxy<shared_connection>& with, const status_line& sl, const headers_list& hl)
{
    return http::message_body(with, hl, sl);
}

status_line get_status_line(network_read_proxy<secure_connection>& with)
{
    return http::read_status(with);
}

headers_list read_all_headers(network_read_proxy<secure_connection>& with)
{
    return http::read_headers(with);
}

body read_message_body(network_read_proxy<secure_connection>& with, const status_line& sl, const headers_list& hl)
{
    return http::message_body(with, hl, sl);
}


status_line get_status_line(network_read_proxy<secure_shared_connection>& with)
{
    return http::read_status(with);
}

headers_list read_all_headers(network_read_proxy<secure_shared_connection>& with)
{
    return http::read_headers(with);
}

body read_message_body(network_read_proxy<secure_shared_connection>& with, const status_line& sl, const headers_list& hl)
{
    return http::message_body(with, hl, sl);
}

}   // end of namespace details

}   // end of namespace http
