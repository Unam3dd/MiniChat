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

//////////////////////////////////
//
//	       DEFINES
//
/////////////////////////////////

#define MAX_THREAD 4

//////////////////////////////////
//
//	       TYPEDEFS
//
/////////////////////////////////

typedef struct client_t 		client_t;
typedef struct server_t 		server_t;
typedef struct sockaddr_in		sin_t;
typedef enum multiplex_opt_t	multiplex_opt_t;
typedef int						fd_t;
typedef int						(*init_callback_t)(server_t	*);
typedef int						(*close_callback_t)(server_t *);

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
	fd_t		fd;
};

struct server_t {
	sin_t			sin;
	fd_t			fd;
	multiplex_opt_t	flags;
	bool			pool;
};

//////////////////////////////////
//
//	       SELECT
//
/////////////////////////////////

int	server_select_init(server_t	*server);
int	server_select_destroy(server_t *server);

//////////////////////////////////
//
//	       POLL
//
/////////////////////////////////

int	server_poll_init(server_t *server);
int	server_poll_destroy(server_t *server);

//////////////////////////////////
//
//	       EPOLL
//
/////////////////////////////////

int	server_epoll_init(server_t *server);
int	server_epoll_destroy(server_t *server);

#endif
