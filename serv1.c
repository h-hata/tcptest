#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<stdio.h>
#include	<string.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<errno.h>
#include <stdlib.h>

#define	 PORT_NUM 12345
static int ExecuteSession(int i_sd, int o_sd);
static void dsp_caddr(struct sockaddr_in *adr);
static  int	 sockfd;
static  int	 sd;
void loop_test(int cli_sock,int ctx);

main(int argc, char **argv)
{
	int			 t,ecount=0;
	int			 pid,i,on,ret;
	int			 salen;
	int			 addlen;
	int			 result;
	struct  sockaddr	salocal;
	struct  sockaddr	sapeer;
	struct  sockaddr_in	 *sin;
	struct  hostent	 *hp;


	//*************************************************Socket Interface
	/*----------------------------------------------socket*/
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror ("socket");
		exit(-1);
	}
	/*----------------------------------------------bind*/
	/* Enable address reuse */
	on = 1;
	setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	sin = (struct sockaddr_in *) & salocal;
	memset ((char *) sin, '\0', sizeof (salocal));
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(INADDR_ANY);
	sin->sin_port = htons(PORT_NUM);
	result = bind (sockfd, & salocal, sizeof (*sin));
	if (result < 0) {
		(void) perror ("bind");
		close(sockfd);
		exit(-1);
	}
	/*----------------------------------------------bind*/
	if(listen(sockfd,5) < 0){
		(void)perror("listen");
		exit(-1);
	}
	for(i=0;;i++) {
		/************************************ACCEPT*/
		sd=accept(sockfd,&sapeer,&addlen);
				dsp_caddr((struct sockaddr_in *)&sapeer);
		if(sd<0){
			close(sockfd);
			exit(-1);
		}
		/****************************Execute Session*/
		ret=ExecuteSession(sd, sd);
		if(ret==-1){
			printf("こちらから切る\n");
			shutdown(sd,SHUT_WR);
			for(;;){
				char b[8];
				size_t len=read(sd,b,8);
				if(len>0){
					continue;
				}
				printf("先方から確認あり\n");
				break;
			}
		}else{
			printf("先方から切られる\n");
		}
		close(sd);
		sd=0;
	}
}

int ExecuteSession(int i_sd, int o_sd)
{
	static char	 sbuff[64*1024*1024];
	static char	 rbuff[64*1024*1024];
	size_t nbytes;

	for(;;){
		nbytes=read(i_sd,rbuff,sizeof(rbuff)-16);
		if(nbytes<=0)   {
			printf("Recv %d\n",nbytes);
			return 0;//相手から切られた
		}
		rbuff[nbytes]='\0';
		sprintf(sbuff,"(%d)DATA->%s",nbytes,rbuff);
		printf("%s\n",sbuff);
		if(*rbuff=='Q'){
			return -1;//自分から切る
		}
		//下のwrite行を前のif文の前に移すとデッドロックする
		write(o_sd,sbuff,strlen(sbuff));
	}
	return 0;
}

static void dsp_caddr(struct sockaddr_in *adr)
{
	unsigned char *p;
	p=(unsigned char *)&adr->sin_addr.s_addr;
	printf("Accept from = %d.%d.%d.%d port=%d\n",
	 p[0],p[1],p[2],p[3],ntohs(adr->sin_port));
}
