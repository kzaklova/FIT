/** 
 * ISA project
 * Name: Traceroute
 * Author: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz, 2012
 **/

#include <stdlib.h>
#include <netdb.h>
#include <iostream>
#include <iomanip>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define BYTE u_int8_t
#define WORD u_int16_t

using namespace std;

/**
 * Structure with arguments
**/
typedef struct params {
	int ecode, first, max;
	string adr;
} TParams;


enum tecodes
{
	EOK = 0,     /**< No error */
	EHELP,       /**< Help msg required */
	EARG,        /**< Invalid argument */
	EMAX,        /**< First greater than max */
	EMESG,       /**< ICMPv4/ICMPv6 messages */
	EBREAK,      /**< Pseudo EOK*/
	EGET,        /**< Host doesn't exist */
	ESOCKET,     /**< Can't create socket */
	ESETS,       /**< Setsockopt error */
	ESEL,        /**< Select error */
	ESEND,       /**< Cannot send */
	ERECV,       /**< Cannot receive */
	EUNKNOWN     /**< Unknown error */
};

const char *ECODE[] =
{
	/* EOK */
	"Everything fine\n",
	/* EHELP */
	"Helpmsg required.\n",
	/* EARG */
	"Help! I need somebody! Help! Not just anybody! Help! Rerun with -h only...\n",
	/* EMAX */
	"First number greater than max...srsly?\n",
	/* EMESG */
	"ICMPv4/v6 message - something unreachable etc.\n",
	/* EBREAK */
	"This is pseudo-OK message :-) \n",
	/* EGET */
	"The host doesn't exist!\n",
	/* ESOCKET */
	"The socket cannot be created. R U root?\n",
	/* ESETS */
	"Setsockopt issues :(\n",
	/* ESEL */
	"Troubles with select, send more cookies!\n",
	/* ESEND */
	"Cannot send anything. Not enough mana.\n",
	/* ERECV */
	"Cannot receive anything.\n",
	/* EUNKNOWN */
	"Whaaat?\n",
};

const char *HELPMSG = {
	"-------------------------------------------------------------------------\n"
	"Hello, I'm Traceroute...\n"
	"I will trace you whatever you want. It just has to be a valid address \n"
	"and I will show you the way! I can handle IPv4 and IPv6. And be assured I\n"
	"prefer IPv6.\n"
	"How can you run me? Just type this:\n"
	"-------------------------------------------------------------------------\n"
	"./trace [-h] [-f first_ttl] [-m max_ttl] host\n"
	"-------------------------------------------------------------------------\n"
	"h                  Need some help?\n"
	"f first_ttl        ttl for first packet, optional\n"
	"m max_ttl          maximal ttl, optional\n"
	"host               IPv4/IPv6/DNS name of the target\n"
	"That's all folks! Have fun!\n"
	"-------------------------------------------------------------------------\n"
};

/**
 * Parse program arguments and returns them in a structure
 * @param argc count of args
 * @param argv array with args
 * @return structure
**/
TParams getParams(int argc, char *argv[])
{

	TParams result = {EOK, 1, 30, ""};
	char *pAux;

	// help
	if (argc == 2) {
		if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-help") == 0) || (strcmp(argv[1], "--help") == 0)) {
			result.ecode = EHELP;
			return result;
		}
		else
			result.adr = argv[1];
	}

	else if (argc == 4) {
		// -f host
		if ((strcmp(argv[1], "-f")) == 0) {
			result.first = strtol((argv[2]), &pAux, 10);
			if (*pAux != '\0' || (result.first > 255))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.adr = argv[3];
		}
		// -m host
		else if ((strcmp(argv[1], "-m")) == 0) {
			result.max = strtol((argv[2]), &pAux, 10);
			if (*pAux != '\0' || (result.max > 255))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.adr = argv[3];
		}
		else
			result.ecode = EARG;
	}

	else if (argc == 6) {
		// -f -m host
		if (((strcmp(argv[1], "-f")) == 0) && ((strcmp(argv[3], "-m")) == 0)) {
			result.first = strtol((argv[2]), &pAux, 10);
			if (*pAux != '\0' || (result.first > 255))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.max = strtol((argv[4]), &pAux, 10);
			if (*pAux != '\0' || (result.max > 255))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.adr = argv[5];
		}
		// -m -f host
		else if (((strcmp(argv[1], "-m")) == 0) && ((strcmp(argv[3], "-f")) == 0)) {
			result.max = strtol((argv[2]), &pAux, 10);
			if (*pAux != '\0' || (result.max > 255) || (result.max < 1))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.first = strtol((argv[4]), &pAux, 10);
			if (*pAux != '\0' || (result.first > 255) || (result.first < 1))
				result.ecode = EARG;
			else
				result.ecode = EOK;
			result.adr = argv[5];
		}
		else
			result.ecode = EARG;
	}

	else
		result.ecode = EARG;

	return result;
}

