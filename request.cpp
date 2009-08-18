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

quality_entry::quality_string(std::string const & name):
	name(name),
	quality_value(1.0f)
{
}

quality_entry::quality_string(std::string const & name, float quality_value):
	name(name),
	quality_value(quality_value)
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

			output.append(hex_byte);
		}
		else
			output.append(byte);
	}
	return true;
}

process_header_result::type process_header(std::string const & input, http_request & output, std::string & error_message)
{
	using namespace process_header_result;
	using namespace http_request_method;
	using namespace http_protocol;
	using namespace http_form_content;

	std::size_t offset = input.find(delimiter);
	if(offset == std::string::npos)
		return no_delimiter;

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

	if(!decode_string(encoded_path, path))
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
				content_type = application_x_www_form_urlencoded;
			else if(argument == "multipart/form-data")
				content_type = multipart_form_data;
			else
			{
				error_message = "Unknown content type specified";
				return error;
			}
		}
		else if(name == "Content-Length")
		{
			if(!ail::string_to_number<std::size_t>(argument, content_length))
			{
				error_message = "Invalid content length specified";
				return error;
			}
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
		else
		{
			string_vector argument_tokens = ail::tokenise(argument, ",");
			BOOST_FOREACH(std::string token, argument_tokens)
			{
				token = ail::trim(token);
			}
		}
	}
}
