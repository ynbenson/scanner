OBJECTS = main.o scanner.o
	#(listing all of my object files here, separated by space)
HEADERS = scanner.hpp
	#(listing all header files here, separated by spaces)

scan: $(OBJECTS)
	g++ $^ -o scan

%.o: %.cpp $(HEADERS)
	g++ -c $< -o $@

clean:
	rm -f *.o scan


