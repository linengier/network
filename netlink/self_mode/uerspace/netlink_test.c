#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NETLINK_NITRO 17

int main()
{
	struct sockaddr_nl s_nladdr, d_nladdr;
	struct msghdr msg;
	struct iovec iov;
	struct nlmsghdr *nlh = NULL;
	ssize_t  size;
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_NITRO);
    if(fd == -1)
	{
		printf("create socket failed\r\n");
		return 0;
	}

	memset(&d_nladdr, 0, sizeof(d_nladdr));
	d_nladdr.nl_family = AF_NETLINK;
	d_nladdr.nl_pad = 0;
	d_nladdr.nl_pid = 0;

	nlh = (struct nlmsghdr *)malloc(100);
	memset(nlh, 0 , 100);
	strcpy(NLMSG_DATA(nlh), "hello world");
	nlh->nlmsg_len = 100;
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 1;
	//Note that message type values smaller than NLMSG_MIN_TYPE (0x10) are reserved for control
    //messages and may not be used
    //内核netlink_rcv_skb函数在接收消息后会检测此标志，如果小于此值，则不会调用注册的回调函数
	nlh->nlmsg_type = 0x21;
	iov.iov_base = (void *)nlh;
	
	iov.iov_len = nlh->nlmsg_len;

	/* msg */
	memset(&msg,0,sizeof(msg));
	msg.msg_name = (void *) &d_nladdr ;
	msg.msg_namelen=sizeof(d_nladdr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	size = sendmsg(fd, &msg, 0);
    printf("send msg len is %d\r\n", (int)size);
	close(fd);
	return (EXIT_SUCCESS);
}
