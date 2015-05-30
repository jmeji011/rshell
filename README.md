#rshell
rshell is an imitation of the BASH terminal. This implementation allows the use 
of connectors such as `||`, `&&` and `;`.  Anything after a `#` is a comment and 
is ignored by the program. The command `exit` closes the program. 

Connector Meanings:
`||` run command only if first one fails.
`&&` run command only if first one succeeds.
`;` run any command if the command is true.

#How to run rshell
```
$git clone https://github.com/jmeji011/rshell.git
$cd rshell
$git checkout hw0
$make
$bin/rshell
```

#LS
This program imitates the ls command found in the bash terminal.
This program can handle the following flags after calling ls:
`-l`, `-a`, `-R`
It can also handle the flags when they are combined.

#How to run ls
```
$git clone https://github.com/jmeji011/rshell.git
$cd rshell
$git checkout hw1
$make
$bin/ls
```
#LS Bugs
The directories and filenames are not aligned during use of standard ls in comparison
to bashs ls. 


#HW 2: Adding redirection to Rshell
This program is an updated version of my previous rshell.
Rshell now handles input redirection `<`, output redirection `>` and piping `|`.


#How to run HW2
```
$git clone https://github.com/jmeji011/rshell.git
$cd rshell
$git checkout hw2
$make
$bin/rshell
```
#HW 2: Adding cd implementation
This program updates rshell and adds the commands `cd`, `cd -` and `cd <path>`.
This program also now shows the current working directory in the prompt.
This program also has a `^c` which is used to kill shell processes. 
This program also has `^z` signal and `fg` and `bg` commands.
The `^c` and `^z` will fail if there are no processes.
`fg` will return the most recent proccess that was ended.
`bg` will send the most recent process to the background.

#How to run HW3
```
$git clone https://github.com/jmeji011/rshell.git
cd rshell
git checkout hw3
make
bin/rshell
```
#Test Cases
These are located in the tests folder.

#Bugs
Echo with quotes do not work with this shell implementation.
`echo "hello"` , This will display `"hello"`. 
The directories and filenames are not aligned during use of standard ls in comparison
to bashs ls. 
This program is setup to work without the connectors part of rshell from before.
That part of the code is commented out and will continue to work when uncommented.
`fg` and `bg` have some bugs that causes certain processes to be terminated.
