#include"header.h"

struct database **ptr;
struct database base;
int size;

int database_create(void)
{
	int i,n,j,k;
	char buf[50];

	int size;

	/*Reading the password database and forming a linkedlist*/
    FILE *fd = fopen("db.txt","r");
	if(fd == NULL)
	{
		perror("fopen");
		return 0;
	}
	fgets(buf,sizeof(buf),fd);
    size = atoi(buf);
	j = 0;
	k = 0;
    ptr = (struct database **)malloc(size * sizeof(struct database *));

    for(i = 0;i<size;i++)
	{
        ptr[i] = (struct database *)malloc(sizeof(base));
		if(ptr[i] == NULL)
		{
    		perror("malloc");
			return 0;
		}
	}

	while(fgets(buf,sizeof(buf),fd))
    {
     for(i=0;buf[i] != ' ';i++);
     strncpy(ptr[j]->name,buf,i);
     i++;
     for(k=0;buf[i+k] != '\n';k++);
     strncpy(ptr[j++]->password,&buf[i],k);
    }
    fclose(fd);
	return size;
}
int main(int argc,char **argv)
{
	int udpSocket, nBytes;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i,j;
	struct client_info client_info;//To store the client information once connection confirmed 
	int seq_num = 1;
	int index = 0;
	struct st s1;
	int retval;
	int size;
	int fd;

	if(argc < 2)
	{
		printf("Usage : ./ser port_number ip_address\n");
		return 0;
	}
	
	size = database_create();
	if(size == 0)
		return 0;
		
#if 0
	/*Reading the password database and forming a linkedlist*/
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
	fclose(fd);
#endif
	/*Create UDP socket*/
	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(udpSocket < 0)
	{
		perror("socket");
		return 0;
	}

	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[2]);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Bind socket with address struct*/
	if(bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("bind");
		return 0;
	}
	

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverStorage;

	while(1){
    /* Try to receive any incoming UDP datagram. Address and port of 
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage, &addr_size);
	if(seq_num == s1.seq_num)
	{
		printf("Duplicate DATA\n");
		s1.pkt_type = REPLY;
		memset(s1.data,'\0',sizeof(s1.data));
		sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size);
		continue;
	}
	seq_num = s1.seq_num;

	if(nBytes < 0)
	{
		perror("recvfrom::server");
		continue;
	}
     switch(s1.pkt_type)
     {
        case REG:printf("CLIENT REGISTER.....%s  %s\n",s1.name,s1.data);
				/*Logic for the AUTHENTICATION*/
                 for(i=0;i<size;i++)
                 {
                   printf("%s\n",ptr[i]->name);
                   if(!strcmp(s1.name,ptr[i]->name))
                   {
                    printf("%s     %s\n",ptr[i]->password,s1.data);
                      if(!strcmp(ptr[i]->password,s1.data))
                      {
						client_info.port_no = s1.port_no;
						strcpy(client_info.name,s1.name);
                        s1.pkt_type = CONF;
                        break;
                      }
                   }
                 }
                 if(i == size)
                    s1.pkt_type = ERROR;
				 sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size);
				for(j = 0;j<size;j++)
				{
    				free(ptr[j]);
				//	perror("free");
				}
				free(ptr);
			//	perror("free---");
                if(i == size)
                    return 0;
                 break;
		case REQ:if((s1.port_no == client_info.port_no) && (strcmp(s1.name,client_info.name)))
				 {
					s1.pkt_type = ERROR;
    		   		sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size);
					break;
				 }
				 if(index == 0)
				 {
					printf("Name of the file.....%s\n",s1.data);
					if((fd = open(s1.data,O_CREAT|O_WRONLY)) < 0)
					{
						perror("open::file");
						return 0;
					}
				 }
				 else
				 {	
					printf("Data:%s",s1.data);
					if(write(fd,s1.data,strlen(s1.data)) < 0)
					{
						perror("write");
						return 0;
					}
				 }
               	 s1.pkt_type = REPLY;
				 memset(s1.data,'\0',sizeof(s1.data));
				 if((++index) == 5)
					 continue;
    		   	 if(sendto(udpSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverStorage,addr_size) < 0)
				 {
				 	perror("sendto::REQ");
					return 0;
				 }
               	 break;
        default: printf("Unknown\n");
                 break;
     }
  }

  return 0;
}
