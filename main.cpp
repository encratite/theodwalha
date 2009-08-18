#include <iostream>
#include <boost/asio.hpp>
#include <ail/types.hpp>
#include "server.hpp"
#include "configuration.hpp"

bool run_server()
{
	boost::asio::io_service io_service;
	http_server server(io_service);
	if(!server.launch_server(http_server_port))
	{
		std::cout << "Unable to bind port" << std::endl;
		return false;
	}
	std::cout << "Running IO service" << std::endl;
	io_service.run();
	return true;
}

int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << argv[0] << " <configuration file>" << std::endl;
		return 1;
	}

	std::string configuration_file_path = argv[1];
	if(!parse_configuration_file(configuration_file_path))
		return 1;

	if(!run_server())
		return 1;

	return 0;
}
