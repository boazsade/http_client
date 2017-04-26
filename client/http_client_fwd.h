#pragma once

#include "client/http_stream.h"
#include "client/http_networking.h"
#include "libs/http_base/http_response.h"
#include "client/http_send.h"


namespace http
{
// this file is the "right include file, it would include what this user needs
// in order to do the low level operation on any type of client

class client;
class shared_client;
class secure_client;
class shared_secure_client;
struct host_address;
// start the trasaction  action 
bool open(client& c, const host_address& adr);
bool open(shared_client& c, const host_address& adr);
bool open(secure_client& c, const host_address& adr);
bool open(shared_secure_client& c, const host_address& adr);
// read the all replay
response read(client& ); 
response read(shared_client&);
response read(secure_client&);
response read(shared_secure_client&);


}   // end of namespace http


