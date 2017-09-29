// 20162076, DHAWNIT KHURANA

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

//===============================================EXECUTE====================================================//

void executeProcess(int in,int out,char **argv){

	int pid=fork();
	
	if(pid==0){

		if(in!=0){
			dup2(in,0);
			close(in);
		}
		if (out != 1){
			dup2(out,1);
			close(out);
		}

		execvp(argv[0],argv);
		perror("D_shell");
		exit(0);
	}

}
//===========================================EXECUTE BUILTINS=============================================//

void executeShellBuiltIn(char **argv){

	if ( strcmp(argv[0],"cd")==0 || strcmp(argv[0],"cd.")==0 || strcmp(argv[0],"cd..")==0 ){
		
		if( strcmp(argv[0],"cd.")==0 || strcmp(argv[0],"cd.." ) ==0 ){

			cout<<argv[0]<<": command not found\n";
			return;
		}
		else if(argv[1]==NULL){	
			chdir(getenv("HOME"));
			return;
		}
		else{

			int status=chdir(argv[1]);
			if(status==-1){
				cout<<"D_shell: cd: "<<argv[1]<<": No such file or directory\n";
				return;
			}
		}

	}
	else if ( strcmp(argv[0],"pwd")==0 ){
		char pwd[1000];
		cout<<getcwd(pwd, sizeof(pwd))<<"\n";
		return;
	}
}


//========================================HANDLE COMMANDS=================================================//

void handleCommands(char ***argv,int cmdCounter){
	int i=0, s, in=0, fd[2];
	int pid;
	for(i=0;i<cmdCounter-1;i++){

		if( strcmp(argv[i][0],"cd")==0 || strcmp(argv[i][0],"cd.")==0 || strcmp(argv[i][0],"cd..")==0 || strcmp(argv[i][0],"pwd")==0 || strcmp(argv[i][0],"echo")==0 || strcmp(argv[i][0],"export")==0 ){
			executeShellBuiltIn(argv[i]);
		}
		else{

			pipe(fd);
			executeProcess(in,fd[1],argv[i]);
			close(fd[1]);
			in=fd[0];
		}
	}

	if( strcmp(argv[i][0],"cd")==0 || strcmp(argv[i][0],"cd.")==0 || strcmp(argv[i][0],"cd..")==0 || strcmp(argv[i][0],"pwd")==0 || strcmp(argv[i][0],"echo")==0 || strcmp(argv[i][0],"export")==0 ){
		executeShellBuiltIn(argv[i]);
	}
	else{
		pid=fork();

		if(pid==0){
			if (in != 0)
				dup2 (in, 0);
			execvp(argv[i][0],argv[i]);
			perror("D_shell");
			exit(0);
		}
		else
			while (wait(&s) != pid);
	}
}

//========================================MAKE COMMANDS=================================================//

void makeCommands(vector<string> tokens){

	char ***argv;
	string temp1;
	int x=0,c=0,k,l,m=0,n=0,pipeCounter=0,cmdCounter=0;
	
	for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		if(*i=="|")
			pipeCounter++;
	}
	cmdCounter = pipeCounter+1;
	argv=new char**[cmdCounter];

	for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		
		temp1=*i;
		if(temp1!="|" ){
			c++;
			continue;
		}
		else if(temp1=="|"){

			argv[x]=new char *[c+1];
			for (l = m; l < m+c; ++l){
				k=tokens[l].length();
				argv[x][n] = new char[k+1];
				strcpy(argv[x][n], tokens[l].c_str());
				n++;
			}
			argv[x][n]=NULL;
			m=m+c+1;
			c=n=0;
			x++;
		}
	}
	argv[x]=new char *[c+1];
	for (l = m; l < m+c; ++l){
		k=tokens[l].length();
		argv[x][n] = new char[k+1];
		strcpy(argv[x][n], tokens[l].c_str());
		n++;
	}
	argv[x][n]=NULL;
	handleCommands(argv,cmdCounter);

}

//=============================================TOKENS PARSING==============================================//

vector<string> parse(string line){

	vector<string> tokens;
	string token="";
	int in=1,out=0;
	int state=out;
	int count=0;
	string special="";

	for (int i = 0; i < (int)line.length(); ++i)
	{	
		
		if(line[i]=='|' || line[i]=='<' || line[i]=='>' || line[i]=='!'){
			if(token!="")
				tokens.push_back(token);
			special+=line[i];
			tokens.push_back(special);
			token="";
			special="";
			state=out;
		}
		else if(line[i]!=' ' && line[i]!='\t' && state==in){
			token+=line[i];
		}
		else if((line[i]==' ' || line[i]=='\t') && count==0){
			state=out;
			count++;
			if(token!="")
				tokens.push_back(token);
			token="";
		}
		else if((line[i]==' ' || line[i]=='\t') && count!=0){
			count++;
		}
		else if(state==out){
			state=in;
			count=0;
			if(line[i]=='\"'){
				i++;
				while(line[i]!='\"' && line[i]!='\0')
					token+=line[i++];
				if(line[i]=='\0' && line[i]!='\"'){
					token='\"'+token;
				}
			}
			else if(line[i]=='\''){
				i++;
				while(line[i]!='\'' && line[i]!='\0')
					token+=line[i++];
				if(line[i]=='\0' && line[i]!='\''){
					token='\''+token;
				}
			}
			else	
				token+=line[i];	
		}
	}
	if(token!="")
		tokens.push_back(token);
	return tokens;

}
	//======================================================================================//
	//========================================ECHO==========================================//
	//======================================================================================//


//==================================================MAIN FUNCTION=========================================//

int main()
{
	string line;
	char pwd[1000];

	while(true){

		cout<<"\033[1;32mD_shell:\033[0m"<<"\033[1;34m"<<getcwd(pwd, sizeof(pwd))<<"\033[0m"<<"$ ";
		getline(cin,line);

		const char* whiteSpaces = " \t";
		line.erase(0,line.find_first_not_of(whiteSpaces));
		line.erase(line.find_last_not_of(whiteSpaces)+1);

		if(line=="")
			continue;

		if(line=="exit")
			break;

		vector<string> tokens = parse(line);
		makeCommands(tokens);

		// for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		// 	cout<<*i<<"\n";
		// }
	}

	return 0;
}