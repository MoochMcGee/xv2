CXX=g++
CFLAGS=-std=c++17 -g
LIBS=
OBJ = xv2.o \
vecstream.o \
strformat.o

all: $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o xv2 $(LIBS)

clean:
	rm *.o && rm xv2

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

%.o: %.cc
	$(CXX) $(CFLAGS) -c $<