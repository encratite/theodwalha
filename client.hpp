#pragma once

#include <string>
#include <ail/file.hpp>
#include <boost/asio.hpp>
#include "temporary.hpp"

struct http_server_client
{
	boost::asio::ip::tcp::socket socket;
	char * read_buffer;
	std::string extended_buffer;
	std::string temporary_file_name;
	temporary_file_manager & file_manager;
	ail::file temporary_file;
	std::size_t bytes_read;

	http_server_client(boost::asio::io_service & io_service, temporary_file_manager & file_manager);
	~http_server_client();

	void start();
	void read();
	void terminate();

	void read_event(boost::system::error_code const & error, std::size_t bytes_read);
};
