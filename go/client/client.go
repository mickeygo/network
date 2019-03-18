package main

/*
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void str_cli(FILE *fp, int sockfd)
{
    char sendline[1024], recvline[1024];
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

int start(int argc , char** argv)
{
    int sockfd;
    struct sockaddr_in seraddr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\r\n");
    }

    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(13000);
    if((inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr)) < 0)
    {
        printf("inet_pton error\r\n");
	}

    if(connect(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0)
    {
        printf("connect error\r\n");
    }

    str_cli(stdin, sockfd);
    return 0;
}
*/
import "C"

func main() {
	C.start(0, nil)
}
