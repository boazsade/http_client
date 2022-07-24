# http_client
source code for http client side, inlcudng support for compression connetent indentification. also has support for chunks
You can create both http and https connections, and this supports compressed and chunked messages as well.

## How to use this
See the subdirectory examples for use of this code

An example for building a put message to the server:
```cpp
void build_request(http_put& with, const char* resource, const std::string& host, const char* b)
{
    using namespace std::string_literals;
    using namespace http::literals;
        with<<http::resource{resource}<<http::v1_1
            <<("User-Agent"_hdrk + "curl/7.29.0"_hdrv)
            <<("Host"_hdrk + http::header_value(host))
            <<http::body_entry(http::body(b), "application/json"s);
}
```

## Note
- that this uses boost and openssl (need to install them to build), and it assumes c++ 20
- This library uses boost, speicfically it uses boost ASIO and boost iostreams. please also note that the compression/uncompress is done in boost iostrams that uses Z lib, so you need to have that one as well installed
- This was tested with G++ version 11 and on Ubuntu 22 LTS.
- You can test buid this by building and running the dockerfile here:
  docker build -t http_client . && docker run --rm -ti http_client
- You can also test builds with this docker, but please comment out the "entrypoint" in the docker and "mount" (-v) the location of these source code files there.
- to successfully build your version without docker, you can see the dependencies from the dockerfile.

## To Do's
- add support for async operations (these are supported in boost asio just need to add to the API).
- support for other http version than http 1/1.1


