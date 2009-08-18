#pragma once

#include <string>
#include <set>
#include <ail/types.hpp>

struct temporary_file_entry
{
	std::string name;
	ulong timestamp;

	temporary_file_entry(std::string const & name);
	bool operator<(temporary_file_entry const & other) const;
	bool operator==(temporary_file_entry const & other) const;
};

class temporary_file_manager
{
public:
	temporary_file_manager(std::string const & directory);
	std::string generate_name();

private:
	std::string directory;
	std::set<temporary_file_entry> files;

	void clean();
	void clean_all();
};