/**
 * File: rdtclient.cpp
 * Author: Katerina Zaklova
 * Original file: udtdemo.c by Ondrej Rysavy
 * Changes made: 75%
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


#define MAXLEN 350
#define MAXLINE 512
#define PROGRAM "rdtclient"
#define PROGRAM_INFO "rdtclient by Katerina Zaklova, xzaklo00 \n\n"

enum tecodes { EOK = 0, ENOTMINE };

using namespace std;

// structure for packet information
typedef struct tmsg {
	string name;
	string data;
	int sn, ack, win, tack, time;
} tMsg;

// global variables
tMsg msg = {"xzaklo00", "", 0, 0, 0, 0, 0};
struct itimerval timer;
struct timeval timeout;
timeval time_now;
vector<tMsg> packet;
vector<tMsg> window;
vector<tMsg> backup;
int udt, rec, win = 1, ti = 15;
int rtt[3] = {0, 0, 0};

in_addr_t remote_addr = 0x7f000001;
in_port_t remote_port = 0;
in_port_t local_port = 0;

/**
 * Make xml message for sending
 * @mesg structure with info
 * @return string with message
 */
string make_xml (tMsg mesg) {
	string xml_str =  "";
	stringstream to_str;
	xml_str = ("<rdt-segment id=\"");
	xml_str.append(mesg.name);
	xml_str.append("\"><header sn=\"");
	to_str << mesg.sn;
	xml_str.append(to_str.str());
	to_str.str("");
	xml_str.append("\" ack=\"");
	to_str << mesg.ack;
	xml_str.append(to_str.str());
	to_str.str("");
	xml_str.append("\" win=\"");
	to_str << mesg.win;
	xml_str.append(to_str.str());
	to_str.str("");
	xml_str.append("\" tack=\"");
	to_str << mesg.tack;
	xml_str.append(to_str.str());
	to_str.str("");
	xml_str.append("\"></header><data>");
	xml_str.append(mesg.data);
	xml_str.append("</data></rdt-segment>");
	return xml_str;
}

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
 * Fill sliding window with packets
 */
void fill_window (void) {
	size_t i;
	int j;

	for (i = 0; i < packet.size(); i++) {
		if (packet[i].ack == 0)
			break;
	}

	for (j = 0; j < win; j++) {
		if (i >= packet.size())
			break;
		window.resize(j+1);
		window[j] = packet[i];
		i++;
	}
}

/**
 * Set timer values
 */
void set_timer (void) {
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = ti*1.3*1000;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = ti*1.3*1000;
	setitimer(ITIMER_REAL, &timer, NULL);
}

/**
 * Handle timeout signal for timer and resend the window
 * @sig signal id
 */
void use_timer(int sig) {
	size_t i;
	string xmlmes = "";
	sig = sig;
	cerr<<""; 
	if (win > 2)
		win = win/3;

	// fill sliding window
	fill_window();
	// count Tack for timer
	ti = (ti + ((2*rtt[0] + rtt[1] + rtt[2])/4))/2;
	// save window size and tack
	for (i = 0; i < window.size(); i++) {
		window[i].win = win;
		window[i].tack = ti*1.3;
	}
	// get sending time
	gettimeofday(&time_now, NULL);
	set_timer();
	for (i = 0; i < window.size(); i++) {
		xmlmes = make_xml(window[i]);
		// send message
		if (!udt_send(udt, remote_addr, remote_port, (void *)xmlmes.c_str(), xmlmes.length())) {
			perror("message cannot be sent");
		}
	}
	// backup window for timer
	backup = window;
	// store sending time to packet
	for (i = 0; i < window.size(); i++)
		packet[window[i].sn].time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
	// clear window for next sending
	window.clear();

	// handle timeout
	signal(SIGALRM, use_timer);
}

/****************MAIN********************/
int main(int argc, char **argv ) {
	char ch;
	int num, millis, sn = 0;
	char recvline[MAXLINE];
	string xmlmes;
	size_t i;

	signal(SIGALRM, use_timer);

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
				fprintf(stdout, "usage: rdtclient -s port -d port\n\n");
				fprintf(stdout, "  s port    : source port\n");
				fprintf(stdout, "  d port    : destination port\n");
				exit(EXIT_SUCCESS);
		}
	}

	// some argument is wrong
	if (remote_port == local_port || remote_port == 0 || local_port == 0) {
		fprintf(stderr, "Wrong port numbers! Use '-h' for help.\n");
		exit(EXIT_FAILURE);
	}

	udt = udt_init(local_port);

	//fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	// make stdin reading non-clocking

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	FD_SET(STDIN_FILENO, &readfds);

	// save stdin into vector
	if (FD_ISSET(STDIN_FILENO, &readfds)) {
		while (!feof(stdin)) {
			rec = fread(recvline, sizeof(char), MAXLEN, stdin);
			recvline[rec] = 0;
			msg.data = recvline;
			msg.sn = sn;
			sn++;
			packet.push_back(msg);
		}
	}
	
	while (select(udt+1, &readfds, NULL, NULL, NULL)) {
		// receive ACK
		if (FD_ISSET(udt, &readfds)) {
			// receive new message
			rec = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
			recvline[rec] = 0;
			gettimeofday(&time_now, NULL);
			millis = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
			cerr<<""; 
			//get packet sn
			num = get_packet_id(recvline);
			//if (num == ENOTMINE)
				//cerr<<"client: some stranger bothering me"<<endl;
			for (i = 0; i < packet.size(); i++) {
				// packet has not got ACK yet
				if (packet[i].sn == num && packet[i].ack == 0) {
					// store ACK
					packet[i].ack = 1;
					// store RTT to packet
					packet[i].time = millis - packet[i].time;
					// store newest RTT
					rtt[2] = rtt[1];
					rtt[1] = rtt[0];
					rtt[0] = packet[i].time;
					// make window bigger
					win += 1;
				}
				if (packet[i].sn < num)
					packet[i].ack = 1;
			}
			if (packet[packet.size()-1].ack == 1)
				return EOK;
		}

		// fill sliding window
		cerr<<""; 
		fill_window();
		// count Tack for timer
		ti = (ti + ((2*rtt[0] + rtt[1] + rtt[2])/4))/2;
		// save window size and tack
		for (i = 0; i < window.size(); i++) {
			window[i].win = win;
			window[i].tack = ti*1.3;
		}

		// get sending time
		gettimeofday(&time_now, NULL);
		set_timer();
		cerr<<""; 
		for (i = 0; i < window.size(); i++) {
			xmlmes = make_xml(window[i]);
			// send message
			if (!udt_send(udt, remote_addr, remote_port, (void *)xmlmes.c_str(), xmlmes.length())) {
				perror("message cannot be sent");
			}
		}
		// backup window for timer
		backup.clear();
		backup = window;
		// store sending time to packet
		for (i = 0; i < window.size(); i++)
			packet[window[i].sn].time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
		window.clear();

		FD_ZERO(&readfds);
		FD_SET(udt, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
	}
	return EOK;
}