/**
 * Print error code message
 * @param ecode error code
 */
void printEcode(int ecode)
{
	if (ecode < EOK || ecode > ERECV)
		ecode = EUNKNOWN;

	cerr<<ECODE[ecode]<<endl;
}

/**
 * Compute checksum for header
 * Source: http://www.bozon.net/projects/ncfes/code/syntax-highlighted/icmp.c.html
 * @param startpos starting position
 * @param checklen transmit buffer
 * @return 0/1
 */
WORD chksm(WORD *startpos, WORD checklen)
{
	ulong sum = 0;
	WORD answer = 0;

	while (checklen > 1) {
		sum += *startpos++;
		checklen -= 2;
	}

	if (checklen == 1) {
		*(BYTE *)(&answer) = *(BYTE *)startpos;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	
	return answer;
}

/**
 * Free all the buffers, sockets and stuff
 * @param recv_buffer receiving buffer
 * @param buffer sending buffer
 * @param sock sending socket
 * @param recv receiving socket
 * @param results structure with address info
 * @return EOK or error code
 */
void cleanTheMess(void *recv_buffer, char *buffer, int sock, int recv, addrinfo *results) {
	free(recv_buffer);
	free(buffer);
	close(sock);
	close(recv);
	freeaddrinfo(results);
}


/**
 * Make sending and receiving sockets for IPv4
 * @param send sending socket
 * @param recv receiving socket
 * @return EOK or error code
 */
int makeSocket (int *send, int *recv) {
	// make socket for sending
	if ((*send = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		return ESOCKET;
	}

	// make socket for receiving
	if ((*recv = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		return ESOCKET;
	}

	return EOK;
}
/**
 * Make sending and receiving sockets for IPv6
 * @param send sending socket
 * @param recv receiving socket
 * @return EOK or error code
 */
int makeSocket6 (int *send, int *recv) {
	// make socket for sending
	if ((*send = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) == -1) {
		return ESOCKET;
	}

	// make socket for receiving
	if ((*recv = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) == -1) {
		return ESOCKET;
	}

	return EOK;
}

/**
 * Print results of IPv4 traceroute
 * @param icmp_dest structure with 
 * @param hoplimit time to live
 * @param hname string with DNS name
 * @param str_adr string with address
 * @param sendTime time of routing
 * @param getname auxiliary variable of getnameinfo
 * @return EOK or error code
 */
int printIPv4 (icmphdr *icmp_dest, int hoplimit, char *hname, char *str_adr, double sendTime, int getname) {
	// if address has DNS name
	if (getname == 0) {
		// we reached the destination...hello there!
		if(icmp_dest->type == ICMP_ECHOREPLY) {
			cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
			return EBREAK;
		}
		// destination is unreachable
		else if (icmp_dest->type == ICMP_DEST_UNREACH) {
			// network is unreachable
			if (icmp_dest->code == ICMP_NET_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"N!"<<endl;
				return EMESG;
			}
			// host is unreachable
			else if (icmp_dest->code == ICMP_HOST_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"H!"<<endl;
				return EMESG;
			}
			// protocol os unreachable
			else if (icmp_dest->code == ICMP_PROT_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"P!"<<endl;
				return EMESG;
			}
			// communication administratively prohibited
			else if (icmp_dest->code == ICMP_UNREACH_FILTER_PROHIB) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"X!"<<endl;
				return EMESG;
			}
		}
		// we are not in destination
		else cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
	}
	// address does not have DNS name
	else {
		// we reached the destination
		if(icmp_dest->type == ICMP_ECHOREPLY) {
			cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
			return EBREAK;
		}
		// destination is unreachable
		else if (icmp_dest->type == ICMP_DEST_UNREACH)  {
			// network is unreachable
			if (icmp_dest->code == ICMP_NET_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"N!"<<endl;
				return EMESG;
			}
			// host is unreachable
			else if (icmp_dest->code == ICMP_HOST_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"H!"<<endl;
				return EMESG;
			}
			// protocol unreachable
			else if (icmp_dest->code == ICMP_PROT_UNREACH) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"P!"<<endl;
				return EMESG;
			}
			// communication administratively prohibited
			else if (icmp_dest->code == ICMP_UNREACH_FILTER_PROHIB) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"X!"<<endl;
				return EMESG;
			}
		}
		// we are not in destination
		else cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
	}
	return EOK;
}

