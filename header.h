#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

#define REG     0x01
#define CONF    0x02
#define REQ     0x03
#define REPLY   0x04
#define ERROR   0x05

#define MAX_SEC 5
#define MAX_USEC 0

struct database
{
    char name[10];
    char password[10];
};

struct client_info
{
    int port_no;
    char name[10];
};

struct st
{
	short int pkt_type;
	short int seq_num;
	long port_no;
	char name[10];
	char data[20];
};
