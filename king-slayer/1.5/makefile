all:
	g++ -Wall --std=c++11 -lpthread -o launch launch.cpp
	g++ -Wall --std=c++11 -lpthread -o server server.cpp
	gcc -Wall -o client client.c -lpthread
clean:
	rm -f launch server client
