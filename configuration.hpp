#pragma once

#include <string>
#include <ail/types.hpp>

extern uword backlog;
extern ushort http_server_port;
extern std::size_t read_buffer_size;
extern std::size_t maximal_extended_buffer_size;
extern std::size_t maximal_request_size;
extern std::string temporary_file_directory;
extern ulong temporary_file_expiration;

bool parse_configuration_file(std::string const & path);
