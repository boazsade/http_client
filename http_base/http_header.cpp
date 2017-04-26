#include "http_base/http_header.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/compare.hpp>
#include  <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/finder.hpp>
#include  <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include <iostream>



namespace http
{

const char* delimiter::value() const
{
    return "\r\n";
}
///////////////////////////////////////////////////////////////////////////////

header_key::header_key(const char* str, std::size_t s) : key(str, s)
{
}

header_key::header_key(const std::string& k) : key(k)
{
}

header_key& header_key::operator = (const std::string& k)
{
    key = k;
    return *this;
}

const std::string& header_key::get() const
{
    return key;
}

bool operator == (const header_key& left, const std::string& right)
{
    return boost::algorithm::iequals(left.get(), right);
}

bool operator == (const std::string& left, const header_key& right)
{
    return right == left;
}

bool operator == (const header_key& left, const header_key& right)
{
    // note that with key name we need to compare without case!!
    return boost::algorithm::iequals(left.get(), right.get());
}

bool operator != (const header_key& left, const std::string& right)
{
    return !(left == right);
}

bool operator != (const std::string& left, const header_key& right)
{
    return !(left == right);
}


bool operator != (const header_key& left, const header_key& right)
{
    return !(left == right);
}

bool operator < (const header_key& left, const header_key& right)
{
    return boost::algorithm::ilexicographical_compare(left.get(), right.get());
}

bool operator < (const header_key& left, const std::string& right)
{
    return boost::algorithm::ilexicographical_compare(left.get(), right);
}

bool operator < (const std::string& left, const header_key& right)
{
    return boost::algorithm::ilexicographical_compare(left, right.get());
}

bool operator > (const header_key& left, const header_key& right)
{
    return left != right && !(left < right);
}

bool operator <= (const header_key& left, const header_key& right)
{
    return !(left > right);
}

bool operator >= (const header_key& left, const header_key& right)
{
    return !(left < right);
}

std::ostream& operator << (std::ostream& os, const header_key& k)
{
    return os<<"'"<<k.get()<<"'";
}

///////////////////////////////////////////////////////////////////////////////

bool header_value::valid() const
{
    return !get().empty();
}

header_value::header_value(const char* str, std::size_t s) : value(str, s)
{
}

header_value::header_value(const std::string& str) : value(str)
{
}

header_value& header_value::operator = (const std::string& s)
{
    value = s;
    return *this;
}

const std::string& header_value::get() const
{
    return value;
}

bool operator == (const header_value& left, const header_value& right)
{
    return boost::algorithm::iequals(left.get(), right.get());
}
bool operator == (const std::string& left, const header_value& right)
{
    return boost::algorithm::iequals(left, right.get());
}

bool operator == (const header_value& left, const std::string& right)
{
    return right == left;
}

bool operator != (const header_value& left, const header_value& right)
{
    return !(left == right);
}

bool operator != (const std::string& left, const header_value& right)
{
    return !(left == right);
}

bool operator != (const header_value& left, const std::string& right)
{
    return !(left == right);
}

bool operator < (const header_value& left, const header_value& right)
{
    return boost::algorithm::ilexicographical_compare(left.get(), right.get());
}

bool operator < (const header_value& left, const std::string& right)
{
    return boost::algorithm::ilexicographical_compare(left.get(), right);
}

bool operator < (const std::string& left, const header_value& right)
{
    return boost::algorithm::ilexicographical_compare(left, right.get());
}

bool operator > (const header_value& left, const header_value& right)
{
    return left != right && !(left < right);
}

bool operator <= (const header_value& left, const header_value& right)
{
    return !(left.get() > right.get());
}

bool operator >= (const header_value& left, const header_value& right)
{
    return !(left.get() < right.get());
}


std::ostream& operator << (std::ostream& os, const header_value& k)
{
    return os<<"'"<<k.get()<<"'";
}

///////////////////////////////////////////////////////////////////////////////
header::header (const header_key& name, const header_value& value) : _name(name), _value(value)
{
}


const header_key& header::name() const
{
    return _name;
}

const header_value& header::value() const
{
    return _value;
}

const std::string& key_name(const header& h)
{
    return h.name().get();
}

const std::string& value_name(const header& h)
{
    return h.value().get();
}

header operator + (const header_key& k, const header_value& v)
{
    return header(k, v);
}

header operator + (header_key&& k, header_value&& v)
{
    return header(k, v);
}

bool equal_field(const header& left, const header& right)
{
    return left.name() == right.name();
}

bool lessthan_field(const header& left, const header& right)
{
    return left.name() < right.name();
}

bool different_field(const header& left, const header& right)
{
    return !equal_field(left, right);
}

header make_header(const char* name, const char* value)
{
    return header(header_key(name), header_value(value));
}

header make_header(const std::string& name, const std::string& value)
{
    return header(header_key(name), header_value(value));
}

bool operator == (const header& left, const header& right)
{
    return left.name() == right.name() && left.value() == right.value();
}

bool operator != (const header& left, const header& right)
{
    return !(left == right);
}

bool operator < (const header& left, const header& right)
{
    return left.name() < right.name() && left.value() == right.value();
}
bool operator > (const header& left, const header& right)
{
    return left.name() > right.name();
}
bool operator >= (const header& left, const header& right)
{
    return !(left.name() < right.name());
}

bool operator <= (const header& left, const header& right)
{
    return !(left.name() > right.name());
}


header make_header(const std::string& line)
{
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(":");
    tokenizer tokens(line, sep);
    auto t = tokens.begin();
    std::string n, v;
    //std::cout<<"creating header entry from "<<line<<std::endl;
    if (t != tokens.end()) {
        n = *t;
        ++t;
        if (t != tokens.end()) {
            v = *t;
            boost::algorithm::trim(n);
            boost::algorithm::trim(v);
            return header(header_key(n), header_value(v));
        }
    }

    return invalid_header; 
}

headers_list make_headers(const std::string& header_lines)
{
    typedef boost::algorithm::split_iterator<std::string::const_iterator> string_split_iterator;

    static const std::string patt = the_delimiter.value();

    headers_list headers;
    //std::cout<<"processing "<<header_lines.size()<<" bytes for headers"<<std::endl;
    if (!header_lines.empty()) {
        string_split_iterator end = string_split_iterator();
        string_split_iterator it = boost::algorithm::make_split_iterator(header_lines, boost::algorithm::first_finder(patt, boost::algorithm::is_equal())); 
        
        for (; it != end; ++it) {   // now we have the lines with headers, we need to make each line a tuple of name:value
            headers.insert(make_header(boost::copy_range<std::string>(*it)));
        }
        //std::cout<<"finish processing "<<headers.size()<<" headers"<<std::endl;
    }
    return headers;
}

std::ostream& operator << (std::ostream& os, const header& h)
{
    return os<<h.name()<<": "<<h.value();
}

///////////////////////////////////////////////////////////////////////////////

headers_list::headers_list(std::initializer_list<value_type> init_list) : data(init_list)
{
}

headers_list::iterator headers_list::begin()
{
    return data.begin();
}

headers_list::iterator headers_list::end()
{
    return data.end();
}

headers_list::const_iterator headers_list::begin() const
{
    return data.begin();
}

headers_list::const_iterator headers_list::end() const
{
    return data.end();
}

bool headers_list::empty() const
{
    return data.empty();
}

std::size_t headers_list::size() const
{
    return data.size();
}

const headers_list::value_type::mapped_type& headers_list::operator [] (const value_type::key_type& key) const
{
    auto i = std::find_if(begin(), end(), [&key](const auto& node) { return (node.name()) == key; });
    return i == end() ? bad_http_value : i->value();
}

bool headers_list::insert(value_type&& v)
{
    if (!has(*this, v)) {
        data.push_back(v);
        return true;
    } else {
        return false;
    }
}

bool headers_list::insert(const value_type& v)
{
    if (!has(*this, v)) {
        data.push_back(v);
        return true;
    } else {
        return false;
    }
}

headers_list::comp_res headers_list::copmare(const headers_list& other) const
{
    if (data == other.data) {
        return EQ;
    } else {
        return data < other.data ? LT : GT;
    }
}


headers_list& headers_list::operator += (const value_type& v)
{
    insert(v);
    return *this;
}


bool headers_list::erase(const value_type& v)
{
    auto i = find(*this, v);
    if (i != end()) {
        data.erase(i);
        return true;
    } else {
        return false;
    }
}

bool headers_list::erase(const value_type::key_type& v)
{
    auto i = find(*this, v);
    if (i != end()) {
        data.erase(i);
        return true;
    } else {
        return false;
    }
}

void headers_list::clear()
{
    data.clear();
}

bool operator == (const headers_list& left, const headers_list& right)
{
    return left.copmare(right) == headers_list::EQ;
}

bool operator != (const headers_list& left, const headers_list& right)
{
    return !(left == right);
}

bool operator < (const headers_list& left, const headers_list& right)
{
    return left.copmare(right) == headers_list::LT;
}


headers_list::const_iterator cfind(const headers_list& on, const headers_list::value_type& entry)
{
    return std::find_if(on.begin(), on.end(), [&entry](const auto& i) { return equal_field(entry, i); } );
}

headers_list::const_iterator cfind(const headers_list& on, const headers_list::value_type::key_type& entry)
{
    return std::find_if(on.begin(), on.end(), [&entry](const auto& i) { return (i.name() == entry); } );
}

headers_list::iterator find(headers_list& on, const headers_list::value_type& entry)
{
    return std::find_if(on.begin(), on.end(), [&entry](const auto& i) { return equal_field(i, entry); } );
}

headers_list::iterator find(headers_list& on, const headers_list::value_type::key_type& entry)
{
    return std::find_if(on.begin(), on.end(), [&entry](auto& i) { return entry == i.name(); } );
}

bool has(const headers_list& on, const headers_list::value_type& entry) 
{
    return cfind(on, entry) != on.end();
}

bool has(const headers_list& on, const headers_list::value_type::key_type& v) 
{
    return cfind(on, v) != on.end();
}

headers_list& operator + (headers_list& headers, const headers_list::value_type& v)
{
    headers += v;
    return headers;
}

std::ostream& operator << (std::ostream& os, const headers_list& hl)
{
    std::for_each(std::begin(hl), std::end(hl), [&os] (const auto& hdr) { os<<hdr<<"\n"; } );
    return os;
}
bool keep_alive(const headers_list& hl, bool ver1)
{
    static const auto connection_header = "Connection"_hdrk;
    static const auto keep_alive_header = "Keep-Alive"_hdrk;
    if (ver1) {
        auto i = cfind(hl, keep_alive_header);
        return i != std::end(hl);
    } else {
        auto i = cfind(hl,  connection_header);
        if (i != std::end(hl) && i->value() == "close") {
            return false;
        }
    } 
    return true;
}


bool chunked_message(const headers_list& hl)
{
    static const auto encode = "Transfer-Encoding"_hdrk;
    static const auto cencode = "Chunked"_hdrv;
    //static const auto chunk_transfer = "chunked"_hdrk;
    auto i = cfind(hl, encode);
    if (i != hl.end() && i->value() == cencode) { 
        return true;
    } else {
        return false;
    }
}

encoding_type::encoding_type(const header_value& s)
{
    parse(s);
}

encoding_type& encoding_type::operator = (const header_value& s)
{
    parse(s);
    return *this;
}

bool encoding_type::none() const
{
    return !store.any();
}

bool encoding_type::gzip() const
{
    return store[GZIP];
}

void encoding_type::parse(const header_value& s)
{
    // we need to look up the string that can be the name of the encoding types
    // and set accodingly
    using mapping_entry = std::pair<std::string, int>;
    static const std::vector<mapping_entry> mapping{
        {"gzip", GZIP}, {"compress", COMPRESS}, 
        {"deflate", DEFLATE}, {"identity", IDENTITY},
        {"br", BR}
    };

    for (const auto& e : mapping) {
        if (boost::algorithm::icontains(s.get(), e.first)) {
            store[e.second] = 1;
        }
    }
}

bool encoding_type::compress() const
{
    return store[COMPRESS];
}


bool encoding_type::deflate() const
{
    return store[DEFLATE];
}

bool encoding_type::identety() const
{
    return store[IDENTITY];
}

bool encoding_type::br() const
{
    return store[BR];
}

std::ostream& operator << (std::ostream& os, const encoding_type& et)
{
    if (et.none()) {
        return os<<"none";
    } else {
        if (et.br()) {
            os<<"'br' ";
        }
        if (et.gzip()) {
            os<<"'gzip' ";
        }
        if (et.compress()) {
            os<<"'copress' ";
        }
        if (et.deflate()) {
            os<<"'deflate' ";
        }
        if (et.identety()) {
            os<<"'identety' ";
        }
    }
    return os;
}

encoding_type encoded_type(const headers_list& hl)
{
    static const auto hdr_name = "Content-Encoding"_hdrk;

    auto i = cfind(hl, hdr_name);
    if (i != std::end(hl)) {
        return encoding_type(i->value());
    } else {
        static const auto no_encoding = encoding_type();
        return no_encoding;
    }
}




}   // end of namespace http

namespace literals {
}

