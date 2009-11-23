#include <boost/bind.hpp>
#include <iostream>
#include <theodwalha/server.hpp>
#include <theodwalha/configuration.hpp>

http_server::http_server(boost::asio::io_service & io_service, std::string const & temporary_file_directory):
	running(false),
	io_service(io_service),
	acceptor(io_service),
	file_manager(temporary_file_directory)
{
}

bool http_server::launch_server(ushort new_port)
{
	if(running)
	{
		std::cout << "Server is already running" << std::endl;
		return false;
	}

	if(!file_manager.initialise())
	{
		std::cout << "Failed to initialise temporary file manager" << std::endl;
		return false;
	}

	if(!modules.load_modules(module_directory))
		return false;

	boost::system::error_code error;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), new_port);
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint, error);
	acceptor.listen(backlog);
	if(!error)
	{
		running = true;
		port = new_port;
		accept();
		return true;
	}
	else
	{
		std::cout << "Failed to bind acceptor on port " << new_port << ": " << error << std::endl;
		return false;
	}
}

void http_server::accept()
{
	http_server_client * new_client = new http_server_client(io_service, file_manager, modules);
	acceptor.async_accept(new_client->socket, boost::bind(&http_server::accept_event, this, boost::asio::placeholders::error, new_client));
}

void http_server::accept_event(boost::system::error_code const & error, http_server_client * new_client)
{
	if(!error)
	{
		boost::asio::ip::tcp::endpoint endpoint = new_client->socket.remote_endpoint();
		std::cout << "New connection from " << endpoint.address().to_string() << ":" << endpoint.port() << std::endl;
		new_client->start();
		accept();
	}
	else
	{
		std::cout << "Accept error: " << error << std::endl;
		delete new_client;
	}
}
