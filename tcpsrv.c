#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <libgen.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>

#define MAX 80
#define SA struct sockaddr

int g_debug = 0;

void usage(char* program)
{
    printf(
            "USAGE: %s [ARGS] <port>\n"
            "ARGS:\n"
            " -d        Print some debug info\n"
            " -h        Print this help and exit\n"
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

// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int i;
    // infinite loop for chat
    while (1)
    {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // strip of last \n if present
        if (buff[strlen(buff) - 1] == '\n')
            buff[strlen(buff) - 1] = '\0';

        // print buffer which contains the client contents
        info("From client: [%s], echoing msg back.\n", buff);

	// Convert the message to uppercase
	for (i = 0; i < strlen(buff); ++i)
	{
		buff[i] = toupper(buff[i]);
	}

        // and send that buffer to client
        write(sockfd, buff, sizeof(buff));
    }
}

// Driver function
int main(int argc, char* argv[])
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    int c, portno;

    // Parse options
    while ((c = getopt(argc, argv, "dhl:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                g_debug = 1;
                break;
            case 'h':
                usage(argv[0]);
                exit (0);
            default:
                usage(argv[0]);
                exit (1);
        }
    }
    // parse the positional arguments
    if (argc - optind != 1)
    {
        usage(argv[0]);
        exit (1);
    }
    portno = atoi(argv[optind]);

    // socket create and verification
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit (1);
    }
    info("Socket successfully created.\n");

    bzero(&servaddr, sizeof(servaddr));

    // Assign IP and port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
    {
        perror("bind");
        exit (1);
    }
    info("Socket successfully binded.\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        perror("listen");
        exit (1);
    }
    info("Server listening to 0.0.0.0/0 on port %d ...\n", portno);

    len = sizeof(cli);
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0)
    {
        perror("accept");
        exit (1);
    }
    info("Server accepting incoming client.\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}

