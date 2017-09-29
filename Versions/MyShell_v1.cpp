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

//========================================TOKENS PARSING========================================//

// void handle(char **argv){
// 	// int fd[2];
// 	// pipe(fd[2]);
// 	int pid=fork();
// 	int s;
// 	if(pid==0){
// 		execvp(argv[0],argv);
// 		perror("D_shell");
// 		exit(0);
// 	}
// 	else
// 		while (wait(&s) != pid);
	
// }

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
			token+=line[i];
			count=0;
			if(line[i]=='\"'){
				i++;
				while(line[i]!='\"' && line[i]!='\0')
					token+=line[i++];
				if(line[i]!='\0')
					token+='\"';
			}
		}
	}
	if(token!="")
		tokens.push_back(token);
	return tokens;

}

//========================================EXECUTE=================================================//

void execute(vector<string> tokens){

	char **argv;
	string temp1;
	int c=0,k,l,m=0,n=0,pipeCounter=0;

	for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		if(*i=="|")
			pipeCounter++;
		//cout<<*i<<endl;
	}

	for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		
		temp1=*i;
		if(temp1!="|" ){
			c++;
			continue;
		}
		else if(temp1=="|"){
			cout<<c<<endl;
			argv=new char *[c+1];
			for (l = m; l < m+c; ++l){
				k=tokens[l].length();
				argv[n] = new char[k+1];
				strcpy(argv[n], tokens[l].c_str());
				n++;
			}
			argv[n]=NULL;
			//handle(argv);
			//cout<<argv[1]<<endl;
			m=m+c+1;
			c=n=0;
		}
	}

}





	//======================================================================================//



	//========================================= CD =========================================//
	// int cdFound,status;
	// string path;
	// if(line=="cd." || line=="cd.."){
	// 	cout<<line<<": command not found\n";
	// 	return;
	// }
	// cdFound=line.find("cd");

	// if(cdFound != -1 && cdFound==0){
	// 	line=line.substr(2);
	// 	if(line==""){
	// 		chdir(getenv("HOME"));
	// 		return;
	// 	}
	// 	path=line.substr(line.find_first_not_of(whiteSpaces));
	// 	if(path==".")
	// 		return;
	// 	else if(path=="..")
	// 		chdir("..");
	// 	else{
	// 		status=chdir(path.c_str());
	// 		if(status==-1){
	// 			cout<<"D_shell: cd: "<<path<<": No such file or directory\n";
	// 		}
	// 	}
	// 	return;
	// }


	// //=========================================PWD========================================//
	// int pwdFound;
	// char pwd[1000];
	// pwdFound=line.find("pwd");

	// if(pwdFound != -1 && pwdFound==0){
	// 	cout<<getcwd(pwd, sizeof(pwd))<<"\n";
	// 	return;
	// }

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
		execute(tokens);


		// for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		// 	cout<<*i<<"\n";
		// }
	}

	return 0;
}