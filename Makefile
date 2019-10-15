all:clim
clim:clim.c
	gcc -o clim -pthread clim.c
clean:
	rm -fr cli clim serv1 serv2 serv3
