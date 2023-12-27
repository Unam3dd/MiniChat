#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/logging.h>
#include <netinet/in.h>
#include <unistd.h>
#include "mini_chat.h"

#define TEST_SELECT_SERVER_HOST "127.0.0.1"
#define TEST_SELECT_SERVER_PORT 8081

typedef struct sockaddr_in  sin_t;

TestSuite(select_test);

Test(select_test, simple_message)
{
    sin_t   sin;
    int fd  = 0;

    cr_log_info("creating socket file descriptor...");

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    cr_expect(fd != -1, "\033[31m [ ERROR ] socket file descriptor was not created successfully !\033[00m\n");

    cr_log_info("socket file descriptor has been created %d !\n", fd);

    cr_log_info("zeromemory on " GREEN "%p (%zd)"RST" sockaddr_in structure", &sin, sizeof(sin_t));

    memset(&sin, 0, sizeof(sin_t));

    sin.sin_addr.s_addr = inet_addr(TEST_SELECT_SERVER_HOST);
    sin.sin_port = htons(TEST_SELECT_SERVER_PORT);
    sin.sin_family = AF_INET;

    cr_log_info("FAMILY : AF_INET | sin.sin_addr.s_addr = %x (127.0.0.1) | sin.sin_port = %x (%d)\n", \
            sin.sin_addr.s_addr, sin.sin_port, TEST_SELECT_SERVER_PORT);

    cr_log_info("Trying connect to the remote server !\n");

    cr_expect(connect(fd, (struct sockaddr *)&sin, sizeof(sin)) != -1, "\033[31m [ ERROR ] failed connect to the remote server !\033[00m\n");

    cr_log_info("["GREEN"+"RST"] You are connected to the target remote server !\n");

    cr_log_info("Sending Message...\n");

    cr_expect(send(fd, "hello", 5, 0) != -1, "\033[31m [ ERROR ] failed to send message to the target server !\033[00m\n");

    cr_log_info("The message was sent successfully !\n");

    cr_log_info("closing %d socket file descriptor\n", fd);

    close(fd);
}

Test(select_test, get_select_server_version)
{
    char    version[0x40];
    sin_t   sin;
    int     fd = 0;

    memset(version, 0, sizeof(version));
    memset(&sin, 0, sizeof(sin));

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    cr_expect(fd != -1, "[ ERROR ] socket system call failed !\n");

    sin.sin_addr.s_addr = inet_addr(TEST_SELECT_SERVER_HOST);
    sin.sin_port = htons(TEST_SELECT_SERVER_PORT);
    sin.sin_family = AF_INET;

    cr_expect(connect(fd, (struct sockaddr*)&sin, sizeof(sin_t)) != -1, "Error connect to the target server !\n");

    cr_log_info("Sending version payload...");

    cr_expect(send(fd, "version\n", 8, 0) != -1, "Error sending version payload !\n");

    cr_expect(recv(fd, version, sizeof(version), 0) != -1, "Error when recv system call was called !\n");

    cr_log_info("recv : %s", version);

    close(fd);
}
