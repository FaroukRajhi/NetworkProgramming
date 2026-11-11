#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>


/* TAP file descriptor*/

static int tap_fd = -1;

/* Stop the program safely 
Designed for variables that are accessed by:
- The normal execution for your program.
- A signal handöer
*/

static volatile sig_atomic_t running = 1;

void signal_handler(int sig_num)
{
    (void)sig_num;
    running = 0;
}

/* Create a TAP interface*/

int create_tap(char *interface_name)
{
    struct ifreq ifr;
    int fd = open("/dev/net/tun", O_RDWR);

    if (fd < 0)
    {
        perror("Open /dev/net/tun");

        return -1;        
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);

}