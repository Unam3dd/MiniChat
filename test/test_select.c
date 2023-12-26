#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <netinet/in.h>
#include <unistd.h>

TestSuite(select_test);

Test(select_test, simple_message)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    cr_expect(fd != -1, "Error when socket file descriptor is created !");

    close(fd);
}
