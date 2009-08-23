#include <iostream>
#include <ail/file.hpp>
#include <boost/foreach.hpp>
#include <theodwalha/module_manager.hpp>

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

		modules.push_back(new_module);
	}

	return true;
}

void module_manager::process_request(http_request & request)
{
}
