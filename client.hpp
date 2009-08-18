#pragma once

#include <boost/asio.hpp>

struct http_server_client
{
	boost::asio::ip::tcp::socket socket;
	char * read_buffer;
	std::string read_data;

	http_server_client(boost::asio::io_service & io_service);
	~http_server_client();

	void start();
	void read();
	void terminate();

	void read_event(boost::system::error_code const & error, std::size_t bytes_transferred);
};
