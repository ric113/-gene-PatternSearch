CC = g++

all:main

hw1:main.cpp
	$(CC) main.cpp -o main

clean:
	rm main
