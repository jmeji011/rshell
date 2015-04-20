#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

void prompt()
{
	char* log = getlogin();
	
	if(!getlogin())
	{
		perror("getlogin()");
	}

	char host[BUFSIZ];
	
	if(gethostname(host, BUFSIZ) == -1)
	{
		perror("gethostname()");
	}

	cout << log;
	cout << "@" ;
	cout << host;
	cout << ":";
	cout << "$ ";
}

int main(int argc, char* argv[])
{
	prompt();


	return 0;
}
