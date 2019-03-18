#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void str_cli(FILE *fp, int sockfd)
{
    char sendline[1024], recvline[1024];

    // fgets函数，读入一行文本，当遇到文件结束符或错误时，返回一个空指针
    // fgets函数会阻塞进程，直到输入 EOF(Control-D) 字符时将终止循环
    while(fgets(sendline, 1024, fp) != NULL)
    {
        write(sockfd, sendline, strlen(sendline));
        if(read(sockfd, recvline, 1024) <= 0)
        {
            printf("server terminated\n");
        }

        fputs(recvline, stdout);
    }
}

int main(int argc , char** argv)
{
    int sockfd;
    struct sockaddr_in seraddr;

    if(argc != 2)
    {
        printf("argc error\r\n");
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\r\n");
    }

    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(13000);
    if((inet_pton(AF_INET, argv[1], &seraddr.sin_addr)) < 0)
    {
        printf("inet_pton error\r\n");
    }

    // connect 之前没有 bind，不需要与指定的port绑定
    // 引发 TCP 的三次握手协议
    //  客户端端口号分配规则是什么？-- 随机分配
    //  服务器在什么状态下会请求失败？
    if(connect(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0)
    {
        printf("connect error\r\n");
    }

    str_cli(stdin, sockfd);
    return 0;
}