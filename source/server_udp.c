#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXLINE     4096    // 数据缓冲大小
#define SERV_PORT   9878   // 端口号

void dg_echo(int sockfd, struct sockaddr* pcliaddr, socklen_t clilen)
{
    int         n;
    socklen_t   len;
    char    mesg[MAXLINE];

    for(;;)
    {
        len = clilen;
        // recvfrom 会阻塞；提供了一个迭代服务器
        // pcliaddr 参数表示 from, 指向数据报发送者协议地址的套接字地址结构; len 表示其长度
        // recvfrom 返回 0 可接受（TCP 中表示对端已关闭连接）
        n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }
}

int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    // SOCK_DGRAM 表示是 UDP 套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    return 0;
}