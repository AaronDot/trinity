/*
 * SYSCALL_DEFINE5(setsockopt, int, fd, int, level, int, optname, char __user *, optval, int, optlen)
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include <netinet/udp.h>
#include <netipx/ipx.h>
#include <netatalk/at.h>
#include <netax25/ax25.h>
#include <netrose/rose.h>
#include <netrom/netrom.h>
#include <linux/dn.h>
#include <linux/tipc.h>
#include <linux/filter.h>
#include <linux/icmpv6.h>
#include <linux/icmp.h>
#include <linux/if_packet.h>
#include <linux/atmdev.h>
#include <linux/atm.h>
#include <linux/irda.h>
#include <linux/if.h>
#include <linux/llc.h>
#include <linux/dccp.h>
#include <linux/netlink.h>
#include <linux/if_pppol2tp.h>

#include "trinity.h"
#include "sanitise.h"
#include "compat.h"
#include "shm.h"

#define SOL_TCP		6
#define SOL_SCTP        132
#define SOL_UDPLITE     136
#define SOL_NETBEUI     267
#define SOL_LLC         268
#define SOL_DCCP        269
#define SOL_NETLINK     270
#define SOL_RXRPC       272
#define SOL_PPPOL2TP    273
#define SOL_BLUETOOTH   274
#define SOL_PNPIPE      275
#define SOL_RDS         276
#define SOL_IUCV        277
#define SOL_CAIF        278
#define SOL_ALG         279

#define NR_SOL_IP_OPTS 19
static int ip_opts[NR_SOL_IP_OPTS] = { IP_TOS, IP_TTL, IP_HDRINCL, IP_OPTIONS,
	IP_ROUTER_ALERT, IP_RECVOPTS, IP_RETOPTS, IP_PKTINFO,
	IP_PKTOPTIONS, IP_MTU_DISCOVER, IP_RECVERR, IP_RECVTTL,
	IP_RECVTOS, IP_MTU, IP_FREEBIND, IP_IPSEC_POLICY,
	IP_XFRM_POLICY, IP_PASSSEC, IP_TRANSPARENT };

#define NR_SOL_SOCKET_OPTS 46
static int socket_opts[NR_SOL_SOCKET_OPTS] = { SO_DEBUG, SO_REUSEADDR, SO_TYPE, SO_ERROR,
	SO_DONTROUTE, SO_BROADCAST, SO_SNDBUF, SO_RCVBUF,
	SO_SNDBUFFORCE, SO_RCVBUFFORCE, SO_KEEPALIVE, SO_OOBINLINE,
	SO_NO_CHECK, SO_PRIORITY, SO_LINGER, SO_BSDCOMPAT,
	SO_PASSCRED, SO_PEERCRED, SO_RCVLOWAT, SO_SNDLOWAT,
	SO_RCVTIMEO, SO_SNDTIMEO, SO_SECURITY_AUTHENTICATION, SO_SECURITY_ENCRYPTION_TRANSPORT,
	SO_SECURITY_ENCRYPTION_NETWORK, SO_BINDTODEVICE, SO_ATTACH_FILTER, SO_DETACH_FILTER,
	SO_PEERNAME, SO_TIMESTAMP, SO_ACCEPTCONN, SO_PEERSEC,
	SO_PASSSEC, SO_TIMESTAMPNS, SO_MARK, SO_TIMESTAMPING,
	SO_PROTOCOL, SO_DOMAIN, SO_RXQ_OVFL, SO_WIFI_STATUS,
	SO_PEEK_OFF, SO_NOFCS };

#define NR_SOL_TCP_OPTS 23
static int tcp_opts[NR_SOL_TCP_OPTS] = { TCP_NODELAY, TCP_MAXSEG, TCP_CORK, TCP_KEEPIDLE,
	TCP_KEEPINTVL, TCP_KEEPCNT, TCP_SYNCNT, TCP_LINGER2,
	TCP_DEFER_ACCEPT, TCP_WINDOW_CLAMP, TCP_INFO, TCP_QUICKACK,
	TCP_CONGESTION, TCP_MD5SIG, TCP_COOKIE_TRANSACTIONS, TCP_THIN_LINEAR_TIMEOUTS,
	TCP_THIN_DUPACK, TCP_USER_TIMEOUT, TCP_REPAIR, TCP_REPAIR_QUEUE,
	TCP_QUEUE_SEQ, TCP_REPAIR_OPTIONS, TCP_FASTOPEN};

#define NR_SOL_UDP_OPTS 2
static int udp_opts[NR_SOL_UDP_OPTS] = { UDP_CORK, UDP_ENCAP };

#define NR_SOL_UDPLITE_OPTS 4
static int udplite_opts[NR_SOL_UDPLITE_OPTS] = { UDP_CORK, UDP_ENCAP, UDPLITE_SEND_CSCOV, UDPLITE_RECV_CSCOV };

#define NR_SOL_IPV6_OPTS 24
static int ipv6_opts[NR_SOL_IPV6_OPTS] = {
	IPV6_ADDRFORM, IPV6_2292PKTINFO, IPV6_2292HOPOPTS, IPV6_2292DSTOPTS,
	IPV6_2292RTHDR, IPV6_2292PKTOPTIONS, IPV6_CHECKSUM, IPV6_2292HOPLIMIT,
	IPV6_NEXTHOP, IPV6_AUTHHDR, IPV6_FLOWINFO, IPV6_UNICAST_HOPS,
	IPV6_MULTICAST_IF, IPV6_MULTICAST_HOPS, IPV6_MULTICAST_LOOP, IPV6_ADD_MEMBERSHIP,
	IPV6_DROP_MEMBERSHIP, IPV6_ROUTER_ALERT, IPV6_MTU_DISCOVER, IPV6_MTU,
	IPV6_RECVERR, IPV6_V6ONLY, IPV6_JOIN_ANYCAST, IPV6_LEAVE_ANYCAST };

#define NR_SOL_ICMPV6_OPTS 1
static int icmpv6_opts[NR_SOL_ICMPV6_OPTS] = { ICMPV6_FILTER };

#define NR_SOL_SCTP_OPTS 39
static int sctp_opts[NR_SOL_SCTP_OPTS] = {
	SCTP_RTOINFO, SCTP_ASSOCINFO, SCTP_INITMSG, SCTP_NODELAY,
	SCTP_AUTOCLOSE, SCTP_SET_PEER_PRIMARY_ADDR, SCTP_PRIMARY_ADDR, SCTP_ADAPTATION_LAYER,
	SCTP_DISABLE_FRAGMENTS, SCTP_PEER_ADDR_PARAMS, SCTP_DEFAULT_SEND_PARAM, SCTP_EVENTS,
	SCTP_I_WANT_MAPPED_V4_ADDR, SCTP_MAXSEG, SCTP_STATUS, SCTP_GET_PEER_ADDR_INFO,
	SCTP_DELAYED_ACK_TIME, SCTP_CONTEXT, SCTP_FRAGMENT_INTERLEAVE, SCTP_PARTIAL_DELIVERY_POINT,
	SCTP_MAX_BURST, SCTP_AUTH_CHUNK, SCTP_HMAC_IDENT, SCTP_AUTH_KEY,
	SCTP_AUTH_ACTIVE_KEY, SCTP_AUTH_DELETE_KEY, SCTP_PEER_AUTH_CHUNKS, SCTP_LOCAL_AUTH_CHUNKS,
	SCTP_GET_ASSOC_NUMBER, SCTP_GET_ASSOC_ID_LIST, SCTP_AUTO_ASCONF,

	SCTP_SOCKOPT_BINDX_ADD, SCTP_SOCKOPT_BINDX_REM, SCTP_SOCKOPT_PEELOFF, SCTP_SOCKOPT_CONNECTX_OLD,
	SCTP_GET_PEER_ADDRS, SCTP_GET_LOCAL_ADDRS, SCTP_SOCKOPT_CONNECTX, SCTP_SOCKOPT_CONNECTX3 };

#define NR_SOL_AX25_OPTS 12
static int ax25_opts[NR_SOL_AX25_OPTS] = {
	AX25_WINDOW, AX25_T1, AX25_N2, AX25_T3,
	AX25_T2, AX25_BACKOFF, AX25_EXTSEQ, AX25_PIDINCL,
	AX25_IDLE, AX25_PACLEN, AX25_IAMDIGI,
	SO_BINDTODEVICE };

#define NR_SOL_NETROM_OPTS 5
static int netrom_opts[NR_SOL_NETROM_OPTS] = {
	NETROM_T1, NETROM_T2, NETROM_N2, NETROM_T4, NETROM_IDLE };

#define NR_SOL_ROSE_OPTS 7
static int rose_opts[NR_SOL_ROSE_OPTS] = {
	ROSE_DEFER, ROSE_T1, ROSE_T2, ROSE_T3,
	ROSE_IDLE, ROSE_QBITINCL, ROSE_HOLDBACK };

#define NR_SOL_DECNET_OPTS 18
static int decnet_opts[NR_SOL_DECNET_OPTS] = {
	SO_CONDATA, SO_CONACCESS, SO_PROXYUSR, SO_LINKINFO,
	DSO_CONDATA, DSO_DISDATA, DSO_CONACCESS, DSO_ACCEPTMODE,
	DSO_CONACCEPT, DSO_CONREJECT, DSO_LINKINFO, DSO_STREAM,
	DSO_SEQPACKET, DSO_MAXWINDOW, DSO_NODELAY, DSO_CORK,
	DSO_SERVICES, DSO_INFO
};

#define NR_SOL_PACKET_OPTS 22
static int packet_opts[NR_SOL_PACKET_OPTS] = {
	PACKET_ADD_MEMBERSHIP, PACKET_DROP_MEMBERSHIP, PACKET_RECV_OUTPUT, 4,	/* Value 4 is still used by obsolete turbo-packet. */
	PACKET_RX_RING, PACKET_STATISTICS, PACKET_COPY_THRESH, PACKET_AUXDATA,
	PACKET_ORIGDEV, PACKET_VERSION, PACKET_HDRLEN, PACKET_RESERVE,
	PACKET_TX_RING, PACKET_LOSS, PACKET_VNET_HDR, PACKET_TX_TIMESTAMP,
	PACKET_TIMESTAMP, PACKET_FANOUT };

