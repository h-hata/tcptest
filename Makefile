all:clim
clim:clim.c
	gcc -o clim -lpthread clim.c
clean:
	rm -fr cli clim serv1 serv2 serv3