/**
 * Print results of IPv6 traceroute
 * @param icmp_dest structure with 
 * @param hoplimit time to live
 * @param hname string with DNS name
 * @param str_adr string with address
 * @param sendTime time of routing
 * @param getname auxiliary variable of getnameinfo
 * @return EOK or error code
 */
int printIPv6(icmp6_hdr *icmp_dest, int hoplimit, char *hname, char *str_adr, double sendTime, int getname) {
	// if address has DNS name
	if (getname == 0) {
		// we are not in destination
		if (icmp_dest->icmp6_type == ICMP6_TIME_EXCEEDED)
			cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
		// we arrived...hello there!
		else if(icmp_dest->icmp6_type == ICMP6_ECHO_REPLY) {
			cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
			return EBREAK;
		}
		// destination is unreachable
		else if (icmp_dest->icmp6_type == ICMP6_DST_UNREACH) {
			// network is unreachable
			if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_NOROUTE) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"N!"<<endl;
				return EMESG;
			}
			// host is unreachable
			else if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_ADDR) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"H!"<<endl;
				return EMESG;
			}
			// communication administratively prohibited
			else if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_ADMIN) {
				cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"X!"<<endl;
				return EMESG;
			}
		}
		// protocol is unreachable
		else if (icmp_dest->icmp6_type == ICMP6_PARAM_PROB && icmp_dest->icmp6_code == ICMP6_PARAMPROB_NEXTHEADER) {
			cout<<setw(2)<<hoplimit<<"  "<<string(hname)<<" ("<<string(str_adr)<<")  "<<"P!"<<endl;
			return EMESG;
		}
	}
	// address does not have DNS name
	else {
		// we are not in destination
		if (icmp_dest->icmp6_type == ICMP6_TIME_EXCEEDED)
			cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
		// we reached the destination
		else if(icmp_dest->icmp6_type == ICMP6_ECHO_REPLY) {
			cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<setprecision(3)<<sendTime<<" ms"<<endl;
			return EBREAK;
		}
		// destination is unreachable
		else if (icmp_dest->icmp6_type == ICMP6_DST_UNREACH) {
			// network is unreachable
			if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_NOROUTE) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"N!"<<endl;
				return EMESG;
			}
			// host is unreachable
			else if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_ADDR) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"H!"<<endl;
				return EMESG;
			}
			// communication administratively prohibited
			else if (icmp_dest->icmp6_code == ICMP6_DST_UNREACH_ADMIN) {
				cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"X!"<<endl;
				return EMESG;
			}
		}
		// protocol unreachable
		else if (icmp_dest->icmp6_type == ICMP6_PARAM_PROB && icmp_dest->icmp6_code == ICMP6_PARAMPROB_NEXTHEADER) {
			cout<<setw(2)<<hoplimit<<"  "<<string(str_adr)<<" ("<<string(str_adr)<<")  "<<"P!"<<endl;
			return EMESG;
		}
	}
	return EOK;
}

