CXX      = g++
CXXFLAGS = -O0 -g3 -std=c++17

all: GenAlloc

GenAlloc: main.o allocation.o
	$(CXX) $(CXXFLAGS) -o GenAlloc main.o allocation.o

main.o: main.cpp allocation.h
	$(CXX) $(CXXFLAGS) -c main.cpp

allocation.o: allocation.cpp allocation.h
	$(CXX) $(CXXFLAGS) -c allocation.cpp


# CheckAlloc

check: CheckAlloc

CheckAlloc: CheckAlloc.o
	$(CXX) $(CXXFLAGS) -o CheckAlloc CheckAlloc.o

CheckAlloc.o: CheckAlloc.cpp
	$(CXX) $(CXXFLAGS) -c CheckAlloc.cpp

clean:
	rm -f *~ *.o *.stackdump GenAlloc CheckAlloc

