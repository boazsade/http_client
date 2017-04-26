#include "http_base/http_request.h"


namespace http
{

const char* version_1::value() const
{
    return "HTTP/1.0";
}

const char* version_1_1::value() const
{
    return "HTTP/1.1";
}

resource::resource(const std::string& path) : value(path)
{
}

resource::resource(std::string&& path) : value(std::move(path))
{
}

resource& resource::operator = (const std::string& from)
{
    value = from;
    return *this;
}

const std::string& resource::get() const
{
    return value;
}

const char* get_command::value() const
{
    return "GET";
}

const char* post_command::value() const
{
    return "POST";
}

const char* put_command::value() const
{
    return "PUT";
}

any_request_line::any_request_line(const std::string& from)
{
   parse(from.begin(), from.end());
}

bool any_request_line::valid() const
{
    return mtype != NOT_SET && mversion != NO_VERSION;
}

const char* any_request_line::type() const
{
    static const char* tags[] {
        "invalid", http_get_c.value(), http_put_c.value(), http_post_c.value(),
        "HEAD", "DELETE", "OPTIONS", "CONNECT"
    };
    return tags[mtype];
}

any_request_line::METHOD_TYPE any_request_line::method() const
{
    return mtype;
}

const char* any_request_line::version() const
{
    static const char* tags[] = {
        "invalid", v1.value(), v1_1.value()
    };
    return tags[mversion];
}

const resource& any_request_line::the_resource() const
{
    return res;
}

any_request_line::METHOD_TYPE any_request_line::match_method(const std::string& m)
{
    static const std::string methods[] = {
       "123456_dummy", "GET", "PUT", "POST", "HEAD", "DELETE", "OPTIONS", "CONNECT"
    };

    auto i = std::find(std::begin(methods), std::end(methods), m);
    if (i == std::end(methods)) {
        return NOT_SET;
    } else {
        auto idx = std::distance(std::begin(methods), i);
        return (METHOD_TYPE)idx;
    }
}

any_request_line::VT any_request_line::match_version(const std::string& v)
{
    if (v == v1.value()) {
        return VERSION1;
    } else if (v == v1_1.value()) {
        return VERSION11;
    } else {
        return NO_VERSION;
    }
}

bool any_request_line::version_1() const
{
    return mversion == VERSION1;
}

bool any_request_line::version1_1() const
{
    return mversion == VERSION11;
}

bool operator == (const any_request_line& left, const any_request_line& right)
{
    return ((left.version_1() == right.version_1()) && (left.version1_1() == right.version1_1()) &&
                left.the_resource() == right.the_resource() &&
                left.method() == right.method());
}

bool operator != (const any_request_line& left, const any_request_line& right)
{
    return !(left == right);
}

bool operator < (const any_request_line& left, const any_request_line& right)
{
    if (left.version_1() && right.version_1()) {
       if (left.method() == right.method()) {
          return left.the_resource() < right.the_resource();
       } else {
          return left.method() < right.method();
       }
    } else {
       return left.version_1();
    } 
}

std::ostream& operator << (std::ostream& os, const any_request_line& rl)
{
    if (rl.version_1() || rl.version1_1()) {
        return os<<"["<<rl.type()<<" "<<rl.the_resource()<<" "<<rl.version();
    } else {
        return os<<"[invalid request line]";
    }
}

bool operator == (const resource& left, const resource& right)
{
    return left.get() == right.get();
}

bool operator != (const resource& left, const resource& right)
{
    return !(left == right);
}

bool operator < (const resource& left, const resource& right)
{
    return left.get() < right.get();
}

std::ostream& operator << (std::ostream& os, const resource& r)
{
    return os<<"'"<<r.get()<<"'";
}
//
///////////////////////////////////////////////////////////////////////////////

request_base::request_base(header&& h) : headers({std::move(h)})
{
}

request_base::request_base(const header& h) : headers({h})
{
}
request_base::request_base(const header& h, const body& b) : headers({h}), the_body(b)
{
}

request_base::request_base(header&& h, body&& b) : headers({std::move(h)}), the_body(std::move(b))
{
}

request_base::request_base(const headers_list& h, const body& b) : headers(h), the_body(b)
{
}

request_base::request_base(headers_list&& h, body&& b) : headers(std::move(h)), the_body(std::move(b))
{
}

request_base& request_base::operator += (header&& h)
{
    headers += h;
    return *this;
}

request_base& request_base::operator += (const header& h)
{
    headers += h;
    return *this;
}

request_base& request_base::operator += (body&& b)
{
    the_body = std::move(b);
    return *this;
}

request_base& request_base::operator += (const body& b)
{
    the_body = b;
    return *this;
}

bool request_base::insert(header&& hdr)
{
    return headers.insert(hdr);
}

bool request_base::insert(const header& hdr)
{
    return headers.insert(hdr);
}

const body& request_base::get_body() const
{
    return  the_body;
}
const header_value& request_base::operator [] (const header_key& name) const
{
    return get_header(name);
}

const header_value& request_base::get_header(const header_key& name) const
{
    return headers[name];
}
const header_value& request_base::get_header(const header& hdr) const
{
    return get_header(hdr.name());
}
std::pair<headers_list::const_iterator, headers_list::const_iterator> request_base::all_headers() const
{
    return std::make_pair(headers.begin(), headers.end());
}

headers_list& request_base::the_headers()
{
    return headers;
}
const headers_list& request_base::the_headers() const
{
    return headers;
}

std::ostream& operator << (std::ostream& os, const request_base& rq)
{
    if (rq.the_headers().empty()) {
        return os<<"empty request";
    } else {
        return os<<rq.the_headers()<<"\n\n"<<"body size: "<<rq.get_body().size();
    }
}

///////////////////////////////////////////////////////////////////////////////
//

request_message::request_message(const any_request_line& rl) : start_line(rl)
{
}

request_message::request_message(any_request_line&& rl) : start_line(std::move(rl))
{
}

request_message::request_message(const any_request_line& rl, const header& h) : request_base(h), start_line(rl)
{
}

request_message::request_message(any_request_line&& rl, header&& h) : request_base(h), start_line(std::move(rl))
{
}


request_message::request_message(const any_request_line& rl, const headers_list& h, const body& b) : request_base(h, b), start_line(rl)
{
}

request_message::request_message(any_request_line&& rl, headers_list&& h, body&& b) : request_base(h, b), start_line(std::move(rl))
{
}

request_message& request_message::operator += (any_request_line&& r)
{
    std::swap(start_line,r);
    return *this;
}

any_request_line& request_message::requestline()
{
    return start_line;
}

const any_request_line& request_message::requestline() const
{
    return start_line;
}

std::ostream& operator << (std::ostream& os, const request_message& rq)
{
    if (rq.requestline().valid()) {
        os<<rq.requestline();
        return os<<"\n\n'"<<static_cast<const request_base&>(rq);
    } else {
        return os<<"invlalid request";
    }
}

}   // end of namespace http


