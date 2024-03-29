#include<bits/stdc++.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h> 
#include <errno.h>   
#include <sys/wait.h> 
#include <unistd.h>
#include<fcntl.h>

using namespace std;

int last_proc_status=0;

bool script_flag=false;
char  script_path[256];

bool check_single_arrow=false;

std::vector<char *> v;
std::vector<char *> vector_IO;

void parseCommand_IO(char * command,char **argv)
{
		char* token = strtok(command, " "); 

	  	char ** temp=argv;
	  
		// Keep printing tokens while one of the 
		// delimiters present in str[]. 
		while (token != NULL) { 
			//printf("%s\n", token); 
	      	*temp=token;
	      	*temp++;
			token = strtok(NULL, " "); 
		} 

		*temp=NULL;
}



void exec_for_script_io(char * command)
{
	int fd = open(script_path, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	int sz = write(fd, command, strlen(command));
	sz = write(fd, "\n", strlen("\n"));  
	int x=dup(1);

	if(!fork())
	{
		pid_t pid;
		char * argv[50];
	  	parseCommand_IO(command,argv);

	  	//cout<<argv[0]<<"\t";//<<argv[1];
	  	dup2(fd,1);

	     pid=fork();

	     if(pid==0)
	     {
	     	if(execvp(argv[0], argv) < 0)    
                printf("Error: command not found\n");
	     }
	     else{
	     	wait(NULL);
	     	abort();
	     }
	     
	}
	else
	{
		wait(NULL);
		dup2(x,1);
		close(fd);
	}
}


bool checkIO(char * command)
{
	while(*command)
	{
		if(*command==124 || *command=='>')
			return true;
		command++;
	}
	return false;
}

void split_by_pipe(char * command)
{


		//cout<<"IM";
	  	char *end_str;
		char* token_pipe = strtok_r(command, "|", &end_str); 
		//cout<<token_pipe;

		while(token_pipe!=NULL)
		{
			v.push_back(token_pipe);
			//cout<<token_pipe;
			token_pipe = strtok_r(NULL, "|",&end_str);
		}

		// for(int i=0 ;i<v.size();i++)
		// 	cout<<"\n i "<<i<<v[i];
		//cout<<"\n";

}

void split_by_arrow(char * command)
{
		char *end_str;
		char* token_pipe = strtok_r(command, ">", &end_str); 
		//cout<<token_pipe;

		while(token_pipe!=NULL)
		{
			vector_IO.push_back(token_pipe);
			//cout<<token_pipe;
			token_pipe = strtok_r(NULL, ">",&end_str);
		}
		// for(int i=0 ;i<vector_IO.size();i++)
		// 	cout<<"\n i "<<i<<vector_IO[i];
		// cout<<"\n";

}



bool check_arrow_string(char * command)
{	int count=0;
	bool return_val=false;
	while(*command)
	{
		if(*command=='>'){
			 
			return_val=true;
			count++;
		}

		command++;
	}

	if(count==1)
		check_single_arrow=true;

	return return_val;	
}


void redirecting_to_file( char *c)
{

	//cout<<c;

	split_by_arrow(c);

	char * argv[50];
	parseCommand_IO(vector_IO[1],argv);

	int fd;
	if(check_single_arrow){
		fd = open(argv[0], O_CREAT | O_RDWR | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	}
	else{
		fd = open(argv[0], O_CREAT | O_RDWR | O_APPEND,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	}

	if(!fork())
	{
		char * argve[50];
		parseCommand_IO(vector_IO[0],argve);
		dup2(fd,1);
		execvp(argve[0],argve);
		perror("exec");

	}
	//wait(NULL);
	close(fd);
	//cout<<"exit hogya hu m ";
	//exit(0);
	//exit(0);
}

void handleIO( char * command) 
{

	split_by_pipe(command);

	int i;
	char **argv=NULL;


	if(!fork())

	{
		//pipe(fd);	
	    for( i=0; i<v.size()-1; i++)
	    {
	        int pd[2];
	        pipe(pd);

	        if (!fork()) {

	        	char * argv[50];
	        	parseCommand_IO(v[i],argv);
	            dup2(pd[1], 1); // remap output back to parent
	            execvp(argv[0], argv);
	            perror("exec");
	            //abort();
	        }

	        // remap output from previous child to input
	        
	        dup2(pd[0], 0);
	        close(pd[1]);
	    }

	    if( check_arrow_string(v[i]) )
	    {	

	    	redirecting_to_file(v[i]);
	    	abort();
	    }
	    else
	    {	

	  //   	if(script_flag)
			// {	
			// 	exec_for_script_io(v[i]);
			// }
			//cout<<"\nflow is here\n"<<x;
		    char * argve[50];
		    parseCommand_IO(v[i],argve);
		    //wait(NULL);
		    //dup2(fd[1],0)
		    execvp(argve[0], argve);
		    perror("exec");
		    abort();
		}
	}
	else{
		wait(NULL);
		v.clear();
		vector_IO.clear();
	}
	//cout<<"ram";

}



	//handleIO(argv);


	// char** commands[10];

	// int i=0;

	// char **temp = argv;

	// while(temp!=NULL)
	// {
	// 	char * * temp1;
	// 	while(strcmp(*temp, "|"))
	// 	{
	// 		//temp1=**(commands+i);
	// 		*temp1=*temp;
	// 		*temp1++;
	// 		*temp++;
	// 	}
	// 	temp1=NULL;
	// 	commands[i]=temp1;
	// 	temp++;
	// 	i++;
	// }


	// int pfds[2];
 //    pipe(pfds);

 //    pid_t childpid = fork();

 //    if (childpid == 0) {
 //        /* Child */
 //        dup2(pfds[1],1);
 //        close(pfds[0]); 

 //        execlp("ls", "ls",NULL);

 //    } else {
 //        /* Parent */

 //        pid_t retpid;
 //        int  child_stat;
 //        //while ((retpid = waitpid(childpid, &child_stat, 0)) != childpid && retpid != (pid_t) -1);

 //        wait(NULL);

 //        close(pfds[1]); 

 //        char buf[100];
 //        ssize_t bytesread;

 //        int fd = open("beejoutput.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 //        if (fd == -1) {
 //            fprintf(stderr, "Opening of beejoutput.txt failed!\n");
 //            exit(1);
 //        }

 //        /* This part writes to beejoutput.txt */
 //        while ((bytesread = read(pfds[0], buf, 100)) > 0) {
 //            write(fd, buf, bytesread);
 //        }

 //        lseek(fd, (off_t) 0, SEEK_SET);
 //        dup2(fd, 0);
 //        execlp("wc", "wc", "-l", NULL);
 //    }



	// if(fork()==0)
	// {
	// 	int fd=open("ioCheck.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	// 	dup2(fd,1);
	// 	dup2(fd,2);

	// 	close(fd);

	// 	execvp(argv[0], argv);
	// }
