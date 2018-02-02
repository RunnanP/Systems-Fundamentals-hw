#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>

#include "sfish.h"
#include "debug.h"

#include <signal.h>
#include <wait.h>


#include <time.h>

//open
#include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>

//space
#include <sys/ioctl.h>

//string
#include <string.h>




#define UNUSED(x) (void)x

int main(int argc, char *argv[], char* envp[]) {

   //  printf("%s\n",argv[0]);
    // printf("%s\n",argv[1]);

   //  addjob(getpid(),"sfish");
     char input[4096];
     char *readlinetemp;

     char lastpath[4096];
     getcwd(lastpath,sizeof(lastpath));
     bool formtty=true;


         if(!isatty(STDIN_FILENO)) {
            formtty=false;
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }

    }



     while(1){
   //   printf("continue2\n");

   // fg=true;


//int i=0;
//while (envp[i]!=NULL){
//  printf("%s\n",envp[i]);
//  i++;
//}

    signal(SIGCHLD,sigchld_handler);
    signal(SIGINT,SIG_DFL);
    signal(SIGTSTP,SIG_DFL);
    signal(SIGCONT,SIG_DFL);

    char inputBackup[4096];
    char *argvList[4096];


    char promptbuffer[4096];
                    if(formtty==true){
                     makeprompt(promptbuffer);
                   }

                     readlinetemp = readline(promptbuffer);
                    //printf("--%s\n",readlinetemp);
                    if (readlinetemp==NULL){
                      printf("\n");
                      continue;
                    }
                     strcpy(input,readlinetemp);

                     rl_free(readlinetemp);





                    strcpy(inputBackup,input);


                                  int argcNUM=0;

                                  argcNUM=splitInput(argvList,input);


                                  if (argcNUM==-1){
                                    continue;
                                  }


                  if(strstr(inputBackup,"`")!=NULL){

                             commandsub(argvList,inputBackup);

                   }else if (strstr(inputBackup,">")!=NULL || strstr(inputBackup,"<")!=NULL || strstr(inputBackup,"|")!=NULL){

                            redirectionAndPipe(argvList,inputBackup);



                    }else if(isBuiltIn(argvList)==true){
                            executeBuiltIn(argcNUM,argvList,lastpath);

                   }else{
                            execute(&argcNUM,argvList);

                   }

                           // rl_free(input);
                 //  printf("continue\n");
                            continue;



     }
 }


