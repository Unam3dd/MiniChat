#ifndef MINI_CHAT_H
#define MINI_CHAT_H

//////////////////////////////////
//
//	       INCLUDES
//
/////////////////////////////////

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <unistd.h>

//////////////////////////////////
//
//	       COLOR
//
/////////////////////////////////

#define RED "\033[38;5;196m"
#define GREEN "\033[38;5;82m"
#define BLUE "\033[38;5;27m"
#define YELLOW "\033[38;5;11m"
#define ORANGE "\033[38;5;202m"
#define PURPLE "\033[38;5;129m"
#define CYAN "\033[38;5;44m"
#define RST "\033[00m"

//////////////////////////////////
//
//	       DEFINES
//
/////////////////////////////////

#define MAX_THREAD 4
#define BUF_SIZE 0x100
#define MAX_CLIENTS 0x1000
#define EVENT_MAX 0x1000
#define EPOLL_EV 0x10

//////////////////////////////////
//
//	       TYPEDEFS
//
/////////////////////////////////

typedef struct client_t 			client_t;
typedef struct server_t 			server_t;
typedef struct server_callbacks_t	server_callbacks_t;
typedef struct command_callbacks_t	command_callbacks_t;
typedef struct client_list_t        client_list_t;
typedef struct sockaddr_in			sin_t;
typedef uint16_t					port_t;
typedef uint32_t					client_id_t;
typedef enum multiplex_opt_t		multiplex_opt_t;
typedef int							fd_t;
typedef int							(*callback_t)(server_t	*);
typedef int							(*callback_listener_t)(server_t *, const char *, port_t);


//////////////////////////////////
//
//	       ENUM
//
/////////////////////////////////

enum multiplex_opt_t
{
	SELECT = 1 << 0,
	POLL = 1 << 1,
	EPOLL = 1 << 2
};


//////////////////////////////////
//
//	       STRUCT
//
/////////////////////////////////

struct client_t {
	const char	*buf;
	size_t		size;
	client_id_t	id;
	fd_t		fd;
};

struct server_callbacks_t {
	callback_t				init;
	callback_listener_t		listen;
	callback_t				wait;
	callback_t				close;
};

struct server_t {
	server_callbacks_t	cb;
	sin_t				sin;
	fd_t				fd;
	multiplex_opt_t		flags;
	bool				pool;
};

//////////////////////////////////
//
//	       CLIENTS
//
/////////////////////////////////

client_t    *gselect(bool clear, size_t *nmemb, size_t *size);
client_t    *gclients(bool clear, size_t *nmemb, size_t *size);
client_t	*get_clients(bool clear, size_t *nmemb, size_t *size);
client_id_t	get_current_id(void);
client_t    *get_client(client_t *c);
int			add_client(client_t *client);
int			remove_client(client_t *client);
int			remove_client_by_id(client_id_t id);
void		send_msg_clients(client_id_t author, const char *buf, size_t size);

//////////////////////////////////
//
//	       SELECT
//
/////////////////////////////////

int		server_select_init(server_t	*server);
int		server_select_wait(server_t *s);
int		server_select_listen(server_t *s, const char *addr, port_t port);
int		server_select_accept(fd_t sfd);
int		server_select_handle(client_t *client);
int		server_select_close(server_t *server);
void	server_select_signal(int signum);

//////////////////////////////////
//
//	       POLL
//
/////////////////////////////////

int		server_poll_init(server_t *server);
int		server_poll_wait(server_t *server);
int		server_poll_listen(server_t *server, const char *addr, port_t port);
int		server_poll_accept(fd_t sfd, struct pollfd *pfds, int *nfds);
int		server_poll_handle(client_t *client);
int		server_poll_close(server_t *server);
void	server_poll_signal(int signum);

//////////////////////////////////
//
//	       EPOLL
//
/////////////////////////////////

int		server_epoll_init(server_t *server);
int		server_epoll_wait(server_t *server);
int		server_epoll_listen(server_t *server, const char *addr, port_t port);
int		server_epoll_accept(fd_t sfd, fd_t efd);
int		server_epoll_handle(client_t *client);
int		server_epoll_close(server_t *server);
void	server_epoll_signal(int signum);

//////////////////////////////////
//
//	       SERVER ST
//
/////////////////////////////////

server_t	*get_server_instance(server_t *server);

///////////////////////////////
//
//			VERSION
//
//////////////////////////////

void	send_version(client_t *client);

//////////////////////////////////
//
//	       MACRO
//
/////////////////////////////////

#define SERVER_ZERO(server) memset(&server, 0, sizeof(server))
#define SERVER_INITIALIZE(name) \
	server_t	name; SERVER_ZERO(name);
#define SERVER_START(name, addr, port)\
	if (name.cb.init && name.cb.init(&name)) {\
		write(2, "Error\n", 6);\
		return (1);\
	}\
	\
	if (name.cb.listen && name.cb.listen(&name, addr, port)) {\
		write(2, "Error\n", 6);\
		return (1);\
	}\

#define SERVER_WAIT(name) {\
	if (name.cb.wait && name.cb.wait(&name)) {\
		write(2, "Error\n", 6);\
		return (1);\
	}\
}

#define SERVER_USE_SELECT(name)\
	name.cb.init = server_select_init;\
	name.cb.listen = server_select_listen;\
	name.cb.wait = server_select_wait;\
	name.cb.close = server_select_close;\
	name.flags = SELECT;\

#define SERVER_USE_POLL(name)\
	name.cb.init = server_poll_init;\
	name.cb.listen = server_poll_listen;\
	name.cb.wait = server_poll_wait;\
	name.cb.close = server_poll_close;\
	name.flags = POLL;\

#define SERVER_USE_EPOLL(name)\
	name.cb.init = server_epoll_init;\
	name.cb.listen = server_epoll_listen;\
	name.cb.wait = server_epoll_wait;\
	name.cb.close = server_epoll_close;\
	name.flags = EPOLL;\

#define SERVER_CLOSE(name) {\
	if (name.cb.close && name.cb.close(&name)) {\
		write(2, "Error\n", 6);\
		return (1);\
	}\
}

#define SERVER_OPTIONS(server)\
	if (!strcmp(av[3], "--select")) { SERVER_USE_SELECT(server); }\
	else if (!strcmp(av[3], "--poll")) { SERVER_USE_POLL(server); }\
	else if (!strcmp(av[3], "--epoll")) { SERVER_USE_EPOLL(server); }\
	else return (fprintf(stderr, "options error : %s not found !\n", av[0]), 1); 

#endif
