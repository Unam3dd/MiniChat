#include "mini_chat.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/poll.h>

int		server_poll_init(server_t *server)
{
    if (!server) return (1);

    server->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server->fd < 0) return (1);

    if (setsockopt(server->fd, SOL_SOCKET,
            SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt");
        return (1);
    }

    printf("[" GREEN "+" RST"] Socket file descriptor has been created %d !\n", server->fd);

    return (0);
}

int		server_poll_wait(server_t *server)
{
    if (!server) return (1);

    static struct pollfd    pfds[EVENT_MAX];
    client_t    *clients = get_clients(0, NULL, NULL);
    int ready = 0, i = 0, nfds = 1;

    memset(pfds, 0, sizeof(pfds));

    pfds[0].fd = server->fd;
    pfds[0].events = POLLIN;

    while (0xBADC0DE) {

        ready = poll(pfds, nfds, -1);

        if (ready < 0) {
            perror("poll");
            break ;
        }

        for (i = 0; i < EVENT_MAX; i++) {

            if (i == 0 && pfds[i].fd == server->fd && pfds[i].revents & POLLIN) {
                printf("[+] Waiting accept client...\n");
                if (server_poll_accept(server->fd, pfds) < 0)
                    continue ;
                nfds++;
            }

            if (pfds[i].revents & POLLIN) {
                server_poll_handle(&clients[i]);
            }
        }
    }

    return (0);
}

int		server_poll_listen(server_t *server, const char *addr, port_t port)
{
    if (!server || !addr) return (1);

    memset(&server->sin, 0, sizeof(sin_t));

    server->sin.sin_addr.s_addr = inet_addr(addr);
    server->sin.sin_port = htons(port);
    server->sin.sin_family = AF_INET;

    if (bind(server->fd, (struct sockaddr*)&server->sin, sizeof(sin_t)) < 0) {
        perror("bind");
        return (1);
    }

    if (listen(server->fd, SOMAXCONN) < 0) {
        perror("listen");
        return (1);
    }

    printf("[" GREEN "+" RST"] Server Starting Listening on "GREEN"%x"RST":"GREEN"%x"RST"\n", server->sin.sin_addr.s_addr, server->sin.sin_port);

    return (0);
}

int		server_poll_accept(fd_t sfd, struct pollfd *pfds)
{
    if (!pfds) return (-1);

    sin_t   sin;
    socklen_t len = sizeof(sin_t);
    client_t    *clients = get_clients(0, NULL, NULL);
    fd_t    fd = 0;
    int     i = 0;

    memset(&sin, 0, sizeof(sin_t));

    fd = accept(sfd, (struct sockaddr*)&sin, &len);

    if (fd < 0) return (-1);

    printf("[+] New client accepted %x:%x (%s:%d)\n", sin.sin_addr.s_addr, sin.sin_port, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

    printf("[+] New client accepted and added in the poll structure...\n");

    for (i = 1; i < EVENT_MAX; i++) {
        if (pfds[i].fd) continue ;

        memset(&pfds[i], 0, sizeof(struct pollfd));

        pfds[i].fd = fd;
        pfds[i].events = POLLIN | POLLHUP;
        clients[i].fd = fd;
        clients[i].id = get_current_id();
    }

    return (0);
}

int		server_poll_handle(client_t *client)
{
    if (!client) return (1);

    char    buf[0x100];
    int     bytes = 0;

    memset(buf, 0, sizeof(buf));

    bytes = recv(client->fd, buf, sizeof(buf), 0);

    if (bytes < 0) return (-1);

    if (!bytes) return (0);

    printf("%s\n", buf);

    return (0);
}

int		server_poll_close(server_t *server)
{
    if (!server) return (1);

    if (close(server->fd) < 0) perror("close");

    printf("[" GREEN "+" RST"] Server closed !\n");

    SERVER_ZERO(*server);

    return (0);
}

void	server_poll_signal(int signum)
{
    (void)signum;
    printf("[+] CTRL+C Catched !\n");
}
