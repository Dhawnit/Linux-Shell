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
#include <csignal>
#include <fstream>
using namespace std;

vector<string> history;
int historyCounter=0;

vector<string> parse(string);
void makeCommands(vector<string>);
//====================================================REDIRECTIONS================================================//

int checkRedirections(char **argv){
	int i=0,c=0;
	while(argv[i]!=NULL){
		if( strcmp(argv[i],"<") == 0 || strcmp(argv[i],">") == 0 )
			c++;
		i++;
	}
	return c;
}


//====================================================BANG================================================//

void handleBang(char *argv){
	string command="";
	vector<string> tokens;
	int k,x;

	if(argv[0] >= 'a' && argv[0] <= 'z'){
		for(x=historyCounter-1 ; x>=0 ; x--){
			if(history[x][0] == argv[0]){
				command=history[x];
				break;
			}
		}
		if(history.back() != command){
			history.push_back(command);
			historyCounter++;
		}
		tokens=parse(command);
		makeCommands(tokens);
		return;
	}
	else if(strcmp(argv,"!")==0){
		k=historyCounter;
		command = history[k-1];
		if(history.back() != command){
			history.push_back(command);
			historyCounter++;
		}
		tokens=parse(command);
		makeCommands(tokens);
		return;
	}
	else if(argv[0]=='-'){
		k=atoi(argv+1);
		command = history[historyCounter-k];
		if(history.back() != command){
			history.push_back(command);
			historyCounter++;
		}
		tokens=parse(command);
		makeCommands(tokens);
		return;
	}
	else{
		k=atoi(argv);
		command = history[k-1];
		if(history.back() != command){
			history.push_back(command);
			historyCounter++;
		}
		tokens=parse(command);
		makeCommands(tokens);
		return;
	}

}

//===============================================HISTORY====================================================//

void handleHistory(char *argv){
	int i,k;
	if(argv==NULL){
		i=0;
		for(i=0;i<historyCounter;i++){
			cout<<i+1<<":\t"<<history[i]<<"\n";
		}
	}
	else{
		k=atoi(argv);
		for(i=historyCounter-k ;i<historyCounter;i++){
			cout<<i+1<<":\t"<<history[i]<<"\n";
		}	
	}
	return;
}
//===============================================SIGNAL=====================================================//

void signalHandler( int signum )
{	
	cout<<"\n"<<"\033[1;32mD_shell:\033[0m"<<"\033[1;34m"<<getenv("PWD")<<"\033[0m"<<"$ ";
	cout.flush();
}

//===============================================EXECUTE====================================================//

