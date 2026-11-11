
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

/* Global TAP file descriptor */
static int tap_fd = -1;

/* Stop the program cleanly */
static volatile sig_atomic_t running = 1;

void handle_signal(int signal_number)
{
    (void)signal_number;
    running = 0;
}

/* Create a TAP interface */
int create_tap(char *interface_name)
{
    struct ifreq ifr;

    int fd = open("/dev/net/tun", O_RDWR);

    if (fd < 0) {
        perror("open /dev/net/tun");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);

    if (ioctl(fd, TUNSETIFF, &ifr) < 0) {
        perror("ioctl TUNSETIFF");
        close(fd);
        return -1;
    }

    printf("[+] TAP interface created: %s\n", ifr.ifr_name);

    return fd;
}

/* Print a MAC address */
void print_mac(const unsigned char *mac)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
}

/* Analyze TCP header */
void process_tcp(unsigned char *data, int length)
{
    if (length < (int)sizeof(struct tcphdr)) {
        printf("    Invalid TCP header\n");
        return;
    }

    struct tcphdr *tcp = (struct tcphdr *)data;

    printf("    Protocol        : TCP\n");
    printf("    Source port     : %u\n", ntohs(tcp->source));
    printf("    Destination port: %u\n", ntohs(tcp->dest));
    printf("    Sequence number : %u\n", ntohl(tcp->seq));
    printf("    Acknowledgment  : %u\n", ntohl(tcp->ack_seq));

    printf("    Flags           :");

    if (tcp->syn) printf(" SYN");
    if (tcp->ack) printf(" ACK");
    if (tcp->fin) printf(" FIN");
    if (tcp->rst) printf(" RST");
    if (tcp->psh) printf(" PSH");
    if (tcp->urg) printf(" URG");

    printf("\n");
}

/* Analyze UDP header */
void process_udp(unsigned char *data, int length)
{
    if (length < (int)sizeof(struct udphdr)) {
        printf("    Invalid UDP header\n");
        return;
    }

    struct udphdr *udp = (struct udphdr *)data;

    printf("    Protocol        : UDP\n");
    printf("    Source port     : %u\n", ntohs(udp->source));
    printf("    Destination port: %u\n", ntohs(udp->dest));
    printf("    UDP length      : %u\n", ntohs(udp->len));
}

/* Analyze ICMP header */
void process_icmp(unsigned char *data, int length)
{
    if (length < (int)sizeof(struct icmphdr)) {
        printf("    Invalid ICMP header\n");
        return;
    }

    struct icmphdr *icmp = (struct icmphdr *)data;

    printf("    Protocol        : ICMP\n");
    printf("    Type            : %u\n", icmp->type);
    printf("    Code            : %u\n", icmp->code);
}

/* Analyze IPv4 packet */
void process_ipv4(unsigned char *data, int length)
{
    if (length < (int)sizeof(struct iphdr)) {
        printf("[-] Incomplete IPv4 header\n");
        return;
    }

    struct iphdr *ip = (struct iphdr *)data;

    int header_length = ip->ihl * 4;

    if (ip->version != 4 ||
        header_length < 20 ||
        length < header_length) {
        printf("[-] Invalid IPv4 header\n");
        return;
    }

    char source[INET_ADDRSTRLEN];
    char destination[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &ip->saddr,
              source, sizeof(source));

    inet_ntop(AF_INET, &ip->daddr,
              destination, sizeof(destination));

    printf("  [IPv4]\n");
    printf("    Source IP       : %s\n", source);
    printf("    Destination IP  : %s\n", destination);
    printf("    TTL             : %u\n", ip->ttl);
    printf("    Header length   : %d\n", header_length);
    printf("    Total IP length : %u\n", ntohs(ip->tot_len));

    /* Validate the IP total length */
    int total_length = ntohs(ip->tot_len);

    if (total_length < header_length || total_length > length) {
        printf("[-] Invalid IPv4 total length\n");
        return;
    }

    unsigned char *transport = data + header_length;
    int transport_length = total_length - header_length;

    switch (ip->protocol)
    {
        case IPPROTO_TCP:
            process_tcp(transport, transport_length);
            break;

        case IPPROTO_UDP:
            process_udp(transport, transport_length);
            break;

        case IPPROTO_ICMP:
            process_icmp(transport, transport_length);
            break;

        default:
            printf("    Other IP protocol: %u\n", ip->protocol);
            break;
    }
}

/* Analyze IPv6 packet */
void process_ipv6(unsigned char *data, int length)
{
    if (length < (int)sizeof(struct ipv6hdr)) {
        printf("[-] Incomplete IPv6 header\n");
        return;
    }

    struct ipv6hdr *ip6 = (struct ipv6hdr *)data;

    if (ip6->version != 6) {
        printf("[-] Invalid IPv6 header\n");
        return;
    }

    char source[INET6_ADDRSTRLEN];
    char destination[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &ip6->saddr,
              source, sizeof(source));

    inet_ntop(AF_INET6, &ip6->daddr,
              destination, sizeof(destination));

    printf("  [IPv6]\n");
    printf("    Source IP       : %s\n", source);
    printf("    Destination IP  : %s\n", destination);
    printf("    Hop limit       : %u\n", ip6->hop_limit);
    printf("    Next header     : %u\n", ip6->nexthdr);

    printf("    Transport parsing: Not implemented for IPv6\n");
}

/* Analyze Ethernet frame */
void process_frame(unsigned char *buffer, int length)
{
    if (length < ETH_HLEN) {
        printf("[-] Invalid Ethernet frame\n");
        return;
    }

    struct ethhdr *eth = (struct ethhdr *)buffer;

    printf("\n====================================\n");
    printf("       ETHERNET FRAME RECEIVED\n");
    printf("====================================\n");

    printf("Source MAC      : ");
    print_mac(eth->h_source);

    printf("\nDestination MAC : ");
    print_mac(eth->h_dest);

    unsigned short protocol = ntohs(eth->h_proto);

    printf("\nEtherType       : 0x%04x\n", protocol);
    printf("Frame size      : %d bytes\n", length);

    unsigned char *payload = buffer + ETH_HLEN;
    int payload_length = length - ETH_HLEN;

    switch (protocol)
    {
        case ETH_P_IP:
            process_ipv4(payload, payload_length);
            break;

        case ETH_P_IPV6:
            process_ipv6(payload, payload_length);
            break;

        case ETH_P_ARP:
            printf("  [ARP] ARP packet detected\n");
            break;

        default:
            printf("  [UNKNOWN] Other Ethernet protocol\n");
            break;
    }
}

/* Main function */
int main(void)
{
    unsigned char buffer[65536];

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("[+] Starting TAP monitor...\n");

    tap_fd = create_tap("tap0");

    if (tap_fd < 0) {
        return EXIT_FAILURE;
    }

    printf("[+] Waiting for Ethernet frames...\n");
    printf("[+] Press CTRL+C to stop.\n");

    while (running)
    {
        int nread = read(tap_fd, buffer, sizeof(buffer));

        if (nread < 0) {
            if (errno == EINTR)
                continue;

            perror("read");
            break;
        }

        if (nread == 0)
            continue;

        process_frame(buffer, nread);
    }

    printf("\n[+] Stopping TAP monitor...\n");

    close(tap_fd);
    tap_fd = -1;

    return EXIT_SUCCESS;
}
