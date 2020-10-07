CFLAGS:= -Wall

all: wairamonitor

wairamonitor: wairamonitor.o consultadatos.o ngui.o lectura_red.o
	$(CXX) -o wairamonitor wairamonitor.o consultadatos.o ngui.o lectura_red.o -lcurl -lncursesw -std=c++11 -lpthread
	@- $(RM) *.o

wairamonitor.o: ./src/wairamonitor.cpp
	$(CXX) $(CFLAGS) -c ./src/wairamonitor.cpp -std=c++11 -lpthread

consultadatos.o: ./src/consultadatos.cpp ./src/consultadatos.h ./src/json.hpp
	$(CXX) $(CFLAGS) -c ./src/consultadatos.cpp -lcurl -std=c++11

ngui.o: ./src/ngui.cpp ./src/ngui.h
	$(CXX) $(CFLAGS) -c ./src/ngui.cpp -lncursesw -std=c++11
	
lectura_red.o: ./src/lectura_red.cpp ./src/lectura_red.h
	$(CXX) $(CFLAGS) -c ./src/lectura_red.cpp -std=c++11


clean:
	$(RM) *.o wairavisor
