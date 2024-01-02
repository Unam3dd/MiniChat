#include "mini_chat.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>

int		server_epoll_init(server_t *server)
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

static void clear_client(int efd)
{
    client_t    *arr = NULL;
    size_t      len = 0, nmemb = 0;
    uint32_t i = 0;

    arr = get_clients(0, &nmemb, &len);

    for (i = 0; i < len; i++) {
        if (!arr[i].fd)
            continue ;
        epoll_ctl(efd, EPOLL_CTL_DEL, arr[i].fd, NULL);
        close(arr[i].fd);
    }

    memset(arr, 0, nmemb);
}

int		server_epoll_wait(server_t *server)
{
    if (!server) return (1);
    struct epoll_event  evs[EPOLL_EV];
    int efd             = 0, nfds = 0, i = 0;
    int status = 0;

    efd = epoll_create1(0);

    if (efd < 0) perror("epoll_create1");

    memset(evs, 0, sizeof(evs));

    evs[0].data.fd = server->fd;
    evs[0].events = EPOLLIN | EPOLLET;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, server->fd, &evs[0]) < 0) {
        perror("epoll_ctl");
        return (1);
    }

    signal(SIGINT, server_epoll_signal);

    while (0xB4DC0D3) {
        nfds = epoll_wait(efd, evs, EPOLL_EV, -1);

        if (nfds < 0)
            break ;

        for (i = 0; i < nfds; i++) {

            if (evs[i].data.fd == server->fd) {
                server_epoll_accept(server->fd, efd);
                continue ;
            }
            
            status = server_epoll_handle((client_t*)evs[i].data.ptr);

            if (status < 0) {
                printf("server_epoll_handler error !\n");
                continue ;
            }

            if (status) {
                remove_client((client_t*)evs[i].data.ptr);
                epoll_ctl(efd, EPOLL_CTL_DEL, ((client_t*)evs[i].data.ptr)->fd, NULL);
            }
        }
    }

    clear_client(efd);

    close(efd);

    return (0);
}

int		server_epoll_listen(server_t *server, const char *addr, port_t port)
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

int		server_epoll_accept(fd_t sfd, fd_t efd)
{
    char         buf[0x100];
    struct epoll_event ev;
    client_t    cli;
    sin_t   sin;
    socklen_t   len = sizeof(sin);
    int nfd = 0;

    memset(&sin, 0, sizeof(sin_t));

    nfd = accept(sfd, (struct sockaddr *)&sin, &len);
    
    if (nfd < 0) {
        perror("accept");
        return (1);
    }

    memset(&ev, 0, sizeof(ev));
    memset(&cli, 0, sizeof(cli));
    memset(buf, 0, sizeof(buf));

    sprintf(buf, "[" GREEN "+" RST"] Server : New client connected to the server %x:%x ("GREEN"%s"RST":"GREEN"%d"RST") !\n", sin.sin_addr.s_addr, sin.sin_port, inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

    printf("%s", buf);

    cli.fd = nfd;

    add_client(&cli);

    client_t    *ptr = get_client(&cli);

    ev.data.ptr = (client_t *)ptr;
    ev.events = EPOLLIN;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, nfd, &ev) < 0) {
        perror("epoll_ctl");
        return (1);
    }

    return (0);
}

int		server_epoll_handle(client_t *client)
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

int		server_epoll_close(server_t *server)
{
    if (server && close(server->fd) < 0) perror("close");

    printf("[" GREEN "+" RST"] Server closed !\n");

    SERVER_ZERO(*server);
    return (0);
}

void	server_epoll_signal(int signum)
{
    (void)signum;
    puts("[-] Ctrl+C");
}
