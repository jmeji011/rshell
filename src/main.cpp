#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

//this function gets the user's login and host name(extra credit prompt)
void prompt()
{
	char* log = getlogin();
	char host[BUFSIZ];
	
	if(!getlogin())
	{
		perror("getlogin()");
	}
	
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

bool exitshell(char** close)
{
	string abort = *close;
	string e("exit");
	
	if(abort == e)
	{
		return true;
	}
	else return false;
}

int main(int argc, char* argv[])
{
	prompt();


	return 0;
}
