#pragma once
#include "client/http_types.h"
#include <string>
#include <boost/asio/ssl.hpp>


namespace http
{
// use this for the perliminery verification of the 
// centification - that is, it would load it from a file
// and verify it. This is in its own class, since we can
// apply this without the first connection being made
//

struct deault_cert_paths {};
constexpr deault_cert_paths use_default_cers = deault_cert_paths();

// the location if set would point to the directory with single certificate file 
// it would be later use to call SSL_CTX_load_verify_locations
struct https_cert {
    enum format {
        NOT_SET, ASN1_FORMAT, PEM_FORMAT
    };
    https_cert();       // create with default would make this use default paths
    https_cert(const std::string& path, format f = NOT_SET);
    https_cert(const std::string& path, bool is_file, format f);    // load from file and not set new paths

    https_cert(https_cert&&) = default;
    https_cert(const https_cert&) = default;
    https_cert& operator= (const https_cert&) = default;
    https_cert& operator= (https_cert&&) = default;

    https_cert& operator = (const std::string& p);  // set new path
    https_cert& operator = (deault_cert_paths);  // set to default

    bool use_default() const;

    const char* get_path() const;

    format get_format() const;

    bool is_file() const;

private:
    std::string path;
    format fformat;
    bool file;
};

class https_config
{
public:
    enum type {
        TYPE_NOT_SET = 0,
        SSL_V2, 
        SSL_V3, TLS_V1, 
        TLS_V11, TLS_V12,
        SSL_TLS
    };
    enum which {
        SIDE_NOT_SET = 100, 
        ANY_SIDE = 200, 
        CLIENT_ONLY = 300, 
        SERVER_ONLY = 400
    };

    https_config(type t = SSL_TLS, which w = ANY_SIDE, const https_cert& cert = https_cert());
    https_config(const https_config&) = default;
    https_config(https_config&&) = default;
    https_config& operator = (https_config&&) = default;
    https_config& operator = (const https_config&) = default;

    https_config& set_protocol(type t, which w);
    https_config& operator = (const https_cert& cert);  // overides the old one

    const https_cert& cert_conf() const;

    int security_type() const;  // this function should not be called in the sense that you dont know that the return value stands for
                                // this function can only be used with security_args, but note that nothing bad would happens from
                                // calling this function, againm it would not be usefull to call it
private:
    int         sec_type;   // use combination from type and which
    https_cert  cert;
};

class security_args
{
    friend class https_client;

public:
    using ssl_context = boost::asio::ssl::context;
    enum verify_what {
        DONT_APPLY = 0,
        CLIENT_ONCE = 1,
        PEER = 2,
        FAIL_CERT_ERR = 4
    };

    security_args(const https_config& conf = https_config());
    security_args(int verify_type,    // either DONT_APPLY or conmbinatin of the others
                    const https_config& conf = https_config());

    security_args(security_args&&) = default;
    security_args(const security_args&) = default;
    security_args& operator = (const security_args&) = default;
    security_args& operator = (security_args&&) = default;

    void sec_method(int type);  // note that this must be combination of verify_what entries

    ssl_context& security_handle();

    bool handshake(https_socket_t& with, const std::string& host_name) const;

    int security_method() const;
private:
    ssl_context context;
    int         verify_action;
};


}   // end of namespace http

