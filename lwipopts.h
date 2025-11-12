#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_API                  0
#define LWIP_DHCP                       1
#define LWIP_IPV4                       1
#define LWIP_ICMP                       1
#define LWIP_DNS                        1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define TCP_QUEUE_OOSEQ                 0
#define MEM_LIBC_MALLOC                 0
#define MEM_ALIGNMENT                   4
#define MEMP_NUM_TCP_PCB                3
#define MEMP_NUM_UDP_PCB                2
#define LWIP_TIMEVAL_PRIVATE            0

#endif /* __LWIPOPTS_H__ */
