#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>

 #include <sys/stat.h>
 //#include <unistsd.h >


#include <wait.h>
//#include <sys/types.h>
#include <signal.h>


#include "sfish.h"
#include "debug.h"






#include <sys/types.h>

 #include <fcntl.h>

#define UNUSED(x) (void)x

int splitInput(char * argvList[], char * input){

       int argcNumber=0;
       char *startPoint;
       char *endPoint;
       if (input[0]==0){
        return -1;
       }
       startPoint=input;
       endPoint=input;


       while (endPoint[0]!=0 && endPoint[0]!='\n'){

                  if(startPoint == endPoint && (endPoint[0]!=' ' && endPoint[0]!='\t')){

                    endPoint++;

                    continue;
                  }else if(startPoint ==endPoint && (endPoint[0]==' ' || endPoint[0]=='\t' )){

                    startPoint++;
                    endPoint++;

                    continue;
                  }else if(startPoint !=endPoint && (endPoint[0]==' ' || endPoint[0]=='\t')){

                    argvList[argcNumber]=startPoint;

                    argcNumber++;
                    endPoint[0]=0;
                    endPoint++;
                    startPoint=endPoint;
                    continue;

                  }else if(startPoint !=endPoint && (endPoint[0]!=' ' && endPoint[0]!='\t')){

                     endPoint++;

                     continue;

                  }


       }



       if(startPoint!=endPoint){
        argvList[argcNumber]=startPoint;
        endPoint[0]=0;

       }else if(startPoint==endPoint){
        argcNumber--;
       }

       argvList[argcNumber+1]=NULL;
       return argcNumber;


}











int splitInputfornewline(char * argvList[], char * input){

       int argcNumber=0;
       char *startPoint;
       char *endPoint;
       if (input[0]==0){
        return -1;
       }
       startPoint=input;
       endPoint=input;


       while (endPoint[0]!=0){

                  if(startPoint == endPoint && endPoint[0]!='\n'){

                    endPoint++;

                    continue;
                  }else if(startPoint ==endPoint && endPoint[0]=='\n' ){

                    startPoint++;
                    endPoint++;

                    continue;
                  }else if(startPoint !=endPoint && endPoint[0]=='\n'){

                    argvList[argcNumber]=startPoint;

                    argcNumber++;
                    endPoint[0]=0;
                    endPoint++;
                    startPoint=endPoint;
                    continue;

                  }else if(startPoint !=endPoint  && endPoint[0]!='\n'){

                     endPoint++;

                     continue;

                  }


       }



       if(startPoint!=endPoint){
        argvList[argcNumber]=startPoint;
        endPoint[0]=0;

       }else if(startPoint==endPoint){
        argcNumber--;
       }

       argvList[argcNumber+1]=NULL;
       return argcNumber;


}














int addjob(pid_t pid,char* name){

   for (int i=0;i<4096;i++){
        if(jobslistpid[i]==0){
          jobslistpid[i]=pid;
          //jobslistname[i]=name;
          char *temp=malloc(64);
         strcpy(temp,name);
          jobslistname[i]=temp;
          return i;
        }
   }
   return -1;

}



int deljobbypid(pid_t pid){

   for (int i=0;i<4096;i++){
    if(jobslistpid[i]==pid){
      jobslistpid[i]=0;
      //joblistname[i]
      free(jobslistname[i]);
      return 1;
    }
   }
   return -1;

}

int deljobbyindex(int index){
  if(jobslistpid[index]==0){
    return -1;
  }else{


  jobslistpid[index]=0;
  free(jobslistname[index]);
   return 1;
 }

}










void sigchld_handler(int s){
  signal(SIGCHLD,SIG_IGN);
  int status;


   if ( (  loopindex=waitpid(-1, &status,  WUNTRACED)  )   >0  ){

       if (WIFEXITED(status)){

           deljobbypid(loopindex);


       }else if(WIFSIGNALED(status)){

          deljobbypid(loopindex);


       }else if(WIFSTOPPED(status)){

       }



    }
  kill(getpid(),SIGCONT);
  tcsetpgrp(0,getpgrp());
  signal(SIGCHLD,sigchld_handler);

}

/*
void pipehandler(int s){
  signal(SIGCHLD,SIG_IGN);
  int status;
  UNUSED(status);
  waitpid(-1,&status,WNOHANG);








  tcsetpgrp(0,getpid());
  signal(SIGCHLD,sigchld_handler);

}
*/
/*
void pipehandler2(int s){
  signal(SIGCHLD,SIG_IGN);
  int status;

  UNUSED(status);

  waitpid(-1, &status,  WUNTRACED);

       if (WIFEXITED(status)){

           deljobbypid(loopindex);


       }else if(WIFSIGNALED(status)){

          deljobbypid(loopindex);


       }else if(WIFSTOPPED(status)){

       }

  tcsetpgrp(0,getpid());
  signal(SIGCHLD,sigchld_handler);

}
*/

/*
void pipereturnimmhandler(int s){
     signal(SIGCHLD,SIG_IGN);
int status;

waitpid(-1,&status,WNOHANG);
     signal(SIGCHLD,sigchld_handler);

}
*/


/*
void sigchld_handler2(int s){
  signal(SIGCHLD,SIG_IGN);
  int status;
  printf("ddd\n");


   if ( ( waitpid(-1, &status,  WUNTRACED)  )   >0  ){

       if (WIFEXITED(status)){


           deljobbypid(loopindex);


       }else if(WIFSIGNALED(status)){

          deljobbypid(loopindex);


       }else if(WIFSTOPPED(status)){

       }



    }
  //kill(getpid(),SIGCONT);
  tcsetpgrp(0,getpgrp());
  signal(SIGCHLD,sigchld_handler);

}*/