#include"site_unblock.h"

#define BUFSIZE 1024
#define PORT 8080
#define HOST "localhost"

int main(int argc, char **argv)
{
	int parentfd;
	int childfd;
	int portno;
	int clientlen;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	struct hostent *hostp;
	char buf[BUFSIZE];
	char *hostaddrp;
	int optval;
	int n;

	portno = PORT;

	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if(parentfd < 0)
	{
		puts("[-]Failed to open socket");
		exit(1);
	}

	optval = 1;
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
//bind
	if(bind(parentfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))<0)
	{
		puts("[-]Failed to binding");
		exit(1);
	}
//listen
	if(listen(parentfd,10)<0)
	{
		puts("[-]Failed to listen");
		exit(1);
	}

	clientlen = sizeof(clientaddr);
	
	puts("[*]Socket Opened");
	while(true)
	{
//accept
		childfd = accept(parentfd, (struct sockaddr*)&clientaddr ,(unsigned int*)&clientlen);
		if(childfd < 0)
		{
			puts("[-]Failed to accept");
			exit(1);
		}

		hostp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr,
													sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if(hostp==NULL)
		{
			puts("[-]Failed to gethostbyaddr");
			exit(1);
		}
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if(hostaddrp == NULL)
		{
			puts("[-]Failed to inet_ntoa");
			exit(1);
		}

//	printf("[+]Server established connection with %s (%s)\n",hostp->h_name, hostaddrp);


		pthread_t thread;
		struct thread_argument thread_argu;
		thread_argu.childfd = childfd;

		pthread_create(&thread, NULL, &th_function, (void*)&thread_argu);
		pthread_detach(thread);
	}

	close(parentfd);
}	
