#pragma once

#include <ail/types.hpp>
#include <theodwalha/request.hpp>

struct http_reply
{
	http_protocol::type protocol;
	uword code;
	std::string description;

	bool gzip;
	bool keep_alive;
	uword keep_alive_timeout;
	uword keep_alive_max;
	std::string content_type;

	std::string content;

	http_reply();

	void ok();
	void not_found();
	void forbidden();

	bool get_packet(std::string & packet);
};
