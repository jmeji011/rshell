#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <stack>
#include <fstream>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
//updated rshell code

using namespace std; 
using namespace boost;
unsigned const nrd = 0, inprd = 1, strrd = 2, outrd = 3, apprd = 4, fdrd =5, fdard =6, pip = 7, npip = 7, rderr = 9, rpipe = 0, wpipe = 1;
int const maxpipes = 40, nomatch = -2,  arrf = 2 ;
string const nofile = "NO FILE";
const char* hd = "HOME";
const char* owd = "OLDPWD";
const char* cwd = "PWD";

stack<int> cp;
int pid = 0, cpid = 0;
siginfo_t *i;

void home(string &s) {
    
    char* homedir;
    
    if ((homedir = getenv(hd)) == NULL) 
    {
        perror("secure_getenv():");
    }
    string h = homedir;
    
    s.replace(0, h.length(), "~");
}

void pcwd() {
    
    char cwd[BUFSIZ];
    int s;
    
    if ((s = pathconf(".", _PC_PATH_MAX)) == -1) 
    {
        perror("pathconf():");
    }
    
    if (getcwd(cwd, s) == NULL) 
    {
        perror("getcwd():");
    }
    
    string cwd1 = cwd;
    home(cwd1);
    cout << cwd1 << "$ ";
}

void prompt() {
    struct passwd *user;
    user = getpwuid(getuid());
    if (user->pw_name == NULL) 
    {
        perror("getpquid()");
    }
    char host[500];
    host[500] = '\0';
    
    if (gethostname(host, 499) == -1) 
    {
        perror("gethostname():");
    }
    cout << user->pw_name << "@" << host << ":";
    pcwd();
}

void parseq(queue<string> &l, const string &s) 
{
    char_separator<char> delim(" ", "\";<>&|#") ; // using <boost/tokenizer.hpp>
    tokenizer<char_separator<char>> mytok(s, delim); //using <boost/tokenizer.hpp>

    for (auto j = mytok.begin(); j != mytok.end(); ++j) //using <boost/tokenizer.hpp>
    {
        l.push(*j);
    }
}

void popq(queue<string> &qu) 
{
    while(!qu.empty()) 
    {
        qu.pop();
    }
}

unsigned set_rd(queue<string> qu) 
{
    unsigned char_count = 0;
    while(!qu.empty() && qu.front() == "<") 
    {
        ++char_count;
        qu.pop();
    }
    
    if (char_count == 1) 
    { 
        return inprd;
    }
    
    else if (char_count == 3) 
    { 
        
        return strrd;
    }
    
    else if (char_count != 0) 
    {
        return rderr;
    }
    
    while (!qu.empty() && qu.front() == ">") 
    {
        ++char_count;
        qu.pop();
    }
    
    if (char_count == 1) 
    { 
        return outrd;
    }
    
    else if (char_count == 2) 
    { 
        return apprd;
    }
    
    else if (char_count != 0) 
    {
        return rderr;
    }
    
    if (!qu.empty() && qu.front() == "|") 
    {
        qu.pop();
        return pip;
    }
    
    return nrd;
}

string mq(int pops, queue<string> &q);
int matq(queue<string> q);

bool get(queue<string> &q, string &fn) 
{
    if (q.empty()) 
    {
        return false;
    }
    
    fn = q.front();
    if (fn == "\"") 
    {
        int pops = matq(q);
            if (pops == nomatch) 
            {
                return nomatch;
            }
        string s = mq(pops, q);
        fn = s;
        
        return true;
    }
    
    q.pop();
    return true;
}

bool poptrue(queue<string> &qu, const unsigned rd, string &fn)
{
    if (rd == nrd)
    {
        return true;
    }
    else if (rd == inprd || rd == outrd || rd == pip) 
    {
        qu.pop();
        if (rd != pip) 
        {
            return (get(qu, fn));
        }
        return true;
    }
    else if (rd == strrd) 
    {
        qu.pop();
        qu.pop();
        qu.pop();
        return (get(qu, fn));
    }
    else if (rd == apprd) 
    {
        qu.pop();
        qu.pop();
        return (get(qu, fn));
    }
    return false;
}


bool cpipe(queue<string> qu) {
    if (!qu.empty() && qu.front() == "|") 
    {
        qu.pop();
        
        if (!qu.empty() && qu.front() != ";"&&  qu.front() != "|" &&  qu.front() != "#" && qu.front() != "&"  ) 
        {
            return true;
        }
    }
        return false;
}

