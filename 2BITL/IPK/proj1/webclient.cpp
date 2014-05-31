/** 
 * Project 1, IPK
 * Name: Webclient
 * Author: Katerina Zaklova, xzaklo00@stud.fit.vutbr.cz, 2013
 **/

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define HTTP 7
#define DEC 10
#define BUF 1024
#define OK "200"
#define REDIR "301"
#define REDIR2 "302"
#define LEN 9
#define LOC 10
#define PORT 80
#define COUNT5 5
#define PARSE_LEN 1


enum tecodes
{
	EOK = 0,     /**< No error */
	ERED,        /**< Redirect needed */
	EER,         /**< Response 40x */
	EARG,        /**< Invalid argument */
	EHTTP,       /**< No HTTP in url */
	EDIGIT,      /**< Not a digit in port number */
	EOVER,       /**< Overflow in port number */
	ESOCKET,     /**< Can't create socket */
	EGETH,       /**< Gethostbyname error */
	ECONN,       /**< Can't connect */
	EWRITE,      /**< Write error */
	EREAD,       /**< Read error */
	ECLOSE,      /**< Close error */
	EFILE,       /**< Saving issues */
	EUNKNOWN     /**< Unknown error */
};

const char *ECODE[] =
{
	/* EOK */
	"Everything fine\n",
	/* ERED */
	"Seems like the redirect is needed...\n",
	/* EER */
	"Mmmm, okay :(\n",
	/* EARG */
	"Nononono, stop it! Rerun with -h...\n",
	/* EHTTP */
	"I told you something about http:// in url...got it?\n",
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
	"Cannot send message to server. Not enough mana.\n",
	/* EREAD */
	"Cannot read response. It just doesn't wanna talk to me! :('\n",
	/* ECLOSE */
	"Cannot close connection to server.\n ...I'm just kiddin'.\n",
	/* EFILE */
	"File issues :( \n",
	/* EUNKNOWN */
	"Wut?\n",
};

const char *HELPMSG = {
	"-------------------------------------------------------------------------\n"
	"Hello, I'm WEBCLIENT, WGET's little brother. I can download whatever you \n"
	"want - pages, photos, files...\n"
	"So how can you run me? It's simple, just type:\n"
	"-------------------------------------------------------------------------\n"
	"./webclient url_address\n"
	"-------------------------------------------------------------------------\n"
	"url_address     is always required with http:// on the beginning. \n"
	"That's all folks! Have fun!\n"
	"-------------------------------------------------------------------------\n"
};

using namespace std;

/**
 * Structure with url info
**/
typedef struct Data {
	int port;
	string name;
	string path;
	string my_file;
} TData;


/**
 * Print error code message
 * @param ecode error code
 */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EFILE)
		ecode = EUNKNOWN;

	cerr<<ECODE[ecode]<<endl;
}

/**
 * Parse url address and save parts
 * @param url string with url address
 * @param data structure with url parts
 * @return EOK or ERRCODE
**/
int parseUrl (char *url, TData *data) {
	string parse;
	size_t index;
	size_t index2;

	parse = url;

	//find and erase http
	if((index = parse.find("http://")) != string::npos)
		parse.erase(index, HTTP);
	else
		return EHTTP;

	index = parse.find('/');
	index2 = parse.find(':');

	//save hostname
	if (index < index2) {
		data->name = parse.substr(0, index);
		parse.erase(0, index);
	}
	else {
		data->name = parse.substr(0, index2);
		parse.erase(0, index2);
	}

	//save port
	if(((index = parse.find(':')) != string::npos) && (index==0)) {
		if (parse.size() == PARSE_LEN) {
			return EDIGIT;
		}
		index +=1;
		int number = 0;
		if((isdigit(parse[index]))==0)
			return EDIGIT;
		while (isdigit(parse[index])) {
			if (((INT_MAX - (parse[index] - '0'))/DEC) < number)
				return EOVER;
			number = number * DEC + (parse[index] - '0');
			index++;
		}
		data->port = number;
		parse.erase(0, index);
	}

	//save path
	if (parse.size() > 0)
		data->path = parse;

	//erase hash'n'sh*ts
	if((index = data->path.find('#')) != string::npos)
		data->path.erase(index);

	//replace spaces
	while ((index = data->path.find(" ")) != string::npos)
		data->path.replace(index, 1, "%20");

	//filename
	if((index = data->path.rfind("/")) != string::npos) {
		//there is some filename
		if (data->path.length()-1 != index) {
			//save filename
			data->my_file = data->path.substr(index+1);
			//hash in filename
			if ((index = data->my_file.find("#")) != string::npos) {
				data->my_file.erase(index);
				//no characters after
				if (index == 0)
					data->my_file = "index.html";
			}
			//question mark in filename
			if ((index = data->my_file.find("?")) != string::npos) {
				data->my_file.erase(index);
				//no characters after
				if (index == 0)
					data->my_file = "index.html";
			}
			//replace spaces back
			while ((index = data->my_file.find("%20")) != string::npos)
				data->my_file.replace(index, 3, " ");
		}
	}

	return EOK;
}

/**
 * Create socket and connect to server
 * @param data structure with url info
 * @param sock socket number
 * @return EOK or ERRCODE
**/
int connectTo(TData data, int *sock) {
	int s;
	struct sockaddr_in sin;
	struct hostent *hptr;

	//create socket
	if ((s = socket(PF_INET, SOCK_STREAM, 0 )) < 0)
		return ESOCKET;

	sin.sin_family = PF_INET;
	sin.sin_port = htons(data.port);

	//does host exist?
	if ((hptr = gethostbyname(data.name.c_str())) == NULL)
		return EGETH;

	memcpy (&sin.sin_addr, hptr->h_addr, hptr->h_length);

	//connect
	if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
		return ECONN;

	*sock = s;

	return EOK;
}

