#include "client/http_client.h"
#include "client/http_stream.h"
#include "http_base/http_error.h"
#include "http_client/http_tranaction.h"
#include "client/http_context.h"
#include "http_base/http_body.h"
#include "http_base/http_header.h"
#include "http_base/http_request.h"
#include "http_base/http_response.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <thread>


void build_request(http_get& with, const char* resource, const std::string& host, const char*)
{
    using namespace http::literals;
        with<<http::resource{resource}<<http::v1_1
            <<("User-Agent"_hdrk + "curl/7.29.0"_hdrv)
            <<("Host"_hdrk + http::header_value(host));
    std::cout<<"message from client "<<with.str()<<std::endl;
            
}

void build_request(http_post& with, const char* resource, const std::string& host, const char* b)
{
    using namespace std::string_literals;
    using namespace http::literals;
        with<<http::resource{resource}<<http::v1_1
            <<("User-Agent"_hdrk + "curl/7.29.0"_hdrv)
            <<("Host"_hdrk + http::header_value(host))
            <<http::body_entry(http::body(b), "application/json"s);
}

void build_request(http_put& with, const char* resource, const std::string& host, const char* b)
{
    using namespace std::string_literals;
    using namespace http::literals;
        with<<http::resource{resource}<<http::v1_1
            <<("User-Agent"_hdrk + "curl/7.29.0"_hdrv)
            <<("Host"_hdrk + http::header_value(host))
            <<http::body_entry(http::body(b), "application/json"s);
}

template<typename Method, typename CTX>
bool send_message(const char* server, const char* service, const char* resource, const char* body, CTX& transaction_context)
{
    Method method;
    try {
        std::string hname = server;
        if (isdigit(*service)) {
            hname += std::string(":") + service;
        }
        build_request(method, resource, hname, body);
        std::cout<<"sending to server messgae\n"<<method.str()<<"\n";
        client_context<CTX> the_client(transaction_context);
        http_istream is = the_client | connect_http | http::remote_host(server) / http::service(service) | method;
    
        is>>http::readsl>>http::readhdr>>http::readb;
        if (is.good()) {
            std::cout<<"successfully read all input from the server: "<<is.status()<<std::endl;
            std::cout<<"server replay is \n---------------------------------\n";
            std::cout<<is.answer();
            std::cout<<"--------------------------\n";
            /*while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }*/
        } else {
            std::cout<<"some errors while trying to read from the server\n";
            std::cout<<"status line: "<<is.status()<<std::endl;
            std::cout<<"headers:\n"<<is.the_headers()<<std::endl;
        }
    } catch (const http::network_error& e) {
        std::cerr<<"http error -  "<<e.what()<<std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr<<"error: "<<e.what()<<std::endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cerr<<"usage: "<<argv[0]<<" <server ip> <server port> <resouce path> [\"bodyfor post\"]\n";
        return -1;
    }

    http_c clients_context;

    if (argc == 4) {
        std::cout<<"running get request "<<argv[1]<<"@"<<argv[2]<<" resource '"<<argv[3]<<"'"<<std::endl;
        send_message<http_get>(argv[1], argv[2], argv[3], nullptr, clients_context);
    } else {
        send_message<http_put>(argv[1], argv[2], argv[3], argv[4], clients_context);
    }
    return 0;
}