void pc(queue<string> &v, queue<string> &qu) {
    while (!qu.empty() && qu.front() != ";"&&  qu.front() != "|" &&  qu.front() != "#" && qu.front() != "&") 
    {
        v.push(qu.front());
        qu.pop();
    }
    
    if (cpipe(qu)) 
    { 
        v.push(qu.front());
        qu.pop();
        pc(v, qu);
    }
}

bool quitp(queue<string> &v) {
    if (iequals(v.front(), "exit")) 
    {
        return true;
    }
    return false;
}

void convcstr(vector<string> &s, vector<char*> &cs) 
{
    for (unsigned i = 0; i != s.size(); ++i) 
    {
        cs[i] = &s[i][0];
    }
}

bool connectortrue(bool status, queue<string> &qu) 
{
    if (!qu.empty() && qu.front() == "&")
    {
        qu.pop();
        
        if (!qu.empty() && qu.front() == "&") 
        {
            qu.pop();
            
            if (!status) 
            {
                return false;
            }
            else 
            {
                return true;
            }
        }
        else if (qu.empty()) 
        {
            cout << "Syntax error" << endl;
            return false;
        }
        else {
            cout << "Syntax error" << endl;
            
            return false;
        }
    }
    else if (!qu.empty() && qu.front() == "|") 
    {
        qu.pop();
        
        if (!qu.empty() && qu.front() == "|") 
        {
            qu.pop();
            if (status) 
            {
                return false;
            }
            
            else 
            {
                return true;
            }
        }
        else if (qu.empty()) 
        {
            cout << "Syntax error" << endl;
            return false;
        }
        else {
            cout << "Syntax error" << endl;
            return false;
        }
    }
    else if (!qu.empty() && qu.front() == ";") 
    {
        qu.pop();
        return true;
    }
    else if (!qu.empty() && qu.front() == "#") 
    {
        return false;
    }
    
    else if (!qu.empty())  
    {
        cout << "Syntax error: " << qu.front() <<  " not a correct connector" << endl;
        return false;
    }
    else {
        return false;
    }
}



int matq(queue<string> qu) {
    int pops = 0;
    
    if (qu.front() == "\"") 
    {
        qu.pop();
        ++pops;
    }
    
    while (!qu.empty() && qu.front() != "\"") 
    {
        qu.pop();
        ++pops;
    }
    
    if (!qu.empty() && qu.front() == "\"") 
    {
        ++pops;
        return pops;
    }
    return nomatch;
}

string mq(int pops, queue<string> &qu) 
{
    qu.pop(); 
    string s = qu.front();
    qu.pop();
    for (int i = 0; !qu.empty() && i < pops - 2 && qu.front() != "\""; ++i) 
    {
        s = s + " " + qu.front();
        qu.pop();
    }
    
    qu.pop(); 
    return s;
}

int pp(queue<string> &qu, vector< vector<string> > &v, queue<pair<unsigned, string> > &p, queue<unsigned> &pipes) {
    int pipc = 0;
    for (unsigned i = 0; !qu.empty(); ++i)
    {
        
        vector<string> sv;
        while (!qu.empty() && qu.front() != "<" && qu.front() != ">" && qu.front() != "|") 
        {
            if (qu.front() == "\"") 
            {
                int pops = matq(qu);
                
                if (pops == nomatch) 
                {
                    return nomatch;
                }
                string s = mq(pops, qu);
                sv.push_back(s);
            }
            else 
            {
                sv.push_back(qu.front());
                qu.pop();
            }
        }
        unsigned rd = set_rd(qu);
        string fn; 
        if (poptrue(qu, rd, fn)) 
        {
            if (rd == nrd || rd == pip) 
            {
                fn = nofile;
            }
            p.push(make_pair(rd, fn));
        }
        else {
            return -1;
        }
        if (rd == pip) {
            pipes.push(pip);
            ++pipc;
        }
        else {
            pipes.push(npip);
        }
        v.push_back(sv);
    }
    return pipc;
}


void change(vector< vector<string> > &v, vector< vector<char*> > &cs) 
{
    for (unsigned j = 0; j < v.size(); ++j) 
    {
        vector<char*> scs(v.at(j).size()+1);
        convcstr(v.at(j), scs);
        cs.push_back(scs);
    }
}


bool sizeOf(vector< vector<string> > &ve, queue< pair<unsigned, string> > pa, queue<unsigned> pipes) 
{
    return ve.size() == pa.size() && ve.size() == pipes.size() && pa.size() == pipes.size();
}

