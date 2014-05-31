/**
 * File: rdtserver.cpp
 * Author: Katerina Zaklova
 * Original file: udtdemo.c by Ondrej Rysavy
 * Changes made: 50%
**/

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <sys/time.h>
#include <signal.h>
#include "udt.h"


#define MAXLINE 500
#define PROGRAM "rdtserver"
#define PROGRAM_INFO "rdtserver by Katerina Zaklova, xzaklo00 \n\n"

enum tecodes { EOK = 0, ENOTMINE };

using namespace std;

in_addr_t remote_addr = 0x7f000001;
in_port_t remote_port = 0;
in_port_t local_port = 0;

/**
 * Get packet id from message
 * @line message
 * @return sequence number or ERRCODE
 */
int get_packet_id (char* line) {
	string my_line = line;
	string num;
	size_t index;
	int len = strlen("header sn=\"");

	if ((index = my_line.find("xzaklo00")) == string::npos)
		return ENOTMINE;

	index = my_line.find("header sn=\"");
	num = my_line.substr(index+len);
	index=num.find("\" ");
	num.erase(index);

	return atoi(num.c_str());
}

/**
 * Print data from message and save response for client
 * @line message
 * @return response
 */
string parse_message (char* line) {
	string my_line = line;
	string data = "";
	string sub = "";
	int index, index2;
	int len = strlen("<data>");

	// cut message data and send it to stdout
	index = my_line.find("<data>");
	data = my_line.substr(index+len);
	index = data.find("</data>");
	data.erase(index);
	fputs(data.c_str(), stdout);
	fflush(stdout);

	// save response message without data
	index = my_line.find("ack=\"");
	len = strlen("ack=\"");
	data = my_line.substr(0, index+len);
	data.append("1\" ");
	index = my_line.find("win=\"");
	index2 = my_line.find("<data>");
	sub = my_line.substr(index, index2+6);
	data.append(sub);
	data.append("</data></rdt-segment>");

	return data;
}

/**
 * Handle signals - kill server and clean the mess
 * @param signal_num number of signal
 */
void KillItWithFire(int signal_num) {
	cerr<<"A wild signal appeared "<<signal_num<<endl;
	cerr<<"Destroying server in 10...9...8..."<<endl;
	exit(0);
}

/****************MAIN********************/
int main(int argc, char **argv ) {
	char ch;
	char recvline[MAXLINE];
	int udt, n, num;
	int to_ack = 0;
	string ack_mes;

	signal(SIGTERM, KillItWithFire);
	signal(SIGINT, KillItWithFire);

	while ((ch = getopt(argc,argv,"s:d:h")) != -1) {
		switch(ch) {
			case 's':
					local_port = atol(optarg);
				break;
			case 'd':
					remote_port = atol(optarg);
				break;
			case 'h':
				fprintf(stdout, PROGRAM_INFO);
				fprintf(stdout, "usage: rdtserver -s port -d port\n\n");
				fprintf(stdout, "  s port    : source port\n" );
				fprintf(stdout, "  d port    : destination port\n" );
				exit(EXIT_SUCCESS);
		}
	}

	// some argument is wrong
	if (remote_addr == 0 || remote_port == 0 || local_port == 0) {
		fprintf(stderr, "Wrong port numbers! Use '-h' for help.\n");
		exit(EXIT_FAILURE);
	}

	udt = udt_init(local_port);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	FD_SET(STDIN_FILENO, &readfds);

	while (select(udt+1, &readfds, NULL, NULL, NULL)) {
		// receive packet
		if (FD_ISSET(udt, &readfds)) {
			n = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
			recvline[n] = 0;
			cerr<<""; 
			num = get_packet_id(recvline);
			//if (num == ENOTMINE)
				//cerr<<"server: some stranger bothering me"<<endl;
			if (num == to_ack) {
				ack_mes = parse_message(recvline);
				to_ack++;
			}
			// send ACK
			if (to_ack > 0)
				cerr<<""; 
				if (!udt_send(udt, remote_addr, remote_port, (void *)ack_mes.c_str(), ack_mes.length())) {
					perror("ack cannot be sent");	// some error
				}
		}
		FD_ZERO(&readfds);
		FD_SET(udt, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
	}
	return EOK;
}
