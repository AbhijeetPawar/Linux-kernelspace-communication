#include<linux/module.h>
#include<linux/netlink.h>
#include<linux/skbuff.h>
#include<net/sock.h>

#define NETLINK_USER 30

static struct sock *nl_sock = NULL;

/* sk_buff is a large DS which stores all network related info -
has pointers to N/W layer, Transport Layer, Mac Layer...
nlh = sk_buff-> data !
*/

static void recv_data(struct sk_buff *skb)
{
  struct nlmsghdr *nlh = NULL;
  
  struct sk_buff *skb_out;
  char *msg = "Kernel says YO !";	
  int pid,msg_size,res;				
  
  /* GET MESSAGE FROM USER */
  if(skb==NULL)
  {
    printk("skb is NULL !");
    return;
  }
  nlh = (struct nlmsghdr *)skb->data;
  
  printk(KERN_INFO "%s : recieved netlink message - %s\n",__FUNCTION__,NLMSG_DATA(nlh));
  
  /* SEND MESSAGE TO USER */
  pid = nlh->nlmsg_pid;
  msg_size = strlen(msg);
  
  skb_out = nlmsg_new(msg_size,0); //create a new sk_buff for o/p
  
  if(!skb_out)
  {
  	printk(KERN_ERR "Failed to create skb_out ");
  	return;
  }
  
  nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);	//add a netlink msg header
  strncpy(NLMSG_DATA(nlh),msg,msg_size);				//insert msg in nlh
  
  NETLINK_CB(skb_out).dst_group = 0;
  
  res = nlmsg_unicast(nl_sock,skb_out,pid);
  
  if(res < 0)
  	printk(KERN_INFO "Error while sending data to user");
  
}

/*************/

static int my_init_module()
{
  printk(KERN_INFO "Initializing Netlink Socket !");
  
  nl_sock = netlink_kernel_create(&init_net,NETLINK_USER,0,recv_data,NULL,THIS_MODULE);
  if(!nl_sock)
  {
  	printk(KERN_ALERT "Error creating socket..");
  	return -1;
  }	
  
  return 0;
}

static void my_exit_module()
{
  sock_release(nl_sock->sk_socket);
  printk(KERN_INFO "Exitting...");
}

module_init(my_init_module);
module_exit(my_exit_module);

MODULE_LICENSE("GPL");


