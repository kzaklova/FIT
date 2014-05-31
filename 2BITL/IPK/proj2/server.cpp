/** 
 * Project 2, IPK
 * Name: Server
 * Author: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz, 2013
 **/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <limits.h>
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <pwd.h>

#define CLIENTS 5
#define MSG 256
#define ERASE 29
#define SHOW 15
#define FIND 12
#define PPL 13
#define BROKEN 8
#define CLI 11
#define PORTMIN 1024
#define PORTMAX 65535
#define DEC 10

enum tecodes
{
	EOK = 0,     /**< No error */
	EHELP,       /**< Help msg required */
	EARG,        /**< Invalid argument */
	ESOCKET,     /**< Can't create socket */
	EBIND,       /**< Can't bind */
	ELISTEN,     /**< Can't listen */
	EACCEPT,     /**< Not accepted */
	EWRITE,      /**< Write error */
	EFORK,       /**< Fork issues */
	EDB,         /**< Passwd is broken */
	ECLIENT,     /**< Strange client */
	EREAD,       /**< Read error */
	ECLOSE,      /**< Close error */
	EUNKNOWN     /**< Unknown error */
};

const char *ECODE[] =
{
	/* EOK */
	"Everything fine\n",
	/* EHELP */
	"Helpmsg required.\n",
	/* EARG */
	"Help! I need somebody! Help! Not just anybody! Help! Rerun with -help...\n",
	/* ESOCKET */
	"The socket cannot be created. Try tomorrow.\n",
	/* EBIND */
	"Cannot bind...not enough mana.\n",
	/* ELISTEN */
	"I can't hear yoooou...\n",
	/* EACCEPT */
	"Challenge NOT accepted...nor the connection...\n",
	/* EWRITE */
	"Cannot send kisses, love and response to client... That is sad.\n",
	/* EFORK */
	"Fork failure\n",
	/* EDB */
	"/etc/passwd is broken, you have to fix it :(\n",
	/* ECLIENT */
	"Some stranger is here\n",
	/* EREAD */
	"Unreadable message...bad bad client\n",
	/* ECLOSE */
	"Cannot close connection.\n ...I'm just kiddin'.\n",
	/* EUNKNOWN */
	"Whaaat?\n",
};

const char *HELPMSG = {
	"-------------------------------------------------------------------------\n"
	"Hello, I'm server, as my friend you can call me Hugo. I'm a little friend\n"
	"of client and he needs me to get started. You can run me just like this: \n"
	"-------------------------------------------------------------------------\n"
	"./server -p port_number\n"
	"-------------------------------------------------------------------------\n"
	"help            Need some help?\n"
	"p               useless option, but always required\n"
	"port_number     any number from 1025 to INT_MAX\n"
	"I think we're done here, now visit my sweet friend...\n"
	"-------------------------------------------------------------------------\n"
};

using namespace std;


/**
 * Parse program arguments and returns EOKr or error
 * @param argc count of args
 * @param argv array with args
 * @param port port number
 * @return EOK or error
**/
int getParams (int argc, char *argv[], int *port)
{
	char *endptr;

	if ((argc == 2) && (strcmp(argv[1], "-help") == 0))
		return EHELP;
	else if((argc == 3) && (strcmp(argv[1], "-p") == 0)) {
		*port = strtol(argv[2], &endptr, DEC);
		if((*endptr != '\0') || (*port > PORTMAX) || (*port <= PORTMIN))
			return EARG;
	}
	else return EARG;

	return EOK;
}

/**
 * Print error code message
 * @param ecode error code
 */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > ECLOSE)
		ecode = EUNKNOWN;

	cerr<<ECODE[ecode]<<endl;
}

/**
 * Make socket and start listening
 * @param port port number from arguments
 * @param sock socket descriptor
 * @param serv structure with server settings
 * @return EOK or ERRCODE
 */
