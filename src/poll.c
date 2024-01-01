#include "mini_chat.h"
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/poll.h>
#include <string.h>

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

int		server_poll_accept(fd_t sfd, struct pollfd *pfds, int *nfds)
{
    if (!pfds) return (-1);
    char        buf[0x100];
    client_t    cli;
    sin_t       sin;
    socklen_t   len = sizeof(sin_t);
    uint32_t    i = 0;
    int fd      = 0;

    fd = accept(sfd, (struct sockaddr*)&sin, &len);

    if (fd < 0) return (-1);

    for (i = 1; i < EVENT_MAX; i++) {
        if (pfds[i].fd) continue ;
        
        pfds[i].fd = fd;
        pfds[i].events = POLLIN;
        break ;
    }

    if (i >= EVENT_MAX) {
        printf("[+] The server refused the client ("GREEN"%s"RST":"GREEN"%d"RST")!\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
        close(fd);
        return (1);
    }

    cli.buf = NULL;
    cli.fd = fd;
    cli.size = 0;

    add_client(&cli);
    if (nfds) *nfds += 1;

    memset(buf, 0, sizeof(buf));

    sprintf(buf, "[" GREEN "+" RST"] Server : New client connected to the server %x:%x ("GREEN"%s"RST":"GREEN"%d"RST") !\n", sin.sin_addr.s_addr, sin.sin_port, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

    printf("%s", buf);

    send_msg_clients(cli.id, buf, strlen(buf));

    return (0);
}

static void clear_client(struct pollfd  *pfds)
{
    client_t    *clients = NULL;
    size_t      size = 0;
    size_t      i = 0;

    clients = get_clients(0, NULL, &size);

    for (i = 0; i < size; i++)
        close(clients[i].fd);
    memset(pfds, 0, sizeof(struct pollfd) * EVENT_MAX);
    memset(clients, 0, (sizeof(client_t) * size));
}

int		server_poll_wait(server_t *server)
{
    if (!server) return (1);

    struct pollfd   pfds[EVENT_MAX];
    client_t        *clients = get_clients(0, NULL, NULL);
    int             nfds = 1;
    int             i = 0;
    int             s = 0;

    memset(pfds, 0, sizeof(pfds));

    pfds[0].fd = server->fd;
    pfds[0].events = POLLIN;

    while (0xB4DC0D3) {

        if (poll(pfds, nfds, -1) < 0) {
            perror("poll");
            break ;
        }

        if (pfds[0].revents & POLLIN) {
            if (server_poll_accept(server->fd, pfds, &nfds) < 0)
                perror("server_poll_accept");
            continue ;
        }

        for (i = 1; i < EVENT_MAX; i++) {
            
            if (!(pfds[i].revents & POLLIN))
                continue ;

            s = server_poll_handle(&clients[i-1]);
            if (s < 0) perror("server_poll_handle");
            if (s) {
                remove_client(&clients[i-1]);
                memset(&pfds[i], 0, sizeof(struct pollfd));
                nfds--;
            }
        }
    }

    clear_client(pfds);

    return (0);
}

int		server_poll_handle(client_t *client)
{
    if (!client) return (1);

    char    buf[0x100];
    char    snd[0x200];
    int     status = 0;

    memset(buf, 0, sizeof(buf));

    status = recv(client->fd, buf, sizeof(buf), 0);
    
    if (!status) return (1);

    if (status < 0) return (-1);

    if (!strcmp("version\n", buf))
        return (send_version(client), 0);

    memset(snd, 0, sizeof(snd));

    snprintf(snd, sizeof(snd), "client (%d): %s", client->id, buf);

    send_msg_clients(client->id, snd, strlen(snd));

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
