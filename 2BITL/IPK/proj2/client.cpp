/** 
 * Project 2, IPK
 * Name: Client
 * Author: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz, 2013
 **/

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEC 10
#define BUF 1024
#define PORT 65535

enum tecodes
{
	EOK = 0,     /**< No error */
	EHELP,       /**< Help msg required */
	EARG,        /**< Invalid argument */
	EUID,        /**< UID not a digit */
	EDIGIT,      /**< Not a digit in port number */
	EOVER,       /**< Overflow in port number */
	ESOCKET,     /**< Can't create socket */
	EGETH,       /**< Gethostbyname error */
	ECONN,       /**< Can't connect */
	EWRITE,      /**< Write error */
	EDB,         /**< Passwd broken */
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
	/* EUID */
	"User ID is a digit...\n",
	/* EDIGIT */
	"Port is not a digit...u gotta be kiddin' me...\n",
	/* EOVER */
	"Too big port number...srsly?\n",
	/* ESOCKET */
	"The socket cannot be created. Try tomorrow.\n",
	/* EGETH */
	"Houston, we've got a problem...gethostbyname problem.\n",
	/* ECONN */
	"Cannot connect to the server. Go make some coffee and try again later.\n",
	/* EWRITE */
	"Cannot send message to server. Call emergency services for help. (0118999881999119725 3)\n",
	/* EDB */
	"/etc/passwd is broken, you have to fix it :(\n",
	/* ECLIENT */
	"Some stranger is here\n",
	/* EREAD */
	"Cannot read response. It just doesn't wanna talk to me! :('\n",
	/* ECLOSE */
	"Cannot close connection to server.\n ...I'm just kiddin'.\n",
	/* EUNKNOWN */
	"Whaaaaaaaaat?\n",
};

const char *HELPMSG = {
	"-------------------------------------------------------------------------\n"
	"Hello, I'm user seeker, nice to meet you. If you run me corectly, I'll\n"
	"find you whoever you will ask for (if he have entry in /etc/passwd).\n"
	"You can search according to login or user ID and I can show you up to 6\n"
	"options described below. Each option can be requested only once.\n"
	"But first of all I need my little best friend server to be started,\n"
	"run him as ./server -help and he will tell you...\n"
	"Now you may ask how can you run me? It's simple, just type:\n"
	"-------------------------------------------------------------------------\n"
	"./client -help -h hostname -p port -l login -u uid -LUGNHS\n"
	"-------------------------------------------------------------------------\n"
	"help               Need some help?\n"
	"hostname           server name\n"
	"port               port number\n"
	"login              search for login\n"
	"uid                search for user ID\n"
	"L                  show login, optional\n"
	"U                  show user ID, optional\n"
	"G                  show group ID, optional\n"
	"N                  show gecos, optional\n"
	"H                  show home folder, optional\n"
	"S                  show shell, optional\n"
	"That's all folks! Have fun!\n"
	"-------------------------------------------------------------------------\n"
};

using namespace std;

/**
 * Structure with arguments
**/
typedef struct params {
	int ecode, port, l, u, g, n, h, s;
	string hostname, login, id, opts;
} TParams;


