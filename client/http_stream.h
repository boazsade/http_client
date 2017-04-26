#pragma once
#include "http_base/http_header.h"
#include "http_base/http_request.h"
#include "http_base/http_body.h"
#include "http_base/http_request.h"
#include <boost/lexical_cast.hpp>
#include <string>

namespace http
{

// use this to "signal" that we are done with the headers
struct _endheaders {
};

constexpr _endheaders endh = _endheaders(); // a manipulator to use as headers end

struct body_entry
{
    body_entry(const body& b, std::string content_type);
    //body_entry(body b, std::string content_type);

    const body& get_body() const;

    const std::string& content_type() const;

private:
    const body& the_body;
    std::string type; 
};


// create http message from the http client
// this would format it so that we can 
// send this over the wire to the server
struct ostream_base
{    
protected:
    ostream_base() = default;

public:
    void clear() ;

    std::string::size_type size() const;

    bool empty() const;

   

   using boolean_type = const std::string&(ostream_base::*) () const;

    bool good() const;

    bool bad() const;

    operator boolean_type () const
    {
        return good() ? &ostream_base::get_str : (boolean_type)nullptr;
    }

protected:
    const std::string& get_str() const;

    template<typename T>
    void add_resource(const request_line<T>& rl) const
    {
        static const typename request_line<T>::method_type m = typename request_line<T>::method_type(); 
        data = std::string(m.value()) + " " + rl.get_resource().get() + " " + rl.version_str();
        add_eol();
    }

    void add_headers(headers_list::const_iterator h_from, headers_list::const_iterator h_to) const;

    void add_body(const body& b) const;

    void add_eol() const;   // add the end of line

private:

    void add_header(const header& h) const;

    mutable std::string data;
    
//    int state;
};

template<typename T>
struct ostream : public ostream_base 
{
    using request_type = request<T>;
    using request_line_t = typename request_type::request_type;

    ostream() = default;

    ostream& operator << (const request_line_t& rl)
    {
        data += rl;
        return *this;
    }

    ostream& operator << (const version_1& v1)
    {
        data.requestline() = v1;
        return *this;
    }

    ostream& operator << (const version_1_1& v11)
    {
        data.requestline() = v11;
        return *this;
    }


    ostream& operator << (const resource& r)
    {
        data.requestline() = r;
        return *this;
    }

    ostream& operator << (const body_entry& b)
    {
        data += b.get_body();
        data.insert(header("Content-Type"_hdrk, header_value(b.content_type())));
        data.insert(header("Content-Length"_hdrk, header_value(boost::lexical_cast<std::string>(b.get_body().size()))));
        return *this;
    }

    ostream& operator << (const header& h)
    {
        data.insert(h);
        return *this;
    }

    const std::string& str() const
    {
        if (ostream_base::empty()) {
            convert2str();
        }
        return ostream_base::get_str();
    }

private:
    void convert2str() const
    {
        if (data.requestline().valid()) {
            ostream_base::add_resource(data.requestline());
            auto hdrs = data.all_headers();
            if (std::distance(hdrs.first, hdrs.second) > 0) {
                ostream_base::add_headers(hdrs.first, hdrs.second);
            }
            ostream_base::add_eol();
            if (!data.get_body().empty()) {
                ostream_base::add_body(data.get_body());
            }
        }
    }

    request_type data;
};

using post = ostream<post_command>;
using put = ostream<put_command>;
using get = ostream<get_command>;

}   // end of namespace http

using http_post = http::post;
using http_get = http::get;
using http_put = http::put;

