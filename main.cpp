#include <iostream>
#include <boost/asio.hpp>
#include <ail/types.hpp>
#include "server.hpp"

int main()
{
	ushort port = 80;

	boost::asio::io_service io_service;
	http_server server(io_service);
	if(!server.launch_server(port))
	{
		std::cout << "Unable to bind port" << std::endl;
		return 1;
	}
	std::cout << "Running IO service" << std::endl;
	io_service.run();
	return 0;
}