bool mp(int f[][arrf], const unsigned npipes) {
    if (npipes == 0) 
    {
        return false;
    }
    for (unsigned i = 0; i < npipes; ++i) 
    {
        if (pipe(f[i]) == -1) 
        {
            perror("pipe():");
            _exit(-1);
        }
    }
    return true;
}


vector<char*> makesubvec (vector< vector<char*> > &v, unsigned &i) 
{
    vector<char*> s;
    
    for (unsigned a = 0; a < v.at(i).size(); ++a) 
    {
        s.push_back(v.at(i).at(a));
    }
    
    i = i+1; 
    
    return s;
}

pair<unsigned, string> makesubqueue (queue< pair<unsigned, string> > &qu) 
{
    pair<unsigned, string> s;
    
    s = make_pair(qu.front().first, qu.front().second);
    qu.pop();
    
    return s;
}

unsigned makep (queue<unsigned> &p) {
    unsigned p1 = p.front();
    p.pop();
    return p1;
}

int mrd(vector< vector<char*> > &v, unsigned i) 
{
    vector<char*> s;
    while (s.size() <= 1 && i < v.size()) 
    {
        s.clear();
        s = makesubvec(v, i);
    }
    
    if (s.size() > 1) {
        return i -1;
    }
    return i;
}

void eq(queue< pair<unsigned, string> > &qu, queue<unsigned> &pa, int i) 
{
    for (int j = 0; j < i-1 && !qu.empty() && !pa.empty(); ++j) 
    {
        qu.pop();
        pa.pop();
    }
    
    qu.pop();
}

void pdata(const vector<char*> &ve, const pair<unsigned, string> &pa, const unsigned pipe) 
{
    for (unsigned i = 0; i < ve.size() -1; ++i) 
    {
        cout << "[" << ve.at(i) << "] ";
    }
    cout << "(" << pa.first << ", " << pa.second << ") ";
    
    if (pipe == pip) 
    {
        cout << "pipe" << endl;
    }
    
    else 
    {
        cout << "npipe" << endl;
    }
}


