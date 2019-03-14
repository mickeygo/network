package main

/*
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[1024];

    while((n = read(sockfd, buf, 1024)) > 0)  // 从 socket 中读取数据
    {
        write(sockfd, buf, n);  // 将数据写入 socket
    }

    if(n < 0)
    {
        printf("read error\r\n");
    }
}


int start(int argc, char** argv)
{
    int listenfd, connectfd; // listenfd -- ; connectfd --
    socklen_t client_len;
    struct sockaddr_in servaddr, client_addr;
    pid_t childpid;

    // 创建 socket
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("sock error\r\n");
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13000);
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    // bind socket 到 ip
    if(bind(listenfd, (struct sockaddr* )&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error\r\n");
        return 0;
    }

    // listen
    if(listen(listenfd, 0) < 0)
    {
        printf("listen error\r\n");
        return 0;
    }

    for( ; ; )
    {
        client_len = sizeof(client_addr);
        connectfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);

        if( (childpid = fork())== 0)
        {
            close(listenfd);
            str_echo(connectfd);
            exit(0);
        }

        close(connectfd);
    }
}
*/
import "C"

func main() {
	C.start(0, nil)
}
