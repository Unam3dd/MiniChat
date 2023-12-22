#include "mini_chat.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void	display_banner(void)
{
	printf(
			PURPLE"	_______ _____ __   _ _____ _______ _     _ _______ _______\n"
			"	|  |  |   |   | \\  |   |   |       |_____| |_____|    |   \n"
			"	|  |  | __|__ |  \\_| __|__ |_____  |     | |     |    | \n"
			"\n\n\t\tVersion: "GREEN VERSION PURPLE"\n\t\tAuthor: "GREEN"Unam3dd\n\n"RST
			);
}

static int show_help(const char *name)
{

	display_banner();
	printf("usage : "GREEN"%s"RST" <addr> <port>\n\t"GREEN"%s"RST" 127.0.0.1 8081 --["GREEN"select/poll/epoll"RST"]\n"
			"\t"GREEN"%s"RST " 192.168.1.63 8081 --["GREEN"select/poll/epoll"RST"]\n\n\tOptions:\n",\
		name, name, name);
	printf("\t\t"GREEN"--select"RST" | start a server with select\n"
		   "\t\t"GREEN"--poll"RST"	 | start a server with poll\n"
		   "\t\t"GREEN"--epoll"RST"  | start a server with epoll\n\n");
	return (1);
}

int main(int ac, char **av)
{
	if (ac != 4) return (show_help(av[0]));

	display_banner();

	SERVER_INITIALIZE(server);

	SERVER_OPTIONS(server);

	SERVER_START(server, av[1], (uint16_t)atoi(av[2]));

	SERVER_WAIT(server);

	SERVER_CLOSE(server);

	return (0);
}
