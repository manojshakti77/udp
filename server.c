#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<ctype.h>

#define REG		0x01
#define CONF 	0x02
#define REQ	 	0x03
#define REPLY 	0x04
#define ERROR 	0x05

struct database
{
    char name[10];
    char password[10];
};

int main()
{
  int udpSocket, nBytes;
  char buf[64];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i,n,size,j,k;
  struct database **ptr;
   struct database base;
	
  struct st
  {
       short int pkt_type;
       long port_no;
       char name[10];
       char data[10];
  };
	struct st s1;
	FILE *fd = fopen("db.txt","r");
    perror("open");

	fgets(buf,sizeof(buf),fd);
    size = atoi(buf);
    ptr = (struct database **)malloc(size * sizeof(struct database *));
	    
	for(i = 0;i<size;i++)
    	ptr[i] = (struct database *)malloc(sizeof(base));
    perror("malloc");
    while(fgets(buf,sizeof(buf),fd))
    {
     for(i=0;buf[i] != ' ';i++);
     strncpy(ptr[j]->name,buf,i);
     i++;
     for(k=0;buf[i+k] != '\n';k++);
     strncpy(ptr[j++]->password,&buf[i],k);
    }

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;

  while(1){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage, &addr_size);

	if(nBytes < 0)
	{
		perror("recvfrom::server");
		continue;
	}
     switch(s1.pkt_type)
     {
        case REG:printf("CLIENT REGISTER.....%s  %s\n",s1.name,s1.data);
                 for(i=0;i<size;i++)
                 {
                   printf("%s\n",ptr[i]->name);
                   if(!strcmp(s1.name,ptr[i]->name))
                   {
                    printf("%s     %s",ptr[i]->password,s1.data);
                      if(!strcmp(ptr[i]->password,s1.data))
                      {
                        s1.pkt_type = CONF;
                        break;
                      }
                   }
                 }
                 if(i == size)
                    s1.pkt_type = ERROR;
                // write(sock,&s1,sizeof(s1));
    		   	sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size);
                 perror("write");
                 if(i == size)
                    return 0;

                 break;
		case REQ : printf("REQUEST DATA FROM CLIENT.....%s\n",s1.data);
               i = strlen(s1.data);
               sprintf(s1.data,"%d",i);
               printf("Data sent....%s\n",s1.data);
               s1.pkt_type = REPLY;
    		   sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size);
               //write(sock,&s1,sizeof(s1));
               perror("write");
               break;

        default: printf("Unknown\n");
                 break;
     }

    /*Send uppercase message back to client, using serverStorage as the address*/
//	while(1);
  }

  return 0;
}
