#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdlib.h>

union val
{
    int     i_val;
    long    l_val;
    struct  linger  linger_val;
    struct  timeval timeval_val;
} val;

static char *sock_str_flag(union val *, int);
static char *sock_str_int(union val *, int);
static char *sock_str_linger(union val *, int);
static char *sock_str_timval(union val *, int);

struct sock_opts
{
    const char *opt_str;
    int         opt_level;
    int         opt_name;
    char    *(*opt_val_str)(union val *, int);
} sock_opts[] = {
    { "SO_BROADCAST", SOL_SOCKET, SO_BROADCAST, sock_str_flag}
};

int main(int argc, char **argv) {
    int fd;
    socklen_t len;
    struct sock_opts *str;

    exit(0);
}