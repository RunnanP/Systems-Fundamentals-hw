
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

void execute(int *argcNUM,char *argvList[]){
    bool fg=sonIsFg(argcNUM,argvList);


    char commondbackup[4096];
    strcpy(commondbackup,argvList[0]);

    if(findpath(*argcNUM,argvList)==-1){

        printf(EXEC_NOT_FOUND, commondbackup);
        return;

    }

    if (fg==true){
         exeFG(*argcNUM,argvList);
         return;
    }else{
         exeBG(*argcNUM,argvList);
        return;
    }




}




void exeFG(int argcNUM,char *argvList[]){
   // struct stat sb;
         pid_t pid;

         sigset_t mask, prev,try;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);    //sigchld handler installed


        sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



                            sigprocmask(SIG_BLOCK,&mask,&prev);

                             if (   (pid=fork()  )==0){
                                  setpgid(0,0);
                                  sigprocmask(SIG_SETMASK,&prev,NULL);

                                        if(execvp(argvList[0],argvList)<0){
                                            printf(EXEC_ERROR, "Execute error\n");
                                            exit(0);
                                        }


                                }

                                          setpgid(pid,0);
                                          addjob(pid,argvList[0]);



                                          tcsetpgrp(0,pid);




                                         loopindex=0;
                                         while(!loopindex)
                                            sigsuspend(&try);

                                            sigprocmask(SIG_SETMASK,&prev,NULL);
                                            return;


}




void exeBG(int argcNUM,char *argvList[]){
       // struct stat sb;
         pid_t pid;

         sigset_t mask, prev,try;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);    //sigchld handler installed


        sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



                            sigprocmask(SIG_BLOCK,&mask,&prev);

                             if (   (pid=fork()  )==0){
                                  setpgid(0,0);
                                  //sigcont....
                                  sigprocmask(SIG_SETMASK,&prev,NULL);

                                        if(execvp(argvList[0],argvList)<0){
                                            printf(EXEC_ERROR, "Execute error\n");
                                            exit(0);
                                        }


                                }

                                          setpgid(pid,0);
                                          addjob(pid,argvList[0]);

                                         sigprocmask(SIG_SETMASK,&prev,NULL);
                                         return;


}





void exeFGRED(int argcNUM,char *argvList[]){

         pid_t pid;

         sigset_t mask, prev,try;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);    //sigchld handler installed


        sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



if(strcmp(argvList[0],"pwd")==0){


                   sigprocmask(SIG_BLOCK,&mask,&prev);
                   if (   (pid=fork()  )==0){
                   setpgid(0,0);
                  char tempbuff[4096];
                  getcwd(tempbuff,sizeof(tempbuff));
                  printf("%s\n",tempbuff);
                  exit(0);

                  }


                            setpgid(pid,0);
                            addjob(pid,argvList[0]);
                                        tcsetpgrp(0,pid);
                                        loopindex=0;
                                        while(!loopindex)
                                            sigsuspend(&try);
                                        sigprocmask(SIG_SETMASK,&prev,NULL);

                                    return;











}else if(strcmp(argvList[0],"help")==0){

         sigprocmask(SIG_BLOCK,&mask,&prev);
                   if (   (pid=fork()  )==0){
                      USAGE(argvList[0]);
                  exit(0);

                  }


                            setpgid(pid,0);
                            addjob(pid,argvList[0]);
                                      tcsetpgrp(0,pid);
                                        loopindex=0;
                                        while(!loopindex)
                                           sigsuspend(&try);
                                        sigprocmask(SIG_SETMASK,&prev,NULL);

                                    return;


}


















                            sigprocmask(SIG_BLOCK,&mask,&prev);

                             if (   (pid=fork()  )==0){
                                  setpgid(0,0);
                                  sigprocmask(SIG_SETMASK,&prev,NULL);
                                   // printf("%s\n",argvList);
                                        if(execvp(argvList[0],argvList)<0){
                                            printf(SYNTAX_ERROR, "Execute error\n");
                                            exit(-1);
                                        }


                                }

                                          setpgid(pid,0);
                                          addjob(pid,argvList[0]);



                                          tcsetpgrp(0,pid);




                                         loopindex=0;
                                         while(!loopindex)
                                            sigsuspend(&try);

                                            sigprocmask(SIG_SETMASK,&prev,NULL);
                                            return;


}








void exeBGRED(int argcNUM,char *argvList[]){
    //    struct stat sb;
         pid_t pid;

         sigset_t mask, prev,try;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);    //sigchld handler installed


        sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



                            sigprocmask(SIG_BLOCK,&mask,&prev);

                             if (   (pid=fork()  )==0){
                                  setpgid(0,0);
                                  //sigcont....
                                  sigprocmask(SIG_SETMASK,&prev,NULL);

                                        if(execvp(argvList[0],argvList)<0){
                                            printf(SYNTAX_ERROR, "Execute error\n");
                                            exit(0);
                                        }


                                }

                                          setpgid(pid,0);
                                          addjob(pid,argvList[0]);

                                         sigprocmask(SIG_SETMASK,&prev,NULL);
                                         return;


}









bool sonIsFg(int *argcNUM,char *argvList[]){

      int sl=strlen(argvList[*argcNUM]);
      if(argvList[*argcNUM][sl-1]=='&'){
        if(sl==1){
            argvList[*argcNUM]=NULL;
            *argcNUM=*argcNUM-1;
        }else{
            argvList[*argcNUM][sl-1]=0;
        }
        return false;
      }else {
        return true;
      }
}





//1 can try to execute,-1 not such pro
int findpath(int argcNUM,char *argvList[]){
   struct stat sb;
        char commandbackup[4096];
        char *tt;
        tt=argvList[0];
        strcpy(commandbackup,tt);

        if(strstr(argvList[0],"/")==NULL){  //ls     ...have to find




                                              char * tempbuff;
                                              tempbuff=getenv("PATH");
                                              int startindex=0;
                                              int endindex=0;

                                              char stringTempBuffer[4096];
                                              while( endindex!=strlen(tempbuff) ){


                                                           if (tempbuff[endindex]==':'){

                                                              strncpy(stringTempBuffer,tempbuff+startindex,endindex-startindex);
                                                              stringTempBuffer[endindex-startindex]=0;

                                                              strcat(stringTempBuffer,"/");
                                                              strcat(stringTempBuffer,tt);

                                                              endindex++;
                                                              startindex=endindex;

                                                               argvList[0]=stringTempBuffer;

                                                                         if(stat(stringTempBuffer,&sb)!=-1){

                                                                               return 1;



                                                                         }else{
                                                                            continue;
                                                                         }


                                                           }else{
                                                              endindex++;
                                                           }

                                              }


                                              return -1;





        }else {

            if(stat(argvList[0],&sb)!=-1){
                   return 1;
            }else{
              return -1;
            }

        }
    return -1;

}











void exeFGPIPE(int argcNUM,char *argvList[]){


if(strcmp(argvList[0],"pwd")==0){



                  char tempbuff[4096];
                  getcwd(tempbuff,sizeof(tempbuff));
                  printf("%s\n",tempbuff);
                  exit(0);



}else if(strcmp(argvList[0],"help")==0){


                      USAGE(argvList[0]);
                  exit(0);





}




              if(execvp(argvList[0],argvList)<0){
                  printf(SYNTAX_ERROR, "Execute error\n");
                  exit(-1);
              }



}

