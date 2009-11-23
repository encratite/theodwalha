#include <iostream>
#include <ail/file.hpp>
#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include <theodwalha/module_manager.hpp>
#include <theodwalha/configuration.hpp>

bool module_manager::load_modules(std::string const & directory)
{
	string_vector
		files,
		directories;

	if(!ail::read_directory(directory, files, directories))
		return false;

	BOOST_FOREACH(std::string const & file, files)
	{
		std::string path = ail::join_paths(directory, file);

		ail::dynamic_module * new_module = new ail::dynamic_module();
		if(!new_module->load(path))
		{
			std::cout << "Failed to load module " << path << std::endl;
			return false;
		}

		void * function_pointer;
		if(!new_module->get_function("get_process_request_address", function_pointer))
		{
			std::cout << "Failed to retrieve function pointer from module " << path << std::endl;
			return false;
		}

		std::cout << "Loaded " << path << std::endl;

		typedef void * (* get_process_request_address_type)();
		get_process_request_address_type get_process_request_address = reinterpret_cast<get_process_request_address_type>(function_pointer);
		void * void_pointer = get_process_request_address();

		//std::cout << "get_process_request_address returned: " << ail::hex_string_32(reinterpret_cast<uword>(void_pointer)) << std::endl;

		module_entry current_entry;
		current_entry.name = path;
		current_entry.module_pointer = new_module;
		current_entry.handler = reinterpret_cast<request_handler_function_type *>(void_pointer);
		module_entries.push_back(current_entry);
	}

	return true;
}

bool module_manager::process_request(http_request & request, module_result & result)
{
	std::cout << "module_manager::process_request" << std::endl;

	BOOST_FOREACH(module_entry & entry, module_entries)
	{
		std::cout << entry.name << ":" << std::endl;

		entry.handler(request, result);

		bool output = false;

		switch(result.result)
		{
			case request_handler_result::no_match:
				std::cout << "No match" << std::endl;
				break;

			case request_handler_result::success:
				std::cout << "Success!" << std::endl; 
				output = true;
				break;

			case request_handler_result::error:
				std::cout << "Error: " << result.error_message << std::endl;
				break;

			default:
				std::cout << "Invalid result specified: " << ail::hex_string_32(static_cast<uword>(result.result)) << std::endl;
				break;
		}

		if(result.command == request_handler_command::stop)
			return output;
	}

	std::cout << "None of the modules succeeded at processing the request" << std::endl;

	return false;
}
