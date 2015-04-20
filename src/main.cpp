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

//this function exits the program if "exit" is typed.
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

//this function parses the command string
void parse(string& str, char** dilem, char*& token)
{
	char** dilem1 = (char**) malloc (BUFSIZ);
	bool tokenbool = false;
	int cntr = 0;
	string space = " ",
			andkey = "&&",
			orkey = "||",
			semi = ";";
	string input;
	size_t sz1 = -1;
	
	while(token != NULL && !tokenbool)
	{
		string t = token;
		
		if((t.find(andkey) != sz1) || (t.find(orkey) != sz1) || (t.find(semi) != sz1)) tokenbool = true;
		
		if(!tokenbool) dilem1[cntr] = token;

		else
		{
			dilem1[cntr] = NULL;
			str = t;		
		}
		
		cntr++;
		token = strtok(NULL, " "); 
	}

	if(!tokenbool) dilem1[cntr] = NULL;
	
	dilem = dilem1;
	
}

int main(int argc, char* argv[])
{
	prompt();


	return 0;
}
