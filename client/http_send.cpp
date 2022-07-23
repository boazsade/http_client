#include "http_send.h"
//#include "http_client_shared.hpp"
#include "http_connection.h"
#include "http_client.h"
#include "http_secure_client.h"
#include "https_connection.h"


namespace http
{

namespace 
{

template<typename C, typename R>
bool send_message(C& connection, const R& request)
{
    if (connection.is_open()) {
        return send_request(*(connection.get()), request.str());
    } else {
        return false;
    }
}

}   // end of local namespace 

bool send(connection& c, const post& to)
{
    return send_message(c, to);
}

bool send(secure_connection& c, const post& to)        // https
{
    return send_message(c, to);
}

bool send(shared_connection& c, const post& to)        // http
{
    return send_message(c, to);
}

bool send(secure_shared_connection& c, const post& to) // https
{
    return send_message(c, to);
}

    //  do get 
bool send(connection& c, const get& to)               // normal http
{
    return send_message(c, to);
}

bool send(secure_connection& c, const get& to)        // https
{
    return send_message(c, to);
}

bool send(shared_connection& c, const get& to)        // http
{
    return send_message(c, to);
}

bool send(secure_shared_connection& c, const get& to) // https
{
    return send_message(c, to);
}

    // do put
bool send(connection& c, const put& to)               // normal http
{
    return send_message(c, to);
}

bool send(secure_connection& c, const put& to)        // https
{
    return send_message(c, to);
}

bool send(shared_connection& c, const put& to)
{
    return send_message(c, to);
}

bool send(secure_shared_connection& c, const put& to)
{
    return send_message(c, to);
}

bool send(client& c, const post& to)
{
    return c.send(to);
}

bool send(shared_client& c, const post& to)
{
    return c.send(to);
}
bool send(secure_client& c, const post& to)
{
    return c.send(to);
}
bool send(shared_secured_client& c, const post& to)
{
    return c.send(to);
}
    //  do get 
bool send(client& c, const get& to)
{
    return c.send(to);
}
    //  do get 
bool send(shared_client& c, const get& to)
{
    return c.send(to);
}
    //  do get 
bool send(secure_client& c, const get& to)
{
    return c.send(to);
}
    //  do get 
bool send(shared_secured_client& c, const get& to)
{
    return c.send(to);
}
    //  do get 
    // do put
bool send(client& c, const put& to)
{
    return c.send(to);
}
    //  do get 
bool send(shared_client& c, const put& to)
{
    return c.send(to);
}
    //  do get 
bool send(secure_client& c, const put& to)
{
    return c.send(to);
}
    //  do get 
bool send(shared_secured_client& c, const put& to)
{
    return c.send(to);
}
    //  do get 

}   // end of namespace http

