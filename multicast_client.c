/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include<errno.h>
#include <string.h>
 
struct sockaddr_in localSock;
socklen_t locallen;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	{
		int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	}
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	group.imr_interface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");
	 
	/* Read from the socket. */
/*	datalen = sizeof(databuf);
	if(read(sd, databuf, datalen) < 0)
	{
		perror("Reading datagram message error");
		close(sd);
		exit(1);
	}
	else
	{
		printf("Reading datagram message...OK.\n");
		printf("The message from multicast server is: \"%s\"\n", databuf);
	}*/

	while(1)
	{
		int numBytes;
		char buf[1024];
		if((numBytes=recvfrom(sd,buf,1024,0,(struct sockaddr *)&localSock, &locallen))==-1)
		{
			perror("recv");
			exit(1);
		}
		buf[numBytes]='\0';
		/*send file when you receive file*/
		if(strcmp(buf,"file")==0)
		{
			char filename4[1024];
			recvfrom(sd,filename4,1024,0,(struct sockaddr *)&localSock, &locallen);/*receive file name*/
			FILE *fr=fopen(filename4,"w");/*write file*/
			printf("receiveing file from server ......\n");
			if(fr==NULL)
			{
				printf("file %s cannot be opened\n",filename4);
			}
			else
			{
				bzero(buf,1024);
				int fr_block_sz=0;
	    			while((fr_block_sz=recvfrom(sd,buf,1024,0,(struct sockaddr *)&localSock, &locallen))>0)
	    			{
					int write_sz=fwrite(buf,sizeof(char),fr_block_sz,fr);
	        			if(write_sz<fr_block_sz)
					{
	            				error("file write failed\n");
	        			}
					bzero(buf,1024);
					if (fr_block_sz==0||fr_block_sz!=1024)
					{
						break;
					}
				}
				if(fr_block_sz<0)
        			{
					if(errno==EAGAIN)
					{
						printf("recv() timed out\n");
					}
					else
					{
						fprintf(stderr,"recv() failed due to errno=%d\n", errno);
					}
				}
	    			printf("received from server\n");
	    			fclose(fr);
			}
		}
		/*quit when you receive quit*/
		else if(strcmp(buf,"quit")==0)
		{
			printf("Server is closed\n");
			close(sd);
			exit(1);
		}
		/*print message*/
		else printf("error\n");
	}


	return 0;
}
