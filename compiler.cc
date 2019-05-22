#ifdef __linux__
    //linux code goes here
	#define LINUX 1
#else
	#define LINUX 0
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define SOUND_PATH "/home/joshua/Projects/screamingCompiler/betterG++/" //Change this line to where you are storing the sound files.

using namespace std;

/**
	Method that plays the sound files
	Pass through true for the success sound, false for failure
	Does terminate the program
*/
void playSound(bool success){
	string path = string(SOUND_PATH) + string(success ? "success.mp3" : "failure.mp3");
	char* param_array[] = {"mpg123", "-q", (char *) path.c_str(), NULL};
	execv("/usr/bin/mpg123", (char * const *) param_array);
}

int main(int argc, char* argv[]){
	if(LINUX){ //Ensure this only runs on Linux systems as this is all I've accounted for so far
		pid_t pid = fork(); //Make a child process
		if(pid==0){ //If we are the child process

			char* arr[argc+1] = {"g++"}; //Construct an array from the arguments passed through cmd line so that execv can use it
			for(int i = 1; i < argc; i++){
				arr[i] = argv[i];
			}
			arr[argc+1] = NULL;
			execv("/usr/bin/g++", (char * const *) arr); //Replace the child process with the g++ process that compiles our code
			// If we get here, something is wrong.
		    perror("/usr/bin/g++");
		    exit(255);
		} else if (pid == -1){ // fork failed
		    perror("fork");
		    exit(1);
		} else { //Boom we are the parent process
			int status;
			if( waitpid(pid, &status, 0) < 0 ) { //Wait for the child process to exit
				//If the child process didn't exit properly then through an error
				perror("wait");
				exit(254);
			}
			if(WIFEXITED(status)) { //If it exited
				//printf("Process %d returned %d\n", pid, WEXITSTATUS(status)); //Debug message
				if(WEXITSTATUS(status) == 0){ //If we exited with a 0 value then it compiled successfully
					playSound(true);
				}else{ //Otherwise the compilation failed
					playSound(false);
				}
				exit(WEXITSTATUS(status)); //Exit with the status of the child processes exit
			}
			if(WIFSIGNALED(status)) { //If core was dumped
				printf("Process %d killed: signal %d%s\n", pid, WTERMSIG(status), WCOREDUMP(status) ? " - core dumped" : "");
				exit(1);
			}
		}
	} else { //Not on a Linux system so the file system may be different
		printf("Warning: Not on linux system.\n");
	}
}
