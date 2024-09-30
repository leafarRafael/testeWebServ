/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbutzke <rbutzke@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 10:47:40 by rbutzke           #+#    #+#             */
/*   Updated: 2024/09/27 11:17:01 by rbutzke          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::~Response(){}

Response::Response(){
	this->_fillHttp[STATUS][HTTP_VERSION] = "HTTP/1.1 ";
	this->_fillHttp[STATUS][VALUE] = "";
	this->_fillHttp[STATUS][REASON] = "";
	this->_fillHttp[STATUS][END_LINE] = "\r\n";
	this->_fillHttp[TYPE][HEADER] = "Content-Type: ";
	this->_fillHttp[TYPE][HEADER_TYPE] = "";
	this->_fillHttp[TYPE][ENDL] = "\r\n";
	this->_fillHttp[CONNECTION][HEADER] = "Connection: ";
	this->_fillHttp[CONNECTION][HEADER_TYPE] = "";
	this->_fillHttp[CONNECTION][ENDL] = "\r\n";
	this->_fillHttp[LENGTH][HEADER] = "Content-Length: ";
	this->_fillHttp[LENGTH][HEADER_TYPE] = "";
	this->_fillHttp[LENGTH][ENDL] = "\r\n\r\n";
	this->_body = "";
	this->_htpp = "";
}

void	Response::setStatus(std::string value, std::string reason){
	this->_fillHttp[STATUS][VALUE] = value;
	this->_fillHttp[STATUS][REASON] = reason;
}
void	Response::setType(std::string type){
	this->_fillHttp[TYPE][HEADER_TYPE] = type;
}

void	Response::setConnection(std::string connection){
	this->_fillHttp[CONNECTION][HEADER_TYPE] = connection;
}

void	Response::setLength(std::string length){
	this->_fillHttp[LENGTH][HEADER_TYPE] = length;
}

void	Response::setBody(std::string body){
	this->_body = body;
}

std::string	Response::getHttp(){
	this->_htpp += this->_fillHttp[STATUS][HTTP_VERSION];
	this->_htpp += this->_fillHttp[STATUS][VALUE];
	this->_htpp += this->_fillHttp[STATUS][REASON];
	this->_htpp += this->_fillHttp[STATUS][END_LINE];
	this->_htpp += this->_fillHttp[TYPE][HEADER];
	this->_htpp += this->_fillHttp[TYPE][HEADER_TYPE];
	this->_htpp += this->_fillHttp[TYPE][ENDL];
	this->_htpp += this->_fillHttp[CONNECTION][HEADER];
	this->_htpp += this->_fillHttp[CONNECTION][HEADER_TYPE];
	this->_htpp += this->_fillHttp[CONNECTION][ENDL];
	this->_htpp += this->_fillHttp[LENGTH][HEADER];
	this->_htpp += this->_fillHttp[LENGTH][HEADER_TYPE];
	this->_htpp += this->_fillHttp[LENGTH][ENDL];
	this->_htpp += this->_body;
	return (this->_htpp);
}