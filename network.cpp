#include <sys/ioctl.h>      /* ioctl        */
#include <net/if.h>
#include <sys/socket.h>     /* socket       */
#include <netdb.h>
#include <arpa/inet.h>      /* inet_addr    */
#include <string.h>
#include <stdio.h>
#include <unistd.h>         /* close        */

static bool get_inf_Addr(char *id, const char* iface)
{
    struct ifreq ifr;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (fd < 0) {
        perror("Open socket failed");
        return false;
    } 

    bool ret = false;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
        for (int i = 0; i < 6; ++i)
            snprintf(id+i*2, 3,"%02x", (unsigned char)ifr.ifr_addr.sa_data[i]);
        ret = true;
    }
    else {
        perror("ioctl failed");
        ret = false;
    }
    close(fd);
    return ret;
}

void get_macAddr(char *id)
{
    if (get_inf_Addr(id, "enp1s0"))
        return;
    else // mac address is fixed by /etc/udev/rules.d/75-net-setup-link.rules
        get_inf_Addr(id, "eth0");
}

void set_hostIP(const char* if_name, const char* if_addr, const char* if_mask)
{
    struct ifreq ifr;
    struct sockaddr_in sai;
    int sockfd;                     /* socket fd we use to manipulate stuff with */

    /* Create a channel to the NET kernel. */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* get interface name */
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);

    // Setting or deleting the interface address is a privileged operation.
    memset(&sai, 0, sizeof(struct sockaddr));
    sai.sin_family = AF_INET;
    sai.sin_port = 0;
    sai.sin_addr.s_addr = inet_addr(if_addr);
    memcpy( &(ifr.ifr_addr) , &sai, sizeof(struct sockaddr));
    ioctl(sockfd, SIOCSIFADDR, &ifr);

    // Setting the network mask is a privileged operation.
    memset(&sai, 0, sizeof(struct sockaddr));
    sai.sin_family = AF_INET;
    sai.sin_port = 0;
    sai.sin_addr.s_addr = inet_addr(if_mask);
    memcpy( &(ifr.ifr_addr) , &sai, sizeof(struct sockaddr));
    ioctl(sockfd, SIOCSIFNETMASK, &ifr); 

    ioctl(sockfd, SIOCGIFFLAGS, &ifr);

    ifr.ifr_flags |= IFF_UP;

    ioctl(sockfd, SIOCSIFFLAGS, &ifr);
    close(sockfd);
}
