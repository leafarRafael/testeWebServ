/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbutzke <rbutzke@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 10:39:51 by rbutzke           #+#    #+#             */
/*   Updated: 2024/10/01 12:39:20 by rbutzke          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

class Response
{
	private:
		std::string	_fillHttp[4][4];
		std::string	_htpp;
		std::string	_status[4];
		std::string	_type[3];
		std::string	_connection[3];
		std::string	_length[3];
		std::string	_body;

	public:
		Response();
		~Response();

		void setStatus(std::string value, std::string reason);
		void setType(std::string type);
		void setConnection(std::string connection);
		void setLength(std::string length);
		void setBody(std::string body);
		std::string	getHttp();
};

enum e_response
{
	STATUS = 0,
	TYPE,
	CONNECTION,
	LENGTH,
};

enum e_status
{
	HTTP_VERSION = 0,
	VALUE,
	REASON,
	END_LINE,
};

enum e_header
{
	HEADER = 0,
	HEADER_TYPE,
	ENDL,
};

#endif