#pragma once

#include <ail/types.hpp>
#include <boost/asio.hpp>
#include "client.hpp"

class http_server
{
public:

	http_server(boost::asio::io_service & io_service);

	bool launch_server(ushort new_port);

private:

	bool running;
	ushort port;

	boost::asio::io_service & io_service;
	boost::asio::ip::tcp::acceptor acceptor;

	void accept();

	void accept_event(boost::system::error_code const & error, http_server_client * new_client);
};