int connectTo (int port, int *sock, struct sockaddr_in *serv)
{
	int s;
	struct sockaddr_in server;

	// make socket
	if ((s = socket(PF_INET, SOCK_STREAM, 0 )) < 0) {
		printECode(ESOCKET);
		return ESOCKET;
	}

	// set protocol, port and IP
	server.sin_family = PF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr  = INADDR_ANY;

	// bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
		return EBIND;

	// listen
	if (listen(s, CLIENTS))
		return ELISTEN;


	*sock = s;
	*serv = server;

	return EOK;
}

/**
 * Talk with client
 * @param conn connection descriptor
 * @return EOK or ERRCODE
 */
int ChitChat(int *conn)
{
	char msg[MSG];
	char *endptr;
	string mesg = "";
	string resp = "";
	string status = "";
	string opts = "";
	string line, backup, tmp;
	int n, x = 0, lines = 0, found = 0, t = *conn;
	size_t index, i;
	struct passwd *pd;
	uid_t uid;
	stringstream str;
	ifstream passfile("/etc/passwd");

	// read msg from client
	while ((n = read(t, msg, MSG)) > 0) {
		mesg.append(msg, n);
		if ((index = mesg.find("\r\n\r\n")) != string::npos) {
			mesg.erase(index);
			break;
		}
	}

	if (n < 0)
		return EREAD;

	// count valid lines in /etc/passwd
	while(getpwent() != NULL) {
		x++;
	}
	endpwent();

	// count total lines in /etc/passwd
	while (getline(passfile, line)) {
		lines++;
	}

	// lines does not match, file is broken
	if (x != lines)
		return EDB;

	// we accept only messages based on our protocol
	if (mesg.substr(0, ERASE) != "AWESOME PASSWD PROTOCOL 1.0\r\n")
		return ECLIENT;

	// erase first line
	mesg.erase(0, ERASE);

	// get options we have to show
	mesg.erase(0, SHOW);
	index = mesg.find ("\r\n");
	opts = mesg.substr(0, index);
	mesg.erase(0, index+2);

	// find according to uid or login?
	mesg.erase(0, FIND);
	index = mesg.find("\r\n");
	status = mesg.substr(0, index);
	mesg.erase(0, index+2);

	mesg.erase(0, PPL);

	// backup string for matching duplicate search items
	backup = " ";

	// search for each item
	while ((index = mesg.find("  ")) != string::npos) {
		
		// store item to its own string
		tmp = mesg.substr(0, index);
		resp.clear();

		// did we want the same item before?
		if (backup.find(" " + tmp + " ")!= string::npos) {
			resp.append("Error: duplicate request for ");
			resp.append(tmp);
			resp.append("\n");
		}

		backup.append(mesg, 0, index+1);
		mesg.erase(0, index+2);

		// we search according to username
		if ((status.compare("log") == 0) && (resp.compare("")== 0)) {
			// we found entry
			if((pd = getpwnam(tmp.c_str())) != NULL) {
				// no options required
				if (opts.size() == 0) {
					resp.append(tmp);
					resp.append(": entry was found\n");
				}
				for (i = 0; i < opts.size(); i++) {
					str.str("");	// clear any data stored in stream
					// username
					if (opts[i] == 'l')
						resp.append(pd->pw_name);
					// uid
					if (opts[i] == 'u') {
						str << pd->pw_uid;
						resp.append(str.str());
					}
					// gid
					if (opts[i] == 'g') {
						str << pd->pw_gid;
						resp.append(str.str());
					}
					// home
					if (opts[i] == 'h')
						resp.append(pd->pw_dir);
					// gecos
					if (opts[i] == 'n')
						resp.append(pd->pw_gecos);
					// shell
					if (opts[i] == 's')
						resp.append(pd->pw_shell);
					// we are not at the end of entry
					if (i < opts.size()-1)
						resp.append(" ");
					// end of entry, line feed and next please
					else
						resp.append("\n");
				}
			}
			// no entry there
			else {
				resp.append("Error: unknown login ");
				resp.append(tmp);
				resp.append("\n");
			}
		}
		// we search according to user id
		if ((status.compare("uid") == 0) && (resp.compare("")== 0)) {
			uid = strtol(tmp.c_str(), &endptr, DEC);
			// rewind file to the start
			setpwent();
			// we found valid entry
			while ((pd = getpwent()) != NULL) {
				// and it's exactly the entry we are looking for
				if (pd->pw_uid == uid) {
					found = 1;
					// no options required
					if (opts.size() == 0) {
						resp.append(tmp);
						resp.append(": entry was found\n");
					}
					for (i = 0; i < opts.size(); i++) {
						str.str("");	// clear any data stored in stream
						// username
						if (opts[i] == 'l')
							resp.append(pd->pw_name);
						// uid
						if (opts[i] == 'u') {
							str << pd->pw_uid;
							resp.append(str.str());
						}
						// gid
						if (opts[i] == 'g') {
							str << pd->pw_gid;
							resp.append(str.str());
						}
						// home
						if (opts[i] == 'h')
							resp.append(pd->pw_dir);
						// gecos
						if (opts[i] == 'n')
							resp.append(pd->pw_gecos);
						// shell
						if (opts[i] == 's')
							resp.append(pd->pw_shell);
						// we are not at the end of entry
						if (i < opts.size()-1)
							resp.append(" ");
						// end of entry, line feed and next please
						else
							resp.append("\n");
					}
				}
			}
			// end searching
			endpwent();
			// entry is not there
			if (found == 0) {
				resp.append("Error: unknown login ");
				resp.append(tmp);
				resp.append("\n");
			}
			found = 0;
		}

		// send kisses and msg back to the client
		if (write(t, (resp).c_str(), resp.size()) < 0 )
			return EWRITE;
	}

	*conn = t;

	return EOK;
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

int main (int argc, char **argv)
{
	int sock, t, pid = 0, port;
	socklen_t sinlen;
	long p;
	struct sockaddr_in server;

	signal(SIGTERM, KillItWithFire);
	signal(SIGINT, KillItWithFire);
	signal(SIGCHLD, SIG_IGN);

// get params
	int ecode = getParams(argc, argv, &port);
	if (ecode == EHELP) {
	cout<<HELPMSG;
	return EXIT_SUCCESS;
}
	if (ecode != EOK) {
		printECode(EARG);
		return EARG;
	}

	// connect
	ecode = connectTo(port, &sock, &server);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}

	sinlen = sizeof(server);

	while (1) {
		// accept new connection
		if ((t = accept(sock, (struct sockaddr *) &server, &sinlen)) < 0){
			printECode(EACCEPT);
			return EACCEPT;
		}
		// make love and processes
		if ((pid = fork()) > 0) {
			// close connection
			if (close(t) < 0) {
				printECode(ECLOSE);
				return ECLOSE;
			}
		}
		else if (pid == 0) {
			p = long(getpid());
			// chat with client
			ecode = ChitChat(&t);
			if (ecode == EDB) {
				if (write(t, "itsbroken", BROKEN) < 0 ) {
					printECode(EWRITE);
					return EWRITE;
				}
				printECode(EDB);
				return EDB;
			}
			if (ecode == ECLIENT) {
				if (write(t, "wrongclient", CLI) < 0 ) {
					printECode(EWRITE);
					return EWRITE;
				}
				printECode(ECLIENT);
				return ECLIENT;
			}
			// close connection
			if (close(t) < 0) {
				printECode(ECLOSE);
				return ECLOSE;
			}
			// close socket
			if (close(sock) < 0) {
				printECode(ECLOSE);
				return ECLOSE;
			}
			exit(0);
		}
		else
			return EFORK;
	}

	// close socket
	if (close(sock) < 0) {
		printECode(ECLOSE);
		return ECLOSE;
	}

	return EOK;
}
