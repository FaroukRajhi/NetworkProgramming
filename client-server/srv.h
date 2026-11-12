#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#define PORT "8888" /* the users will connect to this port*/
#define BACKLOG 10 /* queue will hold pending connections*/

void sigchld_handler(int s)
{
    void(s);

    int saved_errno = errno; /* waitpid might overwrite errno*/

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}