/**
 * Communicate with server and save the response
 * @param data structure with url info
 * @param sock socket number
 * @param tmp string with response
 * @param chunked chunked data indicator
 * @return EOK or ERRCODE
**/
int letsTalk(TData data, int sock, string *tmp, int *chunked) {
	char buffer[BUF];
	int n;

	memset (buffer, 0, sizeof (char) * BUF);

	string msg(
		"GET " +  data.path + " HTTP/1.1\r\n"
		"Host: " + data.name + "\r\n"
		"Connection: close\r\n\r\n"
	);

	//send request
	if (write(sock, msg.c_str(), msg.size()) < 0 )
		return EWRITE;

	//get reply
	while ((n = read(sock, buffer, BUF)) > 0)
		(*tmp).append(buffer, n);

	if (n < 0)
		return EREAD;

	//close socket
	if (close(sock) < 0)
		return ECLOSE;
	//we have chunked data
	if ((*tmp).find("Transfer-Encoding: chunked\r\n") != string::npos)
		*chunked = 1;
	else
		*chunked = 0;

	return EOK;
}

/**
 * Find response from server - OK or error code
 * @param msg response from server
 * @return EOK, ERED or EER 
**/
int findResponse (string *msg) {
	(*msg).erase(0, LEN);
	int index = (*msg).find("\n");
	(*msg).erase(index);

	//200
	if((*msg).find(OK) != string::npos)
		return EOK;
	//301 & 302
	else if((*msg).find(REDIR) != string::npos)
		return ERED;
	else if((*msg).find(REDIR2) != string::npos)
		return ERED;
	//anything else
	else
		return EER;
}

/**
 * Find location to redirect in response message
 * @param msg string with response
**/
void redirect (string *msg) {
	int index;

	index = (*msg).find("Location:");
	(*msg).erase(0, index+LOC);
	index = (*msg).find("\n");
	(*msg).erase(index);
}

/**
 * Cut the message body and store it into a file
 * @param msg string with response
 * @param chunked chunked data indicator
 * @param data structure with url info
 * @return EOK or ERRCODE
**/
int writeAndSave (string msg, int chunked, TData data) {
	size_t index;
	string hexa = "", aux = "";
	long int number = -1;

	//delete header
	if ((index = msg.find("\r\n\r\n")) != string::npos)
		msg.erase(0, index+4);

	//chunked data
	if (chunked == 1) {
		while (number != 0) {
			if ((index = msg.find("\r\n")) != string::npos) {
				//get number
				hexa = msg.substr(0, index);
				hexa.append("\0");
				number = strtol(hexa.c_str(), NULL, 16);
				//delete crlf
				msg.erase(0, index+2);
				//copy "number" of characters
				aux.append(msg, 0, number);
				//delete crlf
				msg.erase(0, number+2);
			}
		}
		msg = aux;
	}

	//save message into a file
	ofstream OutFile;
	OutFile.open(data.my_file.c_str(), ios::out | ios::binary);
	if(OutFile.fail())
		return EFILE;
	OutFile.write(msg.c_str(), msg.length());
	OutFile.close();

	return EOK;
}


/**
 * MAIN
**/
int main (int argc, char **argv) {

	char* url;
	int sock, aux, chunked = 0, ecode = EOK;
	string err, msg = "";
	TData data = {PORT, "", "/", "index.html"};

	//print help
	if (argc == 2 && strcmp(argv[1], "-h") == 0) {
		cout<<HELPMSG;
		return EXIT_SUCCESS;
	}
	//get arguments
	if (argc > 1 && optind < argc) {
		url = argv[optind];
		ecode = parseUrl(url, &data);
		if (ecode != (EOK)) {
			printECode(ecode);
			return EXIT_FAILURE;
	}
	}
	else {
		ecode = EARG;
		printECode(ecode);
		return EXIT_FAILURE;
	}

	ecode = connectTo(data, &sock);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}

	ecode = letsTalk(data, sock, &msg, &chunked);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}

	err = msg;
	aux = findResponse(&err);

	if (aux == ERED) {
		err = msg;
		redirect(&err);
		url = (char*)err.c_str();

		for (int j= 0; j < COUNT5; j++) {
			TData data = {PORT, "", "/", "index.html"};

			ecode = parseUrl(url, &data);
			if (ecode != (EOK)) {
				printECode(ecode);
				return EXIT_FAILURE;
			}

			ecode = connectTo(data, &sock);
			if (ecode != (EOK)) {
				printECode(ecode);
				return EXIT_FAILURE;
			}

			msg.clear();
			ecode = letsTalk(data, sock, &msg, &chunked);
			if (ecode != (EOK)) {
				printECode(ecode);
				return EXIT_FAILURE;
			}

			err = msg;
			aux = findResponse(&err);

			if (aux == EOK || aux == EER)
				break;
			else {
				err = msg;
				redirect(&err);
				url = (char*)err.c_str();
			}
		}
	}

	if (aux == EER) {
		cerr<<err<<endl;
		return EXIT_FAILURE;
	}

	ecode = writeAndSave(msg, chunked, data);
	if (ecode != (EOK)) {
		printECode(ecode);
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}
