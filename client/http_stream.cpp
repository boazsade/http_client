#include "client/http_stream.h"
#include "http_base/http_error.h"
#include <algorithm>


namespace http
{

namespace 
{

const std::string header_intern_delim = std::string(": ");

}   // end of local namespace

void ostream_base::clear()
{
    data.clear();
}


std::string::size_type ostream_base::size() const
{
    return data.size();
}

bool ostream_base::empty() const
{
    return data.empty();
}

void ostream_base::add_body (const body& b) const
{
    data += b.value();
}

void ostream_base::add_header(const header& h) const
{
    data += h.name().get();
    data += header_intern_delim;
    data += h.value().get();
    add_eol();
}


void ostream_base::add_eol() const
{
    data += the_delimiter.value();
}

void ostream_base::add_headers(headers_list::const_iterator h_from, headers_list::const_iterator h_to) const
{
    std::for_each(h_from, h_to, [this](const auto& h) {
                this->add_header(h);
           }
    ); 
}

const std::string& ostream_base::get_str() const
{
    static const std::string invalid = std::string();

    return data;
}

bool ostream_base::good() const
{
    return !bad();
}

bool ostream_base::bad() const
{
    return data.empty();
}

body_entry::body_entry(const body& b, std::string ct) : the_body(b), type(std::move(ct))
{
}

/*body_entry::body_entry(body b, std::string ct) : the_body(std::move(b)), type(std::move(ct))
{
}*/

const body& body_entry::get_body() const
{
    return the_body;
}

const std::string& body_entry::content_type() const
{
    return type;
}

///////////////////////////////////////////////////////////////////////////////
//

}   // end of namespace http
