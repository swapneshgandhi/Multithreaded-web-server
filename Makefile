default: myhttpd

myhttpd: all.o excecute.o que_func.o serv_th.o server_in.o schd_th.o
	gcc -o myhttpd all.o excecute.o que_func.o serv_th.o server_in.o schd_th.o -lpthread -g
	
all.o: all.c excecute.c excecute.h que_func.c que_func.h queue.h serv_th.c serv_th.h th.h server_in.c server_in.h schd_th.c schd_th.h
	gcc -c all.c -lpthread

excecute.o: excecute.c excecute.h queue.h
	gcc -c excecute.c
	
que_func.o: que_func.c que_func.h queue.h excecute.c excecute.h
	gcc -c que_func.c

serv_th.o: serv_th.c th.h serv_th.h
	gcc -c serv_th.c

server_in.o: server_in.c server_in.h
	gcc -c server_in.c
	
schd_th.o:schd_th.c schd_th.h th.h que_func.h
	gcc -c schd_th.c
	
