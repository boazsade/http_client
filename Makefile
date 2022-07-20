# this would be used for building the source here, this assumes that you are using this throug the docker file!
# if you are trying to use this make file, you need to setup the correct path to boost
CXX=g++
CXX_VER=-std=c++20
CXX_GLAGS=-Wall -Wextra
BOOST_INCLUDE=-isystem
