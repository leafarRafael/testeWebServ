/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbutzke <rbutzke@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 10:39:38 by rbutzke           #+#    #+#             */
/*   Updated: 2024/09/27 21:16:24 by rbutzke          ###   ########.fr       */
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
#define MAX_EVENTS 10
#define SIZE 10024

int		initTCP();
int		setnonblocking(int sock);
void	sendResponse(int fd, Response http);
void	initEpoll(int socketFd, Response response);

int	main()
{
	int			socketFd, clientFd;
	struct		epoll_event ev, events[MAX_EVENTS];
	Response	response;

	socketFd = initTCP();
	listen(socketFd, MAX_EVENTS);
	initEpoll(socketFd, response);
}

int	initTCP()
{
	int			socketFd, enable;
	sockaddr_in	addr;

	enable = 1;
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(socketFd, (struct sockaddr *)&addr, sizeof(addr));
	return socketFd;
}

void	sendResponse(int fd, Response http)
{
	std::ifstream	file("index.html");
	std::string		html;
	std::getline(file, html, '\0');
	http.setStatus(std::to_string(200), " Ok");
	http.setType("text/html");
	http.setConnection("");
	http.setLength(std::to_string(html.length()));
	http.setBody(html);
	send(fd, http.getHttp().c_str(),http.getHttp().length(), 0);
}


void	initEpoll(int socketFd, Response response){
	int					llisten_sock, clientFd, nfds, epollfd;
	char				buffer[SIZE];
	struct epoll_event	ev, events[MAX_EVENTS];

	epollfd = epoll_create1(EPOLL_CLOEXEC);
	ev.events = EPOLLIN;
	ev.data.fd = socketFd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, socketFd, &ev);
	while (1){
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		std::cout << "ola\n";
		for(int i = 0; i < nfds; i++){
			if (events[i].data.fd == socketFd){
				clientFd = accept(socketFd, nullptr, nullptr);
				setnonblocking(clientFd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = clientFd;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, clientFd, &ev);
			}else{
				sendResponse(clientFd, response);
				close(clientFd);
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