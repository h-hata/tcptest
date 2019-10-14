#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

main(int argc,char **argv)
{
	int 			s;
	struct hostent  	*hent;
	struct sockaddr_in      p_addr;
	int 			len;
	int			ret;
	char			buff[2048];
	char			recb[2048];
	struct timeval tv;

	if(argc!=3){
		printf("%s addr port\n",argv[0]);
		_exit(1);
	}
	s = socket(AF_INET,SOCK_STREAM,0);
  p_addr.sin_addr.s_addr=inet_addr(argv[1]);
	if(p_addr.sin_addr.s_addr == INADDR_NONE){
  	hent=gethostbyname(argv[1]);
 		if(hent==NULL){
 	 		close(s);
   		return -1;
		}
		p_addr.sin_addr.s_addr=*(unsigned long *)hent->h_addr_list[0];
  }
	p_addr.sin_family = AF_INET;
	p_addr.sin_port=htons(atoi(argv[2]));
	printf("Trying...");
	ret=connect(s,(struct sockaddr *)&p_addr,sizeof(p_addr));
	if(ret==-1){
		printf("connect error\n");
		exit(-1);
	}
	printf("connected \n");
	tv.tv_sec=1;
	tv.tv_usec=0;
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	for(;;){
		len=read(0, buff,2048);
		buff[len]='\0';
		len=strlen(buff);
		if(len==1){
			printf("自分から切る\n");
			shutdown(s,SHUT_WR);
			for(;;){
				char b[8];
				len = recv(s,b,8,0);
				if(len<=0){
					break;
				}
			}
			printf("相手から確認あり\n");
			break;
		}
		if(len){
			send(s,buff,len,0);
			len = recv(s,recb,2048,0);
			printf("recv=%d\n",len);
			if(len<=0){
				printf("相手から切られた\n");
				shutdown(s,SHUT_WR);
				break;
			}
			if(len){
				recb[len]='\0';
				printf("%s\n",recb);	
			}	
		}
	}
	close(s);
	printf("close\n");
}
