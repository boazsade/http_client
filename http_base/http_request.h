#pragma once
#include "http_base/http_header.h"
#include "http_base/http_body.h"
#include <boost/mpl/assert.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/has_key.hpp>
#include <string>
#include <iostream>
#include <utility>
#include <regex>


namespace http
{
// HTTP version 1.0
struct version_1
{
    const char* value() const;
};

// HTTP version 1.1
struct version_1_1 
{
    const char* value() const;
};

// the name of the resouce to get from the server
// 
struct resource
{
    explicit resource(const std::string& path);
    explicit resource(std::string&& path);

    resource() = default;
    resource(resource&&) = default;
    resource(const resource&) = default;
    resource& operator = (const resource&) = default;
    resource& operator = (resource&&) = default;

    resource& operator = (const std::string& from);

    const std::string& get() const;

private:
    std::string value;
};

bool operator == (const resource& left, const resource& right);
bool operator != (const resource& left, const resource& right);
bool operator < (const resource& left, const resource& right);
std::ostream& operator << (std::ostream& os, const resource& r);

// HTTP get command
struct get_command
{
    const char* value() const;
};

// HTTP post command
struct post_command
{
    const char* value() const;
};

// HTTP put command
struct put_command
{
    const char* value() const;
};

constexpr version_1_1 v1_1 = version_1_1();
constexpr version_1 v1 = version_1();

template<typename T>
class request_line_validator {
    using valid_request_t = boost::mpl::set<
                                        get_command, post_command, put_command
                                    >;
     BOOST_MPL_ASSERT(( boost::mpl::has_key<valid_request_t, T> ));
};

struct any_request_line
{
    enum METHOD_TYPE {
        NOT_SET = 0, 
        GET_T, 
        PUT_T, 
        POST_T,
        HEAD_T,
        DELETE_T,
        OPTIONS_T,
        CONNECT_T
    };

    any_request_line() = default;
    any_request_line(any_request_line&&) = default;
    any_request_line(const any_request_line&) = default;
    any_request_line& operator = (const any_request_line&) = default;
    any_request_line& operator = (any_request_line&&) = default;

    explicit any_request_line(const std::string& from);

    template<typename I>
    any_request_line(I itf, I itt)
    {
        parse(itf, itt);
    }

    const char* type() const;   // return either GET/POST/PUT..

    const char* version() const;

    const resource& the_resource() const;

    bool version_1() const;

    bool version1_1() const;

    METHOD_TYPE method() const;

    bool valid() const;

private:
    enum VT {
        NO_VERSION = 0,
        VERSION1, VERSION11
    };

    template<typename It>
    void parse(It from, It to)
    {
        // assume that this is a resource line that we to read - the args are the range
        static const std::regex rl{"([\\w]{3,10})[ \t]+([^ ]+).+(HTTP/[12]\\.[01])"};
        std::match_results<It> matches;
        if (std::regex_search(from, to, matches, rl)) {
            const std::string m{matches[1].first, matches[1].second};
            const std::string r{matches[2].first, matches[2].second};
            const std::string v{matches[3].first, matches[3].second};
            mtype = match_method(m);
            mversion = match_version(v);
            res = r;
        } 
    }

    static METHOD_TYPE match_method(const std::string& m);
    static VT match_version(const std::string& v);

    METHOD_TYPE mtype = NOT_SET;
    VT mversion = NO_VERSION;
    resource res;
};

bool operator == (const any_request_line& left, const any_request_line& right);
bool operator != (const any_request_line& left, const any_request_line& right);
bool operator < (const any_request_line& left, const any_request_line& right);

std::ostream& operator << (std::ostream& os, const any_request_line& rl);

// request line would contain the method (one of get, post or put), version and resource
template<typename T>
struct request_line : private request_line_validator<T>
{
    using method_type = T;

    request_line() = default;

    request_line(request_line&&) = default;
    request_line(const request_line&) = default;
    request_line& operator = (const request_line&) = default;
    request_line& operator = (request_line&&) = default;

    request_line(version_1_1) : ver(VERSION_1_1)  // create version 1.1 request
    {
    }

    request_line(version_1) : ver( VERSION_1)   // create version 1.0 request
    {
    }

    request_line(version_1_1, const resource& r) : ver(VERSION_1_1), res(r)
    {
    }

    request_line(version_1, const resource& r) : ver(VERSION_1_1), res(r)
    {
    }

    request_line& operator = (const resource& r)
    {
        res = r;
        return *this;
    }

    request_line& operator = (version_1)
    {
        ver = VERSION_1;
        return *this;
    }

    request_line& operator = (version_1_1)
    {
        ver = VERSION_1_1;
        return *this;
    }

    constexpr bool is_version1()  const
    {
        return ver == VERSION_1;
    }

    constexpr bool is_version1_1() const
    {
        return ver == VERSION_1_1;
    }

    constexpr bool valid() const
    {
        return ver != NOT_SET;
    }
    
    const resource& get_resource() const
    {
        return res;
    }

    const char* version_str() const
    {
        if (valid()) {
            if (is_version1()) {
                return v1.value();
            } else {
                return v1_1.value();
            }
        } else {
            return nullptr;
        }
    }

private:
    enum version {
        NOT_SET,
        VERSION_1,
        VERSION_1_1
    };

