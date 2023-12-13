#include "mini_chat.h"

//////////////////////////////////
//
//	       SERVER
//
/////////////////////////////////

server_t	*get_server_instance(server_t *server)
{
	static server_t	*ptr = NULL;

	if (server) ptr = server;

	return (ptr);
}
