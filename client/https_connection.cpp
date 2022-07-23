#include "https_connection.h"


namespace http
{

void sec_connection_base::init(int sm)
{
    sec_method = sm;
}
sec_connection_base::sec_connection_base(int vm) : sec_method(vm)
{
}

int sec_connection_base::security_method() const
{
    return sec_method;
}


secure_connection::secure_connection(boost::asio::io_service& ios, security_args& sa) : sec_connection_base(sa.security_method()), socket_(ios, sa.security_handle())
{
}

void secure_connection::init(boost::asio::io_service&, security_args&)
{
}

secure_connection::socket_type* secure_connection::get()
{
    return &socket_;
}

boost::asio::io_service* secure_connection::service_handle()
{
    return &socket_.lowest_layer().get_io_service();
}

bool secure_connection::is_open() const
{
    return socket_.lowest_layer().is_open();
}

secure_connection::socket_type::lowest_layer_type* secure_connection::stream_handle()
{
    return &socket_.lowest_layer();
}

///////////////////////////////////////////////////////////////////////////////
//


secure_shared_connection::secure_shared_connection(boost::asio::io_service& ios, security_args& sa) : 
                                        sec_connection_base(sa.security_method()),
                                        socket_(std::make_unique<socket_type>(ios, sa.security_handle()))
{
}

void secure_shared_connection::init(boost::asio::io_service& ios, security_args& sa)
{
   if (!socket_) {  // we don't allow for reseting only in case that this was not set in the first place!
      socket_ = std::make_unique<socket_type>(ios, sa.security_handle());
      sec_connection_base::init(sa.security_method());
   } 
}

secure_shared_connection::socket_type* secure_shared_connection::get()
{
    return socket_.get();
}

const secure_shared_connection::socket_type* secure_shared_connection::get() const
{
    return socket_.get();
}

// boost::asio::io_service* secure_shared_connection::service_handle()
// {
//     return get() ? &(socket_->lowest_layer().get_io_service()) : nullptr;
// }

bool secure_shared_connection::is_open() const
{
    return get() && socket_->lowest_layer().is_open();
}

secure_shared_connection::socket_type::lowest_layer_type* secure_shared_connection::stream_handle()
{
    if (get()) {
        return &(socket_->lowest_layer());
    } else {
        return nullptr;
    }
}

}	// end of namespace http
