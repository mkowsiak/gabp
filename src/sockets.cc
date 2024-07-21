#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "monitor.h"
#include "sockets.h"
#include "other.h"

int createSocket(int port)
{
	struct sockaddr_in server;
	int		server_size;
	int sock;
	
	if((sock=socket(AF_INET,SOCK_STREAM,0)) < 0)
		return -1;
	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	server_size = sizeof(server);
	if((bind(sock,(struct sockaddr*) &server,server_size)) < 0)
		return -1;
	if(listen(sock,5)<0)
		return -1;
	return sock;
}

int closeClientSocket(int sock)
{
	return close(sock);
}

/*int acceptConnection(int sock)
{
	struct sockaddr_in client;	
	socklen_t client_size = sizeof(client);
	//return accept(sock,(struct sockaddr*)&client,&client_size);

	int rtn;
	
	fd_set readf;
	fd_set writef;
	struct timeval to;
	
	do
	{
		FD_ZERO(&readf);
		FD_ZERO(&writef);
		FD_SET(sock,&readf);
		FD_SET(sock,&writef);
		to.tv_sec = 5;
		rtn = select(sock + 1,&readf,&writef,0,&to);
		if(rtn == 0)
		{
			fprintf(stderr,"TIMED OUT\n");
			fflush(stderr);
		}
		if((FD_ISSET(sock,&readf)) || (FD_ISSET(sock,&writef)))
		{
			return accept(sock,(struct sockaddr*)&client,&client_size);
		}
	}while(1);
}*/

int readLine(int sock,char *buf)
{
	char c;
	char *startOfBuf=buf;
	int readVal;
	int readCnt = 0;
	do
	{
		readVal = read(sock,&c,sizeof(char));
		if(readVal == -1)
			//return -1;
			return 0;
		if(readVal)
		{
			readCnt ++;
			if(c == '\n' || c == '\0')
			{
				*buf = '\0';
				while(*startOfBuf != '\n' && *startOfBuf != '\0' && *startOfBuf != '#' && *startOfBuf != '!')
					startOfBuf++;
				*startOfBuf = '\0';
				return readCnt;
			}
			else
			{
				*buf = c;
				buf++;
			}
		}	
	}while(readVal);
	return 0;//readCnt;
}

ProgramParameters *readProgramParametersFromSocket(int sock)
{
	char buf[120];
	char **tokens;
	
	readLine(sock,buf);
	tokens = getTokens(buf);
	
	if(tokens)
	{
		
	}
	else
	{
	}
		
	clearTokens(tokens);
		
	ProgramParameters *pp = NULL;
	return pp;
}