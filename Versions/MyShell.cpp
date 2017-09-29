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

void parse(string line){

	const char* whiteSpaces = " \t";
	line.erase(0,line.find_first_not_of(whiteSpaces));
	line.erase(line.find_last_not_of(whiteSpaces)+1);
	
	if(line=="")
		return;
	//======================================================================================//

	





	//========================================= CD =========================================//
	int cdFound,status;
	string path;
	if(line=="cd." || line=="cd.."){
		cout<<line<<": command not found\n";
		return;
	}
	cdFound=line.find("cd");
	
	if(cdFound != -1 && cdFound==0){
		line=line.substr(2);
		if(line==""){
			chdir(getenv("HOME"));
			return;
		}
		path=line.substr(line.find_first_not_of(whiteSpaces));
		if(path==".")
			return;
		else if(path=="..")
			chdir("..");
		else{
			status=chdir(path.c_str());
			if(status==-1){
				cout<<"D_shell: cd: "<<path<<": No such file or directory\n";
			}
		}
		return;
	}


	//=========================================PWD========================================//
	int pwdFound;
	char pwd[1000];
	pwdFound=line.find("pwd");
	
	if(pwdFound != -1 && pwdFound==0){
		cout<<getcwd(pwd, sizeof(pwd))<<"\n";
		return;
	}

	//========================================ECHO==========================================//







	//======================================================================================//
	

	//========================================TOKENS========================================//

	vector<string> tokens;
	string token;
	int spaceFound=line.find(" ");
	int tabFound=line.find("\t");
	
	while(spaceFound!=-1 || tabFound!=-1){
		
		token=line.substr(line.find_first_not_of(whiteSpaces));
		token=token.substr(0,token.find_first_of(whiteSpaces));

		line=line.substr(line.find_first_of(whiteSpaces)+1);
		line.erase(0,line.find_first_not_of(whiteSpaces));

		tokens.push_back(token);

		spaceFound=line.find(" ");
		tabFound=line.find("\t");
	}
	
	tokens.push_back(line);

	for (std::vector<string>::iterator i = tokens.begin(); i != tokens.end(); ++i){
		cout<<*i<<"\n";
	}

}

//==================================================MAIN FUNCTION=========================================//

int main()
{
	string line;
	char pwd[1000];

	while(true){

		cout<<"\033[1;32mD_shell:\033[0m"<<"\033[1;34m"<<getcwd(pwd, sizeof(pwd))<<"\033[0m"<<"$ ";
		getline(cin,line);

		if(line=="exit")
			break;
		if(line=="\n")
			continue;

		parse(line);
	}

	return 0;
}