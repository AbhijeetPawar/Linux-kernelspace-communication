#include<sys/socket.h>
#include<linux/netlink.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define NETLINK_USER 30
#define MAX_PAYLOAD 1024

struct sockaddr_nl src_addr,dest_addr;

struct nlmsghdr *nlh = NULL;

struct msghdr msg;

struct iovec iov;

int sock_fd;

int main()
{
  sock_fd = socket(PF_NETLINK,SOCK_RAW,NETLINK_USER);
  if(sock_fd < 0)
    return -1;

  memset(&src_addr,0,sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = getpid(); /* self pid */
  
  bind(sock_fd,(struct sockaddr *)&src_addr,sizeof(src_addr));

  memset(&dest_addr,0,sizeof(src_addr));
  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid = 0;    /* dest = kernel */
  dest_addr.nl_groups = 0; /* unicast */

  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh,0,NLMSG_SPACE(MAX_PAYLOAD));
  nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;//1

  strcpy(NLMSG_DATA(nlh),"Hey Kernel !");

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;

  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  printf("\nSending msg to Kernel... ");
  sendmsg(sock_fd,&msg,0);
  
  printf("\n\nWaiting for msg from KERNEL...\n");
  recvmsg(sock_fd,&msg,0);
  printf("Received message : %s\n",NLMSG_DATA(nlh));

  close(sock_fd);
  return (EXIT_SUCCESS);
}
