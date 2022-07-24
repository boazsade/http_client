#include "http_connection.h"
#include  <memory>


namespace http
{


connection::connection(boost::asio::io_service& ios) : socket_(ios)
{
}

void connection::init(boost::asio::io_service&)
{
}

connection::socket_type* connection::get()
{
    return &socket_;
}

bool connection::is_open() const
{
    return socket_.is_open();
}

connection::socket_type::lowest_layer_type* connection::stream_handle()
{
    return  &(socket_.lowest_layer());
}

///////////////////////////////////////////////////////////////////////////////
//


shared_connection::shared_connection(boost::asio::io_service& ios) :
                                        socket_(std::make_unique<socket_type>(ios))
{
}


void shared_connection::init(boost::asio::io_service& ios)
{
   if (!socket_) {  // we don't allow for reseting only in case that this was not set in the first place!
      socket_ = std::make_unique<socket_type>(ios);
   } 
}

shared_connection::socket_type* shared_connection::get()
{
    return socket_.get();
}

const shared_connection::socket_type* shared_connection::get() const
{
    return socket_.get();
}

bool shared_connection::is_open() const
{
    return get() && socket_->is_open();
}

shared_connection::socket_type::lowest_layer_type* shared_connection::stream_handle()
{
    if (get()) {
        return &(socket_->lowest_layer());
    }
    return nullptr;
}

}	// end of namespace http
