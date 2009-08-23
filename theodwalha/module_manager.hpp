#pragma once

#include <string>
#include <vector>
#include <ail/module.hpp>
#include <theodwalha/module.hpp>

typedef std::vector<ail::dynamic_module *> module_vector;
typedef std::vector<request_handler_function> request_handler_vector;

class module_manager
{
public:
	bool load_modules(std::string const & directory);
	void process_request(http_request & request);

private:
	module_vector modules;
	request_handler_vector request_handlers;
};
