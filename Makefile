all: rshell

rshell:
		mkdir -p ./bin
		g++ -Wall -Werror -g -std=c++11 ./src/rshell.cpp -o ./bin/rshell
		g++ -Wall -Werror -g -std=c++11 ./src/ls.cpp -o ./bin/ls
		g++ -Wall -Werror -g -std=c++11 ./src/mv.cpp -o ./bin/mv
		g++ -Wall -Werror -g -std=c++11 ./src/rm.cpp -o ./bin/rm
