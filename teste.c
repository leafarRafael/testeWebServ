#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

int setnonblocking(int sock);
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Criar socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
	int enable = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
    // Configurar endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associar o socket ao endereço
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escutar por conexões
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor aguardando conexão na porta %d...\n", PORT);

    // Aceitar uma conexão
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
	setnonblocking(client_fd);

    printf("Cliente conectado.\n");

    // Tentativa de ler dados do cliente (bloqueante)
    printf("Esperando para ler dados do cliente...\n");

	char j;
    int bytes_read;
	for(;;){
		bytes_read = read(client_fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			continue ;
		else
			break ;
	}


    if (bytes_read < 0) {
        perror("read failed");
    } else if (bytes_read == 0) {
        printf("Cliente desconectado.\n");
    } else {
        buffer[bytes_read] = '\0'; // Adiciona um terminador de string
        printf("Recebido: %s\n", buffer);
    }

    // Fechar sockets
    close(client_fd);
    close(server_fd);
    return 0;
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