# http_client
source code for http client side, inlcudng support for compression connetent indentification. also has support for chunks

Note
This library uses boost, speicfically it uses boost ASIO and boost iostreams. please also note that the compression/uncompress is done in boost iostrams that uses Z lib, so you need to have that one as well installed

How to use this
See the subdirectory examples for use of this code

Note that this uses boost and openssl (need to install them to build), and it assumes c++ 20
