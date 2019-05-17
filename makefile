FLAGS = -Wall -Werror -pedantic 

server: server.o game.o
	gcc -o server server.o game.o -lm

server.o: server.c
	gcc -c server.c

game.o: game.c
	gcc -c game.c

clean:
	rm -f *.o

clear: 
	rm -f *.o
	rm server