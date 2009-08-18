#pragma once

#include <string>
#include <set>
#include <ail/types.hpp>
#include <boost/thread/mutex.hpp>

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
	bool initialise();
	std::string generate_name();
	void release(std::string const & path);

private:
	std::string directory;
	std::set<temporary_file_entry> files;
	boost::mutex mutex;

	bool clean();
	bool clean_all();
};