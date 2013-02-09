CC=g++
CFLAGS=-c -g3 -ggdb -Wall -std=c++11
LFLAGS=-lsndfile -lportaudio -lstdc++

all: audiotest

audiotest: main.o pa_callback.o
	$(CC) main.o pa_callback.o -o audiotest $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

pa_callback.o: pa_callback.cpp
	$(CC) $(CFLAGS) pa_callback.cpp

clean:
	rm -rf *o audiotest