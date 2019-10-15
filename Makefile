all:cli clim serv1 serv2 serv3
cli:cli.c
	gcc -o cli cli.c
clim:clim.c
	gcc -o clim -pthread clim.c
serv1:serv1.c
	gcc -o serv1 serv1.c
serv2:serv2.c
	gcc -o serv2 serv2.c
serv3:serv3.c
	gcc -o serv3 serv3.c
clean:
	rm -fr cli clim serv1 serv2 serv3
