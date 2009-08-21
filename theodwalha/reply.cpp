#include <iostream>
#include <ail/string.hpp>
#include <ail/zlib.hpp>
#include <theodwalha/reply.hpp>

http_reply::http_reply():
	gzip(false),
	keep_alive(false)
{
}

void http_reply::ok()
{
	code = 200;
	description = "OK";
}

void http_reply::not_found()
{
	code = 404;
	description = "Not Found";
}

void http_reply::forbidden()
{
	code = 403;
	description = "Forbidden";
}

bool http_reply::get_packet(std::string & packet)
{
	/*
	HTTP/1.1 200 OK
	Date: Tue, 18 Aug 2009 03:14:21 GMT
	Server: Apache/2.2.11 (Debian) PHP/5.2.9-4 with Suhosin-Patch
	X-Powered-By: PHP/5.2.9-4
	Vary: Accept-Encoding
	Content-Encoding: gzip
	Content-Length: 730
	Keep-Alive: timeout=15, max=100
	Connection: Keep-Alive
	Content-Type: text/html
	*/

	std::string const delimiter = "\r\n";

	packet = std::string();

	switch(protocol)
	{
		case http_protocol::version_1_1:
			packet += "HTTP/1.1";
			break;

		case http_protocol::version_1_0:
			packet += "HTTP/1.0";
			break;

		default:
			std::cout << "Invalid protocol version specific in the HTTP reply" << std::endl;
			return false;
	}

	packet += " " + ail::number_to_string(code) + description + delimiter;
	if(gzip)
	{
		try
		{
			std::string compressed_content;
			ail::compress_gzip(content, compressed_content);
			content = compressed_content;
		}
		catch(ail::exception & exception)
		{
			std::cout << exception.get_message() << std::endl;
		}
		packet += "Content-Encoding: gzip" + delimiter;
	}

	packet += "Content-Length: " + ail::number_to_string(content.size()) + delimiter;

	if(keep_alive)
	{
		packet += "Keep-Alive: timeout=" + ail::number_to_string(keep_alive_timeout) + ", max=" + ail::number_to_string(keep_alive_max) + delimiter;
		packet += "Connection: Keep-Alive" + delimiter;
	}

	packet += "Content-Type: " + content_type;

	packet += content;

	return true;
}
