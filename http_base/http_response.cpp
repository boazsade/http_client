#include "http_base/http_response.h"
#include "http_base/http_request.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/compare.hpp>
#include  <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <algorithm>
#include <boost/convert.hpp>
#include <boost/convert/spirit.hpp>
#include <iostream>
#include <iomanip>
#include <iterator>


namespace http
{


namespace
{
int get_code(const std::string& from);
std::string get_description(const std::string& from);
body get_body_from(const std::string& from);
status_line  get_status_from(const std::string& from);
void parse_headers(const std::string& msg, headers_list& to);
constexpr auto INVALID_STATUS_LINE = "Not Initialized";

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

body get_body_from(const std::string& from)
{
    static const delimiter del = delimiter();
    static const std::string body_delim = std::string(del.value()) + del.value();

    if (!from.empty()) {
        auto i =
                  boost::algorithm::make_split_iterator(from, boost::algorithm::first_finder(body_delim.c_str(), 
                                                            boost::algorithm::is_equal()));
            ++i;
            return body(boost::copy_range<std::string>(*i));
    }
    static const body empty = body(std::string());
    return empty;
}

status_line get_status_from(const std::string& from)
{
    static const delimiter del = delimiter();
// this is the first line in the input
    std::string st;
    if (!from.empty()) {
        boost::char_separator<char> sep(del.value());
        tokenizer tokens(from, sep);
        st = *tokens.begin();
    }
    return status_line(st);
}


void parse_headers(const std::string& msg, headers_list& to)
{
    // the first line is the status line, ignore it
    // then read each line until we have empty line,
    // which is the body
    static const std::string patt = the_delimiter.value();
    static const std::string split_str = patt+patt;
    std::string::size_type p = msg.find(patt);
    if (p != std::string::npos && p < msg.size()) { // make sure that we are skiping the status line
        p += patt.size();
        auto s = msg.substr(p);
        auto i = boost::algorithm::make_split_iterator(s, boost::algorithm::first_finder(split_str, boost::algorithm::is_equal()));
        s = boost::copy_range<std::string>(*i);
        to = make_headers(s);
    }
}

std::string get_description(const std::string& from)
{
    std::string desc;
    if (!from.empty()) {
        boost::char_separator<char> sep(" \t");
        tokenizer tokens(from, sep);
        auto ti = tokens.begin();
        for (auto i = 0; i < 2 && ti != tokens.end(); i++, ++ti) {} // skip first 2 as they are not description
        if (ti != tokens.end()) {
            std::for_each(ti, tokens.end(), [&desc](const std::string& cur) { desc +=  " " + cur; } );
        }
    }
    return desc;   
}

int get_code(const std::string& from)
{
    int code = 400; // error code
    if (!from.empty()) {
        boost::char_separator<char> sep(" \t");
        tokenizer tokens(from, sep);
        auto ti = tokens.begin();
        if (ti != tokens.end()) {
            ++ti;
        }
        if (ti != tokens.end()) {
            boost::cnv::spirit co;
            code = boost::convert<int>(*ti, co).value_or(400);
        }
        
    }
    return code;
}

}   // end of local namepsace

code::code() : desc(INVALID_STATUS_LINE), val(400) // 400 is error code for http, since we didn't initialized it it would signal for some error
{
}

code::code(const std::string& value) : desc(get_description(value)), val(get_code(value))
{
}

code& code::operator = (const std::string& value) 
{
    desc = get_description(value);
    val = get_code(value);
    return *this;
}

const std::string& code::description() const
{
/*    if (good()) {
        return good_code(value());
    } else {
        switch (value()) {
            case 300:
                return "Multiple Choices";
            case 301:
                return "Moved Permanently";
            case 302:
                return "Found";
            case 303:
                return "See Other";
            case 304:
                return "Not Modified";
            case 305:
                return "Use Proxy";
            case 307:
                return "Temporary Redirect";
            case 400:
                return "Bad Request";
            case 401:
                return "Unauthorized";
            case 402:
                return "Payment Required";
            case 403:
                return "Forbidden";
            case 404:
                return "Not Found";
            case 405:
                return "Method Not Allowed";
            case 406:
                return "Not Acceptable";
            case 407:
                return "Proxy Authentication Required";
            case 408:
                return "Request Timeout";
            case 409:
                return "Conflict";
            case 410:
                return "Gone";
            case 411:
                return "Length Required";
            case 412:
                return "Precondition Failed";
            case 413:
                return "Request Entity Too Large";
            case 414:
                return "Request-URI Too Long";
            case 415:
                return "Unsupported Media Type";
            case 416:
                return "Requested Range Not Satisfiable";
            case 417:
                return "Expectation Failed";
            case 500:
                return "Internal Server Error";
            case 501:
                return "Not Implemented";
            case 502:
                return "Bad Gateway";
            case 503:
                return "Service Unavailable";
            case 504:
                return "Gateway Timedout";
            case 505:
                return "HTTP version Not Supported";
            default:
                return "Uknown error code";

        }
    }*/
    return desc;
}

bool code::good() const
{
    return (value() / 100) == 2;  // all codes in the range of 200 are good
}

bool code::bad() const
{
    return !good();
}

int code::value() const
{
    return val;
}

bool response::got() const
{
    return !the_body.empty();
}

const status_line& response::get_status() const
{
    return sline;
}

response::response(const status_line& sl) : sline(sl)
{
}

response::response(const status_line& sl, const header& h) : sline(sl), headers({h})
{
}

response::response(const status_line& sl, const headers_list& h) : sline(sl), headers(h)
{
}

response::response(const status_line& sl, const headers_list& h, const body& b) : sline(sl), headers(h), the_body(b)
{
}

response::response(const std::string& all_msg) : sline(get_status_from(all_msg)), the_body(get_body_from(all_msg))
{
    parse_headers(all_msg, headers);
}

//const body& response::get_body()

const body& response::payload() const
{
    return the_body;
}

status_line::status_line() : ver(NOT_SET)
{
}

status_line::status_line(const std::string& sl) : ver(NOT_SET), info(sl)
{
    parse(sl);
    //std::cout<<"got status line string:\n--------------------------------\n"<<sl<<"\n---------------------------------------------\n";
}

const code& status_line::get_code() const
{
    return info;
}

void status_line::parse(const std::string& s)
{
    if (s.find(v1.value()) != std::string::npos) {
        ver = VERSION_1;
    } else {
        if (s.find(v1_1.value()) != std::string::npos) {
            ver = VERSION_1_1;
        } else {
            ver = NOT_SET;
        }
    }
}

bool status_line::version_1()  const
{
    return ver == VERSION_1;
}

bool status_line::version_1_1() const
{
    return ver == VERSION_1_1;
}

bool status_line::valid() const
{
    return ver != NOT_SET;
}
const char* status_line::version_str() const
{
    if (valid()) {
        if (version_1()) {
            return v1.value();
        } else {
            return v1_1.value();
        }
    } else {
        return "invalid";
    }
}

response& response::operator = (const std::string& input)
{
    *this = response(input);
    return *this;
}

response& response::operator = (const status_line& sl)
{
    *this = response(sl);
    return *this;
}

response& response::operator += (const header& h)
{
    if (sline.get_code().description() != INVALID_STATUS_LINE) {
        headers.insert(h);
    }
    return *this;

}

response& response::operator += (const body& b)
{
    if (sline.get_code().description() != INVALID_STATUS_LINE) {
        the_body = b;
    }
    return *this;
}

const header& response::operator [] (const header_key& name) const
{
    return get_header(name);
}

const header& response::get_header(const header_key& name) const
{
    auto i = cfind(headers, name);

    return i == headers.end() ? invalid_header : *i;
}

const header& response::get_header(const header& hdr) const
{
    return get_header(hdr.name());
}

bool no_content(const code& c)
{
    return c.value() == 204;
}

bool ok_reply(const code& c)
{
    return c.value() == 200;
}

bool moved(const code& c)
{
    return c.value() == 301;
}

bool created_resource(const code& c)
{
    return c.value() == 201;
}

bool accepted_request(const code& c)
{
    return c.value() == 202;
}
bool partial_content(const code& c)
{
    return c.value() == 206;
}

bool use_proxy(const code& c)
{
    return c.value() == 305;
}

bool bad_request(const code& c)
{
    return c.value() == 400;
}

bool unautorized(const code& c)
{
    return c.value() == 401;
}
bool forbidden(const code& c)
{
    return c.value() == 403;
}

bool not_found(const code& c)
{
    return c.value() == 404;
}

bool method_not_allowed(const code& c)
{
    return c.value() == 405;
}

bool not_acceptable(const code& c)
{
    return c.value() == 406;
}

bool request_time_out(const code& c)
{
    return c.value() == 408;
}

bool length_required(const code& c)
{
    return c.value() == 411;
}

bool request_to_large(const code& c)
{
    return c.value() == 413;
}

bool internal_sever_error(const code& c)
{
    return c.value() == 500;
}

bool not_implemented(const code& c)
{
    return c.value() == 501;
}

bool service_unavailable(const code& c)
{
    return c.value() == 503;
}

bool version_not_supported(const code& c)
{
    return c.value() == 505;
}

response& response::operator = (const headers_list& hl)
{
    headers = hl;
    return *this;
}

std::pair<headers_list::const_iterator, headers_list::const_iterator> response::all_headers() const
{
    return std::make_pair(headers.begin(), headers.end());
}

bool chunked_message(const response& res)
{
    return chunked_message(res.headers);
}

bool keep_alive(const response& res)
{
    if (res.sline.valid()) {
        return keep_alive(res.headers, res.sline.version_1());
    } else {
        return false;
    }
}

encoding_type encoded_type(const response& res)
{
    return encoded_type(res.headers);
}

const headers_list& response::the_headers() const
{
    return headers;
}
///////////////////////////////////////////////////////////////////////////////


std::ostream& operator <<(std::ostream& os, const response& r)
{
    os<<"response:\n";
    os<<"\nstatus line: "<<r.get_status()<<"\n";
    os<<"headers:\n";
    auto hdrs = r.all_headers();
    auto i = 0;
    while (hdrs.first != hdrs.second) {
        os<<"["<<i++<<": "<<*hdrs.first<<"]\n";
        hdrs.first++;
    }
    if (not std::empty(r.payload())) {
        os<<"\nmessage body:\n"<<r.payload().value();
    } else {
        os<<"\nno message body";
    }
    return os;
    //std::copy(hdrs.first, hdrs.second, std::ostream_iterator<headers_list::value_type>(os, "\n"));
}

std::ostream& operator <<(std::ostream& os, const status_line& sl)
{
    return os<<sl.version_str()<<" "<<sl.get_code();
}

std::ostream& operator << (std::ostream& os, const code& c)
{
    return os<<c.value()<<": "<<c.description();
}

bool operator == (const code& left, const code& right)
{
    return left.value() == right.value();
}

bool operator != (const code& left, const code& right)
{
    return !(left == right);
}

bool operator < (const code& left, const code& right)
{
    return left.value() < right.value();
}

bool operator == (const status_line& left, const status_line& right)
{
    return left.get_code() == right.get_code();
}

bool operator != (const status_line& left, const status_line& right)
{
    return !(left == right);
}

bool operator < (const status_line& left, const status_line& right)
{
    return left.get_code() < right.get_code();
}

bool no_content(const status_line& c)
{
    return no_content(c.get_code());
}

bool ok_reply(const status_line& c)
{
    return ok_reply(c.get_code());
}

bool moved(const status_line& c)
{
    return moved(c.get_code());
}

bool created_resource(const status_line& c)
{
    return created_resource(c.get_code());
}

bool accepted_request(const status_line& c)
{
    return accepted_request(c.get_code());
}

bool partial_content(const status_line& c)
{
    return partial_content(c.get_code());
}

bool use_proxy(const status_line& c)
{
    return use_proxy(c.get_code());
}

bool bad_request(const status_line& c)
{
    return bad_request(c.get_code());
}

bool unautorized(const status_line& c)
{
    return unautorized(c.get_code());
}

bool forbidden(const status_line& c)
{
    return forbidden(c.get_code());
}

bool not_found(const status_line& c)
{
    return not_found(c.get_code());
}

bool method_not_allowed(const status_line& c)
{
    return method_not_allowed(c.get_code());
}

bool not_acceptable(const status_line& c)
{
    return not_acceptable(c.get_code());
}

bool request_time_out(const status_line& c)
{
    return request_time_out(c.get_code());
}

bool length_required(const status_line& c)
{
    return length_required(c.get_code());
}

bool request_to_large(const status_line& c)
{
    return request_to_large(c.get_code());
}

bool internal_sever_error(const status_line& c)
{
    return internal_sever_error(c.get_code());
}

bool not_implemented(const status_line& c)
{
    return not_implemented(c.get_code());
}

bool service_unavailable(const status_line& c)
{
    return service_unavailable(c.get_code());
}

bool version_not_supported(const status_line& c)
{
    return version_not_supported(c.get_code());
}

bool status_line::good() const
{
    return get_code().good();
}

body full_payload(const response& message)
{
    encoding_type en = encoded_type(message.the_headers());
    if (en.gzip()) {
        body result{message.payload()};
        if (inflate(result)) {
            return result;
        } else {
            static const body empty = body{};
            return empty;
        }
    } else {
        return message.payload();
    }
}

}   // end of namespace http

