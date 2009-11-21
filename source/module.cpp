#include <theodwalha/module.hpp>

void module_result::success()
{
	command = request_handler_command::stop;
	result = request_handler_result::success;
	return_code = request_handler_return_code::ok;
}

void module_result::text()
{
	set_type("text/plain");
}

void module_result::html()
{
	set_type("text/html");
}

void module_result::set_type(std::string const & new_content_type)
{
	content_type = new_content_type;
	success();
}
