SRCS = ./src/client.cpp

run: *.cpp ./src* 
	skill testclient
	g++ -o testclient -I ./src -I . *.cpp $(SRCS)
	./testclient
