#include <ail/file.hpp>
#include <ail/random.hpp>
#include <ail/types.hpp>
#include <ail/time.hpp>
#include <ail/types.hpp>
#include <boost/foreach.hpp>
#include <theodwalha/temporary.hpp>
#include <theodwalha/configuration.hpp>

temporary_file_entry::temporary_file_entry(std::string const & name):
	name(name),
	timestamp(ail::time())
{
}

bool temporary_file_entry::operator<(temporary_file_entry const & other) const
{
	return name < other.name;
}

bool temporary_file_entry::operator==(temporary_file_entry const & other) const
{
	return name == other.name;
}

temporary_file_manager::temporary_file_manager(std::string const & directory):
	directory(directory)
{
}

bool temporary_file_manager::initialise()
{
	ail::create_directory(directory);
	return clean_all();
}

char get_random_character(char initial_value, uword maximum)
{
	return static_cast<char>(initial_value + ail::random_integer<uword>(0, maximum - 1));
}

std::string temporary_file_manager::generate_name()
{
	while(true)
	{
		std::string name;
		for(std::size_t i = 0; i < temporary_file_name_length; i++)
		{
			if(ail::random_integer<uword>(0, 1) == 0)
				name.push_back(get_random_character('0', 10));
			else
				name.push_back(get_random_character('a', 26));
		}
		std::string path = ail::join_paths(directory, name);
		if(!ail::file_exists(name))
		{
			boost::mutex::scoped_lock scoped_lock(mutex);
			files.insert(temporary_file_entry(name));
			return path;
		}
	}
}

void temporary_file_manager::release(std::string const & path)
{
	boost::mutex::scoped_lock scoped_lock(mutex);

	if(ail::remove_file(path))
		files.erase(path);
}

bool temporary_file_manager::clean()
{
	boost::mutex::scoped_lock scoped_lock(mutex);

	ulong current_time = ail::time();
	string_vector removed_strings;
	BOOST_FOREACH(temporary_file_entry & file_entry, files)
	{
		if(current_time - file_entry.timestamp < temporary_file_expiration)
			continue;

		if(ail::remove_file(file_entry.name))
			removed_strings.push_back(file_entry.name);
	}

	BOOST_FOREACH(std::string const & path, removed_strings)
		files.erase(path);

	return true;
}

bool temporary_file_manager::clean_all()
{
	boost::mutex::scoped_lock scoped_lock(mutex);

	string_vector
		files,
		directories;

	if(!ail::read_directory(directory, files, directories))
		return false;

	BOOST_FOREACH(std::string const & file, files)
	{
		std::string path = ail::join_paths(directory, file);
		//std::cout << "Deleting " << path << std::endl;
		ail::remove_file(path);
	}

	return true;
}
