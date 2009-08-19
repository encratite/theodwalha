#pragma once

#include <string>
#include <vector>
#include <map>

namespace http_protocol
{
	enum type
	{
		version_1_0,
		version_1_1
	};
}

namespace http_request_method
{
	enum type
	{
		get,
		post
	};
}

namespace http_form_content
{
	enum type
	{
		application_x_www_form_urlencoded,
		multipart_form_data
	};
}

struct quality_entry
{
	std::string name;
	float quality_value;

	quality_entry();
	quality_entry(std::string const & name);
	quality_entry(std::string const & name, float quality_value);
};

typedef std::vector<quality_entry> quality_entries;

struct http_request
{
	http_protocol::type protocol_version;
	http_request_method::type method;
	http_form_content::type content_type;
	std::size_t header_size;
	std::size_t content_length;

	bool
		has_content_type,
		has_content_length,
		keep_alive,
		gzip;

	std::string
		path,
		user_agent,
		host,
		referrer;

	quality_entries accepted_encodings;

	std::map<std::string, std::string>
		cookies,
		form_fields;

	http_request();
};

namespace process_header_result
{
	enum type
	{
		no_delimiter,
		error,
		success
	};
}

bool decode_path(std::string const & input, std::string & output);
process_header_result::type process_header(std::string const & input, http_request & output, std::string & error_message);
