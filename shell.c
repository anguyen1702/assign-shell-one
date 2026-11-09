#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BSIZE 256

/***********************************************************************
 * This starter shell implements a simple shell that implements:
 * 
 * (1) An internal EXIT command that exits the shell.
 * (2) Executes a external program without any command line arguments
 * 
 ***********************************************************************/

pid_t currChild = 0;
void handlingSigint(int sig){
    if(currChild > 0){
        kill(currChild, SIGINT);
    }
    write(STDOUT_FILENO, "\n", 1);
}

int main(int argc, char *argv[]){
    char buffer[BSIZE];
    pid_t child;
    int status;
    char *args[10]={"",NULL};

    signal(SIGINT, handlingSigint);
    while(1){
        printf("shell> ");
        if(fgets(buffer,BSIZE-1,stdin) == NULL){
            break;
        }
        
        buffer[(strlen(buffer)-1)]='\0';  //Remove NewLine

        int i = 0;
        char *token = strtok(buffer, " ");
        while(token != NULL){
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        if(args[0] == NULL){
            continue;
        }
        
        //** Internal Command **
        if(strcmp(args[0],"exit")==0){
            break;
        }else if(strcmp(args[0], "cd")==0){
            if(args[1] == NULL){
                fprintf(stderr, "Missing directory\n");
            }else if(chdir(args[1]) != 0){
                perror("cd failed");           
            }
            continue;
        }

        //** External Command **
        child = fork();
        if(child==0){
            //*** Child Process ***
            execvp(args[0],args);
            perror("Command failed");
            exit(0);
        }else if(child > 0){
            //*** Parent Process **
            currChild = child;
            waitpid(child,&status,0);
            currChild = 0;
        }else{
            perror("fork failed");
        }

    }
    return 0;
}