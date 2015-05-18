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

//these four functions depend on how many '<' or '>' there are
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

void redctrl(const vector<struct redirect*> &file)
{
	if(file.size() == 0) return;
    
	for(unsigned i = 0; i < file.size(); ++i)
	{
		int filedes;
        
		int filedes1[2];
        
		int lengtha = strlen(file.at(i)->file);
        
		
		char temp[500];
        
		switch(file.at(i)->io)
		{
			case 0: 
				filedes = open(file.at(i)->file, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
                
				if(filedes == -1)
				{
					perror("open()0");
					exit(1);
				}
                
				if(dup2(filedes, file.at(i)->filedes) == -1)
				{
					perror("dup2()0");
					exit(1);
				}
				break;
                
			case 1: 
            
				filedes = open(file.at(i)->file, O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
                
				if(filedes == -1)
				{
					perror("open()1");
					exit(1);
				}
                
				if(dup2(filedes, file.at(i)->filedes) == -1)
				{
					perror("dup2()1");
					exit(1);
				}
				break;
                
			case 2: 
            
				filedes = open(file.at(i)->file, O_RDONLY);
                
				if(filedes == -1)
				{
					perror("open()2");
					exit(1);
				}
                
				if(dup2(filedes, file.at(i)->filedes) == -1)
				{
					perror("dup2()2");
				}
                
				break;
                
			case 3: 
				strcpy(temp, file.at(i)->file);
                
				strcat(temp, "\n");
                
				if(-1 == pipe(filedes1))
				{
					perror("pipe()");
					exit(1);
				}
                
				if(write(filedes1[wpipe], temp, lengtha + 1) == -1)
				{
					perror("write()");
					exit(1);
				}
                
				if(dup2(filedes1[rpipe], 0) == -1)
				{
					perror("dup2()");
					exit(1);
				}
                
				if(close(filedes1[wpipe]) == -1)
				{
					perror("close()");
					exit(1);
				}
				break;
		}
	}
}


//determines how many '<' or '>' there are
void org(char cntrl[], int &f)
{
    char ink = '<',
           outk = '>';
	f = 0;
    
	string temp;
    
	int length = strlen(cntrl);
    
	for (int i = 0; i < length; ++i)
	{
		if (cntrl[i] == '#')
		{
			cntrl[i] = '\0';
            
			break;
		}
	}
    
	length = strlen(cntrl);
    
	for (int i = 0; i < length; ++i)
	{
		if (cntrl[i] == '\'' || cntrl[i] == '\"')
		{
			for (int j = i; j < length; ++j)
			{
				cntrl[j] = cntrl[j+1];
			}
			--length;
			--i;
		}
	}
    
	length = strlen(cntrl);
    
	for (int i = 0; i < length; ++i)
	{
		if (cntrl[i] == '\t')
		{
			cntrl[i] = ' ';
		}
	}
	
	for (int i = 0; i < length; ++i)
	{
		if (i == 0)
		{
			if (isdigit(cntrl[i]))
			{
				if (cntrl[i+1] == ink && cntrl[i+2] != ink)
                {
					lengthb(cntrl, length, i);
                }
				else if (cntrl[i+1] == ink && cntrl[i+2] == ink && cntrl[i+3] == ink)
                {
					lengthd(cntrl ,length, i);
                }
				else if (cntrl[i+1] == outk && cntrl[i+2] != outk)
                {
					lengthb(cntrl, length, i);
                }
				else if (cntrl[i+1] == outk && cntrl[i+2] == outk)
                {
					lengthc(cntrl, length, i);
                }
			}
			else if (cntrl[i] == ink && cntrl[i+1] != ink)
            {
				lengtha(cntrl, length, i);
            }
			else if (cntrl[i] == ink && cntrl[i+1] == ink && cntrl[i+2] == ink)
            {
				lengthc(cntrl, length, i);
            }
			else if (cntrl[i] == outk && cntrl[i+1] != outk)
            {
				lengtha(cntrl, length, i);
            }
			else if (cntrl[i] == outk && cntrl[i+1] == outk)
            {
				lengthb(cntrl, length, i);
            }
			else if (cntrl[i] == '|')
            {
				lengtha(cntrl, length, i);
            }

		}
		else
		{
			if (isdigit(cntrl[i]) && cntrl[i-1] == ' ')
			{
				if (cntrl[i+1] == ink && cntrl[i+2] != ink)
                {
					lengthb(cntrl, length, i);
                }
				else if (cntrl[i+1] == ink && cntrl[i+2] == ink && cntrl[i+3] == ink)
                {
					lengthd(cntrl, length, i);
                }
				else if (cntrl[i+1] == outk && cntrl[i+2] != outk)
                {
					lengthb(cntrl, length, i);
                }
				else if (cntrl[i+1] == outk && cntrl[i+2] == outk)
                {
					lengthc(cntrl, length, i);
                }
			}
			else if (cntrl[i] == ink && cntrl[i+1] != ink)
            {
				lengtha(cntrl, length, i);
            }
			else if (cntrl[i] == ink && cntrl[i+1] == ink && cntrl[i+2] == ink)
            {
				lengthc(cntrl, length, i);
            }
			else if (cntrl[i] == outk && cntrl[i+1] != outk)
            {
				lengtha(cntrl, length, i);
            }
			else if (cntrl[i] == outk && cntrl[i+1] == outk)
            {
				lengthb(cntrl, length, i);
            }
			else if (cntrl[i] == '|')
            {
				lengtha(cntrl, length, i);
            }
	
		}
	}
	
	
	for (int i = 0; i < length; ++i)
	{
		if (cntrl[i] == ' ')
		{
			if (cntrl[i+1] == ' ')
			{
				for (int j = i; j < length; ++j)
				{
					cntrl[j] = cntrl[j+1];
				}
				--i;
				--length;
			}
		}
	}

	length = strlen(cntrl);
    
	while (cntrl[0] == ' ')
	{
		for (int i = 0; i < length; ++i)
		{
			cntrl[i] = cntrl[i+1];
		}
	}

	length = strlen(cntrl);
    
	while (cntrl[length-1] == ' ')
	{
		cntrl[length-1] = '\0';
		-- length;
	}

	for (int i = 0; cntrl[i+2] != '\0'; ++i)
	{
		if(cntrl[i] == '|' && cntrl[i+2] == '|')
		{
			f = 1;
		}
	}
}

bool numctrl(string s, int &io)
{
	if (isdigit(s.at(0)))
	{
		s.erase(s.begin());
        
		if(s == ">")
        {
			io = 0;
			return true;
		}
		
        else if(s == "<<<")
        {
			io = 3;
			return true;
		}
		else if(s == "<")
        {
			io = 2;
			return true;
		}
        else if(s == ">>")
        {
			io = 1;
			return true;
		}
		
	}
	return false;
}

void parse (char *comm[], char cntrl[], vector<struct redirect*> &file, int &f)
{
	f = 0;
    char * get[500];
	struct redirect* temp = 0;
	vector<string> s;
	char * tk, *ptr;
	string tempa;
    
	tk = strtok_r(cntrl, " ", &ptr);
    
	int c = 0;
    
	get[c] = tk;
    
	tempa = tk;

	while (tk != NULL)
	{
		++c;
		tempa = tk; 
        
		s.push_back(tempa);
        
		tk = strtok_r(NULL, " ", &ptr);
        
		get[c] = tk;
	}

	int j = 0;
	int filedes;
	int io;
    
	for (unsigned i = 0; i < s.size()-1; ++i)
	{
		if((s.at(i+1)== "<" || s.at(i+1) == "<<<" || s.at(i+1) == ">" || s.at(i+1) == ">>" || numctrl(s.at(i+1), io)) && (s.at(i)== "<" || s.at(i)=="<<<" || s.at(i)== ">" ||s.at(i) == ">>" || numctrl(s.at(i), io))) f= 1;
	}

	for(unsigned i = 0; i < s.size(); ++i)
	{	
			if(s.at(i) == ">") 
			{
				temp = new struct redirect(get[i+1], 0, 1);
                
				file.push_back(temp);
                
				++i;
			}
            
            else if(s.at(i) == "<") 
			{
			
				temp = new struct redirect(get[i+1], 2, 0);
				file.push_back(temp);
				++i;
			}
            
			else if(s.at(i) == ">>") 
			{
				
				temp = new struct redirect(get[i+1], 1, 1);
                
				file.push_back(temp);
                
				++i;
			}
			
            else if(numctrl(s.at(i), io))
			{
				filedes = s.at(i)[0] - '0';
                
				temp = new struct redirect(get[i+1], io, filedes);
                
				file.push_back(temp);
				++i;
			}
            
			else if(s.at(i) == "<<<") 
			{

				temp = new struct redirect(get[i+1], 3, 0);
                
				file.push_back(temp);
				++i;
			}
        
			else
			{
				comm[j] = get[i];
				++j;
			}

	}
}

void exec(char cntrl[], int save_stdin = -1)
{
	
	char *left;
    char *right;
    char *comm[500] = {0};
    int f = 0;
	unsigned i = 0;
	
	for (i = 0; i < strlen(cntrl); ++i)
	{
		if (cntrl[i] == '|' && i != 0 && i != strlen(cntrl)-1)
		{
			left = cntrl; 
            
			cntrl[i] = '\0';
             
			right = cntrl + i + 1;

			int filedes[2];
            
			if(-1 == pipe(filedes))
			{
				perror("pipe()");
				exit(1);
			}
            
			pid_t pid;
            
			if(-1 == (pid = fork()))
			{
				perror("fork()");
				exit(1);
			}
            
			else if(pid == 0) 
			{
				if(-1 == dup2(filedes[wpipe], 1))
				{
					perror("dup2()");
					exit(1);
				}	
                
				if(-1 == close(filedes[rpipe]))
				{
					perror("close()");
					exit(1);
				}
                
				exec(left, save_stdin);
                
				exit(0);
			}
            
			else 
			{
				
				if (save_stdin == -1)
				{
					if(-1 == (save_stdin = dup(0)))
					{
						perror("dup()");
						exit(1);
					}
				}	
				
				if(close(filedes[wpipe]) == -1)
				{
					perror("close()");
					exit(1);
				}
				
				if(dup2(filedes[rpipe], 0) == -1)
				{
					perror("dup2()");
					exit(1);
				}
				exec(right, save_stdin);
                
				if(dup2(save_stdin, 0) == -1)
				{
					perror("dup2()");
					exit(1);
				}
				
				if(close(filedes[rpipe]) == -1)
				{
					perror("close()");
					exit(1);
				}
				
				if(wait(0) == -1)
				{
					perror("wait()");
					exit(1);
				}
				return;
			}
		}
	}
	vector<struct redirect*> files;
    
	parse(comm, cntrl, files, f);
	if(f == 1)
	{
		cerr << "Syntax error." << endl;
		return;
	}
	pid_t pid;
	int status;
	if ((pid = fork()) < 0)
	{
		perror("fork failed\n");
		r = false;
		exit(1);
	}
	else if (pid == 0)
	{
		redctrl(files);
		
		if (execvp(*comm, comm) < 0)
		{
			perror ("exec failed\n");
			r = false;
			exit(1);
		}
	}
	else
	{
		if (-1 == waitpid(pid, &status, 0))
		{
			perror("wait error\n");
			r = false;
			exit(1);
		}
		if (WEXITSTATUS(status) == 0)
		{
			r = true;
		}
		else if (WEXITSTATUS(status) == 1)
		{
			r = false;
		}
	}
	return;
}

/*
//this function parses the command string
void parse2(string& str, char**& dilem, char*& token)
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
*/
int main(int argc, char**argv)
{
    vector<struct redirect*> file;
	int f = 0;
	while(1)
	{
        prompt();
        /*
        		string input;
		string space = " ",
				andkey = "&&",
				orkey = "||",
				semi = ";",
				hashtag = "#";
		
		//display prompt
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
				parse2(str, dilem, tkn);

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
		}*/
        char ctrl[500];
        
		memset(ctrl, '\0', 500);

		readi(ctrl);
		org(ctrl, f);
		
		if(f == 1)
		{
			cerr << "Syntax Error." << endl;
			continue;
		}

		if (ctrl[strlen(ctrl)-1] == '>' || ctrl[strlen(ctrl)-1] == '<')
		{
			cerr << "Invalid number of arguments. " << endl;
			continue;
		}
        
		if (ctrl[0] == '\0') continue;
            
		if (strncmp(ctrl, "exit", 4) == 0)
		{
			if (ctrl[4] == '\0' || ctrl[4] == ' ')
			{
				cout << "End of program." << endl;
				exit(0);
			}
		}
		exec(ctrl);
        
		int length = file.size();
        
		for (int i = 0; i < length; ++i)
		{
			if (file.at(i) != 0) 
            {
				delete file.at(i);
				file.at(i) = 0;
			}
		}
        
		file.clear();
	}
    
    return 0;
}

