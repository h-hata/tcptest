#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
static void *rr(void *);
static int flag;
static int s;
void handler(int sig) {
	printf("Handler0\n");
	flag = 0;
}
main(int argc,char **argv)
{
	struct hostent  	*hent;
	struct sockaddr_in      p_addr;
	int 			len;
	int				ret,rc;
	char			buff[2048];
	char			recb[2048];
	ssize_t		sret;
	struct timeval tv;
	pthread_attr_t        attr;
	pthread_t	tid;
	struct sigaction act;

	if(argc!=3){
		printf("%s addr port\n",argv[0]);
		_exit(1);
	}
	/* シグナル設定 */
	memset(&act, 0, sizeof(act));
	act.sa_handler = handler; /* 関数ポインタを指定する */
  //act.sa_flags = SA_RESTART;
  act.sa_flags = 0;
	/* SIGINTにシグナルハンドラを設定する */
	rc = sigaction(SIGUSR1, &act, NULL);
	if(rc < 0){
		printf("Error: sigaction() %s\n", strerror(errno));
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
	flag=1;
	pthread_create(&tid,NULL,rr,NULL);
	for(;;){
		if(flag==0){
			break;
		}
		printf("Key enter:\n");
		len=read(0, buff,2048);
		if(len==1){
			printf("自分から切る\n");
			shutdown(s,SHUT_WR);
			for(;flag;){
				sleep(1);
			}
			printf("相手から確認あり");
			break;
		}else if(len){
			buff[len]='\0';
			len=strlen(buff);
			sret=send(s,buff,len,0);
			if(sret<0){
				printf("送信失敗した\n");
				shutdown(s,SHUT_WR);
				break;
			}
		}else{
			printf("read error\n");
		}
	}
	printf("close\n");
	close(s);
}

void *rr(void *p){
	char			recb[2048];
	int len;
	pthread_t tid;
	tid=pthread_self();
	pthread_detach(tid);
	for(;;){
		len = recv(s,recb,2048,0);
		printf("recv=%d\n",len);
		if(len<=0){
			printf("相手から切断された");
			close(0);
			break;
		}
		if(len){
			recb[len]='\0';
			printf("%s\n",recb);	
		}	
	}	
	flag=0;
	pid_t pid=getpid();
	//kill(pid,SIGUSR1);
	return NULL;
}

