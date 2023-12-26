#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/logging.h>
#include <netinet/in.h>
#include <unistd.h>

TestSuite(select_test);

Test(select_test, simple_message)
{
    cr_log_info("test !");
}