    version ver;
    resource res;
};


template<typename T>
inline std::ostream& operator << (std::ostream& os, const request_line<T>& rl)
{
    static const typename request_line<T>::method_type m = typename request_line<T>::method_type();
    if (rl.valid()) {
        m.value()<<" "<<rl.get_resource()<<" ";
        os<<rl.version_str();
    } else {
        os<<"invalid request line";
    }
    return os;
}

struct request_base
{
    request_base() = default;
    request_base(request_base&&) = default;
    request_base(const request_base&) = default;
    request_base& operator = (const request_base&) = default;
    request_base& operator = (request_base&&) = default;

    explicit request_base(header&& h);
    explicit request_base(const  header& h);
    request_base(const header& h, const body& b);
    request_base(header&& h, body&& b);
    request_base(const headers_list& h, const body& b);
    request_base(headers_list&& h, body&& b);

    template<typename It>
    request_base( It headers_begin, It headers_end) : headers(headers_begin, headers_end)
    {
    }

    template<typename It>
    request_base( It headers_begin, It headers_end, body&& b) : headers(headers_begin, headers_end), the_body(b)
    {
    }

    request_base& operator += (header&& h);    // must have a valid status line set before
    request_base& operator += (const header& h);    // must have a valid status line set before
    request_base& operator += (body&& b);      // must have a valid status line before
    request_base& operator += (const body& b);      // must have a valid status line before

    bool insert(header&& hdr);
    bool insert(const header& h);
    const body& get_body() const;
    const header_value& operator [] (const header_key& name) const;
    const header_value& get_header(const header_key& name) const;
    const header_value& get_header(const header& hdr) const;
    std::pair<headers_list::const_iterator, headers_list::const_iterator> all_headers() const;
    headers_list& the_headers();
    const headers_list& the_headers() const;

private:
    headers_list    headers;
    body            the_body;
};

// for reading and not when creating internally
struct request_message : public request_base
{
    request_message() = default;
    request_message(request_message&&) = default;
    request_message(const request_message&) = default;
    request_message& operator = (request_message&&) = default;
    request_message& operator = (const request_message&) = default;

    explicit request_message(const any_request_line& rl);

    explicit request_message(any_request_line&& rl);

    request_message(const any_request_line& rl, const header& h);
    request_message(any_request_line&& rl, header&& h);

    request_message(const any_request_line& rl, const headers_list& h, const body& b = body{});
    request_message(any_request_line&& rl, headers_list&& h, body&& b);

    template<typename It>
    request_message(any_request_line&& sl, It headers_begin, It headers_end) : request_base(headers_begin, headers_end), start_line(sl)
    {
    }

    template<typename It>
    request_message(any_request_line&& sl, It headers_begin, It headers_end, body&& b) : request_base(headers_begin, headers_end, b), start_line(sl)
    {
    }

    request_message& operator += (any_request_line&& r);

    any_request_line& requestline();

    const any_request_line& requestline() const;
private:
    any_request_line start_line;
};

std::ostream& operator << (std::ostream& os, const request_base& rq);
std::ostream& operator << (std::ostream& os, const request_message& rq);

// hold a message that is internally generated (not read from the network)
// for implementing client message
template<typename T>
struct request : public request_base 
{
    using request_type = request_line<T>;
    using method_type = typename request_type::method_type;

   request() = default; 
   request(request&&) = default; 
   request(const request&) = default; 
   request& operator = (request&&) = default; 
   request& operator = (const request&) = default; 

    explicit request(const request_type& rl) : start_line(rl)
    {
    }

    request(const request_type& rl, const header& h) : request_base(h), start_line(rl) 
    {
    }

    request(const request_type& rl, const headers_list& h, const body& b) : request_base(h, b), start_line(rl)
    {
    }

    template<typename It>
    request(const request_type& sl, It headers_begin, It headers_end) : request_base(headers_begin, headers_end), start_line(sl)
    {
    }

    template<typename It>
    request(const request_type& sl, 
            It headers_begin, It headers_end,
            const body& b) : request_base(headers_begin, headers_end, b), start_line(sl)
    {
    }

    request& operator += (const request_type& r)
    {
        start_line  = r;
        return *this;
    }
    request& operator += (header&& h)
    {
        static_cast<request_base&>(*this) += h;
        return *this;
    }

    request& operator += (const header& h)
    {
        static_cast<request_base&>(*this) += h;
        return *this;
    }

    request& operator += (body&& b)
    {
        static_cast<request_base&>(*this) += b;
        return *this;
    }

    request& operator += (const body& b)
    {
        static_cast<request_base&>(*this) += b;
        return *this;
    }

    const request_type& requestline() const
    {
        return start_line;
    }

    request_type& requestline() 
    {
        return start_line;
    }
private:
    request_type    start_line;
};

template<typename T>
inline std::ostream& operator << (std::ostream& os, const request<T>& req)
{
    if (req.requestline().valid()) {
        os<<req.requestline()<<"\n";
        return os<<static_cast<const request_base&>(req);
    } else {
        return os<<"invalid or empty request\n";
    }
}


using get_line = request_line<get_command>;
using put_line = request_line<put_command>;
using post_line = request_line<post_command>;

using get_message = request<get_command>;
using put_message = request<put_command>;
using post_message = request<post_command>;

}	// end of namespace http

constexpr http::put_command http_put_c = http::put_command();
constexpr http::get_command http_get_c = http::get_command();
constexpr http::post_command http_post_c = http::post_command();

using http_get_request = http::get_line;
using http_put_request = http::put_line;
using http_post_request = http::post_line;

using http_get_msg = http::get_message;
using http_put_msg = http::put_message;
using http_post_msg = http::post_message;



