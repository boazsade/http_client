#pragma once
#include <string>
#include <iosfwd>
#include <vector>
#include <initializer_list>
#include <bitset>


namespace http
{
// the command is the first entry in the message comming from the 
// client to the server,
// it contains the command type (see bellow),
// the resource name and the http version
struct delimiter
{
    const char* value() const;
};

struct header_key
{
    header_key() = default;
    header_key(const header_key&) = default;
    header_key(header_key&&) = default;

    explicit header_key(const char* str, std::size_t s);

    explicit header_key(const std::string& v);

    header_key& operator = (const header_key& ) = default;
    header_key& operator = (header_key&& ) = default;

    header_key& operator = (const std::string& s);

    const std::string& get() const;
private:
    std::string key;

};

bool operator == (const header_key& left, const header_key& right);
bool operator == (const header_key& left, const std::string& right);
bool operator == (const std::string& left, const header_key& right);
bool operator != (const header_key& left, const header_key& right);
bool operator != (const header_key& left, const std::string& right);
bool operator != (const std::string& left, const header_key& right);
bool operator < (const header_key& left, const header_key& right);
bool operator < (const header_key& left, const std::string& right);
bool operator < (const std::string& left, const header_key& right);
bool operator > (const header_key& left, const header_key& right);
bool operator <= (const header_key& left, const header_key& right);
bool operator >= (const header_key& left, const header_key& right);

std::ostream& operator << (std::ostream& os, const header_key& k);


struct header_value 
{
    header_value() = default;
    header_value(const header_value&) = default;
    header_value(header_value&&) = default;
    ~header_value() = default;

    explicit header_value(const char* str, std::size_t s);

    explicit header_value(const std::string& v);

    header_value& operator = (const header_value&) = default;
    header_value& operator = (header_value&&) = default;

    header_value& operator = (const std::string& s);

    const std::string& get() const;

    bool valid() const;

    using boolean_type = bool(header_value::*)()const;

    operator boolean_type() const
    {
        return valid() ? &header_value::valid : (boolean_type)nullptr;
    }

private:
    std::string value;
};

const header_value bad_http_value = header_value();

bool operator == (const header_value& left, const header_value& right);
bool operator == (const std::string& left, const header_value& right);
bool operator == (const header_value& left, const std::string& right);
bool operator != (const header_value& left, const header_value& right);
bool operator != (const std::string& left, const header_value& right);
bool operator != (const header_value& left, const std::string& right);
bool operator < (const header_value& left, const header_value& right);
bool operator < (const header_value& left, const std::string& right);
bool operator < (const std::string& left, const header_value& right);
bool operator > (const header_value& left, const header_value& right);
bool operator <= (const header_value& left, const header_value& right);
bool operator >= (const header_value& left, const header_value& right);

std::ostream& operator << (std::ostream& os, const header_value& k);


// http message is a list of headers that 
// we are placing with the sequence \r and \n
// at the end. each entry is a tuple of
// name of the header follow with the value
struct header 
{
    using key_type = header_key;
    using mapped_type = header_value;

    header() = default; // make invalid header
    header(const header&) = default; // make invalid header
    header(header&&) = default; // make invalid header
    header& operator = (header&&) = default; // make invalid header
    header& operator = (const header&) = default; // make invalid header

    header(const header_key& name, const header_value& value);

    const header_key& name() const;

    const header_value& value() const;

private:
    header_key _name;
    header_value _value;
};

const std::string& key_name(const header& h);
const std::string& value_name(const header& h);
// compare only based on the header name
bool equal_field(const header& left, const header& right);
bool lessthan_field(const header& left, const header& right);
bool different_field(const header& left, const header& right);

bool operator == (const header& left, const header& right);
bool operator != (const header& left, const header& right);
bool operator < (const header& left, const header& right); 
bool operator > (const header& left, const header& right); 
bool operator >= (const header& left, const header& right); 
bool operator <= (const header& left, const header& right);

header operator + (const header_key& k, const header_value& v);
header operator + (header_key&& k, header_value&& v);
std::ostream& operator << (std::ostream& os, const header& h);

header make_header(const char* name, const char* value);
header make_header(const std::string& name, const std::string& value);
header make_header(const std::string& line);    // assume that this is a line read from the http response or request


constexpr delimiter the_delimiter = delimiter();
const header invalid_header = header();


// this class would allow holding a list of headers that have in the http message
// what we would like is to be able to retrive them based on the name of the value
class headers_list 
{
public:
    using value_type = header;
private:
    using data_type = std::vector<value_type>;
public:
    using iterator = data_type::iterator;
    using const_iterator = data_type::const_iterator;