/**
 * Traceroute for IPv4
 * @param params structure with arguments
 * @param results structure with getaddrinfo results
 * @param send sending socket
 * @param recv receiving socket
 * @return EOK or error code
 */
int trace4(TParams params, addrinfo *results, int send, int recv) {
	int ecode = 0;
	int sel, getname;

	iphdr *ip, *ip_dest; // IP headers for packet
	icmphdr *icmp, *icmp_dest, *icmp_aux; // ICMP headers for packet
	sockaddr_in sSock, name_info; //structure with address info

	char *buffer, *recv_buffer; // buffers for packet
	int total; // length of packet
	char adr_buf[NI_MAXHOST], str_adr[255], hname[NI_MAXHOST]; // buffers for address, dns anme and other stuff

	struct timeval timeout;
	timeval time;
	fd_set fds;

	double millis, millis2, sendTime; //time variables

	memset(&sSock, 0, sizeof(sockaddr_in));
	// copy address
	memcpy(&sSock, results->ai_addr, results->ai_addrlen);

	// prepare buffer for packet
	total = sizeof(iphdr) + sizeof(icmphdr);
	buffer = (char *)calloc(total,1);
	ip = (iphdr *)buffer;
	icmp = (icmphdr *)(buffer + sizeof(iphdr));

	//set IP header
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(total);
	ip->id = 0;
	ip->frag_off = htons(16384); //0100000000000000 binary
	ip->protocol = IPPROTO_ICMP;
	ip->check = 0;
	ip->saddr = 0;
	ip->daddr = ((struct sockaddr_in*)results->ai_addr)->sin_addr.s_addr;

	// set ICMP header
	icmp->type = 8;
	icmp->code = 0;
	icmp->un.echo.id = getpid();

	recv_buffer = (char *)calloc(IP_MAXPACKET, 1);

	cout<<"trace to "<<params.adr<<" ("<<inet_ntop(AF_INET,&((struct sockaddr_in*)results->ai_addr)->sin_addr.s_addr, adr_buf, NI_MAXHOST)<<")"<<endl;
	// trace
	for(ip->ttl = params.first; ip->ttl<=params.max;ip->ttl++) {
		// set sequence number
		icmp->un.echo.sequence = (int)ip->ttl;
		// compute checksum
		icmp->checksum = 0;
		icmp->checksum = chksm((WORD *)icmp, (WORD)(sizeof(icmphdr)));

		//send packet
		if (sendto(send, buffer, total, 0, (sockaddr *)&sSock, sizeof(sSock)) == -1) {
			printEcode(ESEND);
			return ESEND;
		}
		// get time
		gettimeofday(&time, NULL);
		millis = (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
		// set timeout and options for select
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(recv, &fds);

		while (1) {
			sel = select(sizeof(fds), &fds, NULL, NULL, &timeout);
			// select error
			if (sel == -1) {
				printEcode(ESEL);
				return ESEL;
			}
			// select OK
			else if (sel > 0) {
				// receive packet
				if (recvfrom (recv, recv_buffer, IP_MAXPACKET, 0, NULL, NULL) == -1) {
					printEcode(ERECV);
					return ERECV;
				}
				// save packet to headers
				icmp_dest= (icmphdr*)(recv_buffer+sizeof(iphdr));
				icmp_aux =  (struct icmphdr *)(((char *)recv_buffer) + sizeof(iphdr)*2 + sizeof(icmphdr));
				ip_dest = (struct iphdr *) recv_buffer;
				// save destination address for DNS name
				bzero(&name_info,sizeof(name_info));
				name_info.sin_family = AF_INET;
				name_info.sin_addr.s_addr = ip_dest->saddr;
				// "translate" destination address
				inet_ntop(AF_INET, &(ip_dest->saddr), str_adr, 255);

				// here be dragons
				// id and sequence number are correct - we got the right packet, yesss
				if (((icmp_aux->un.echo.id == icmp->un.echo.id) && (icmp_aux->un.echo.sequence == icmp->un.echo.sequence)) || ((icmp_dest->un.echo.id == icmp->un.echo.id) && (icmp_dest->un.echo.sequence == icmp->un.echo.sequence) && (icmp_dest->type == ICMP_ECHOREPLY))) {
					//get the second time
					gettimeofday(&time, NULL);
					millis2 = (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
					sendTime = millis2-millis;
					// get DNS name, if possible
					getname = getnameinfo((sockaddr*)&name_info, sizeof(struct sockaddr_in), hname, sizeof(hname), NULL, 0, NI_NAMEREQD);
					// print results
					ecode = printIPv4(icmp_dest, (int)ip->ttl, hname, str_adr, sendTime, getname);
					if ((ecode != EOK) && (ecode != EBREAK)) {
						// clean :-)
						cleanTheMess(recv_buffer, buffer, send, recv, results);
						return ecode;
					}
					break;
				}
				else continue;
			}
			else {
				cout<<setw(2)<<(int)ip->ttl<<" *"<<endl;
				break;
			}
		} // end while
		if (ecode == EBREAK) break;
		// just for sure, we don't want overflow :-)
		if ((int)ip->ttl == 255) break;
	} // end for
	// clean :-)
	cleanTheMess(recv_buffer, buffer, send, recv, results);
	return EOK;
}

/**
 * Traceroute for IPv6
 * @param params structure with arguments
 * @param results structure with getaddrinfo results
 * @param send sending socket
 * @param recv receiving socket
 * @return EOK or error code
 */
int trace6(TParams params, addrinfo *results, int send, int recv) {
	int ecode = 0;
	int sel, getname;

	icmp6_hdr *icmp6, *icmp_dest, *icmp_aux; // ICMPv6 headers for packet
	sockaddr_in6 sSock, rSock; //structure with address info

	char *buffer, *recv_buffer; // buffers for packet
	int total; // length of packet
	unsigned int size_adr;

	char adr_buf[NI_MAXHOST], str_adr[255], hname[NI_MAXHOST]; // buffers for address, dns anme and other stuff

	struct timeval timeout;
	timeval time;
	fd_set fds;

	double millis, millis2, sendTime; //time variables
	int hoplimit; //ttl

	memset(&sSock, 0, sizeof(sockaddr_in6));
	// copy address
	memcpy(&sSock, results->ai_addr, results->ai_addrlen);

	// prepare buffer for packet
	total = sizeof(icmp6_hdr);
	buffer = (char *)calloc(total, 1);
	icmp6 = (icmp6_hdr *)(buffer);

	// set ICMPv6 header
	icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp6->icmp6_code = 0;
	icmp6->icmp6_id = getpid();

	recv_buffer = (char *)calloc(IP_MAXPACKET, 1);
	size_adr = sizeof(rSock);

	cout<<"trace to "<<params.adr<<" ("<<inet_ntop(results->ai_family, &((struct sockaddr_in6*)results->ai_addr)->sin6_addr.s6_addr, adr_buf, NI_MAXHOST)<<")"<<endl;
	// trace
	for(hoplimit = params.first; hoplimit<=params.max; hoplimit++) {
		// increase TTL
		if (setsockopt(send, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char *)&hoplimit, sizeof(hoplimit)) != 0) {
			printEcode(ESETS);
			return ESETS;
		}

		// set sequence number
		icmp6->icmp6_seq = hoplimit;
		// compute checksum
		icmp6->icmp6_cksum = 0;
		icmp6->icmp6_cksum = chksm((WORD *)icmp6, (WORD)(sizeof(icmp6_hdr)));

		//send packet
		if (sendto(send, buffer, total, 0, (sockaddr *)&sSock, sizeof(sSock)) ==  -1) {
			printEcode(ESEND);
			return ESEND;
		}
		// get time
		gettimeofday(&time, NULL);
		millis = (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
		// set timeout and options for select
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(recv, &fds);

		while (1) {
			sel = select(sizeof(fds), &fds, NULL, NULL, &timeout);
			// select error
			if (sel == -1) {
				printEcode(ESEL);
				return ESEL;
			}
			// select OK
			else if (sel > 0) {
				// receive packet
				if (recvfrom (recv, recv_buffer, IP_MAXPACKET, 0, (struct sockaddr *)&rSock, &size_adr) ==  -1) {
					printEcode(ERECV);
					return ERECV;
				}
				// save received packet to headers
				icmp_dest= (icmp6_hdr*)(recv_buffer);
				icmp_aux =  (struct icmp6_hdr *)((recv_buffer) + sizeof(ip6_hdr) + sizeof(icmp6_hdr));
				// "translate" destination address
				inet_ntop(AF_INET6, &(rSock.sin6_addr.s6_addr), str_adr, 255);

				// here be lions
				// did we get the right packet?
				if (((icmp_dest->icmp6_id == icmp6->icmp6_id) && (icmp_dest->icmp6_seq == icmp6->icmp6_seq) && (icmp_dest->icmp6_type == ICMP6_ECHO_REPLY)) || ((icmp_aux->icmp6_id == icmp6->icmp6_id) && (icmp_aux->icmp6_seq == icmp6->icmp6_seq))) {
					//get the second time
					gettimeofday(&time, NULL);
					millis2 = (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
					sendTime = millis2-millis;
					// get DNS name, if possible
					getname = getnameinfo((const struct sockaddr *)&rSock, sizeof(rSock), hname, sizeof(hname), NULL, 0, NI_NAMEREQD);
					// print results
					ecode = printIPv6(icmp_dest, hoplimit, hname, str_adr, sendTime, getname);
					if ((ecode != EOK) && (ecode != EBREAK)) {
						// clean :-)
						cleanTheMess(recv_buffer, buffer, send, recv, results);
						return ecode;
					}
					break;
				}
				else continue;
			}
			// timeout expired
			else {
				cout<<setw(2)<<hoplimit<<" *"<<endl;
				break;
			}
		} // end while
		if (ecode == EBREAK) break;
	} // end for
	// clean :-)
	cleanTheMess(recv_buffer, buffer, send, recv, results);
	return EOK;
}


//////////////////////////////////////////////////////////
/////////////////////  MAIN  /////////////////////////////
//////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int ecode = EOK;

	//get params
	TParams params = getParams(argc, argv);
	if (params.ecode == EHELP) {
		cout<<HELPMSG;
		return EOK;
	}
	else if (params.ecode != EOK) {
		printEcode(params.ecode);
		return params.ecode;
	}
	else if (params.first > params.max) {
		printEcode(EMAX);
		return EMAX;
	}

	int sock, recv;
	struct addrinfo hints;
	struct addrinfo *results;

	//set structure for getaddrinfo
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_RAW;
	hints.ai_protocol=0;
	hints.ai_flags=AI_ADDRCONFIG;

	if (getaddrinfo(params.adr.c_str(),NULL,&hints, &results) != 0) {
		printEcode(EGET);
		return EGET;
	}

	//IPv6
	if (results->ai_family == AF_INET6) {
		if ((ecode = makeSocket6(&sock, &recv)) != 0) {
			printEcode(ecode);
			return ecode;
		}

		ecode = trace6(params, results, sock, recv);

		if (ecode == EMESG)
			return EOK;
		else if(ecode != EOK) {
			printEcode(ecode);
			return ecode;
		}
	}
	//IPv4
	else if (results->ai_family == AF_INET) {
		if ((ecode = makeSocket(&sock, &recv)) != 0) {
			printEcode(ecode);
			return ecode;
		}
		ecode = trace4(params, results, sock, recv);

		if (ecode == EMESG)
			return EOK;
		else if(ecode != EOK) {
			printEcode(ecode);
			return ecode;
		}
	}

	return EOK;
}

