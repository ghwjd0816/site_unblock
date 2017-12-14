all : site_unblock

site_unblock : site_unblock.o main.o
	g++ -g -o site_unblock site_unblock.o main.o -pthread

site_unblock.o :
	g++ -g -c -o site_unblock.o site_unblock.cpp

main.o :
	g++ -g -c -o main.o main.cpp

clean :
	rm -f *.o
	rm -f site_unblock
