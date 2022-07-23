#pragma once
#include "http_error.h"
//#include "http_read.h"
#include "http_response.h"
#include "http_request.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <type_traits>



struct length_reader {
    explicit length_reader(std::size_t s) : expected_size{s}
    {
    }

    template<typename It>
    std::pair<It, bool>
    operator () (It begin, It end) const
    {
        const auto diff = std::distance(begin, end);
        //std::cout<<"we have "<<diff<<" bytes, we have "<<expected_size<<" still to read"<<std::endl;
        if (diff < 0 || expected_size == 0) {
            //std::cout<<"finish reading - we read what we expected"<<std::endl;
            return std::make_pair(begin, true);
        } else if (diff == 0) {
            return std::make_pair(begin, expected_size == 0);
        } else {    // check if we read enough
            const auto step = std::min((std::size_t)diff, expected_size);
            //std::cout<<"moving forward "<<step<<" number of bytes"<<std::endl;
            expected_size -= (std::size_t)step;
            std::advance(begin, step);
            //std::cout<<"we now have  "<<expected_size<<" bytes to read"<<std::endl;
            return std::make_pair(begin, expected_size == 0);
        }
    }

private:
    mutable std::size_t expected_size;
};

struct chucked_matcher {
    chucked_matcher()
    {
    }

    template<typename It>
    std::pair<It, bool>
    operator () (It begin, It ) const
    {
        return std::make_pair(begin, true);
    }
};

namespace boost { namespace asio {
    template <> struct is_match_condition<length_reader>
        : public boost::true_type {};

    template <> struct is_match_condition<chucked_matcher>
        : public boost::true_type {};
} // namespace asio 
}   // namespace boost