bool begin_exec(vector< vector<char*> > &v, queue< pair<unsigned, string> > &q, queue<unsigned> &pipes, const int n)
{
    int exec_status;
    int f [maxpipes][arrf];
    
    int cp = 0;
    mp(f, n);
    
    
    for (unsigned i = 0; i < v.size() && !q.empty() && !pipes.empty(); ) 
    {
        vector<char*> sv = makesubvec(v, i);
        pair<unsigned, string> sq = makesubqueue(q);
        unsigned rd = sq.first;
        string fn = sq.second;
    
        int mc = mrd(v, i) - i;
        
        if (mc > 0)
        {
          
            eq(q, pipes, mc);
            i += mc;
           
        }
     
        pid = fork();
        if (pid < 0) 
        {
            perror("fork()");
            _exit(-1);
        }
        else if (pid == 0) 
        { 
            int oldfd, newfd;
            
            if (rd == inprd) 
            {
                if ((newfd = open(fn.c_str(), O_RDONLY)) == -1) 
                {
                    perror(" inprd: open failed");
                    _exit(-1);
                }
                
                if ((oldfd = dup(0)) == -1) 
                {
                    perror("inprd: dup()");
                    _exit(-1);
                }
                
                if (dup2(newfd, 0) == -1) {
                    perror("inprd: dup2()");
                    _exit(-1);
                }
            }
            
            else if (rd == strrd) 
            {
                string s = fn;
                string file = "file.txt";
                ofstream out(file);
                out << s << '\n';
                out.close();
                
                if ((newfd = open(file.c_str(), O_RDONLY)) == -1) 
                {
                    perror("strrd: open failed");
                    _exit(-1);
                }
                if ((oldfd = dup(0)) == -1) 
                {
                    perror("strrd: dup()");
                    _exit(-1);
                }
                if (dup2(newfd, 0) == -1) 
                {
                    perror("strrd: dup2()");
                    _exit(-1);
                }
                if (unlink(file.c_str()) == -1) 
                {
                    perror("inprd: unlink");
                    _exit(-1);
                }
            }
            
            else if (rd == outrd) 
            {
                
                if ((newfd = open(fn.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) 
                {
                    perror("outrd: open failed");
                    _exit(-1);
                }
                
                if ((oldfd = dup(1)) == -1) 
                {
                    perror("outrd: dup()");
                    _exit(-1);
                }
                
                if (dup2(newfd, 1) == -1) 
                {
                    perror("outrd: dup()");
                    _exit(-1);
                }
            }
            
            else if (rd == apprd) 
            {
                if ((newfd = open(fn.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) == -1) 
                {
                    perror("apprd: open failed");
                    _exit(-1);
                }
                
                if ((oldfd = dup(1)) == -1) 
                {
                    perror("apprd: dup()");
                    _exit(-1);
                }
                
                if (dup2(newfd, 1) == -1) 
                {
                    perror("apprd: dup2()");
                    _exit(-1);
                }
            }
            
            if (n > 0) 
            { 
                
                if (cp == 0) 
                {
                    if (dup2(f[cp][wpipe], 1) == -1) 
                    { 
                        perror("dup2()");
                        _exit(-1);
                    }
                    
                    if (close(f[cp][rpipe]) == -1) 
                    { 
                        perror("close()");
                        _exit(-1);
                    }
                }
                
                else if (cp == n) 
                { 
                    if (-1 == dup2(f[cp-1][rpipe], 0))
                    { 
                        perror("dup2()");
                        _exit(-1);
                    }
                    
                    if (-1 == close(f[cp-1][wpipe]))
                    { 
                        perror("close()");
                        _exit(-1);
                    }
                }
                
                else 
                {
                    if (-1 == dup2(f[cp-1][rpipe], 0)) 
                    { 
                        perror("dup2()");
                        _exit(-1);
                    }
                    
                    if (-1 == close(f[cp-1][wpipe])) { 
                        perror("close()");
                        _exit(-1);
                    }
                    
                    if (-1 == dup2(f[cp][wpipe], 1)) { 
                        perror("dup2()");
                        _exit(-1);
                    }
               }
           }
          
           if (-1 == execvp(sv[0], sv.data())) 
           {
               perror("execvp()");
               _exit(-1);
           }
        }
        
        else if (pid > 0) 
        { 
            if (n > 0) 
            {
                if (cp > 0) 
                {
                    if (close(f[cp - 1][0]) == -1) 
                    {
                        perror("close()");
                    }
                    if (close(f[cp - 1][1]) == -1) 
                    {
                        perror("close()");
                    }
                }
            }
            if (-1 == waitpid(pid, &exec_status, WUNTRACED)) 
            {
                perror("waitpid()");
            }
            
            ++cp;
            
            if (exec_status != 0) 
            {
                return false;
            }
        }
    }
    return true;
}

bool cd(queue<string> &q) 
{
    if (q.front() == "cd") 
    {
        q.pop();
        return true;
    }
    return false;
}

bool directhome() {
    char prev[BUFSIZ]; 
    char *h;
    int s;
    
    if ((s = pathconf(".", _PC_PATH_MAX)) == -1) 
    {
        perror("pathconf():");
    }
    
    if (getcwd(prev, s) == NULL)
    {
        perror("getcwd():");
    }
    
    if (setenv(owd, prev, 1) == -1) 
    {
        perror("setenv():");
    }
    
    if ((h = getenv(hd)) == NULL) 
    {
        perror("secure_getenv():");
    }
    if (chdir(h) == -1)
    {
       
        perror("chdir():");
        return false;
    }
    
    if (-1 == setenv(cwd, h, 1)) 
    {
        perror("setenv(cwd):");
    }
    return true;
}

bool dirp(const string &s) 
{
    char *p; 
    char* c;
    
    if ((c = getenv(owd)) == NULL) 
    {
        perror("secure_getenv(owd):");
    }
    
    if ((p = getenv(cwd)) == NULL) 
    {
        perror("secure_getenv(cwd):");
    }
    
    if (chdir(c) == -1) 
    {
        perror("chdir():");
        return false;
    }
    
    if (setenv(owd, p, 1) == -1) 
    {
        perror("setenv(owd):");
    }
    
    if (setenv(cwd, c, 1) == -1) 
    {
        perror("setenv(cwd):");
    }
    
    return true;
}

void rowd(char* sowd) 
{
    if (setenv(owd, sowd, 1) == -1) 
    {
        perror("setenv(owd):");
    }
}

bool dirpath(const string &s) 
{
    char p[BUFSIZ]; 
    char c[BUFSIZ];
    char *sowd;
    int sz;
    
    if ((sz = pathconf(".", _PC_PATH_MAX)) == -1) 
    {
        perror("pathconf()");
    }
    
    if (getcwd(p, sz) == NULL) 
    {
        perror("getcwd(prev):");
    }
    
    if ((sowd = getenv(owd)) == NULL) 
    {
        perror("secure_getenv():");
    }
    
    if (setenv(owd, p, 1) == -1) 
    {
        perror("setenv(owd):");
    }
    if (chdir(s.c_str()) == -1) 
    {
        perror("chdir()");
        rowd(sowd);
        return false;
    }
    
    if (getcwd(c, sz) == NULL) 
    {
        perror("getcwd()");
    }
    
    if (setenv(cwd, c, 1) == -1) 
    {
        perror("setenv(cwd):");
    }
    
    return true;
}

bool dirc(queue<string> &q) 
{
    string s;
    
    if (q.empty()) 
    {
        return directhome();
    }
    
    else if (q.front() == "-") 
    {
        s = q.front(); 
        q.pop();
        return dirp(s);
    }
    
    else 
    {
        s = q.front(); 
        q.pop();
        return dirpath(s);
    }
}

void suspend_process(int sig, siginfo_t *i, void *context) {
    if (pid == 0) 
    { 
        pid = getpid() + 1; 
    }
    if (kill(pid, SIGTSTP) == -1) 
    {
        perror("kill():");
        return;
    }
    
    cp.push(pid);
}

void killc(int sig, siginfo_t *i, void *context) 
{
    if (pid == 0) 
    {
        pid = getpid() + 1;
    }
    if (kill(pid, sig) == -1) 
    {
        perror("kill():");
    }
}

void sigc() {
    struct sigaction ctrlC;
    ctrlC.sa_sigaction = &killc;
    ctrlC.sa_flags = SA_SIGINFO | SA_RESTART;
    
    if (sigaction(SIGINT, &ctrlC, NULL) < 0) 
    {
        perror("sigaction(ctrlc):");
    }
}

void sigz() {
    struct sigaction ctrlZ;
    
    ctrlZ.sa_sigaction = &suspend_process;
    
    ctrlZ.sa_flags = SA_SIGINFO | SA_RESTART;
    
    if (sigaction(SIGTSTP, &ctrlZ, NULL) < 0) 
    {
        perror("sigaction(ctrz):");
    }
}

bool commands(const queue<string> &q) {
    if (q.front() == "fg" || q.front() == "bg") 
    {
        return true;
    }
    return false;
}


bool send_sig(queue<string> &q) 
{
    string sig = q.front(); q.pop();
    
    if (sig == "fg") 
    {
        if (cp.empty()) 
        {
            cout << "fg: can't find process" << endl;
            return false;
        }
        
        int status;
        
        cpid = cp.top(); cp.pop();
        
        if (kill(cpid, SIGCONT) == -1) 
        {
            perror("kill():");
        }
        
        pid = cpid;
        
        if (waitpid(cpid, &status, WUNTRACED) == -1) 
        {
            perror("waitpid():");
        }
        return true;
    }
    else if (sig == "bg") {
        if (cp.empty()) 
        {
            cout << "bg: can't find process" << endl;
            return false;
        }
        
        cpid = cp.top();
        
        if (kill(cpid, SIGCONT) == -1) 
        {
            perror("kill():");
        }
        return true;
    }
    return false;
}

int main() {
    
    sigc();
    sigz();
    while (1) {
        
        string inp;
        
        queue<string> l;
        prompt();
        
        getline(cin, inp);
        parseq(l, inp);
        
        queue<string> command;
        
        bool cdtrue = false; bool sigtrue = false; bool exectrue = false;
        
        while (!l.empty()) 
        {
            if (l.front() == "#") 
            {
                break;
            }
            pc(command, l);
            
            if (command.empty()) 
            {
                cout << "Error: command" << endl;
                break;
            }
            else if (quitp(command))
            {
                return 0;
            }
            else {
                
                if (cd(command)) 
                {
                    cdtrue = dirc(command);
                    if (!connectortrue(cdtrue, l)) 
                    {
                        break;
                    }
                }
                else if (commands(command)) 
                {
                    sigtrue = send_sig(command);
                    
                    if (!connectortrue(sigtrue, l)) 
                    {
                        break;
                    }
                }
                else 
                {
                    vector< vector<string> > v;
                    queue< pair<unsigned, string> > rfil;
                    queue<unsigned> pipes;
                    int n = pp(command, v, rfil, pipes);
                    
                    if (n == nomatch) 
                    {
                        cout << "Syntax error" << endl;
                        break;
                    }
                    if (sizeOf(v, rfil, pipes) && v.size() != 0 && n != -1)
                    {
                        vector< vector<char*> > cmds;
                        change(v, cmds);
                        exectrue = begin_exec(cmds, rfil, pipes, n);
                    }
                    else 
                    {
                        cout << "Syntax error" << endl;
                        break;
                    }
                    if (!connectortrue(exectrue, l)) 
                    {
                        break;

                    }
                }
            }
            popq(command);
        }
    }
    return 0;
}
