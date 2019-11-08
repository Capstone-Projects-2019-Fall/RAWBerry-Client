SRCS = ./main.cpp

run: *.cpp ./src* 
	skill testclient
	g++ -o testclient $(SRCS)
	./testclient