#define NR_SOL_ATM_OPTS 6
static int atm_opts[NR_SOL_ATM_OPTS] = {
	SO_SETCLP, SO_CIRANGE, SO_ATMQOS, SO_ATMSAP, SO_ATMPVC, SO_MULTIPOINT };

#define NR_SOL_IRDA_OPTS 11
static int irda_opts[NR_SOL_IRDA_OPTS] = {
	IRLMP_ENUMDEVICES, IRLMP_IAS_SET, IRLMP_IAS_QUERY, IRLMP_HINTS_SET,
	IRLMP_QOS_SET, IRLMP_QOS_GET, IRLMP_MAX_SDU_SIZE, IRLMP_IAS_GET,
	IRLMP_IAS_DEL, IRLMP_HINT_MASK_SET, IRLMP_WAITDEVICE };

#define NR_SOL_LLC_OPTS 9
static int llc_opts[NR_SOL_LLC_OPTS] = {
	LLC_OPT_RETRY, LLC_OPT_SIZE, LLC_OPT_ACK_TMR_EXP, LLC_OPT_P_TMR_EXP,
	LLC_OPT_REJ_TMR_EXP, LLC_OPT_BUSY_TMR_EXP, LLC_OPT_TX_WIN, LLC_OPT_RX_WIN,
	LLC_OPT_PKTINFO };

