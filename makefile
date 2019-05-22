FLAGS = -Wall -Werror -pedantic 

server: server.o game.o header.h
	gcc -o server server.o game.o -lm

server.o: server.c header.h
	gcc -c server.c

game.o: game.c header.h
	gcc -c game.c

clean:
	rm -f *.o

clear: 
	rm -f *.o
	rm server