// socket 客户端，可参考文档 http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
#include <time.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h> 

// 从客户读取数据，并把这些数据 echo 给客户
// `sockfd` socket 描述符
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

// 
void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    // 使用 waitpid 而不是 wait 函数来处理 SIGCHLD 信号; 使用 while 循环，以获取所有已终止子进程的状态。
    // -1 监听范围扩大到任意子进程
    // WNOHANG: 告知waitpid在有尚未终止的子进程在运行时不要阻塞
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);

    return;
}

int main(int argc, char** argv)
{
    int listenfd, connectfd; // listenfd 套接字描述符; 
    socklen_t client_len;
    struct sockaddr_in servaddr, client_addr;
    pid_t childpid;

    // 创建 socket, 
    // AF_INET -- IPv4; 
    // SOCK_STREAM -- 字节流套接字
    // protocol: 套接字所用的协议， 0 表示缺省值
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("sock error\r\n");
        return 0;
    }

    // bzero 函数，置字节字符串前n个字节为零且包括‘\0’, 可使用 ANSI C函数 memset(&servaddr, 0, sizeof(servaddr)) 代替
    // sockaddr_in IPv4 网际套接字地址结构，用于指明地址信息
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13000);  // 绑定到 13000 端口, htons() 将一个无符号短整型数值转换为网络字节序,大端模式(big-endian)
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0"); // 32位二进制网络字节的IPv4地址

    // 将一个本地协议地址（IPv4/IPv6 + TCP/UDP端口号）赋予此 socket
    // bind 第三个参数是该地址结构(第二个参数)的长度
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("bind error\r\n");
        return 0;
    }

    // listen 指示内核应该接受指向该套接字的连接(connect)请求
    // 第二个参数 backlog 规定了内核应该为相应套接字排队的最大连接个数
    //  note: listen 将上面未连接的套接字转换为一个被动套接字
    //      listen 与 accept 区别在哪？ listen 函数难道仅仅是标识 socket 为被动套接字？ 连接队列和已完成连接队列什么阶段产生的？
    //      调用 listen 函数时有没有开始进行侦听，还是说在调用 accept 是才开始侦听？
    //      参考网址：https://stackoverflow.com/questions/34073871/socket-programming-whats-the-difference-between-listen-and-accept
    //  listen 表示愿意接收即将到来的连接，并为要进入的连接指定一个队列的大小（此时还未开始进行监听）
    //  accept 启动监听，并堵塞进程，直到已完成连接的队列中有数据；同时也会创建一个新的 socket
    if(listen(listenfd, 0) < 0)
    {
        printf("listen error\r\n");
        return 0;
    }

    // 处理僵死子线程
    // SIGCHLD: 当子进程状态变更了时(停止或退出)，会发送这个信号通知父进程。父进程可通过 wait/waitpid 来获悉这些状态。
    signal(SIGCHLD, sig_chld);

    for( ; ; )
    {
        client_len = sizeof(client_addr);

        // accept 从已完成连接队列的头部返回下一个已完成的连接，若此队列为空，进程被投入睡眠(进入阻塞)
        // listenfd 上述创建的监听套接字
        // connectfd 已连接的套接字
        //  accept 函数会产生一个新的 socket 用于与客户进行通信，这个新套接字是怎么样的？
        //  -- 源端口为当前绑定的端口，目的端口号为客户端的端口号
        // fork 之后，服务器继续调用此函数
        // improved, 处理被中断的系统调用
        // note: 慢系统调用(slow system call)
        if ((connectfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            // errno为EINTR表征的意义:
            //  由于信号中断，没写成功任何数据。  
            //  由于信号中断，没读到任何数据。  
            //  由于信号中断返回，没有任何数据可用。  
            //  函数调用被信号处理函数中断。  
            if (errno == EINTR) {
                continue;
            } else {
                // err_sys("accept error"); // throw error and exit
                exit(1);
            }
        }

        // fork 函数
        // 为 0 表示为当前进程为子进程
        // 父进程与子进程引用同一个 socket，此套接字引用描述符为 2，close() 函数表示将套接字引用减 1（注：并不是关闭 socket，其关闭由内核决定）
        // note: fork之后，操作系统会复制一个与父进程完全相同的子进程，虽说是父子关系，但是在操作系统看来，他们更像兄弟关系，这2个进程共享代码空间，
        //  但是数据空间是互相独立的，子进程数据空间中的内容是父进程的完整拷贝，指令指针也完全相同，
        //  子进程拥有父进程当前运行到的位置（两进程的程序计数器pc值相同，也就是说，子进程是从fork返回处开始执行的），
        //  至于那一个最先运行，可能与操作系统（调度算法）有关。
        // 关于父进程与子进程关系，可参考文章 http://www.cnblogs.com/wuguiyunwei/p/7058056.html
        if((childpid = fork()) == 0)
        {
            // 在 fork 的同时，子线程也复制了引用监听的套接字引用对象，需要关闭
            close(listenfd);
            str_echo(connectfd);
            exit(0);    // 子进程退出
        }

        // 父进程与子进程都关闭已连接的socket
        close(connectfd);
    }
}
