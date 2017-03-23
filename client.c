/************* UDP CLIENT CODE *******************/
#include"header.h"

int main(int argc,char **argv)
{
	int clientSocket, portNum, nBytes;
	struct sockaddr_in serverAddr;
  	struct timeval tv;
  	socklen_t addr_size;
  	FILE *fd;
  	int seq_num = 0;
  	struct st s1;
	int retval;

	if(argc < 4)
	{
		printf("Usage : ./cli port_number ip_address username password file_name\n");
		return 0;
	}
  	/*Create UDP socket*/
  	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(clientSocket < 0)
	{
		perror("ClientSocket");
		return 0;
	}

	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[2]);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverAddr;
	
	tv.tv_sec = MAX_SEC;
	tv.tv_usec = MAX_USEC;
	if((setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval))) < 0)
	{
		perror("setsockopt");
		return 0;
	}

	s1.pkt_type = REG;
	s1.seq_num = seq_num;
	s1.port_no = 3000;
	strcpy(s1.name,argv[3]);
	strcpy(s1.data,argv[4]);

	if((sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size)) < 0)
	{
		perror("sendto");
		return 0;
	}

	/*Opening the file whose data need to be sent to the SERVER*/
	if(argc == 5)
	{
		/*Opening the user supllied file*/
		fd = fopen(argv[5],"r");
	}
	else	
	{
		/*Opening the default file(If user doesn't supply any file in the cmd arguments*/
		fd = fopen("input.txt","r");
	}	
	if(fd == NULL)
	{
		perror("fopen");
		return 0;
	}
	while(1)
	{
	/*Receive message from server*/
    nBytes = recvfrom(clientSocket,&s1,sizeof(s1),0,NULL, NULL);
    /*Checking the status of recvfrom  & if successfully received checking the sequnce number*/
	if((nBytes < 0) || (s1.seq_num != seq_num))
    {
	    	perror("recvfrom");
  		sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size);
		continue;	
    }
  	switch(s1.pkt_type)
  	{
    case CONF : printf("CONNECTION CONFIRMED\n");
				if(argc == 5)
					strcpy(s1.data,argv[5]);
				else
					strcpy(s1.data,"input.txt");
		  		seq_num = ((seq_num)^(0x01));
        		s1.pkt_type = REQ;
		  		s1.seq_num = seq_num;
    			if((sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size)) < 0)
				{
					perror("sendto::CONF");
					return 0;
				}
        		break;
    case ERROR: printf("CONNECTION ERROR\n");
				return 0;
    case REPLY: printf("REPLY FROM SERVER\n");
		  		if(fgets(s1.data,sizeof(s1.data),fd))
				{
		  		seq_num = ((seq_num)^(0x01));
				s1.pkt_type = REQ;
		  		s1.seq_num = seq_num;
    		 	if((sendto(clientSocket,&s1,sizeof(s1),0,(struct sockaddr *)&serverAddr,addr_size)) < 0)
				{
					perror("sento::REPLY");
					return 0;
				}
				}
				else
					return 0;
        	 	break;
    default : printf("Invalid packet\n");
              break;
	}
	}
  return 0;
}
