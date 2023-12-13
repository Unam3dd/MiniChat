#include "mini_chat.h"
#include <stdint.h>
#include <sys/select.h>
#include <stdlib.h>

//////////////////////////////////
//
//	       CLIENTS
//
/////////////////////////////////

client_t	*get_clients(bool clear, size_t *nmemb, size_t *size)
{
	static client_t	clients[FD_SETSIZE];

	if (nmemb) *nmemb = sizeof(clients);

	if (size) *size = sizeof(clients) / sizeof(client_t);

	if (clear) memset(clients, 0, sizeof(clients));

	return (clients);
}

client_id_t	get_current_id(void)
{
	static client_id_t	id = 0;

	return (id++);
}

int	add_client(client_t *client)
{
	if (!client) return (1);

	client_t	*clients = NULL;
	size_t		size = 0;

	clients = get_clients(0, NULL, &size);

	for (uint32_t i = 0; i < size; i++) {
		if (clients[i].fd) continue ;

		clients[i] = *client;
		clients[i].id = get_current_id();
		break;
	}

	return (0);
}

int	remove_client(client_t *client)
{
	if (!client) return (1);

	client_t	*clients = NULL;
	size_t		size = 0;

	clients = get_clients(0, NULL, &size);

	for (uint32_t i = 0; i < size; i++) {
		if (&clients[i] != client) continue ;

		if (clients[i].buf) free((void *)clients[i].buf);
		close(clients[i].fd);
		memset(&clients[i], 0, sizeof(client_t));
		break ;
	}

	return (0);
}

int	remove_client_by_id(client_id_t id)
{
	client_t	*clients = NULL;
	size_t		size = 0;

	clients = get_clients(0, NULL, &size);

	for (uint32_t i = 0; i < size; i++) {
		if (clients[i].id != id) continue ;

		if (clients[i].buf) free((void *)clients[i].buf);
		close(clients[i].fd);
		memset(&clients[i], 0, sizeof(client_t));
		break ;
	}

	return (0);
}
