#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include "request.hpp"

namespace
{
	std::string const delimiter = "\r\n";
	std::string end_of_header = delimiter + delimiter;
}

quality_entry::quality_entry()
{
}

quality_entry::quality_entry(std::string const & name):
	name(name),
	quality_value(1.0f)
{
}

quality_entry::quality_entry(std::string const & name, float quality_value):
	name(name),
	quality_value(quality_value)
{
}

http_request::http_request():
	has_content_type(false),
	has_content_length(false),
	keep_alive(false),
	gzip(false),
	content_length(0)
{
}

bool decode_string(std::string const & input, std::string & output)
{
	for(std::size_t i = 0, end = input.size(); i < end; i++)
	{
		char byte = input[i];
		if(byte == '%')
		{
			i++;

			std::size_t const hex_length = 2;

			std::size_t remaining_bytes = end - i;
			if(remaining_bytes < hex_length)
				return false;

			std::string hex_string = input.substr(i, hex_length);

			char hex_byte;
			if(!ail::string_to_number<char>(hex_string, hex_byte, std::ios_base::hex))
				return false;

			output.push_back(hex_byte);
		}
		else
			output.push_back(byte);
	}
	return true;
}

bool parse_quality_entries(std::string const & input, quality_entries & output, std::string & error_message)
{
	string_vector argument_tokens = ail::tokenise(input, ",");
	BOOST_FOREACH(std::string token, argument_tokens)
	{
		string_vector quality_tokens = ail::tokenise(ail::trim(token), ";q=");
		switch(quality_tokens.size())
		{
			case 1:
				output.push_back(quality_entry(quality_tokens[0]));
				break;

			case 2:
			{
				float quality_value;
				if(!ail::string_to_number(quality_tokens[1], quality_value))
				{
					error_message = "Failed to parse quality value";
					return false;
				}
				if(quality_value < 0.0f || quality_value > 1.0f)
				{
					error_message = "Quality value out of range";
					return false;
				}
				output.push_back(quality_entry(quality_tokens[1]));
				break;
			}

			default:
				error_message = "Invalid quality value token count";
				return false;
		}
	}
	return true;
}

process_header_result::type process_header(std::string const & input, http_request & output, std::string & error_message)
{
	using namespace process_header_result;
	using namespace http_request_method;
	using namespace http_protocol;
	using namespace http_form_content;

	std::size_t offset = input.find(end_of_header);
	if(offset == std::string::npos)
		return no_delimiter;

	output.header_size = offset + end_of_header.size();

	std::string header = input.substr(0, offset);
	string_vector lines = ail::tokenise(header, delimiter);
	if(lines.empty())
	{
		error_message = "Invalid line count in the header";
		return error;
	}

	string_vector tokens = ail::tokenise(lines[0], " ");
	std::string const
		& method_string = tokens[0],
		& encoded_path = tokens[1],
		& version_string = tokens[2];

	if(method_string == "GET")
		output.method = get;
	else if(method_string == "POST")
		output.method = post;
	else
	{
		error_message = "Invalid method specified";
		return error;
	}

	if(version_string == "HTTP/1.1")
		output.protocol_version = version_1_1;
	else if(version_string == "HTTP/1.0")
		output.protocol_version = version_1_0;
	else
	{
		error_message = "Invalid protocol specified";
		return error;
	}

	if(!decode_string(encoded_path, output.path))
	{
		error_message = "Failed to decode path";
		return error;
	}

	for(std::size_t i = 1, end = lines.size(); i < end; i++)
	{
		std::string const target = ": ";
		std::string const & current_line = lines[i];
		std::size_t offset = current_line.find(target);
		if(offset == std::string::npos)
		{
			error_message = "Unable to detect colon in HTTP header line";
			return error;
		}

		std::string name = current_line.substr(0, offset);
		offset += target.size();
		std::string argument = current_line.substr(offset);

		if(name == "Content-Type")
		{
			if(argument == "application/x-www-form-urlencoded")
				output.content_type = application_x_www_form_urlencoded;
			else if(argument == "multipart/form-data")
				output.content_type = multipart_form_data;
			else
			{
				error_message = "Unknown content type specified";
				return error;
			}
			output.has_content_type = true;
		}
		else if(name == "Content-Length")
		{
			if(!ail::string_to_number<std::size_t>(argument, output.content_length))
			{
				error_message = "Invalid content length specified";
				return error;
			}
			output.has_content_length = true;
		}
		else if(name == "Cookie")
		{
			string_vector argument_tokens = ail::tokenise(argument, ";");
			BOOST_FOREACH(std::string const & token, argument_tokens)
			{
				string_vector cookie_tokens = ail::tokenise(ail::trim(token), "=");
				if(cookie_tokens.size() != 2)
				{
					error_message = "Failed to parse a cookie assignment";
					return error;
				}

				std::string
					cookie,
					value;

				if(
					!decode_string(cookie_tokens[0], cookie) ||
					!decode_string(cookie_tokens[1], value)
				)
				{
					error_message = "Failed to decode a cookie";
					return error;
				}
			}
		}
		else if(name == "Accept-Encoding")
		{
			if(!parse_quality_entries(argument, output.accepted_encodings, error_message))
				return error;
			BOOST_FOREACH(quality_entry & entry, output.accepted_encodings)
				if(entry.name == "gzip" && entry.quality_value > 0.0f)
					output.gzip = true;
		}
		else if(name == "Connection")
		{
			string_vector argument_tokens = ail::tokenise(input, ",");
			BOOST_FOREACH(std::string token, argument_tokens)
			{
				token = ail::trim(token);
				if(token == "Keep-Alive")
					output.keep_alive = true;
			}
		}
	}

	return success;
}
