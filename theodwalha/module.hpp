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

namespace request_handler_return_code
{
	enum type
	{
		ok,
		forbidden,
		not_found
	};
}

struct module_result
{
	request_handler_command::type command;
	request_handler_result::type result;
	std::string error_message;
	request_handler_return_code::type return_code;

	std::string
		content_type,
		content;

	void success();
	void text();
	void html();
	void set_type(std::string const & new_content_type);
};

typedef void request_handler_function_type(http_request & request, module_result & output);
typedef boost::function<request_handler_function_type> request_handler_function;
