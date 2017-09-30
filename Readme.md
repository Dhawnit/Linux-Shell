# LINUX SHELL IMPLEMENTATION IN C++
* SHELL NAME: D_shell
* Author: DHAWNIT KHURANA
* Email ID: dhawni.khurana@students.iiit.ac.in

### D_shell Features

This program simulates the working of command line interface in Unix-like environment. Implemented Functionalities are as under:

1. Execute all the External commands (ls, clear, vi etc.)
2. Implement Internal commands: cd, pwd, echo, export
3. Initialize and use environment variables
4. Print environment variables using echo command
5. Redirection operators: STDIN, STDOUT ( > , < ) 
6. Support for history command and '!' operator (history, !!, !-1, !10,!-10 etc)
7. Pipes “|” (multiple) (Ex: ls | grep 'a' | wc)
8. SIGINT has been handled (control-c). It doesnot kills the shell instead it cause the shell to kill the   process in the current foreground job. 

### Input/Output Format

Input from the 'stdin' in an infinite loop till an “exit” is entered. The corresponding output should be printed to 'stdout'.

Example:
```
bash prompt:~$ ./a.out

D_shell:/home/user$ ls
MyShell_v6.cpp a.out
D_shell:/home/user$ fdre
D_shell: fdre: No command found
D_shell:/home/user$ exit

bash prompt:~$
```
