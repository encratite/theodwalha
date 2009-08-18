#include <boost/bind.hpp>
#include <iostream>
#include <cstdlib>
#include "client.hpp"
#include "configuration.hpp"
#include "temporary.hpp"

http_server_client::http_server_client(boost::asio::io_service & io_service, temporary_file_manager & file_manager):
	socket(io_service),
	file_manager(file_manager)
{
	read_buffer = new char[read_buffer_size];
}

http_server_client::~http_server_client()
{
	delete read_buffer;
}

void http_server_client::start()
{
	bytes_received = 0;
	read();
}

void http_server_client::read()
{
	socket.async_read_some(boost::asio::buffer(read_buffer, read_buffer_size), boost::bind(&http_server_client::read_event, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_read));
}

void http_server_client::terminate()
{
	std::cout << "Terminating" << std::endl;
	if(!temporary_file_name.empty())
	{
		temporary_file.close();
		file_manager.release(temporary_file_name);
	}
	delete this;
}

void http_server_client::read_event(boost::system::error_code const & error, std::size_t bytes_read)
{
	if(!error)
	{
		std::cout << "Read " << bytes_read << " bytes:" << std::endl;
		bytes_received += bytes_read;

		if(bytes_received > maximal_request_size)
		{
			std::cout << "Request too large" << std::endl;
			terminate();
			return;
		}

		std::string new_data(read_buffer, bytes_read);

		if(temporary_file_name.empty())
		{
			extended_buffer += new_data;

			//std::cout << extended_buffer << std::endl;

			if(extended_buffer.size() > maximal_extended_buffer_size)
			{
				temporary_file_name = file_manager.generate_name();
				if(!temporary_file.open_create(temporary_file_name))
				{
					std::cout << "Failed to open temporary file " << temporary_file_name << std::endl;
					exit(1);
				}

				temporary_file.write(extended_buffer);
				extended_buffer.clear();

				std::cout << "Using temporary file name " << temporary_file_name << std::endl;
			}
		}
		else
			temporary_file.write(new_data);

		read();
	}
	else
		terminate();
}