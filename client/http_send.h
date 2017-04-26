#pragma once

#include "client/http_stream.h"


namespace http
{

struct connection;
struct shared_connection;
struct secure_shared_connection;
struct secure_connection;

class client;
class shared_client;
class secure_client;
class shared_secured_client;
    // do post
bool send(connection& c, const post& to);               // normal http
bool send(secure_connection& c, const post& to);        // https
bool send(shared_connection& c, const post& to);        // http
bool send(secure_shared_connection& c, const post& to); // https
    //  do get 
bool send(connection& c, const get& to);               // normal http
bool send(secure_connection& c, const get& to);        // https
bool send(shared_connection& c, const get& to);        // http
bool send(secure_shared_connection& c, const get& to); // https
    // do put
bool send(connection& c, const put& to);               // normal http
bool send(secure_connection& c, const put& to);        // https
bool send(shared_connection& c, const put& to);        // http
bool send(secure_shared_connection& c, const put& to); // https
//////////////////////////////////////////////////////////////////////////////
//
    // do post
bool send(client& c, const post& to);               // normal http
bool send(shared_client& c, const post& to);        // https
bool send(secure_client& c, const post& to);        // http
bool send(shared_secured_client& c, const post& to); // https
    //  do get 
bool send(client& c, const get& to);               // normal http
bool send(shared_client& c, const get& to);        // https
bool send(secure_client& c, const get& to);        // http
bool send(shared_secured_client& c, const get& to); // https
    // do put
bool send(client& c, const put& to);               // normal http
bool send(shared_client& c, const put& to);        // https
bool send(secure_client& c, const put& to);        // http
bool send(shared_secured_client& c, const put& to); // https

}   // end of namespace http

