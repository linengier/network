#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include <net/rtnetlink.h>
#include <uapi/linux/rtnetlink.h>
# define NETLINK_NITRO 17
static int count = 0;
static DEFINE_MUTEX(sock_mutex);
static int netlink_test_recvmsg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	//struct net *net = sock_net(skb->sk);
	printk("the protocl NETLINK_NITRO recv message 2222\r\n");
	printk("the message len is %d\r\n", nlmsg_len(nlh));
	return 0;
}
static void netlink_test_recv(struct sk_buff *skb)
{
	int ret;
	//struct nlmsghdr *nlh;
	
	printk("the protocl NETLINK_NITRO recv message %d\r\n", ++count);
	printk("the sky len is %d\r\n", skb->len);
	
	mutex_lock(&sock_mutex);
	ret = netlink_rcv_skb(skb, &netlink_test_recvmsg);
	mutex_unlock(&sock_mutex);
	if(ret != 0)
	{
		printk("netlink_test_recv failed\r\n");
	}
	else
		printk("netlink_test_recv success\r\n");
}
static int __net_init netlink_test_ops_init(struct net *net)
{
	struct sock *sk;
	struct netlink_kernel_cfg cfg = {
		.groups = RTNLGRP_MAX,
		.input = netlink_test_recv, //handler the proto type message
		.flags = NL_CFG_F_NONROOT_RECV,//change
		};
	sk = netlink_kernel_create(net, NETLINK_NITRO, &cfg);
	if(!sk)
	{
		printk("call netlink_kernel_create failed fun\r\n");
		return -ENOMEM;
	}
	printk("call namespace init fun\r\n");
	net->rtnl = sk;
	return 0;
}

static void __net_exit netlink_test_ops_exit(struct net *net)
{
	netlink_kernel_release(net->rtnl);
	net->rtnl = NULL;
}
static struct pernet_operations netlink_test_ops =
{
	.init = netlink_test_ops_init,
	.exit = netlink_test_ops_exit,
};

static int __init netlink_test_init(void)
{
	    // <net/net_namespace.h>
		if(register_pernet_subsys(&netlink_test_ops))
			printk("register the prenet operation failed\r\n");
		printk("register the prenet operation successed\r\n");
		return 0;
}

static void __exit netlink_test_exit(void)
{
	unregister_pernet_subsys(&netlink_test_ops);
	printk("unregister the prenet operation failed\r\n");
}

MODULE_LICENSE("GPL");
module_init(netlink_test_init);
module_exit(netlink_test_exit);
