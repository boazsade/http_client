#include "http_secure_args.h"

namespace http
{

namespace
{

namespace ssl = boost::asio::ssl;

ssl::context init_context(const https_config& conf)
{
    int proto = conf.security_type();
    if (proto == -1) {
        proto = ssl::context_base::sslv23;
    }
    ssl::context ctx((ssl::context::method)proto);
    if (conf.cert_conf().use_default()) {
        ctx.set_default_verify_paths();
    } else {
        if (conf.cert_conf().is_file()) {
            ctx.load_verify_file(conf.cert_conf().get_path());
        } else {
            ctx.add_verify_path(conf.cert_conf().get_path());
        }
    }
    return ctx;
}

#if 0
bool is_server_side(int value)
{
    switch (value) {
        case -1:
            return false;
        case ssl::context_base::sslv2_server:
        case ssl::context_base::sslv3_server:
        case ssl::context_base::tlsv1_server:
        case ssl::context_base::tlsv11_server:
        case ssl::context_base::tlsv12_server:
        case ssl::context_base::sslv23_server:
            return true;
        default:
            return false;
    }
}
bool is_any_side(int value)
{
    switch(value) {
        case -1:
            return false;
        case  ssl::context_base::sslv2:
        case  ssl::context_base::sslv3:
        case  ssl::context_base::tlsv1:
        case  ssl::context_base::tlsv11:
        case  ssl::context_base::tlsv12:
        case  ssl::context_base::sslv23:
            return true;
        default:
            return false;
    }
}
bool is_client_side(int value)
{
    return !(is_any_side(value) || is_server_side(value));
}
#endif

int type2boost_type_client(https_config::type t)
{
    switch (t) {
        case https_config::SSL_V2:
            return ssl::context_base::sslv2_client;
        case https_config::SSL_V3:
            return ssl::context_base::sslv3_client;
        case https_config::TLS_V1:
            return ssl::context_base::tlsv1_client;
        case https_config::TLS_V11:
            return ssl::context_base::tlsv11_client;
        case https_config::TLS_V12:
            return ssl::context_base::tlsv12_client;
        case https_config::SSL_TLS:
            return ssl::context_base::sslv23_client;
        case https_config::TYPE_NOT_SET:
            return -1;
    }
    return -1;
}

int type2boost_type_sever(https_config::type t)
{
    switch (t) {
        case https_config::SSL_V2:
            return ssl::context_base::sslv2_server;
        case https_config::SSL_V3:
            return ssl::context_base::sslv3_server;
        case https_config::TLS_V1:
            return ssl::context_base::tlsv1_server;
        case https_config::TLS_V11:
            return ssl::context_base::tlsv11_server;
        case https_config::TLS_V12:
            return ssl::context_base::tlsv12_server;
        case https_config::SSL_TLS:
            return ssl::context_base::sslv23_server;
        case https_config::TYPE_NOT_SET:
            return -1;
    }
    return -1;
}

int type2boost_type(https_config::type t)
{
    switch (t) {
        case https_config::SSL_V2:
            return ssl::context_base::sslv2;
        case https_config::SSL_V3:
            return ssl::context_base::sslv3;
        case https_config::TLS_V1:
            return ssl::context_base::tlsv1;
        case https_config::TLS_V11:
            return ssl::context_base::tlsv11;
        case https_config::TLS_V12:
            return ssl::context_base::tlsv12;
        case https_config::SSL_TLS:
            return ssl::context_base::sslv23;
        case https_config::TYPE_NOT_SET:
            return -1;
    }
    return -1;
}


int convert2action(https_config::type t, https_config::which w)
{
    switch (w) {
        case https_config::CLIENT_ONLY:
            return type2boost_type_client(t);
        case https_config::SERVER_ONLY:
            return type2boost_type_sever(t);
        default:
            return type2boost_type(t);
    }
    return type2boost_type(t);
}

///////////////////////////////////////////////////////////////////////////////


}   // end of local namespce

https_cert::https_cert() : fformat(NOT_SET), file(false)
{
}  

https_cert::https_cert(const std::string& p, format f) : path(p), fformat(f), file(false)
{
}

https_cert::https_cert(const std::string& p, bool is_file, format f) : path(p), fformat(f), file(is_file)
{
}

bool https_cert::use_default() const
{
    return path.empty();
}

https_cert::format https_cert::get_format() const
{
    return fformat;
}

const char* https_cert::get_path() const
{
    return use_default() ? nullptr : path.c_str();
}

bool https_cert::is_file() const
{
    return file;
}

///////////////////////////////////////////////////////////////////////////////

https_config::https_config(type t, which w, const https_cert& c) : sec_type(convert2action(t, w)), cert(c)
{
}

const https_cert& https_config::cert_conf() const
{
    return cert;
}

https_config& https_config::set_protocol(type t, which w)
{
    sec_type = convert2action(t, w);
    return *this;
}

https_config& https_config::operator = (const https_cert& c)
{
    cert = c;
    return *this;
}

int https_config::security_type() const
{
    return sec_type;
}

///////////////////////////////////////////////////////////////////////////////

security_args::security_args(const https_config& conf) : context(std::move(init_context(conf))), verify_action(security_args::DONT_APPLY)
{
}

security_args::security_args(int verify_type, const https_config& conf) : context(std::move(init_context(conf))), verify_action(verify_type)
{
}

void security_args::sec_method(int type)
{
    verify_action = type;
}

security_args::ssl_context& security_args::security_handle()
{
    return context;
}

int security_args::security_method() const
{
    return verify_action;;
}

}   // end of namespace http

