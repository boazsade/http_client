#pragma once
#include "http_base/http_header.h"
#include "http_base/http_body.h"
#include <string>
#include <iosfwd>
#include <utility>


namespace http
{
// reposense code as read from the message coming from the server
struct code
{

    code();

    explicit code(const std::string& value);

    code& operator = (const std::string& from);

    const std::string& description() const;

    int value() const;

    bool good() const;

    bool bad() const;

    typedef bool(code::*boolean_type)() const;

    // allow use to write a code such as if (code)
    operator boolean_type () const
    {
        return good() ? &code::good : (boolean_type)nullptr;
    }

private:
    std::string desc;
    int val;
};

std::ostream& operator << (std::ostream& os, const code& c);
bool operator == (const code& left, const code& right);
bool operator != (const code& left, const code& right);
bool operator < (const code& left, const code& right);

bool no_content(const code& c);
bool ok_reply(const code& c);
bool moved(const code& c);
bool created_resource(const code& c);
bool accepted_request(const code& c);
bool partial_content(const code& c);
bool use_proxy(const code& c);
bool bad_request(const code& c);
bool unautorized(const code& c);
bool forbidden(const code& c);
bool not_found(const code& c);
bool method_not_allowed(const code& c);
bool not_acceptable(const code& c);
bool request_time_out(const code& c);
bool length_required(const code& c);
bool request_to_large(const code& c);
bool internal_sever_error(const code& c);
bool not_implemented(const code& c);
bool service_unavailable(const code& c);
bool version_not_supported(const code& c);

// the status line in the first line in the message with the status code
struct status_line
{
    enum version {
        NOT_SET = -1,
        VERSION_1 = 0,
        VERSION_1_1 = 1
    };

    status_line();
    status_line(const status_line&) = default;
    status_line(status_line&&) = default;
    status_line& operator = (status_line&&) = default;
    status_line& operator = (const status_line&) = default;

    status_line(const std::string& sl);

    const code& get_code() const;


    const char* version_str() const;

    bool good() const;
    bool valid() const;
    bool version_1_1() const;
    bool version_1()  const;

    using boolean_type = bool(status_line::*)() const;

    operator boolean_type() const
    {
        return good() && valid() ? &status_line::good : (boolean_type)nullptr;
    }

private:
    void parse(const std::string& s);

    version ver;
    code info;
};
std::ostream& operator <<(std::ostream& os, const status_line& sl);
bool operator == (const status_line& left, const status_line& right);
bool operator != (const status_line& left, const status_line& right);
bool operator < (const status_line& left, const status_line& right);
///////
bool no_content(const status_line& c);
bool ok_reply(const status_line& c);
bool moved(const status_line& c);
bool created_resource(const status_line& c);
bool accepted_request(const status_line& c);
bool partial_content(const status_line& c);
bool use_proxy(const status_line& c);
bool bad_request(const status_line& c);
bool unautorized(const status_line& c);
bool forbidden(const status_line& c);
bool not_found(const status_line& c);
bool method_not_allowed(const status_line& c);
bool not_acceptable(const status_line& c);
bool request_time_out(const status_line& c);
bool length_required(const status_line& c);
bool request_to_large(const status_line& c);
bool internal_sever_error(const status_line& c);
bool not_implemented(const status_line& c);
bool service_unavailable(const status_line& c);
bool version_not_supported(const status_line& c);

// response from the server
struct response
{
    response() = default;

    response(const status_line& sl);

    response(const status_line& sl, const header& h);

    response(const status_line& sl, const headers_list& h);

    response(const status_line& sl, const headers_list& h, const body& b);

    response(const std::string& all_msg);

    template<typename It>
    response(const status_line& sl, It headers_begin, It headers_end) : 
        sline(sl), headers(headers_begin, headers_end)
    {
    }

    template<typename It>
    response(const status_line& sl, 
            It headers_begin, It headers_end,
            const body& b) : 
                sline(sl), headers(headers_begin, headers_end), the_body(b)
    {
    }

    response& operator = (const std::string& input);     // would reset the content

    response& operator = (const status_line& line);    // would reset the content

    response& operator = (const headers_list& hl);

    response& operator += (const header& h);    // must have a valid status line set before

    response& operator += (const body& b);      // must have a valid status line before

    bool got() const;   // return true if we have a valid response

    const status_line& get_status() const;

    //const body& get_body();     
                                

    const body& payload() const;    // return the body as is

    const header& operator [] (const header_key& name) const;

    const header& get_header(const header_key& name) const;

    const header& get_header(const header& hdr) const;

    const headers_list& the_headers() const;
    std::pair<headers_list::const_iterator, headers_list::const_iterator> all_headers() const;
    friend bool chunked_message(const response& res);
    friend bool keep_alive(const response& res);
    friend encoding_type encoded_type(const response& res);

private:
    status_line      sline;
    headers_list     headers;
    body             the_body;
};

std::ostream& operator <<(std::ostream& os, const response& r);
// this function would convert the message from what it was to either the same or deflated based
// on whether it is compress or not
body full_payload(const response& message);


}   // end of namespace http

