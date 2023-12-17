#include "mini_chat.h"
#include <stdio.h>

void	send_version(client_t *client)
{
	if (!client) return ;

	dprintf(client->fd, "["GREEN"+"RST"] version : %s\n", VERSION);
}
