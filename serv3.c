#include	<stdio.h>
#include	<string.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<signal.h>


static int ExecuteSession(int i_sd, int o_sd);

main(int argc, char **argv)
{
	ExecuteSession(0,1);
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
		rbuff[nbytes]='\0';
		if(*rbuff=='Q') return;
		sprintf(sbuff,"(%d)DATA->%s",nbytes,rbuff);
		write(o_sd,sbuff,strlen(sbuff));
	}
	return;
}
