package main

/*
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
        n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }
}

int start(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	dg_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

	return 0;
}
*/
import "C"

func main() {
	C.start(0, nil)
}
