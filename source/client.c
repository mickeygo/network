#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define max(a,b) (a>b)?(a):(b)
#define MAXLINE 1024

void str_cli(FILE *fp, int sockfd)
{
    int maxfdpl, stdineof;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];
    char buf[MAXLINE];
    int n;

    stdineof = 0;
    FD_ZERO(&rset);

    // fgets函数，读入一行文本，当遇到文件结束符或错误时，返回一个空指针
    // fgets函数会阻塞进程，直到输入 EOF(Control-D) 字符时将终止循环；此处的 fgets 函数会等待用户在控制台的输入
    for (;;) {
        if (stdineof == 0) {
            FD_SET(fileno(fp), &rset);
        }

        FD_SET(sockfd, &rset);
        maxfdpl = max(fileno(fp), sockfd) + 1;

        // select 函数
        // 此处阻塞进程，当 socket 或 stdin 中任何一方可用，
        select(maxfdpl, &rset, NULL, NULL, NULL);

        // socket 可用
        if (FD_ISSET(sockfd, &rset)) {
            if ((n = read(sockfd, buf, MAXLINE)) == 0) {  // 0 表示读到 EOF
                if (stdineof == 1) {
                    return; // normal termination
                } else {
                    printf("server terminated\n");
                    exit(0);
                }
            }
            write(fileno(fp), buf, n);
        }

        // input 可用
        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = read(fileno(fp), buf, MAXLINE)) == 0) {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);  // SHUT_WR, 发送 FIN
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            write(sockfd, buf, n);
        }
    }
}

int main(int argc , char** argv)
{
    int sockfd;
    struct sockaddr_in seraddr;

    if (argc != 2) {
        printf("argc error\r\n");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error\r\n");
    }

    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(13000);
    if ((inet_pton(AF_INET, argv[1], &seraddr.sin_addr)) < 0) {
        printf("inet_pton error\r\n");
    }

    // connect 之前没有 bind，不需要与指定的port绑定
    // 引发 TCP 的三次握手协议
    //  客户端端口号分配规则是什么？-- 随机分配
    //  服务器在什么状态下会请求失败？
    if (connect(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0) {
        printf("connect error\r\n");
    }

    str_cli(stdin, sockfd);
    return 0;
}