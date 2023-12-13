#include "mini_chat.h"

int main(void)
{
	SERVER_INITIALIZE(server);

	SERVER_USE_SELECT(server);

	SERVER_START(server, "127.0.0.1", 8081);

	SERVER_WAIT(server);

	SERVER_CLOSE(server);

	return (0);
}