#define NR_SOL_DCCP_OPTS 16
static int dccp_opts[NR_SOL_DCCP_OPTS] = {
	DCCP_SOCKOPT_PACKET_SIZE, DCCP_SOCKOPT_SERVICE, DCCP_SOCKOPT_CHANGE_L, DCCP_SOCKOPT_CHANGE_R,
	DCCP_SOCKOPT_GET_CUR_MPS, DCCP_SOCKOPT_SERVER_TIMEWAIT, DCCP_SOCKOPT_SEND_CSCOV, DCCP_SOCKOPT_RECV_CSCOV,
	DCCP_SOCKOPT_AVAILABLE_CCIDS, DCCP_SOCKOPT_CCID, DCCP_SOCKOPT_TX_CCID, DCCP_SOCKOPT_RX_CCID,
	DCCP_SOCKOPT_QPOLICY_ID, DCCP_SOCKOPT_QPOLICY_TXQLEN, DCCP_SOCKOPT_CCID_RX_INFO, DCCP_SOCKOPT_CCID_TX_INFO };

#define NR_SOL_NETLINK_OPTS 5
static int netlink_opts[NR_SOL_NETLINK_OPTS] = {
	NETLINK_ADD_MEMBERSHIP, NETLINK_DROP_MEMBERSHIP, NETLINK_PKTINFO, NETLINK_BROADCAST_ERROR,
	NETLINK_NO_ENOBUFS };

