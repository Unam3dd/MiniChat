#include "mini_chat.h"

//////////////////////////////////
//
//	       INCLUDES
//
/////////////////////////////////

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

//////////////////////////////////
//
//		SIGNAL	       
//
/////////////////////////////////

void	server_select_signal(int signum)
{
	(void)signum;
	printf("[+] CTRL+C Catched !\n");
}

//////////////////////////////////
//
//	       INIT
//
/////////////////////////////////

int	server_select_init(server_t *server)
{
	if (!server) return (1);

	get_server_instance(server);

	if ((server->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return (1);
	}

	if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
		perror("setsockopt");
		return (1);
	}

	printf("[" GREEN "+" RST"] Socket file descriptor has been created %d !\n", server->fd);

	return (0);
}

//////////////////////////////////
//
//	       LISTEN
//
/////////////////////////////////

int	server_select_listen(server_t *s, const char *addr, port_t port)
{
	if (!s || !addr || !port) return (1);

	// Convert String formatted "0.0.0.0" to network byte order address
	// you can also use htonl(0x7F000001) or directly write 0x0100007F
	// If you want listen on any interface you can use INANY_ADDR
	s->sin.sin_addr.s_addr = inet_addr(addr);


	// Convert unsigned short port to network byte Order (Big Endian)
	s->sin.sin_port = htons(port);

	// Use IPV4
	s->sin.sin_family = AF_INET;

	if (bind(s->fd, (struct sockaddr *)&s->sin, sizeof(s->sin)) < 0) {
		perror("bind");
		return (1);
	}
	
	printf("[" GREEN "+" RST"] Server bind on %x:%x ("GREEN"%s"RST":"GREEN"%d"RST") !\n", s->sin.sin_addr.s_addr, s->sin.sin_port, addr, port);

	if (listen(s->fd, SOMAXCONN) < 0) {
		perror("listen");
		return (1);
	}
	
	printf("[" GREEN "+" RST"] Server Starting Listening on "GREEN"%x"RST":"GREEN"%x"RST"\n", s->sin.sin_addr.s_addr, s->sin.sin_port);

	return (0);
}

//////////////////////////////////
//
//	       ACCEPT
//
/////////////////////////////////

int	server_select_accept(fd_t sfd)
{
	char		buf[0x100];
	sin_t		sin;
	socklen_t	len = sizeof(sin_t);
	client_t	client;

	memset(&client, 0, sizeof(client));
	memset(&sin, 0, sizeof(sin));

	if ((client.fd = accept(sfd, (struct sockaddr *)&sin, &len)) < 0) {
		perror("accept");
		return (-1);
	}

	memset(buf, 0, sizeof(buf));

	sprintf(buf, "[" GREEN "+" RST"] Server : New client connected to the server %x:%x ("GREEN"%s"RST":"GREEN"%d"RST") !\n", sin.sin_addr.s_addr, sin.sin_port, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

	add_client(&client);
	
	printf("%s", buf);
	send_msg_clients(client.id, buf, strlen(buf));

	return (client.fd);
}


//////////////////////////////////
//
//	       WAIT
//
/////////////////////////////////

int	server_select_wait(server_t *s)
{
	if (!s) return (1);

	client_t	*clients = NULL;
	fd_set	read_fds, save_fds;
	size_t		size = 0;
	uint32_t	i = 0;
	fd_t		fd = 0;
	int			status = 0;

	signal(SIGINT, server_select_signal);

	clients = get_clients(0, NULL, &size);

	FD_ZERO(&read_fds);
	FD_ZERO(&save_fds);

	FD_SET(s->fd, &save_fds);

	while (0xB4DC0D3) {
		FD_ZERO(&read_fds);

		read_fds = save_fds;

		if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
			perror("select");
			if (errno == EINTR) break;
		}

		if (s->fd && FD_ISSET(s->fd, &read_fds)) {
			fd = server_select_accept(s->fd);
			if (fd) FD_SET(fd, &save_fds);
		}

		for (i = 0; i < size; i++) {
			if (clients[i].fd && FD_ISSET(clients[i].fd, &read_fds)) {
				status = server_select_handle(clients, size, &clients[i]);
				
				if (status == 1) {
					FD_CLR(clients[i].fd, &save_fds);
					remove_client(&clients[i]);
				}

				if (status < 0) break ;
			}
		}
	}
	

	for (i = 0; i < size; i++) {
		FD_CLR(clients[i].fd, &save_fds);
		close(clients[i].fd);
		memset(&clients[i], 0, sizeof(client_t));
	}

	FD_CLR(s->fd, &save_fds);

	FD_ZERO(&read_fds);
	FD_ZERO(&save_fds);

	return (0);
}

int	server_select_handle(client_t *clients, size_t nb, client_t *client)
{
	if (!clients) return (1);

	char		buf[BUF_SIZE];
	char		*token = NULL;
	int			size = 0;
	uint32_t 	i = 0;

	memset(buf, 0, sizeof(buf));

	size = recv(client->fd, buf, sizeof(buf), 0);

	if (!size) return (1);

	if (size < 0) {
		perror("read");
		return (-1);
	}

	buf[size] = 0;

	token = strtok(buf, "\n");

	while (token) {
		
		for (i = 0; i < nb; i++) {
			
			if (client->id == clients[i].id || clients[i].fd <= 0) continue ;
			
			dprintf(clients[i].fd, "["GREEN"+"RST"] client %d: %s\n", client->id, token);
		}

		token = strtok(NULL, "\n");
	}


	return (0);
}

//////////////////////////////////
//
//	       CLOSE
//
/////////////////////////////////

int	server_select_close(server_t *server)
{
	if (!server) return (1);

	if (close(server->fd) < 0) perror("close");
	
	printf("[" GREEN "+" RST"] Server closed !\n");

	SERVER_ZERO(*server);

	return (0);
}
