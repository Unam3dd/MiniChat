#include "mini_chat.h"
#include <stdint.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//////////////////////////////////
//
//	       CLIENTS
//
/////////////////////////////////


client_t      *gselect(bool clear, size_t *nmemb, size_t *size)
{
	static client_t	clients[FD_SETSIZE];

	if (nmemb) *nmemb = sizeof(clients);

	if (size) *size = sizeof(clients) / sizeof(client_t);

	if (clear) memset(clients, 0, sizeof(clients));

    return (clients);
}

client_t    *gclients(bool clear, size_t *nmemb, size_t *size)
{
    static client_t clients[MAX_CLIENTS];

    if (nmemb) *nmemb = sizeof(clients);

    if (size) *size = sizeof(clients) / sizeof(client_t);

    if (clear) memset(clients, 0, sizeof(clients));

    return (clients);
}

client_t	*get_clients(bool clear, size_t *nmemb, size_t *size)
{
    server_t    *s = get_server_instance(NULL);

    if (s && s->flags & SELECT) return (gselect(clear, nmemb, size));

    return (gclients(clear, nmemb, size));
}

client_id_t	get_current_id(void)
{
	static client_id_t	id = 0;

	return (id++);
}

void	send_msg_clients(client_id_t author, const char *buf, size_t size)
{
	client_t	*clients = NULL;
	size_t		len = 0;

	clients = get_clients(0, NULL, &len);

	for (uint32_t i = 0; i < len; i++) {
		if (clients[i].fd <= 0 || clients[i].id == author) continue ;

		send(clients[i].fd, buf, size, 0);
	}
}

int	add_client(client_t *client)
{
	if (!client) return (1);

	client_t	*clients = NULL;
	size_t		size = 0;
    uint32_t    i = 0;

	clients = get_clients(0, NULL, &size);

	for (i = 0; i < size; i++) {
		if (clients[i].fd) continue ;

		clients[i] = *client;
		clients[i].id = get_current_id();
		client->id = clients[i].id;
		break;
	}

    if (client && i >= size) close(client->fd);

	return (0);
}

int	remove_client(client_t *client)
{
	if (!client) return (1);

	char		buf[BUF_SIZE];
	sin_t		sin;
	client_t	*clients = NULL;
	size_t		size = 0;
	socklen_t 	len	= sizeof(sin_t);

	clients = get_clients(0, NULL, &size);

	for (uint32_t i = 0; i < size; i++) {
		if (&clients[i] != client) continue ;

		if (getpeername(client->fd, (struct sockaddr *)&sin, &len) < 0) {
			perror("getpeername");
			return (-1);
		}

		sprintf(buf, "[" GREEN "+" RST"] client (%d) : %s:%d just disconnected !\n", client->id, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		write(1, buf, strlen(buf));
		send_msg_clients(client->id, buf, strlen(buf));
		close(clients[i].fd);
		memset(&clients[i], 0, sizeof(client_t));
		break ;
	}

	return (0);
}

client_t    *get_client(client_t *client)
{
    if (!client) return (NULL);

    client_t *arr = NULL;
    size_t  size = 0;
    uint32_t i = 0;

    arr = get_clients(0, NULL, &size);

    for (i = 0; i < size; i++) {
        if ((arr[i].id == client->id) && (arr[i].fd == client->fd))
            return (&arr[i]);
    }

    return (NULL);
}

int	remove_client_by_id(client_id_t id)
{
	client_t	*clients = NULL;
	size_t		size = 0;

	clients = get_clients(0, NULL, &size);

	for (uint32_t i = 0; i < size; i++) {
		if (clients[i].id != id) continue ;

		close(clients[i].fd);
		memset(&clients[i], 0, sizeof(client_t));
		break ;
	}
	return (0);
}
