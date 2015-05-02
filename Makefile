COMPILER = g++
CFLAGS = -ansi -pedantic -Wall -Werror
RESULT = ./bin/rshell
RESULT1 = ./bin/ls
DIRECTORY = ./src/main.cpp
DIRECTORY1 = ./src/ls.cpp
all: 
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY) -o $(RESULT)
	$(COMPILER) $(CFLAGS) $(DIRECTORY1) -o $(RESULT1)

rshell: 
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY) -o $(RESULT)  
ls:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY1) -o $(RESULT1)

clean:
	rm -rf ./bin

