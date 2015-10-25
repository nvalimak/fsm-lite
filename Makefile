SDSL_INSTALL_PREFIX=/Users/nvalimak/code/sdsl-lite-2.0.3

CC=g++
CPPFLAGS=-std=c++11 -I${SDSL_INSTALL_PREFIX}/include 
DISBALEDOPTIMIZATIONFLAGS = -DNDEBUG -O3 [-msse4.2]
LIBS=-lsdsl -ldivsufsort -ldivsufsort64
OBJ = configuration.o input_reader.o 

fsm-lite: fsm-lite.o $(OBJ)
	$(CC) $(CPPFLAGS) -L${SDSL_INSTALL_PREFIX}/lib -o fsm-lite fsm-lite.o $(OBJ) $(LIBS)

test: fsm-lite
	./fsm-lite -l test.list -t tmp -v --debug -m 1

clean:
	rm -f fsm-lite *.o *~

depend:
	g++ -MM -std=c++11 -I${SDSL_INSTALL_PREFIX}/include *.cpp > dependencies.mk

include dependencies.mk
