#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>


using namespace std;

bool r = false;
const int rpipe = 0;
const int wpipe = 1;

//struct for redirection
struct redirect
{
    char *file;
    int io;
    int filedes;
    
    //constructor
    redirect(const char*file, const int io, const int filedes)
    {
        this->file = new char[strlen(file) + 1];
        strcpy(this->file, file);
        this->io = io;
        this->filedes = filedes;
    }
    
    //deconstructor
    ~redirect()
    {
        delete[] this->file;
    }
};



//this function gets the user's login and host name(extra credit prompt)
void prompt()
{
	char* log = getlogin();
	char host[500];
	
	if(!getlogin())
	{
		perror("getlogin()");
	}
	
	if(gethostname(host, 500) == -1)
	{
		perror("gethostname");
	}

	cout << log << "@" << host << ":"<< "$ ";
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

void readi(char strg[])
{
    if(!fgets(strg,500,stdin)) return;
    
    int length = strlen(strg);
    
    if(strg[length-1] == '\n') strg[length-1] = '\0';
}

void lengtha(char cntrl[], int &length, int &a)
{
	length = length + 2;
    
	cntrl[length] = '\0';
    
	for (int b = length-1; b > a + 2; --b)
    {
		cntrl[b] = cntrl[b - 2];
    }
	cntrl[a + 2] = ' ';
    
	cntrl[a + 1] = cntrl[a];
    
	cntrl[a] = ' ';
    
	a = a + 2;
}

void lengthb(char cntrl[], int &length, int &a)
{
	length = length + 2;
    
	cntrl[length] = '\0';
    
	for (int b = length-1; b > a + 3; --b)
    {
		cntrl[b] = cntrl[b - 2];
    }
	cntrl[a + 3] = ' ';
    
	cntrl[a + 2] = cntrl[a + 1];
    
	cntrl[a + 1] = cntrl[a];
    
	cntrl[a] = ' ';
    
	a = a + 3;
}

void lengthc(char cntrl[], int &length, int &a)
{
	length = length + 2;
    
	cntrl[length] = '\0';
    
	for (int b = length-1; b > a + 4; --b)
    {
		cntrl[b] = cntrl[b - 2];
    }
	cntrl[a + 4] = ' ';
    
	cntrl[a + 3] = cntrl[a + 2];
    
	cntrl[a + 2] = cntrl[a + 1];
    
	cntrl[a + 1] = cntrl[a];
    
	cntrl[a] = ' ';
    
	a = a + 4;
}

void lengthd(char cntrl[], int &length, int &a)
{
	length = length + 2;
    
	cntrl[length] = '\0';
    
	for (int b = length-1; b > a + 5; --b)
    {
		cntrl[b] = cntrl[b - 2];
    }
	cntrl[a + 5] = ' ';
    
    cntrl[a + 4] = cntrl[a + 3];
    
	cntrl[a + 3] = cntrl[a + 2];
    
	cntrl[a + 2] = cntrl[a + 1];
    
	cntrl[a + 1] = cntrl[a];
    
	cntrl[a] = ' ';
    
	a = a + 5;
}


//this function parses the command string
void parse(string& str, char**& dilem, char*& token)
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

bool execute(char** arg)	
{
	int stat = 0;
	
	if(*arg == NULL) return false;
	
	int pid = fork();
	if(pid == -1)
	{
		perror("fork failed");
		exit(1);
	}	
	else if(pid == 0)
	{
		stat = execvp(arg[0], arg);

		if(stat == -1)
		{
			perror("execvp failed");
			exit(1);
		}
	}

	else if(pid > 0)
	{
		int wait = waitpid(pid, &stat, 0);
		string found = arg[0];

		if(wait == -1)	perror("wait failed");
		
		if(exitshell(arg));
		
		else if( found == "true") return true;
		
		else if( found == "false") return false;

		if(stat > 0) return false;
	}
	return true;
}
int main(int argc, char* argv[])
{
	while(cin.good())
	{
		string input;
		string space = " ",
				andkey = "&&",
				orkey = "||",
				semi = ";",
				hashtag = "#";
		
		//display prompt
		prompt();

		getline(cin, input);
		
		int num1 = input.find(hashtag,0);
		int cnt = 0;

		if(num1 >= 0) input = input.substr(0, num1);

		string::iterator i;

		for(i = input.begin(); i < input.end(); i++,cnt++)
		{
			int andchar = input.find(andkey, cnt);
			int	orchar = input.find(orkey, cnt);
			int semichar = input.find(semi, cnt);

			if(andchar >= 0 && (andchar < orchar || orchar == -1) && (andchar < semichar || semichar == -1))
			{
				input.insert(input.find(andkey,cnt), " ");
				input.insert(input.find(andkey,cnt)+2, " ");

				i = input.begin();
				i = i + input.find(andkey,cnt) + 2;
				cnt = input.find(andkey,cnt) + 2;
			}

			else if(orchar >= 0 && (orchar < semichar || semichar == -1) && (orchar < andchar || andchar == -1))
			{
				input.insert(input.find(orkey,cnt), " ");
				input.insert(input.find(orkey,cnt)+2, " ");

				i = input.begin();
				i = i + input.find(orkey,cnt) + 2;
				cnt = input.find(orkey,cnt) + 2;
			}

			else if(semichar >= 0 && (semichar < andchar || andchar == -1) && (semichar < orchar || orchar == -1))
			{
				input.insert(input.find(semi,cnt), " ");
				input.insert(input.find(semi,cnt)+1, " ");

				i = input.begin();
				i = i + input.find(semi,cnt) + 1;
				cnt = input.find(semi,cnt) + 1;
			}
			
		}
		
		char* chr1 = (char*) input.c_str();
		char* tkn;

		tkn = strtok( chr1, " \t\n");

		while( tkn != NULL)
		{
			string str;
			bool booland = true;
			bool boolor = false;
			char** dilem;
			
			while(tkn != NULL)
			{
				parse(str, dilem, tkn);

				if(*dilem != NULL && booland && !boolor)
				{

					if(exitshell(dilem))
					{
						exit(1);
					}
				}
				if(str == orkey)
				{
					if(boolor);

					else if(!booland)
					{
						booland = true;
						boolor = false;
					}
					else if(execute(dilem) == true)
					{
						if(tkn != NULL)
						{
							boolor = true;
						}
					}

				}
					
				else if( str == andkey)
				{ 	
						
					if(!booland);
						
					else if(boolor)
					{
						boolor = false;
						booland = true;
					}
						
					else if(execute(dilem) == false)
					{
						if(tkn != NULL)
						{
							booland = false;
						}
					}
				}	

				else if(str == 	semi)
				{
					if(booland && !boolor)
					{
						if(execute(dilem));
					}

					booland = true;
					boolor = false;
				}	
				else
				{
					if(execute(dilem));
				}
					
				delete dilem;
			}
			break;
		}

	}

	return 0;
}
