#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <libgen.h>
#include <stdarg.h>
#include <signal.h>
#include <getopt.h>

#define MAX 80
#define SA struct sockaddr

int g_debug = 0;
int g_sockfd;

void usage(char* program)
{
    printf(
            "USAGE: %s [ARGS] <hostname> <port>\n"
            "ARGS:\n"
            " -d         Print some debug info\n"
            " -h         Print this help and exit\n"
            " -p <proto> L4 protocol to be used: tcp,udp (defaults to tcp)\n"
            , basename(program)
          );
}

void info(const char* msg, ...)
{
    if (!g_debug) return;
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

int sig_hdlr(int signo)
{
    if (signo == SIGINT)
    {
        info("\nClosing socket, bye!\n");
        close(g_sockfd);
        exit (0);
    }
    return 0;
}

int main_loop()
{
    char buff[MAX];
    int n;
    while (1)
    {
        bzero(buff, sizeof(buff));
        printf("c>>s ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(g_sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(g_sockfd, buff, sizeof(buff));
        printf("\t[%s] s>>c\n", buff);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in servaddr, cli;
    int c, portno;
    int socket_type;
    char hostname[1024];
    char proto[16];

    strcpy(proto, "tcp");
    socket_type = SOCK_STREAM;
    // parse options
    while (( c = getopt(argc, argv, "dhp:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                g_debug = 1;
                break;
            case 'h':
                usage(argv[0]);
                exit (0);
            case 'p':
                // check proto
                if (strcmp(optarg, "tcp") != 0 && strcmp(optarg, "udp"))
                {
                    fprintf(stderr, "Unknown protocol \"%s\"\n", optarg);
                    usage(argv[0]);
                    exit (1);
                }
                strcpy(proto, optarg);
                if (!strcmp(proto, "tcp")) socket_type = SOCK_STREAM;
                else socket_type = SOCK_DGRAM;
                break;
            default:
                usage(argv[0]);
                exit (1);
        }
    }
    if (argc - optind != 2)
    {
        usage(argv[0]);
        exit (1);
    }

    // parse the positional arguments
    strcpy(hostname, argv[optind]);
    portno = atoi(argv[optind + 1]);
    info("Attempting TCP connection to %s on port %d\n", hostname, portno);

    // socket create and varification
    if ((g_sockfd = socket(AF_INET, socket_type, 0)) == -1)
    {
        perror("socket");
        exit (1);
    }
    info("Socket successfully created.\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(hostname);
    servaddr.sin_port = htons(portno);

    // Only TCP connects
    if (socket_type == SOCK_STREAM)
    {
        // connect the client socket to server socket
        if (connect(g_sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
        {
            perror("connect");
            exit (1);
        }
        info("Connected to the remote server.\n");
    }

    // function for chat
    return main_loop();
}

