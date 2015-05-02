#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <list>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm> 
#include <queue>

// Online colors for escape char
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define CLEAR "\033[0m"
#define GRAY "\033[5;30m"


using namespace std;

unsigned getflags(const vector<string> &v) 
{
    int vecflags = 0; 
    for (unsigned i = 0; i < v.size(); ++i) 
    {
        
        for (unsigned j = 1; j < v.at(i).size(); ++j) 
        {
            
            if (v.at(i).at(j) == 'a') 
            {
                vecflags = vecflags | 1;
            }
            else if (v.at(i).at(j) == 'l') 
            {
                vecflags = vecflags | 2;
            }
            else if (v.at(i).at(j) == 'R') 
            {
                vecflags = vecflags | 4;
            }
            else 
            {
                cout << "Invalid flag" << endl;
                exit(1);
            }
        }
    }
    return vecflags;
}

void flagscheck(int argc, char* argv[], queue<string> &p, vector<string> &f) {
    for (int i = 1; i < argc; ++i) 
    {
        if (argv[i][0] == '-') 
        {
            f.push_back(argv[i]);
        }
        else 
        {
            p.push(argv[i]);
        }
    }
    if(p.empty())
    {
        p.push(".");
    }
}



int visit(unsigned flags , string dir)
{   
    struct stat st;
    struct dirent *dp;
    
    struct passwd *pwd;
    struct group *src_gr ;
    
    //NEED TO PUSHBACK DIRECTORIES
    vector<string> direct;
    vector<string> files;

    //OPENS CURRENT DIRECTORY PATH
    DIR *dirp;
    dirp = opendir(dir.c_str());
    int totalblk = 0;
    unsigned len = 0;
    string t;
   
    string home;
    
    dp = readdir( dirp );
    while(dp)
    {
        stat(dp->d_name,&st);
        files.push_back(dp->d_name);
    
        if(files.back().at(0) != '.'  )
        {
            totalblk += st.st_blocks;
        }
        stringstream s;
        s << st.st_size;
        t = s.str();
            
        if(t.size() > len)
            len = t.size();
    
        dp = readdir( dirp );
    }
    
    // GET DIRECTORIES
    for (unsigned i = 0; i < files.size(); i++)
    {
        struct stat st;
       // stat((dir + "/" + files.at(i)).c_str(), &in);
        if(lstat((dir + '/' + (files.at(i))).c_str() , &st ) == -1)
        {
            return -1;
        }

        if (( files.at(i).at(0) != '.') && S_ISDIR( st.st_mode ) ) 
        {
            direct.push_back(files.at(i));
            //cout << files.at(i) << endl;
        }
        if(files.at(i).size() > 1)
        {
            if( ((flags & 1) == 1) && files.at(i).at(0) == '.' && files.at(i).at(1) != '.' && S_ISDIR( st.st_mode ))
            {
                direct.push_back(files.at(i));
            }
        }
         //cout << endl;
    }
    
    //cout << "hello" << endl;
    //NEED TO SORT FILENAMES  
    sort(files.begin(),files.end());   
    if((flags & 2) == 2)
    {
        //NEED TO DISPLAY TOTAL SIZE OF CURRENT DIRECTORY
        if(dir.at(dir.size()-1) == '/')
        {
            cout << dir.substr(0,dir.size()-1) << ":" << endl;
        }
        else 
            cout << dir << ":" << endl;
        cout << "total " << totalblk / 2  << endl;
    
        //NEED TO ITERATE THROUGH DIRECTORY TO DISPLAY FILES
        //AND STORE OTHER DIRECTORIES
        for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
        {
            // puts file or directory inside stat for current path
            if((*it).at(0) != '.' || (flags & 1) == 1)
            {
                if(dir.at(dir.size()-1) != '/' )
                {
                    if(lstat((dir + '/' + (*it)).c_str() , &st ) == -1)
                    {
                        return -1;
                    }
                }
                else
                {
                   if(lstat((dir + (*it)).c_str() , &st ) == -1)
                    {
                        return -1;
                    }
                }
                    
                string username,group;
                if( ( pwd = getpwuid( st.st_uid ) )!= NULL)
                {
                    username = pwd->pw_name;
                }
                if( ( src_gr = getgrgid( st.st_gid ) )!= NULL)
                {
                    group = src_gr->gr_name;
                }
                
               
                //~ ((st.st_mode & S_IFMT) == S_IFDIR) ? cout << "d" : cout << "-";
                if(S_ISDIR(st.st_mode) ) 
                    cout << "d";
                else if(S_ISCHR(st.st_mode))
                    cout << "c";
                else if(S_ISLNK(st.st_mode))
                    cout << "l";
                else  cout << "-";
                (st.st_mode & S_IRUSR) ? cout << "r" : cout << "-";
                (st.st_mode & S_IWUSR) ? cout << "w" : cout << "-";
                (st.st_mode & S_IXUSR) ? cout << "x" : cout << "-";
                (st.st_mode & S_IRGRP) ? cout << "r" : cout << "-";
                (st.st_mode & S_IWGRP) ? cout << "w" : cout << "-";
                (st.st_mode & S_IXGRP) ? cout << "x" : cout << "-";
                (st.st_mode & S_IROTH) ? cout << "r" : cout << "-";
                (st.st_mode & S_IWOTH) ? cout << "w" : cout << "-";
                (st.st_mode & S_IXOTH) ? cout << "x" : cout << "-";
                cout  <<  " " ;
                //hardlinks
                cout << right << st.st_nlink  << " " 
                     << setw(5)
                //user ID name
        
                     << right << username 
                     << setw(1)
                //group ID name
                     << " " << group << " " 
                     
                //blksize of file or directory
                << setw(len) << st.st_size;
                
                //last modified time, month day time

                struct tm * timeinfo;
                char buffer [80];
                timeinfo = localtime (&st.st_mtime);

                strftime (buffer,80,"%b %e %R",timeinfo);
                cout << " " << buffer << " ";
               
                //file or directory name
                if(S_ISDIR(st.st_mode))
                {
                    if(((flags & 1) == 1) && (*it).at(0) == '.')
                        cout << GRAY << BLUE << *it + "/" << CLEAR << endl;
                    else
                        cout << BLUE << *it + "/" << CLEAR << endl;
                }   
                else if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
                {
                    cout << GREEN << *it  << CLEAR << "*" << endl;
                }
                else
                    if(((flags & 1) == 1) && (*it).at(0) == '.')
                        cout << GRAY << BLUE  << *it << CLEAR << endl;
                    else
                        cout  << *it << endl;
            
            }
            //files.pop_front();
        }
    }
    else if((flags & 2) != 2)
    {
        if(dir.at(dir.size()-1) == '/')
        {
            cout << dir.substr(0,dir.size()-1) << ":" << endl;
        }
        else 
            cout << dir << ":" << endl;
        
        for(unsigned j = 0; j < files.size(); j++)
        {
            // puts file or directory inside stat for current path
            if( (files.at(j).at(0) != '.' || (flags & 1) == 1  ) && files.at(j).at(0) != '~')
            {
                if(dir.at(dir.size()-1) != '/' )
                {
                    if(lstat((dir + '/' + (files.at(j))).c_str() , &st ) == -1)
                    {
                        return -1;
                    }
                }
                else
                {
                   if(lstat((dir + (files.at(j))).c_str() , &st ) == -1)
                    {
                        return -1;
                    }
                }
                
                //print file name
                //file or directory name or exec
                if(S_ISDIR(st.st_mode))
                {
                    if(((flags & 1) == 1) &&  files.at(j) .at(0) == '.')
                        cout << GRAY << BLUE <<  files.at(j) + "/" << CLEAR << "  ";
                    else
                        cout << BLUE <<  files.at(j)  + "/" << CLEAR << "  ";
                }   
                else if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
                {
                    cout << GREEN << files.at(j)  << CLEAR << "*" <<  "  ";
                }
                else
                    if(((flags & 1) == 1) &&  files.at(j) .at(0) == '.')
                        cout << GRAY  <<  files.at(j)  << CLEAR << "  ";
                    else
                        cout <<  files.at(j)  << "  ";
                //cout << setw(1);
            }
        }
        cout << endl;
    }
    if ((flags & 4) == 4) 
    {
         
        //NEED TO ORDER FOUND DIRECTORIES
        sort(direct.begin(), direct.end());

        string temp;
        //RECURSIVE DIRECTORY CALLS
        //cout << "Size: " << direct.size() << endl;
        for(unsigned i = 0; i < direct.size(); i++)
        {   cout << endl;
            //cout << direct.at(i) << endl;
            if(dir.at(dir.size()-1) != '/')
                temp = dir + '/' + direct.at(i);
            else
                temp = dir + direct.at(i);
            visit(flags,temp);
        }
    }
    
    closedir(dirp);
    return 1;
}

int main(int argc, char * argv[] )
{
    if (argc == 0) 
    {
        cerr << "Need arguments" << endl;
    }
    else {
        
        vector<string> flags;
        
        queue<string> paths;
        
        flagscheck(argc, argv, paths, flags);
       
        unsigned f = getflags(flags);
        while(!paths.empty())
        {
            visit(f, paths.front());
            paths.pop();
            cout << endl;
        }
    }
    return 0;
    
}
