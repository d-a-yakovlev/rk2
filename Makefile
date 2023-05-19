all: ping_pong launch

ping_pong:
	g++ -pthread app.cpp 

launch:
	./a.out