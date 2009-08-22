#pragma once

#include <string>
#include <theodwalha/

class module_manager
{
public:
	module_manager(std::string const & module_directory);

	void process_request(http_request & request);

private:
};