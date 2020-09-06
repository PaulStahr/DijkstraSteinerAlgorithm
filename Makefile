CFLAGS= -Wall -Wextra -pedantic -g -O2 -fopenmp -std=c++14
CFLAGS += -DNDEBUG

SRC := source
BUILT := built

# -lSDL2main -lSDL2_image -lSDL2
$(BUILT)/util.o: $(SRC)/util.cpp $(SRC)/util.h
	g++ -c $(SRC)/util.cpp $(CFLAGS) -o $(BUILT)/util.o

$(BUILT)/instance_io.o: $(SRC)/instance_io.cpp $(SRC)/instance_io.h
	g++ -c $(SRC)/instance_io.cpp $(CFLAGS) -o $(BUILT)/instance_io.o

$(BUILT)/dijkstra_steiner.o: $(SRC)/bitset_map.h $(SRC)/heap.h $(SRC)/dijkstra_steiner.cpp
	g++ -c $(SRC)/dijkstra_steiner.cpp $(CFLAGS) -o $(BUILT)/dijkstra_steiner.o

$(BUILT)/main.o: $(SRC)/main.cpp
	g++ -c $(SRC)/main.cpp $(CFLAGS) -o $(BUILT)/main.o

$(BUILT)/application_window.o: $(SRC)/application_window.cpp
	g++ -c $(SRC)/application_window.cpp -o $(BUILT)/application_window.o $(CFLAGS) -lGL -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/SDL -lSDL

$(BUILT)/screensaver.o:$(SRC)/screensaver.cpp
	g++ -c $(SRC)/screensaver.cpp -o $(BUILT)/screensaver.o $(CFLAGS) -lGL -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/SDL -lSDL

bin: $(BUILT)/dijkstra_steiner.o $(BUILT)/instance_io.o $(BUILT)/util.o $(BUILT)/main.o
	g++ $(BUILT)/dijkstra_steiner.o $(BUILT)/instance_io.o $(BUILT)/util.o $(BUILT)/main.o $(CFLAGS) -o bin

screensaver: $(BUILT)/screensaver.o $(BUILT)/application_window.o $(BUILT)/dijkstra_steiner.o $(BUILT)/instance_io.o $(BUILT)/util.o
	g++ $(BUILT)/dijkstra_steiner.o $(BUILT)/application_window.o $(BUILT)/instance_io.o $(BUILT)/util.o $(BUILT)/screensaver.o -o screensaver $(CFLAGS) -lGL -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/SDL -lSDL
	#$(pkg-config --cflags --libs sdl)

test: bin
	for file in ./instances/*; do echo -n "$(basename $${file}) "; ./bin $${file}; done

all: bin screensaver

clean:
	rm -f $(BUILT)/util.o
	rm -f $(BUILT)/instance_io.o
	rm -f $(BUILT)/dijkstra_steiner.o
	rm -f $(BUILT)/screensaver.o
	rm -f $(BUILT)/main.o
	rm -f $(BUILT)/application_window.o
	rm -f bin
	rm -f screensaver
