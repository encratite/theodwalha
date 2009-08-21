#pragma once

#include <string>
#include "request.hpp"

namespace request_handler_command
{
	enum type
	{
		continue_processing,
		stop
	};
}

namespace request_handler_result
{
	enum type
	{
		no_match,
		success,
		error
	};
}

struct module_result
{
	request_handler_command::type command;
	request_handler_result::type result;
	std::string error_message;
};

typedef boost::function<module_result (http_request & request)> request_handler_function;
