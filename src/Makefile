CFLAGS:= -Wall

all: wairamonitor

wairamonitor: wairamonitor.o consultadatos.o ngui.o
	$(CXX) -o wairamonitor wairamonitor.o consultadatos.o ngui.o -lcurl -lncursesw -std=c++11 -lpthread
	@- $(RM) *.o

wairavisor.o: wairavisor.cpp
	$(CXX) $(CFLAGS) -c wairamonitor.cpp -std=c++11 -lpthread

consultadatos.o: consultadatos.cpp consultadatos.h json.hpp
	$(CXX) $(CFLAGS) -c consultadatos.cpp -lcurl -std=c++11

ngui.o: ngui.cpp ngui.h
	$(CXX) $(CFLAGS) -c  ngui.cpp -lncursesw -std=c++11


clean:
	$(RM) *.o wairavisor
