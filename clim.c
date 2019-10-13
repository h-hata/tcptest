#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
static ss;
static void *rr(void *);
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
	pthread_attr_t        attr;
	pthread_t	tid;

	if(argc!=3){
		printf("%s addr port\n",argv[0]);
		_exit(1);
	}
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	s = socket(AF_INET,SOCK_STREAM,0);
       /*Convert HOST to IP Address**********************************/
        /*peerIPが１０進ドット*/
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
	tv.tv_sec=10;
	tv.tv_usec=0;
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	for(;;){
		printf("Key enter:\n");
		len=read(0, buff,2048);
		buff[len]='\0';
		len=strlen(buff);
		if(len){
			send(s,buff,len,0);
		}
		ss=s;

		pthread_create(&tid,NULL,rr,NULL);
		//pthread_create(&tid,&attr,rr,NULL);
		printf("tid1=%X\n",tid);
		sleep(3);
		break;
	}
	printf("close\n");
	shutdown(s,SHUT_RD);
	sleep(5);
}

void *rr(void *p){
	char			recb[2048];
	int len;
	pthread_t tid;
	tid=pthread_self();
	printf("tid=%X\n",tid);
	pthread_detach(tid);
	len = recv(ss,recb,2048,0);
	printf("recv=%d\n",len);
	if(len){
		recb[len]='\0';
		printf("%s\n",recb);	
	}	
	return NULL;
}

