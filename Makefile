COMPILER = g++
CFLAGS = -ansi -pedantic -Wall -Werror
RESULT = ./bin/rshell
RESULT1 = ./bin/ls
DIRECTORY = ./src/main.cpp
DIRECTORY1 = ./src/ls.cpp
DIRECTORY2 = ./src/mv.cpp
DIRECTORY3 = ./src/rm.cpp
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
mv:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY2) -o $(RESULT2)  

rm:
	mkdir -p ./bin
	$(COMPILER) $(CFLAGS) $(DIRECTORY3) -o $(RESULT3)  

clean:
	rm -rf ./bin

