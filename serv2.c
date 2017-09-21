#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<errno.h>
#include	<signal.h>
#include	<wait.h>

#define		PORT_NUM	3002	
static int ExecuteSession(int i_sd, int o_sd);
static void dsp_caddr(struct sockaddr_in *adr);
static	char	mes[1024];
static	int	log_flag=1;
static  int	sockfd;
static  int	sd;



void sig_chld(int signo)
{
	pid_t	pid;
	int	stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0){
		printf("chld %d terminated stat=%d.\n",pid,stat);
	}
}

main(int argc, char **argv)
{
	int			pid;
	int			salen;
	int			addlen;
	int			result;
	struct	sockaddr	salocal;
	struct	sockaddr	sapeer;
	struct	sockaddr_in	*sin;
	int		yes=1;


	//*************************************************Socket Interface
	/*----------------------------------------------socket*/
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror ("socket");
		exit(-1);
	}
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	/*----------------------------------------------bind*/
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
	signal(SIGCHLD,sig_chld);
	for(;;) {
		/************************************ACCEPT*/
		sd=accept(sockfd,&sapeer,&addlen);
		if(sd<0){
			close(sockfd);
			exit(-1);
		}
		/****************************Execute Session*/	
		dsp_caddr((struct sockaddr_in *)&sapeer);
		pid=fork();
		if(pid< 0){	//
			perror("fork");	//
			exit(1);	//
		}else if( pid == 0){	//
			close(sockfd);	//
			ExecuteSession(sd,sd);//
			close(sd);	//
			exit(0);	//
		}else{			//
			close(sd);	//
		}			//
	}
}

static int ExecuteSession(int i_sd, int o_sd)
{
	static char	sbuff[65000];
	static char	rbuff[1024];
	int	nbytes;
	
	for(;;){
		nbytes=read(i_sd,rbuff,sizeof(rbuff)-16);
		if(nbytes<=0)	{
			return 0;
		}
		if(nbytes<0){
			return ;
		}
		rbuff[nbytes]='\0';
		printf("%s Read %d byte\n",rbuff,nbytes);
		sprintf(sbuff,"(%d)DATA->%s",nbytes,rbuff);
		sleep(5);
		write(o_sd,sbuff,strlen(sbuff));
		printf("wrote\n");
	}
	return;
}


static void dsp_caddr(struct sockaddr_in *adr)
{
	unsigned char *p;
	p=(unsigned char *)&adr->sin_addr.s_addr;
	

	printf("Accept from = %02d.%02d.%02d.%02d port=%d\n",
	 p[0],p[1],p[2],p[3],ntohs(adr->sin_port));
}

