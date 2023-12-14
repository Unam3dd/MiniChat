#include "mini_chat.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void	display_banner(char **av)
{
	printf(
			PURPLE"	_______ _____ __   _ _____ _______ _     _ _______ _______\n"
			"	|  |  |   |   | \\  |   |   |       |_____| |_____|    |   \n"
			"	|  |  | __|__ |  \\_| __|__ |_____  |     | |     |    | \n"
			"\n\n\t\tVersion: "GREEN VERSION PURPLE"\n\t\tAuthor: "GREEN"Unam3dd\n\n"RST
			);

	printf("usage : "GREEN"%s"RST" <addr> <port>\n\t"GREEN"%s"RST" 127.0.0.1 8081\n\t"GREEN"%s"RST " 192.168.1.63 8081\n\n",\
			av[0], av[0], av[0]);
}

int main(int ac, char **av)
{
	if (ac != 3) {
		display_banner(av);
		return (1);
	}

	SERVER_INITIALIZE(server);

	SERVER_USE_SELECT(server);

	SERVER_START(server, av[1], (uint16_t)atoi(av[2]));

	SERVER_WAIT(server);

	SERVER_CLOSE(server);

	return (0);
}
