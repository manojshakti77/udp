/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<arpa/inet.h>

#define REG 0x01
#define CONF 0x02
#define REQ 0x03
#define REPLY 0x04
#define ERROR 0x05

int main(){
  int clientSocket, portNum, nBytes,i;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct timeval tv;
  socklen_t addr_size;
  struct st
  {
    short int pkt_type;
    long port_no;
    char name[10];
    char data[10];
  };


  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverAddr;
	
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

  struct st s1;
  s1.pkt_type = REG;
  s1.port_no = 3000;
  strcpy(s1.name,"CLIENT3");
  strcpy(s1.data,"PASS");

  //write(clientSocket,&s1,sizeof(s1));
  sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size);
  perror("write");

  while(1)
  {
	/*Receive message from server*/
    nBytes = recvfrom(clientSocket,&s1,sizeof(s1),0,NULL, NULL);
	if(nBytes < 0)
	{
    perror("recvfrom");
	continue;	
	}
  /*---- Print the received message ----*/
  printf("%hd---%ld---%s-----%s\n",s1.pkt_type,s1.port_no,s1.name,s1.data);
  switch(s1.pkt_type)
  {
    case CONF : printf("CONNECTION CONFIRMED\n");
        printf("Enter the data to be sent....:");
//        scanf("%s",s1.data);
//		strcpy(s1.data,"MANOJ");
		sprintf(s1.data,"%s %d","MANOJ",++i);
        s1.pkt_type = REQ;
    	sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size);
        //write(clientSocket,&s1,sizeof(s1));
        perror("write");
        break;
    case ERROR : printf("CONNECTION ERROR\n");
        break;
    case REPLY : printf("REPLY FROM SERVER....%s\n",s1.data);
				sprintf(s1.data,"%s %d","MANOJ",++i);
				sleep(2);
    			sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size);
        		break;
    default : printf("Invalid packet\n");
              break;
  }
  }

  return 0;
}
