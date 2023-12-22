#include "mini_chat.h"

int		server_poll_init(server_t *server) { (void)server; return (1);}
int		server_poll_wait(server_t *server) { (void)server; return (1); }
int		server_poll_listen(server_t *server, const char *addr, port_t port) {(void)server; (void)addr; (void)port; return (1);}
int		server_poll_accept(fd_t sfd) { (void)sfd; return (1); }
int		server_poll_handle(client_t *client) { (void)client; return (1); }
int		server_poll_close(server_t *server) { (void)server; return (1);}
void	server_poll_signal(int signum) { (void)signum; }
