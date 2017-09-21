#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netinet/in.h>
#include        <stdio.h>
#include        <string.h>
#include        <netdb.h>
#include        <unistd.h>
#include        <errno.h>

#define         PORT_NUM        3002
static int ExecuteSession(int i_sd, int o_sd);
static void dsp_caddr(struct sockaddr_in *adr);
static  int     sockfd;
static  int     sd;
main(int argc, char **argv)
{
        int                     t,ecount=0;
        int                     pid;
        int                     salen;
        int                     addlen;
        int                     result;
        struct  sockaddr        salocal;
        struct  sockaddr        sapeer;
        struct  sockaddr_in     *sin;
        struct  hostent         *hp;


        //*************************************************Socket Interface
        /*----------------------------------------------socket*/
        sockfd = socket (AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                perror ("socket");
                exit(-1);
        }
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
        for(;;) {
                /************************************ACCEPT*/
                sd=accept(sockfd,&sapeer,&addlen);
		dsp_caddr((struct sockaddr_in *)&sapeer);
                if(sd<0){
                        close(sockfd);
                        exit(-1);
                }
                /****************************Execute Session*/
                ExecuteSession(sd,sd);
                close(sd);
                sd=0;
        }
}

static int ExecuteSession(int i_sd, int o_sd)
{
        static char     sbuff[65000];
        static char     rbuff[1024];
        int     nbytes;

        for(;;){
                nbytes=read(i_sd,rbuff,sizeof(rbuff)-16);
                if(nbytes<=0)   {
                        return 0;
                }
                if(nbytes<0){
                        return ;
                }
                rbuff[nbytes]='\0';
                sprintf(sbuff,"(%d)DATA->%s",nbytes,rbuff);
                write(o_sd,sbuff,strlen(sbuff));
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
