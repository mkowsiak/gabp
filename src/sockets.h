#ifndef _SOCKETS_H
#define _SOCKETS_H

#include "monitor.h"

#define SOCKET_DATA_REQUEST 0x0001
#define SOCKET_DATA_SENT		0x0002

#define SOCKET_DATA_TYPE_CHAR		0x0003
#define SOCKET_DATA_TYPE_INT		0x0103
#define SOCKET_DATA_TYPE_FLOAT	0x0203
#define SOCKET_DATA_TYPE_DOUBLE	0x0303
#define SOCKET_DATA_TYPE_STRING	0x0403

#define SOCKET_CONN_REQUEST 0x00fe
#define SOCKET_CONN_CLOSE		0x00ff


int createSocket(int);
int acceptConnection(int);
int readLine(int,char *);
int closeClientSocket(int );
ProgramParameters *readProgramParametersFromSocket(int);

#endif