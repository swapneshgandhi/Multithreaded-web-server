// This file contains function which initializes the server

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>
#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netdb.h>
#include        <netinet/in.h>
#include        <inttypes.h>
#include <sys/stat.h>
#include "unistd.h"
#include "server_in.h"

extern int s;
extern int fd;
extern char *port;	
extern struct sockaddr_in serv, remote;
extern	struct servent *se;
extern int newsock, len;
extern int port_num;		
extern int debug;
void setup_server()
{					    
	len = sizeof(remote);
	memset((void *)&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	if (port == NULL){
if (debug ==1 )	{
	fprintf(stderr,"\nport entered is NULL");}
		serv.sin_port = htons(8080);}
	else if (isdigit(*port)){
		serv.sin_port = htons(atoi(port));}
	else {
		if ((se = getservbyname(port, (char *)NULL)) < (struct servent *) 0) {
			perror(port);
			exit(1);
		}
		serv.sin_port = se->s_port;
	}
	if (bind(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
		perror("bind");
		exit(1);
	}
	if (getsockname(s, (struct sockaddr *) &remote, &len) < 0) {
		perror("getsockname");
		exit(1);
	}
	port_num=ntohs(remote.sin_port);
	//fprintf(stderr, "\nWelcome to myhttpd.", ntohs(remote.sin_port));
	//fprintf(stderr, " The server is now listning to requets on port number %d\n", ntohs(remote.sin_port));
}
