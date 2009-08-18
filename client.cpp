#include <boost/bind.hpp>
#include <iostream>
#include "client.hpp"

namespace
{
	std::size_t const read_buffer_size = 1024;
}

http_server_client::http_server_client(boost::asio::io_service & io_service):
	socket(io_service)
{
	read_buffer = new char[read_buffer_size];
}

http_server_client::~http_server_client()
{
	delete read_buffer;
}

void http_server_client::start()
{
	std::cout << "Reading" << std::endl;
	read();
}

void http_server_client::read()
{
	socket.async_read_some(boost::asio::buffer(read_buffer, read_buffer_size), boost::bind(&http_server_client::read_event, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void http_server_client::terminate()
{
	std::cout << "Terminating" << std::endl;
	delete this;
}

void http_server_client::read_event(boost::system::error_code const & error, std::size_t bytes_transferred)
{
	if(!error)
	{
		std::cout << "Read " << bytes_transferred << " bytes:" << std::endl;

		read_data.append(read_buffer, bytes_transferred);
		std::cout << read_buffer << std::endl;

		read();
	}
	else
		terminate();
}