    enum comp_res {
        LT = -1, EQ, GT
    };

    headers_list() = default;
    headers_list(headers_list&&) = default; 
    headers_list(const headers_list&) = default; 
    headers_list& operator = (const headers_list&) = default; 
    headers_list& operator = (headers_list&&) = default; 

    headers_list(std::initializer_list<value_type> init_list);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    bool empty() const;
    std::size_t size() const;

    // both function would return default empty mapped_type in case we failed to find, make sure that you check that..
    const value_type::mapped_type& operator [] (const value_type::key_type& key) const;

    bool insert(const value_type& v);                   // post condition empty() == false, return false if value is already in the list
    bool insert(value_type&& v);                        // move into

    headers_list& operator += (const value_type& v);    // post condition empty() == false
    headers_list& operator += (value_type&& v);    // post condition empty() == false

    bool erase(const value_type& v);
    bool erase(const value_type::key_type& v);

    void clear();   // post condition - empty() == true

    comp_res copmare(const headers_list& other) const;
private:
    data_type data;
};

bool operator == (const headers_list& left, const headers_list& right);
bool operator != (const headers_list& left, const headers_list& right);
bool operator < (const headers_list& left, const headers_list& right);

headers_list::const_iterator cfind(const headers_list& on, const headers_list::value_type& entry);
headers_list::const_iterator cfind(const headers_list& on, const headers_list::value_type::key_type& entry);
headers_list::iterator find(headers_list& on, const headers_list::value_type& entry);
headers_list::iterator find(headers_list& on, const headers_list::value_type::key_type& entry);
bool has(const headers_list& on, const headers_list::value_type::key_type& entry) ;
bool has(const headers_list& on, const headers_list::value_type& v) ;
headers_list& operator + (headers_list& headers, const headers_list::value_type& v);

// call this function to build a list of all headers, assumes that status line as removed
headers_list make_headers(const std::string& header_lines); 

// some usefull functions to query about the message headers
bool chunked_message(const headers_list& hl);
bool keep_alive(const headers_list& hl, bool ver1);   // you need to tell this function if this is a version 1.0 or 1.1

struct encoding_type {
    encoding_type() = default;
    encoding_type(const header_value& s);
    encoding_type(encoding_type&&) = default;
    encoding_type(const encoding_type&) = default;
    encoding_type& operator = (const encoding_type&) = default;
    encoding_type& operator = (encoding_type&&) = default;
    encoding_type& operator = (const header_value& from);

    bool none() const;
    bool gzip() const;
    bool compress() const;
    bool deflate() const;
    bool identety() const;
    bool br() const;

private:

    void parse(const header_value& s);

    enum {
        NOTENCODED = 0,
        GZIP = 1,
        COMPRESS,
        DEFLATE,
        IDENTITY,
        BR
    };
    std::bitset<8> store;
};

std::ostream& operator << (std::ostream& os, const encoding_type& et);
// note that we can have multiple encoding in the same message!!
encoding_type encoded_type(const headers_list& hl); 

std::ostream& operator << (std::ostream& os, const headers_list& hl);

inline namespace literals
{
inline http::header_key operator "" _hdrk(const char* v)
{
    return http::header_key(v);
}

inline http::header_key operator "" _hdrk(const char* v, std::size_t s)
{
    return http::header_key(v, s);
}

inline http::header_value operator "" _hdrv(const char* v, std::size_t s)
{
    return http::header_value(v, s);
}

inline http::header_value operator "" _hdrv(const char* v)
{
    return http::header_value(v);
}


}       // end of namespace literals
}	// end of namespace http

