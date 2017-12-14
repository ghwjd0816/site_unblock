#include"site_unblock.h"

#define BUFSIZE 10000
#define HOSTSIZE 100

char http_method[NUM_OF_METHOD][METHOD_MAX_LENGTH+1] =
		{"GET","POST","HEAD","PUT","DELETE","OPTIONS"};
int http_method_size[NUM_OF_METHOD] = {3,4,4,3,6,7};

void *th_function(void*tharg)
{
	struct thread_argument *th_arg = (struct thread_argument*)tharg;
	
	int sockfd;
	int childfd;
	int portno = 80;
	char buf[BUFSIZE];
	int n;
	int len = 0;

	struct hostent *server;
	struct sockaddr_in serveraddr;
	
	childfd = th_arg->childfd;

	bzero(buf, BUFSIZE);
	while(len<BUFSIZE)
	{
		n = read(childfd, buf+len, BUFSIZE-len);
		puts(buf+len);
		if(n<0)
		{
			printf("[-]Faild to read\n");
			exit(1);
		}
		if(n==0)
			break;
		len += n;
	}
	
	char hostname[HOSTSIZE]="";
	findhostname(hostname, buf, len);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("[-]Failed to open socket");
		exit(1);
	}

	server = gethostbyname(hostname);
	if(server == NULL)
	{
		printf("[-]Failed to find host\n");
		close(sockfd);
		close(childfd);
		return NULL;
	}

	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))<0)
	{
		printf("[-]Faild to connect socket\n");
		exit(1);
	}

	char dummybuf[BUFSIZE];
	int dummylen = dummyhttp(dummybuf,buf, len);
  for(int i=0;i<dummylen;i++)printf("%c",dummybuf[i]);
	n = write(sockfd, dummybuf, dummylen);
	if(n<0)
	{
		printf("[-]Failed to write to socket\n");
		exit(1);
	}

	while(1)
	{
		bzero(buf, BUFSIZE);
		n = read(sockfd, buf, BUFSIZE);
		puts(buf);
		if(n==0||n<0)
		{
			close(sockfd);
			close(childfd);
			return NULL;
		}
		if(memmem(buf, n, " 404 Not Found\r\n",16)||memmem(buf, n, " 400 Bad Request\r\n",18))continue;

		n = write(childfd, buf, n);
		if(n<0)
		{
			printf("[-]Failed to write to socket\n");
			close(sockfd);
			close(childfd);
			return NULL;
		}
	}
	close(sockfd);
	close(childfd);

	return NULL;
}

void findhostname(char*hostname, char*payload, int size)
{
	for(int i=0;i<NUM_OF_METHOD;i++)
	{
		if(!memcmp(payload, http_method[i], http_method_size[i]))
		{
			char *tmp = (char*)memmem(payload, size, "Host: ", 6);
			if(!tmp)break;
			tmp+=6;
			char *cmp = tmp;
			int size_h=0;
			while(cmp[0]!='\r')
			{
				cmp++;
				size_h++;
			}
			strncpy(hostname, tmp, size_h);
			return;
		}
	}
	return;
}

int dummyhttp(char*dummybuf,char*payload,int size)
{
	const char *dummy = "GET / HTTP/1.1\r\nHost: test.gilgil.com\r\n\r\n";
	memcpy(dummybuf, dummy, strlen(dummy));
	memcpy(dummybuf+strlen(dummy),payload,size);
	return size + strlen(dummy);
}

