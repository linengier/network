#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/sockaddr.h>
#include <asm/types.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG  1
#define BUF_LEN  8192

struct sockaddr_nl la;
struct sockaddr_nl pa;

struct msghdr msg;
struct iovec iov;
char recv_buf[BUF_LEN];
struct {
  //netlink message head
  struct nlmsghdr nl;
  //netlink message
  struct rtmsg rt;
  char  buf[BUF_LEN];
} req;

void format_request()
{
	memset(&req, 0, sizeof(req));
	//set the NETLINK header, set message type and flags
	req.nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nl.nlmsg_type = RTM_GETROUTE;

    //set the routeing message header
    //For RTM_GETROUTE,setting rtm_dst_len and rtm_src_len to 0 means you get all
    //entries for the specified routing table.  For the other
    //fields, except rtm_table and rtm_protocol, 0 is the wildcard.
    req.rt.rtm_family = AF_INET;
    req.rt.rtm_table = RT_TABLE_MAIN;
}
int send_request(int sock)
{
	//set the sockaddr_nl to the kernel
	memset(&pa, 0, sizeof(pa));
	pa.nl_family = AF_NETLINK;

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_name = (void *)&pa;
	msg.msg_namelen = sizeof(struct sockaddr_nl);
    //set iov
    iov.iov_base = (void *)&req.nl;
    iov.iov_len = req.nl.nlmsg_len;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    //send the RTNETLINK Message to kernel
    return sendmsg(sock, &msg, 0);  
	
}
int get_rtmessage(int fd, char * buf)
{
	int msglen;
	struct nlmsghdr *pmsg;
	//route message head
	struct rtmsg * rtp;
	struct rtattr *rtap;
	int rtl;
	if(!buf)
		return -1;
	msglen = recv(fd,  buf, BUF_LEN, 0);
	if(msglen)
		printf("recv buf is %d\r\n", msglen);
	pmsg = (struct nlmsghdr *)buf;
	for(; NLMSG_OK(pmsg, msglen); pmsg=NLMSG_NEXT(pmsg, msglen))
	{
		rtp = (struct rtmsg *)NLMSG_DATA(pmsg);
		rtap = (struct rtattr *)RTM_RTA(rtp);
		rtl = RTM_PAYLOAD(pmsg);
		for(; RTA_OK(rtap, rtl); rtap = RTA_NEXT(rtap, rtl))
		{
			printf("get attr from the message\r\n");
			switch(rtap->rta_type)
			{
				case RTA_UNSPEC:
					printf("the attr type is %d\n", RTA_UNSPEC);
					break;
				case RTA_DST: // 1
					printf("the attr type is %d\n", RTA_DST);
					break;
				case RTA_SRC:
					printf("the attr type is %d\n", RTA_SRC);
					break;
				case RTA_IIF:
					printf("the attr type is %d\n", RTA_IIF);
					break;
				case RTA_OIF: // 4
					printf("the attr type is %d\n", RTA_OIF);
					break;
				case RTA_GATEWAY:
					printf("the attr type is %d\n", RTA_GATEWAY);
					break;
				case RTA_PRIORITY:
					printf("the attr type is %d\n", RTA_PRIORITY);
					break;
				case RTA_PREFSRC: // 7
					printf("the attr type is %d\n", RTA_PREFSRC);
					break;
				case RTA_METRICS:
					printf("the attr type is %d\n", RTA_METRICS);
					break;
				case RTA_MULTIPATH:
					printf("the attr type is %d\n", RTA_MULTIPATH);
					break;
				case RTA_PROTOINFO:
					printf("the attr type is %d\n", RTA_PROTOINFO);
					break;
				case RTA_FLOW:
					printf("the attr type is %d\n", RTA_FLOW);
					break;
				case RTA_CACHEINFO:
					printf("the attr type is %d\n", RTA_CACHEINFO);
					break;
				case RTA_SESSION:
					printf("the attr type is %d\n", RTA_SESSION);
					break;
				case RTA_MP_ALGO:
					printf("the attr type is %d\n", RTA_MP_ALGO);
					break;
				case RTA_TABLE: //15
					printf("the attr type is %d, len is %d\n", RTA_TABLE,rtap->rta_len);
					break;
				default:
					printf("the attr is no type\r\n");
			}
		}
	}
	return msglen;
}
int main()
{
	int sock = -1;
	int ret = -1;
	struct sockaddr_nl  h_addr;
	sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	printf("socket is %d\r\n", sock);
	memset(&h_addr, 0, sizeof(h_addr));
    h_addr.nl_family = AF_NETLINK;
    h_addr.nl_pad = 0;
    h_addr.nl_pid = getpid();
    ret = bind(sock, (struct sockaddr *)&h_addr, sizeof(struct sockaddr_nl));
    if(ret != 0)
    {
		printf("bind socket failed,return is %d\r\n", ret);
		return 0;
	}
	printf("pid is %d\r\n", getpid());
	printf("h_addr pid is %d\r\n", h_addr.nl_pid);

    memset(recv_buf, 0, sizeof(recv_buf));
    format_request();
    send_request(sock);
    get_rtmessage(sock, recv_buf);
	close(sock);
	return 0;
	
}