/**
 * Parse program arguments and returns them in a structure
 * @param argc count of args
 * @param argv array with args
 * @return structure
**/
TParams getParams(int argc, char *argv[])
{
	TParams result = {EOK, -1, 0, 0, 0, 0, 0, 0, "", "", "", ""};

	int c = 0;
	char *endptr;
	int id;

	if ((argc == 2) && (strcmp(argv[1], "-help") == 0)) {
		result.ecode = EHELP;
		return result;
	}

	while ((c = getopt(argc, argv, "h:p:luLUGHNS")) != -1) {
		switch (c) {
			case 'h':
				if (result.hostname.compare("") == 0)
					result.hostname = optarg;
				else {
					result.ecode = EARG;
					return result;
				}
			break;
			case 'p':
				if (result.port == -1) {
					result.port = strtol (optarg, &endptr, DEC);
					if (*endptr != '\0' || result.port < 0 || result.port > PORT) {
						result.ecode = EDIGIT;
						return result;
					}
				}
			break;
			case 'l':
				result.login.clear();
				result.id.clear();
				while (optind < argc && *argv[optind] != '-') {
					result.login.append(argv[optind]);
					result.login.append("  ");
					optind++;
				}
			break;
			case 'u':
				result.login.clear();
				result.id.clear();
				while (optind < argc && *argv[optind] != '-') {
					id = strtol (argv[optind], &endptr, DEC);
					if (*endptr != '\0' || id < 0) {
						result.ecode = EUID;
						return result;
					}
					result.id.append(argv[optind]);
					result.id.append("  ");
					optind++;
				}
			break;
			case 'L':
				result.opts.append("l");
				result.l++;
			break;
			case 'U':
				result.opts.append("u");
				result.u++;
			break;
			case 'G':
				result.opts.append("g");
				result.g++;
			break;
			case 'H':
				result.opts.append("h");
				result.h++;
			break;
			case 'N':
				result.opts.append("n");
				result.n++;
			break;
			case 'S':
				result.opts.append("s");
				result.s++;
			break;
			default:
				result.ecode = EARG;
				return result;
			break;
		}
	}

	if (optind != argc) {
		result.ecode = EARG;
		return result;
	}

	if((result.hostname.compare("") == 0) || (result.port == -1) || 
	((result.id.compare("") == 0) && (result.login.compare("") == 0))) {
		result.ecode = EARG;
		return result;
	}

	if (result.l > 1 || result.u > 1 ||
	result.g > 1 || result.h > 1 ||
	result.n > 1 || result.s > 1)
		result.ecode = EARG;

	return result;
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
 * Create socket and connect to server
 * @param data structure with url info
 * @param sock socket number
 * @return EOK or ERRCODE
**/
int connectTo(TParams params, int *sock) {
	int s;
	struct sockaddr_in server;
	struct hostent *hptr;

	// make socket
	if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
		return ESOCKET;

	server.sin_family = PF_INET;
	server.sin_port = htons(params.port);

	// Magic here. Do not touch.
	if ((hptr = gethostbyname(params.hostname.c_str())) == NULL)
		return EGETH;

	memcpy( &server.sin_addr, hptr->h_addr, hptr->h_length);

	// make connection
	if (connect (s, (struct sockaddr *)&server, sizeof(server) ) < 0 )
		return ECONN;

	*sock = s;

	return EOK;
}

/**
 * Communicate with server and save the response
 * @param data structure with url info
 * @param sock socket number
 * @param tmp string with response
 * @return EOK or ERRCODE
**/
int letsTalk(TParams params, int sock, string *tmp) {
	char buffer[BUF];
	int n;
	size_t index;
	string name, id;
	string subst;

	memset (buffer, 0, sizeof (char) * BUF);

	if (params.login != "") {
		id = "log";
		name = params.login;
	}
	else {
		id = "uid";
		name = params.id;
		}

	name.append("\r\n\r\n");

	string msg(
	"AWESOME PASSWD PROTOCOL 1.0\r\nshits to show: " + params.opts + "\r\n" + "search for: " + id + "\r\n" + "ppls to find:" + name
	);

	// chitchat with server here
	if (write(sock, msg.c_str(), msg.size()) < 0 )
		return EWRITE;

	while ((n = read(sock, buffer, BUF)) > 0)
		(*tmp).append(buffer, n);

	if (n < 0)
		return EREAD;

	if ((*tmp).find("itsbroken") != string::npos)
		return EDB;

	if ((*tmp).find("wrongclient") != string::npos)
		return ECLIENT;

	(*tmp).append("#&\n");

	while ((index = (*tmp).find("\n")) != string::npos) {
		subst = (*tmp).substr(0, index+1);
		(*tmp).erase(0, index+1);

		if (subst.compare("#&\n") == 0) {
			break;
		}
		else if ((subst.find("Error: duplicate request for ") != string::npos) ||
		(subst.find("Error: unknown login ") != string::npos)) {
			cerr<<subst;
		}
		else {
			cout<<subst;
		}
	}

	// close connection
	if (close(sock) < 0)
		return ECLOSE;

	return EOK;
}

/**
 * MAIN
**/
int main (int argc, char **argv) {

	int ecode = EOK;
	int sock;
	string aux;
	string msg = "";

	TParams params = getParams(argc, argv);
	if (params.ecode == EHELP) {
		cout<<HELPMSG;
		return EXIT_SUCCESS;
	}
	if (params.ecode != EOK) {
		printECode(params.ecode);
		return EXIT_FAILURE;
	}

	// connect
	ecode = connectTo(params, &sock);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}

	// talk to server
	ecode = letsTalk(params, sock, &msg);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}
