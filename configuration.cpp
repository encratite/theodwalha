#include <ail/configuration.hpp>
#include "configuration.hpp"

uword backlog;
ushort http_server_port;
std::size_t read_buffer_size;
std::size_t maximal_extended_buffer_size;
std::size_t maximal_request_size;
std::string temporary_file_directory;

bool parse_configuration_file(std::string const & path)
{
	ail::configuration configuration;
	if(!configuration.load(path))
	{
		std::cout << "Failed to load configuration file from \"" << path << "\"" << std::endl;
		return false;
	}

	try
	{
		backlog = configuration.number<uword>("backlog");
		http_server_port = configuration.number<ushort>("http_server_port");
		read_buffer_size = configuration.number<std::size_t>("read_buffer_size");
		maximal_extended_buffer_size = configuration.number<std::size_t>("maximal_extended_buffer_size");
		maximal_request_size = configuration.number<std::size_t>("maximal_request_size");
		temporary_file_directory = configuration.string("temporary_file_directory");
	}
	catch(ail::exception & exception)
	{
		std::cout << "Error in configuration file: " << exception.get_message() << std::endl;
		return false;
	}

	return true;
}
