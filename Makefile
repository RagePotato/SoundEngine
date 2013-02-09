CC=g++
CFLAGS=-c -g3 -ggdb -Wall
LFLAGS=-lsndfile -lportaudio -lstdc++

all: audiotest

audiotest: portaudiotest.o
	$(CC) portaudiotest.o -o audiotest $(LFLAGS)

portaudiotest.o: portaudiotest.cpp
	$(CC) $(CFLAGS) portaudiotest.cpp

clean:
	rm -rf *o audiotest