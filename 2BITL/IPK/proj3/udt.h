/*
 ============================================================================
 Name        : udt.c
 Author      : Ondrej Rysavy
 Date        : Feb 26, 2009
 Copyright   : (c) Brno University of Technology
 Description : An implementation of UDT protocol.
               It simply wraps underlaying UDP protocol by
               function more appropriate for serving as pseudo-network layer.
 ============================================================================
 */
#ifndef UDT_H_
#define UDT_H_
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
/*
 * Returns UDT descriptor or NULL if error occurred.
 * local_port - Specifies a local port to which UDT binds.
 */
static inline int udt_init(in_port_t local_port)
{
	int udt = socket(AF_INET, SOCK_DGRAM, 0);
	fcntl(udt, F_SETFL, O_NONBLOCK);
	if (udt <= 0) {
		fprintf(stderr, "UDT: Cannot create UDT descriptor.");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(0);
	sa.sin_port = htons(local_port);
	int err = bind(udt, (const struct sockaddr *) &sa, sizeof(sa));
	if (err == -1) {
		fprintf(stderr, "UDT: Cannot bind to the specified port.");
		exit(EXIT_FAILURE);
	}
	return udt;
}

/*
 * Reads a received datagram in UDT buffer pool, if such exists.
 * udt - Determines UDT descriptor as initialized by udt_init() function.
 * buff - A pointer to buffer used for store the available data.
 * nbytes - Length of the buffer.
 * addr - A pointer to variable to be filled with IP address of the sender.
 *        This can be NULL if such information is not required.
 * port - A pointer to variable to be filled with Port used by the sender.
 *        This can be NULL if such information is not required.
 *
 * Returns the length of the packet received or 0 no packet were read.
 */
static inline int udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port)
{
	struct stat info;
	if (fstat(udt, &info) != 0) {
	      fprintf(stderr, "UDT: descriptor is invalid, probably udt_init not called.\n");
	      exit(EXIT_FAILURE);
	}
	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	socklen_t salen = sizeof(sa);
	ssize_t nrecv = recvfrom(udt, buff, nbytes, MSG_DONTWAIT, (struct sockaddr *) &sa, &salen);
	if(addr != NULL) (*addr) = ntohl(sa.sin_addr.s_addr);
	if(port!=NULL) (*port) = ntohs(sa.sin_port);
	if (nrecv < 0 ) nrecv = 0;
	return nrecv;
}

/*
 * Sends a new UDT datagram with data provided to the specified address and port.
 * udt - Determines UDT descriptor as initialized by udt_init() function.
 * addr - Ip address of the remote node.
 * port - Port on the remote node used for distinguishing different connections.
 * buff - A buffer containing RDT packet.
 * nbytes - THe lenght of the buffer with data.
 *
 * Returns 1 if packet has been successfully send or 0 if a problem occurred.
 */
static inline int udt_send(int udt, in_addr_t addr, in_port_t port, void *buff, size_t nbytes)
{
	struct stat info;
	if (fstat(udt, &info) != 0) {
	      fprintf(stderr, "UDT: descriptor is invalid, probably udt_init not called.\n");
	      exit(EXIT_FAILURE);
	}

	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(addr);
	sa.sin_port = htons(port);
	ssize_t nsend = sendto(udt, buff, nbytes, 0, (const struct sockaddr *) &sa, sizeof(sa));
	return nsend == (ssize_t)nbytes;
}

#endif /* UDT_H_ */