#define NR_SOL_TIPC_OPTS 6
static int tipc_opts[NR_SOL_TIPC_OPTS] = {
	TIPC_IMPORTANCE, TIPC_SRC_DROPPABLE, TIPC_DEST_DROPPABLE, TIPC_CONN_TIMEOUT,
	TIPC_NODE_RECVQ_DEPTH, TIPC_SOCK_RECVQ_DEPTH };

#define NR_SOL_RXRPC_OPTS 8
static int rxrpc_opts[NR_SOL_RXRPC_OPTS] = {
	RXRPC_USER_CALL_ID, RXRPC_ABORT, RXRPC_ACK, RXRPC_NET_ERROR,
	RXRPC_BUSY, RXRPC_LOCAL_ERROR, RXRPC_NEW_CALL, RXRPC_ACCEPT };

#define NR_SOL_PPPOL2TP_OPTS 5
static int pppol2tp_opts[NR_SOL_PPPOL2TP_OPTS] = {
	PPPOL2TP_SO_DEBUG, PPPOL2TP_SO_RECVSEQ, PPPOL2TP_SO_SENDSEQ, PPPOL2TP_SO_LNSMODE,
	PPPOL2TP_SO_REORDERTO };

void sanitise_setsockopt(int childno)
{
	int level;
	unsigned char val;

	shm->a4[childno] = (unsigned long) page_rand;
	shm->a5[childno] = sizeof(int);	// at the minimum, we want an int (overridden below)

	switch (rand() % 33) {
	case 0:	level = SOL_IP;	break;
	case 1:	level = SOL_SOCKET; break;
	case 2:	level = SOL_TCP; break;
	case 3:	level = SOL_UDP; break;
	case 4:	level = SOL_IPV6; break;
	case 5:	level = SOL_ICMPV6; break;
	case 6:	level = SOL_SCTP; break;
	case 7:	level = SOL_UDPLITE; break;
	case 8:	level = SOL_RAW; break;
	case 9:	level = SOL_IPX; break;
	case 10: level = SOL_AX25; break;
	case 11: level = SOL_ATALK; break;
	case 12: level = SOL_NETROM; break;
	case 13: level = SOL_ROSE; break;
	case 14: level = SOL_DECNET; break;
	case 15: level = SOL_X25; break;
	case 16: level = SOL_PACKET; break;
	case 17: level = SOL_ATM; break;
	case 18: level = SOL_AAL; break;
	case 19: level = SOL_IRDA; break;
	case 20: level = SOL_NETBEUI; break;
	case 21: level = SOL_LLC; break;
	case 22: level = SOL_DCCP; break;
	case 23: level = SOL_NETLINK; break;
	case 24: level = SOL_TIPC; break;
	case 25: level = SOL_RXRPC; break;
	case 26: level = SOL_PPPOL2TP; break;
	case 27: level = SOL_BLUETOOTH; break;
	case 28: level = SOL_PNPIPE; break;
	case 29: level = SOL_RDS; break;
	case 30: level = SOL_IUCV; break;
	case 31: level = SOL_CAIF; break;
	case 32: level = SOL_ALG; break;
	default:
		level = rand();
		break;
	}

	shm->a2[childno] = level;

	switch (level) {
	case SOL_IP:
		val = rand() % NR_SOL_IP_OPTS;
		shm->a3[childno] = ip_opts[val];
		break;

	case SOL_SOCKET:
		val = rand() % NR_SOL_SOCKET_OPTS;
		shm->a3[childno] = socket_opts[val];

		/* Adjust length according to operation set. */
		switch (shm->a3[childno]) {
		case SO_LINGER:	shm->a5[childno] = sizeof(struct linger);
			break;
		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
			shm->a5[childno] = sizeof(struct timeval);
			break;
		case SO_ATTACH_FILTER:
			shm->a5[childno] = sizeof(struct sock_fprog);
			break;
		default:
			break;
		}
		break;

	case SOL_TCP:
		val = rand() % NR_SOL_TCP_OPTS;
		shm->a3[childno] = tcp_opts[val];
		break;

	case SOL_UDP:
		val = rand() % NR_SOL_UDP_OPTS;
		shm->a3[childno] = udp_opts[val];

		switch (shm->a3[childno]) {
		case UDP_CORK:
			break;
		case UDP_ENCAP:
			page_rand[0] = (rand() % 3) + 1;	// Encapsulation types.
			break;
		default:
			break;
		}
		break;

	case SOL_IPV6:
		val = rand() % NR_SOL_IPV6_OPTS;
		shm->a3[childno] = ipv6_opts[val];
		break;

	case SOL_ICMPV6:
		val = rand() % NR_SOL_ICMPV6_OPTS;
		shm->a3[childno] = icmpv6_opts[val];
		break;

	case SOL_SCTP:
		val = rand() % NR_SOL_SCTP_OPTS;
		shm->a3[childno] = sctp_opts[val];
		break;

	case SOL_UDPLITE:
		val = rand() % NR_SOL_UDPLITE_OPTS;
		shm->a3[childno] = udplite_opts[val];

		switch (shm->a3[childno]) {
		case UDP_CORK:
			break;
		case UDP_ENCAP:
			page_rand[0] = (rand() % 3) + 1;	// Encapsulation types.
			break;
		case UDPLITE_SEND_CSCOV:
			break;
		case UDPLITE_RECV_CSCOV:
			break;
		default:
			break;
		}

		break;

	case SOL_RAW:
		shm->a3[childno] = ICMP_FILTER;	// that's all (for now?)
		break;

	case SOL_IPX:
		shm->a3[childno] = IPX_TYPE;
		break;

	case SOL_AX25:
		val = rand() % NR_SOL_AX25_OPTS;
		shm->a3[childno] = ax25_opts[val];
		break;

	case SOL_ATALK:	/* sock_no_setsockopt */
		break;

	case SOL_NETROM:
		val = rand() % NR_SOL_NETROM_OPTS;
		shm->a3[childno] = netrom_opts[val];
		break;

	case SOL_ROSE:
		val = rand() % NR_SOL_ROSE_OPTS;
		shm->a3[childno] = rose_opts[val];
		break;

	case SOL_DECNET:
		// TODO: set size correctly
		val = rand() % NR_SOL_DECNET_OPTS;
		shm->a3[childno] = decnet_opts[val];
		break;

	case SOL_X25:
		page_rand[0] = rand() % 2;	/* Just a bool */
		shm->a4[childno] = sizeof(int);
		break;

	case SOL_PACKET:
		val = rand() % NR_SOL_PACKET_OPTS;
		shm->a3[childno] = packet_opts[val];
		break;

	case SOL_ATM:
		val = rand() % NR_SOL_ATM_OPTS;
		shm->a3[childno] = atm_opts[val];
		break;

	case SOL_AAL:	/* no setsockopt */
		break;

	case SOL_IRDA:
		val = rand() % NR_SOL_IRDA_OPTS;
		shm->a3[childno] = irda_opts[val];
		break;

	case SOL_NETBEUI:	/* no setsockopt */
		break;

	case SOL_LLC:
		val = rand() % NR_SOL_LLC_OPTS;
		shm->a3[childno] = llc_opts[val];
		break;

	case SOL_DCCP:
		val = rand() % NR_SOL_DCCP_OPTS;
		shm->a3[childno] = dccp_opts[val];
		break;

	case SOL_NETLINK:
		val = rand() % NR_SOL_NETLINK_OPTS;
		shm->a3[childno] = netlink_opts[val];
		break;

	case SOL_TIPC:
		shm->a4[childno] = sizeof(__u32);
		val = rand() % NR_SOL_TIPC_OPTS;
		shm->a3[childno] = tipc_opts[val];
		break;

	case SOL_RXRPC:
		val = rand() % NR_SOL_RXRPC_OPTS;
		shm->a3[childno] = rxrpc_opts[val];
		break;

	case SOL_PPPOL2TP:
		shm->a4[childno] = sizeof(int);
		val = rand() % NR_SOL_PPPOL2TP_OPTS;
		shm->a3[childno] = pppol2tp_opts[val];
		break;

	case SOL_BLUETOOTH:
	case SOL_PNPIPE:
	case SOL_RDS:
	case SOL_IUCV:
	case SOL_CAIF:
	case SOL_ALG:

	default:
		shm->a3[childno] = (rand() % 0xff);	/* random operation. */
	}

	/* optval should be nonzero to enable a boolean option, or zero if the option is to be disabled.
	 * Let's disable it half the time.
	 */
	if (rand() % 2)
		shm->a4[childno] = 0;

	shm->a4[childno] = sizeof(int);
}

struct syscall syscall_setsockopt = {
	.name = "setsockopt",
	.num_args = 5,
	.arg1name = "fd",
	.arg1type = ARG_FD,
	.arg2name = "level",
	.arg3name = "optname",
	.arg4name = "optval",
	.arg4type = ARG_ADDRESS,
	.arg5name = "optlen",
	.sanitise = sanitise_setsockopt,
};
