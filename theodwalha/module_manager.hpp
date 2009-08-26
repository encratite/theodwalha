#pragma once

#include <string>
#include <vector>
#include <ail/module.hpp>
#include <theodwalha/module.hpp>

struct module_entry
{
	std::string name;
	ail::dynamic_module * module_pointer;
	request_handler_function handler;
};

class module_manager
{
public:
	bool load_modules(std::string const & directory);
	bool process_request(http_request & request, module_result & result);

private:
	std::vector<module_entry> module_entries;
};