void executeProcess(int in,int out,char **argv){

	
	int x,i=0,ipr=0,opr=0,k=0;
	x=checkRedirections(argv);
	char **argvNew;
	char *inputFile, *outputFile;
	if(x != 0){
		
		argvNew= new char*[10];
		while(argv[i]!=NULL){

			if( strcmp(argv[i],"<") == 0 ){
				ipr++;
				inputFile=argv[i+1]; 
			}
			else if( strcmp(argv[i],">") == 0 ){
				opr++;
				outputFile=argv[i+1];
			}
			else if(ipr==0 && opr==0){
				argvNew[k]=new char(strlen(argv[i])+1);
				strcpy(argvNew[k],argv[i]);
				k++;
			}
			i++;
		}
		argvNew[k]=NULL;
	}

	int pid=fork();


	if(pid==0){
		
		if(opr!=0 || ipr!=0){

			if(ipr != 0){
				int fd=open(inputFile, O_RDONLY | O_CREAT);
				//in=fd;
				if(in!=0){
					dup2(fd,0);
					close(fd);
				}
				// out=1;
				// if (out != 1){
				// 	dup2(out,1);
				// 	close(out);
				// }

				execvp(argvNew[0],argvNew);
				perror("D_shell");
				exit(0);
			}
			if(opr != 0){

				int fd1=open(outputFile, O_WRONLY | O_CREAT, 00777);
				
				// if(in!=0){
				// 	dup2(in,0);
				// 	close(in);
				// }
				//if (out != 1){
				dup2(fd1,1);
				close(fd1);
				//}

				execvp(argvNew[0],argvNew);
				perror("D_shell");
				exit(0);
			}

		}
		else{
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

}

//=============================================EXECUTE BUILTINS=============================================//

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
	else if ( strcmp(argv[0],"echo")==0 ){
		int i=1;
		
		while(argv[i]!=NULL){
			
			if( strcmp(argv[i],"$SHELL") == 0)
				cout<<"D_shell"<<" ";
			else if( strcmp(argv[i],"$$") == 0)
				cout<<getpid()<<" ";
			else if( argv[i][0] =='$'){
				int k=1; string enVar="";
				while(argv[i][k]!='\0'){
					enVar+=argv[i][k];
					k++;
				}
				if(getenv(enVar.c_str())!=NULL)
					cout<<getenv(enVar.c_str())<<" ";
				else
					cout<<"";
			}
			else
				cout<<argv[i]<<" ";
			i++;
		}
		cout<<endl;
		return;
	}

	else if ( strcmp(argv[0],"export")==0 ){
		int f=0;
		string s1=""; string s2="";
		if(argv[2]==NULL){

			int j=0;
			while(argv[1][j] != '\0'){
				if(argv[1][j]!='=' && f==0){
					s1+=argv[1][j];
					f=1;
					j++;
				}
				else if(argv[1][j]=='='){
					j++;
					continue;
				}
				else{
					s2+=argv[1][j];
					j++;
				}

			}
			setenv(s1.c_str(),s2.c_str(),1);
		}
		else{
			cout<<"D_shell: export: `"<<argv[2]<<"': not a valid identifier"<<endl;
		}
		return;
	}
	else if ( strcmp(argv[0],"history")==0 ){
		handleHistory(argv[1]);
		return;
	}
	else if ( strcmp(argv[0],"!")==0 ){
		handleBang(argv[1]);
		return;
	}
}

//==========================================HANDLE COMMANDS=================================================//

void handleCommands(char ***argv,int cmdCounter){
	int i=0, s, in=0, fd[2];
	int pid;
	for(i=0;i<cmdCounter-1;i++){

		if( strcmp(argv[i][0],"cd")==0 || strcmp(argv[i][0],"cd.")==0 || strcmp(argv[i][0],"cd..")==0 || strcmp(argv[i][0],"pwd")==0 || strcmp(argv[i][0],"echo")==0 || strcmp(argv[i][0],"export")==0 || strcmp(argv[i][0],"history")==0 || strcmp(argv[i][0],"!")==0 ){
			executeShellBuiltIn(argv[i]);
		}
		else{

			pipe(fd);
			executeProcess(in,fd[1],argv[i]);
			close(fd[1]);
			in=fd[0];
		}
	}

	if( strcmp(argv[i][0],"cd")==0 || strcmp(argv[i][0],"cd.")==0 || strcmp(argv[i][0],"cd..")==0 || strcmp(argv[i][0],"pwd")==0 || strcmp(argv[i][0],"echo")==0 || strcmp(argv[i][0],"export")==0 || strcmp(argv[i][0],"history")==0 || strcmp(argv[i][0],"!")==0 ){
		executeShellBuiltIn(argv[i]);
	}
	else{

		int x,l=0,ipr=0,opr=0,k=0;
		x=checkRedirections(argv[l]);
		char **argvNew;
		char *inputFile, *outputFile;
		if(x != 0){

			argvNew= new char*[10];
			while(argv[i][l]!=NULL){

				if( strcmp(argv[i][l],"<") == 0 ){
					ipr++;
					inputFile=argv[i][l+1]; 
				}
				else if( strcmp(argv[i][l],">") == 0 ){
					opr++;
					outputFile=argv[i][l+1];
				}
				else if(ipr==0 && opr==0){
					argvNew[k]=new char(strlen(argv[i][l])+1);
					strcpy(argvNew[k],argv[i][l]);
					k++;
				}
				l++;
			}
			argvNew[k]=NULL;
		}

		pid=fork();

		if(pid==0){
			
			if(opr!=0 || ipr!=0){
				
				if(ipr != 0){
					int fd=open(inputFile, O_RDONLY | O_CREAT);
					in=fd;
					if(in!=0){
						dup2(fd,0);
						close(fd);
					}
				// out=1;
				// if (out != 1){
				// 	dup2(out,1);
				// 	close(out);
				// }

					execvp(argvNew[0],argvNew);
					perror("D_shell");
					exit(0);
				}
				if(opr != 0){

					int fd1=open(outputFile, O_WRONLY | O_CREAT, 00777);
					
				// if(in!=0){
				// 	dup2(in,0);
				// 	close(in);
				// }
				//if (out != 1){
					dup2(fd1,1);
					close(fd1);
				//}

					execvp(argvNew[0],argvNew);
					perror("D_shell");
					exit(0);
				}

			}
			else{
				if (in != 0)
					dup2 (in, 0);
				execvp(argv[i][0],argv[i]);
				perror("D_shell");
				exit(0);
			}

		}
		else
			while (wait(&s) != pid);
	}
}

//============================================MAKE COMMANDS=================================================//

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

//==================================================MAIN FUNCTION=========================================//

int main()
{
	string line;
	char pwd[1000];
	signal(SIGINT, signalHandler);
	fstream myFile;
	myFile.open ("History.txt", ios::out | ios::app | ios::in);
	myFile<<"\n\n=========================================NEW SESSION======================================\n\n";
	myFile.close();

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
		
		if(line[0]!='!'){
			if( history.size()==0){

				history.push_back(line);
				historyCounter++;
				myFile.open ("History.txt", ios::out | ios::app | ios::in);
				myFile<<historyCounter<<":\t"<<line<<"\n";
				myFile.close();
			}
			else if (history.back() != line){
				history.push_back(line);
				historyCounter++;
				myFile.open ("History.txt", ios::out | ios::app | ios::in);
				myFile<<historyCounter<<":\t"<<line<<"\n";
				myFile.close();	
			}
		}
		vector<string> tokens = parse(line);
		makeCommands(tokens);

	}
	return 0;
}