namespace http
{


template<typename Conn>
std::string get_message_body(Conn& io, const status_line& sline,
                             const headers_list& hl);

template<typename Conn>
std::string read_eof(Conn& input);

template<typename Conn>
std::size_t content_length(Conn& input,  const headers_list& hl);
template<typename Conn>
std::size_t read_chunk_len(Conn& socket);
template<typename Conn>
std::string read_chunks(Conn& input, std::size_t len); 

template<typename Conn, typename Cond>
std::string read_body(Conn& socket, Cond c);


template<typename Conn>
status_line read_status(Conn& input);

template<typename Conn>
headers_list read_headers(Conn& input);

template<typename Conn>
body message_body(Conn& io, const headers_list& header, const status_line& sl);

template<typename Conn>
response get_response(Conn& io);

template<typename R>
bool save_headers_(R& input, headers_list& headers);
///////////////////////////////////////////////////////////////////////////////

template<typename Conn, typename Cond> inline
std::string read_body(Conn& input, Cond pred)
{   
    const auto bytes = input.read_until(pred);
    //std::cout<<"read "<<bytes<<" bytes so far from the server (we have "<<res.size()<<" in the buffer"<<std::endl;
    return  bytes != 0 ?
       string_cast(input, bytes) : std::string{};
}

template<typename R>
inline bool save_headers_(R& input, headers_list& headers)
{
    // loop over the "lines" in the message and convert them to header lines
    auto header = sub_string(input, '\n', '\r');
    while (not std::empty(header) && header != the_delimiter.value()) {
        headers.insert(make_header(header));
        header = sub_string(input, '\n', '\r');
    }
    return not std::empty(headers);
}

struct read_headers_ref
{
    template<typename Reader>
    static bool run(Reader& input, headers_list& headers)
    {
        return save_headers_(input, headers);
    }
};

struct read_headers_c
{
    template<typename R>
    static bool run(R r, headers_list& h)
    {
        return save_headers_(r, h);
    }
};

template<typename Conn> inline 
std::string read_eof(Conn& input)
{
    // read until we have EOF "error"
    const std::string body = string_cast(input);   // first extract whatever we still have in the buffer
    //std::cout<<"message body so far "<<body.size()<<std::endl;
    input.read_all();
    //std::cout<<"successfully read "<<buff.size()<<" ( "<<s<<" )"<<std::endl;
    return body + string_cast(input);
}

template<typename Conn> inline 
std::size_t read_chunk_len(Conn& socket)
{
    // the chunk length is the first line after the headers, find this line
    // and read the length of the chunk
    static const std::string term = "\r\n";

    const auto i =   socket.read(term);
    if (i > 0) {
        const auto cls = string_cast(socket, '\n', '\r');
        if (not std::empty(cls)) { // make sure that we have this line!
            //std::cout<<"found chunked length to be '"<<cls<<"'"<<std::endl;
            return strtol(cls.c_str(), nullptr, 16);
        }
    } 
    return 0;
}

template<typename Conn> inline 
std::string read_chunks(Conn& socket, std::size_t len)
{
    std::string body;
    std::size_t s = 0;
    while (len > 0) {
        //std::cout<<"trying to read message with length "<<len<<std::endl;
        body += read_body(socket, length_reader(len));
        //std::cout<<"read chunk looking for the next one"<<std::endl;
        len = read_chunk_len(socket);
        s += len;
    }
    if (len == 0) {
        const std::string el{std::to_string(len)};
        throw network_error("failed to read message body " + el);
    }
    return body;
}

template<typename Conn> inline 
std::size_t content_length(Conn& socket,  const headers_list& hl)
{
    using namespace http::literals;

    static const auto length_header = "Content-Length"_hdrk;

    std::size_t ret = 0;
    if (chunked_message(hl)) {   // handle the case with chunked data to tell what is the length
        ret = read_chunk_len(socket);
    }
    if (ret == 0) {
        //std::cout<<"no chunked length trying to read "<<length_header<<std::endl;
        const auto i = cfind(hl, length_header);
        if (i != std::end(hl)) {
            //std::cout<<"find length header "<<*i<<std::endl;
            ret = boost::lexical_cast<std::size_t>(i->value().get());
        }
    }
    return ret;
}

template<typename Conn> inline 
std::string get_body_by_len(Conn& io, const headers_list& hl, bool chunked)
{
    std::string msg_body;
    auto len = content_length(io, hl);
    if (len > 0) {  // otherwise this is an error
        if (chunked) { // we need to read until we have 0 len chunk
            msg_body = read_chunks(io, len);
        } else {
            msg_body = read_body(io, length_reader(len));
            if (msg_body.empty()) {
                throw network_error("failed to read body");
            }
        }
    }
    return msg_body;
}

template<typename Conn> inline 
std::string get_message_body(Conn& io, const status_line& sline,
                             const headers_list& hl)
{
    std::string msg_body;
    if (no_content(sline)) {
        return msg_body;
    }
    const auto keepalive = keep_alive(hl, sline.version_1());
    const auto chunked = chunked_message(hl);
    if (keepalive || chunked) {      // just read to the end of the message
        return get_body_by_len(io, hl, chunked);
    } else {
        msg_body = read_eof(io);
    } 
    return msg_body;

}

template<typename Conn> inline 
status_line read_status(Conn& input)
{
    const auto bytes = input.read(the_delimiter.value());
    if (bytes) {
        return status_line {string_cast(input, '\n', '\r')};
    } else {
        static const status_line bad_line = status_line{};
        return bad_line;
    }
}

template<typename Conn> inline 
any_request_line read_requst_line(Conn& input)
{
    const auto bytes = input.read(the_delimiter.value());
    if (bytes) {
        return any_request_line {string_cast(input, '\n', '\r')};
    } else {
        static const auto bad_line = any_request_line{};
        return bad_line;
    }
}

template<typename Conn> inline 
headers_list read_headers(Conn& input)
{
    static const std::string header_term = "\r\n\r\n";

    using f_call = typename boost::mpl::if_c<boost::is_reference<typename std::result_of<decltype(&Conn::read_data)(Conn)>::type>::value,
                                             read_headers_ref, read_headers_c>::type;
    headers_list hl;
    input.read(header_term);
            //std::cout<<"finish reading headers\n";
    if (f_call::run(input.read_data(), hl)) {
        input.commit();
        return hl;
    } else {
        static const auto bad_headers = headers_list{};
        return bad_headers;
    }
}

template<typename Conn> inline 
body message_body(Conn& io, 
                  const headers_list& headers, const status_line& sl)
{
    //if (sl || moved(sl)) {
        std::string msg_body = get_message_body(io, sl, headers);
        return body{msg_body};
    //} else {    // no body is expeced
    //    static const body empty_body = body{};
    //    return empty_body;
    //}
}

template<typename Conn> inline
response get_response(Conn io)
{
    static const response bad_response = response{};
    try {
        const auto sl = read_status(io);
        const auto headers = read_headers(io);
        if (!headers.empty()) {
            const auto b = message_body(io, headers, sl);
            return response{sl, headers, b};
        } else {
            return response{sl};
        }
    } catch (const std::exception& e) {
        static const std::string err_msg = "http client read error: ";
        throw network_error(err_msg + e.what());
    }
    return bad_response;
}

}   // end of namespace http

