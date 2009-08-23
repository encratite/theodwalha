#pragma once

#include <string>
#include <boost/function.hpp>
#include <theodwalha/request.hpp>

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

typedef module_result request_handler_function_type(http_request & request);
typedef boost::function<request_handler_function_type> request_handler_function;
