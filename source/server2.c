#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <limits.h>

#define MAXLINE     4096    // 数据缓冲大小
#define SERV_PORT   9877    // 端口号

#ifndef INFTIM
#define INFTIM      (-1)
#endif

int main(int argc, char** argv) {
    int     i, maxi, listenfd, connfd, sockfd;
    int     nready;
    ssize_t n;
    char    buf[MAXLINE];
    socklen_t clilen;
    struct  pollfd client[OPEN_MAX];
    struct  sockaddr_in cliaddr, servaddr;
    
    // 创建 socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("bind error\r\n");
        return 0;
    }

    if (listen(listenfd, 0) < 0) {
        printf("listen error\r\n");
        return 0;
    }

    // 声明在 pollfd 结构数组中存在 OPEN_MAX 个元素
    // 此处标记第一个文件描述符为监听 socket, 后续的初始化为 -1
    //  events 表示要测试的条件，作为调用值使用
    //  revents 表示描述符的状态，作为返回结果使用
    // POLLRDNORM 表示普通数据可读(所有的 TCP 和 UDP 数据都被认为是普通数据)
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for (int i = 1; i < OPEN_MAX; i++) {
        client[i].fd = -1;  // 将后续事件初始化为 -1
    }
    maxi = 0;  // 指示 client 数组当前正在使用的最大下标值

    for(;;) {
        // pollfd 结构数组，用于指定测试某个给定描述符的条件
        // nfds 数组中元素个数
        // INFTIM -1 表示永远等待, 直到有描述符准备就绪
        // nready 表示就绪的描述符的数目
        nready = poll(client, maxi + 1, INFTIM);

        // 使用 poll 等待新的连接或者现有连接上有数据可读。当有一个新的连接被接受后，在 client 数组中查找第一个描述符成员为负的可用项。
        // 找到一个可用项后，把新的连接描述符保存在其中。client[0] 固定用于监听套接字。

        // 监听事件
        if (client[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

            for(i = 1; i < OPEN_MAX; i++) {
                if (client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;
                }
            }
            if (i == OPEN_MAX) {
                printf("too many clients");
                exit(1);
            }

            client[i].events = POLLRDNORM;
            if (i > maxi)
                maxi = i;

            if (--nready <= 0)
                continue;
        }

        // 调用所有数据的客户端
        for(i = 1; i < maxi; i++) {
            if ((sockfd = client[i].fd) < 0)
                continue;
            
            // POLLERR 发生错误
            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {  // 客户端终止了连接 RST
                        close(sockfd);
                        client[i].fd = -1;
                    } else {
                        printf("read error");
                    }
                } else if (n == 0) {    // 客户端关闭了连接 close (如何判断 read 返回 0 就是客户端关闭了连接？）
                    close(sockfd);
                    client[i].fd = -1;
                } else {
                    write(sockfd, buf, n);
                }

                if (--nready <= 0)
                    break;
            }
        }
    }
}
