/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbutzke <rbutzke@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 10:39:38 by rbutzke           #+#    #+#             */
/*   Updated: 2024/10/01 14:20:20 by rbutzke          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <bitset>
#include <sstream>
#include <error.h>
#define MAX_EVENTS 10
#define SIZE 10024
#define SITE0 0
#define SITE1 1

int		initTCP(int port);
int		setnonblocking(int sock);
void	sendResponseImage(int fd, Response http, std::string image);
void	sendResponseHTML(int fd, Response http, std::string indexHTML);
void	initEpoll(int socketFd[2], Response response);

int	main()
{
	int			socketFd[2];
	struct		epoll_event ev, events[MAX_EVENTS];
	Response	response;

	socketFd[SITE0] = initTCP(8080);
	socketFd[SITE1] = initTCP(8081);
	initEpoll(socketFd, response);
}

int	initTCP(int port)
{
	int			socketFd, enable;
	sockaddr_in	addr;

	enable = 1;
	socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(socketFd, (struct sockaddr *)&addr, sizeof(addr));
	listen(socketFd, MAX_EVENTS);
	return socketFd;
}

void	sendResponseHTML(int fd, Response http, std::string indexHTML)
{
	std::ifstream	file(indexHTML);
	std::string		html;
	std::getline(file, html, '\0');
	http.setStatus(std::to_string(200), " Ok");
	http.setType("text/html");
	http.setConnection("");
	http.setLength(std::to_string(html.length()));
	http.setBody(html);
	send(fd, http.getHttp().c_str(),http.getHttp().length(), 0);
}

void	sendResponseImage(int fd, Response http, std::string image)
{
	std::ifstream		file(image);
	std::stringstream	bImage;

	bImage << file.rdbuf();
	http.setStatus(std::to_string(200), " Ok");
	http.setType("image/png");
	http.setConnection("");
	http.setLength(std::to_string(bImage.str().length()));
	send(fd, http.getHttp().c_str(),http.getHttp().length(), 0);
	send(fd, bImage.str().c_str(), bImage.str().length(), 0);
}

void	initEpoll(int socketFd[2], Response response){
	int					nfds, epollfd;
	char				buffer[SIZE];
	struct epoll_event	ev, events[MAX_EVENTS];

	epollfd = epoll_create1(EPOLL_CLOEXEC);
	ev.events = EPOLLIN;
	ev.data.fd = socketFd[SITE0];
	epoll_ctl(epollfd, EPOLL_CTL_ADD, socketFd[SITE0], &ev);
	ev.data.fd = socketFd[SITE1];
	epoll_ctl(epollfd, EPOLL_CTL_ADD, socketFd[SITE1], &ev);
	while (1){
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		for(int i = 0; i < nfds; i++){
			if (events[i].data.fd == socketFd[SITE0]){
				ev.data.fd = accept(socketFd[SITE0], nullptr, nullptr);
				setnonblocking(ev.data.fd);
				ev.events = EPOLLIN;
				std::cout << "\n File descriptor: " << ev.data.fd << '\n';
				epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
			}
			else if (events[i].data.fd == socketFd[SITE1]){
				ev.data.fd = accept(socketFd[SITE1], nullptr, nullptr);
				setnonblocking(ev.data.fd);
				ev.events = EPOLLIN;
				std::cout << "\n File descriptor: " << ev.data.fd << '\n';
				epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
			}
			else
			{
				ev.events = EPOLLOUT;
				recv(events[i].data.fd, buffer, SIZE, 0);
				std::string resp(buffer);
				std::cout << resp << '\n';
				if (resp.find("Host: localhost:8081") != std::string::npos)
				{
					if (resp.find("Accept: text/") != std::string::npos)
						sendResponseHTML(events[i].data.fd, response, "index.html");
					else if (resp.find("Accept: image/") != std::string::npos)
						sendResponseImage(events[i].data.fd, response, "img.png");
				}
				else{
					if (resp.find("Accept: text/") != std::string::npos)
						sendResponseHTML(events[i].data.fd, response, "index2.html");
					else if (resp.find("Accept: image/") != std::string::npos)
						sendResponseImage(events[i].data.fd, response, "images.png");
				}
				epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
				close(events[i].data.fd);
			}
		}
	}
}

int setnonblocking(int sock)
{
    int result;
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    result = fcntl(sock, F_SETFL ,flags);
    return result;
}