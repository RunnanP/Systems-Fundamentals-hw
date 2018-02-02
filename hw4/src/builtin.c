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
bool isBuiltIn(char *argvList[]){
   int temp=checkBuiltIn(argvList);
   if(temp<=8 && temp>=1){
    return true;
   }
   return false;

}


void executeBuiltIn(int argcNUM,char *argvList[],char lastpath[]){
                          int builtinIndex=checkBuiltIn(argvList);
                                                    if (builtinIndex==1){

                                                          exit(EXIT_SUCCESS);
                                                    }else if (builtinIndex==2){
                                                        if(  pwdBuiltin(argcNUM,argvList)==1){
                                                           return;
                                                        }else{
                                                            printf(BUILTIN_ERROR,"pwd: No such file or directory\n");
                                                           return;
                                                        }

                                                    }else if(builtinIndex==3){
                                                            USAGE(input);
                                                         return;

                                                    }else if(builtinIndex==4){
                                                          if(cdBuiltin(argcNUM,argvList,lastpath)==1){
                                                           return;;
                                                          }else{
                                                            printf(BUILTIN_ERROR,"cd: No such file or directory\n");
                                                           return;
                                                          }

                                                    }else if(builtinIndex==5){
                                                            if (jobsBuiltin(argcNUM)==1){
                                                          return;
                                                            }else{
                                                              printf(BUILTIN_ERROR,"jobs: No such jobs\n");
                                                        return;
                                                            }

                                                    }else if (builtinIndex==6){
                                                            int record;
                                                            if((record=fgBuiltin(argcNUM,argvList))==-1){
                                                              printf(BUILTIN_ERROR,"fg: No such jobs\n");
                                                              return;
                                                            }else if(record==0){
                                                              printf(BUILTIN_ERROR,"fg: No such jobs\n");
                                                          return;
                                                            }
                                                            else{

                                                               sigset_t oth,mask,prev;
                                                                sigemptyset(&mask);
                                                                sigaddset(&mask,SIGCHLD);

                                                             sigfillset(&oth);
                                                             sigdelset(&oth,SIGCHLD);


                                                             sigprocmask(SIG_BLOCK,&mask,&prev);

                                                              kill(-record,SIGCONT);

                                                              tcsetpgrp(0,record);

                                                            loopindex=0;
                                                              while(!loopindex){
                                                              sigsuspend(&oth);}


                                                              sigprocmask(SIG_SETMASK,&prev,NULL);

                                                            return;
                                                            }

                                                    }else if(builtinIndex==7){
                                                      int record;
                                                            if((record=killBuiltin(argcNUM,argvList))==-1){
                                                              printf(BUILTIN_ERROR,"kill: No such jobs\n");
                                                              return;
                                                            }else if(record==0){
                                                              printf(BUILTIN_ERROR,"kill: No such jobs\n");
                                                            return;
                                                            }else{
                                                              kill(-record,SIGKILL);

                                                              deljobbypid(record);
                                                              return;
                                                            }

                                                    }else if(builtinIndex==8){
                                                     int record;
                                                           if((record=colorBuiltin(argcNUM,argvList))==-1){
                                                            printf(BUILTIN_ERROR,"color: No such color\n");
                                                          return;
                                                           }else{
                                                           return;
                                                           }

                                                    }






}


int checkBuiltIn(char * argvList[]){

        if (strcmp(argvList[0], "exit") == 0){
                          return 1;
         }else if(strcmp(argvList[0],"pwd")==0){
                          return 2;

         }else if(strcmp(argvList[0],"help")==0){
                          return 3;
         }else if (strcmp(argvList[0],"cd")==0){
                          return 4;
         }else if(strcmp(argvList[0],"jobs")==0){
                          return 5;

        }else if(strcmp(argvList[0],"fg")==0){
                          return 6;

        }else if(strcmp(argvList[0],"kill")==0){
                          return 7;

        }else if(strcmp(argvList[0],"color")==0){
                          return 8;
        }

    return -1;
}




int pwdBuiltin(int argcNum,char * argvList[]){


                            char tempbuff[4096];
                            getcwd(tempbuff,sizeof(tempbuff));
                            printf("%s\n",tempbuff);
                            return 1;



}




int cdBuiltin(int argcNum,char *argvList[],char lastPath[]){




                     if(argcNum==0){
                                char testprompt[4096];
                                getcwd(testprompt,sizeof(testprompt));
                                strcpy(lastPath,testprompt);

                            if(chdir(getenv("HOME"))==-1){
                                return -1;

                            }
                             return 1;

                }else if(strcmp(argvList[1],"-")==0){

                            char  tempp[4096];
                            strcpy(tempp,lastPath);


                            printf("%s\n",lastPath);

                            char testprompt[4096];
                            getcwd(testprompt,sizeof(testprompt));
                            strcpy(lastPath,testprompt);

                    if( chdir(tempp)==-1){
                        return -1;
                    }
                    return 1;


                }else {

                            char testprompt[4096];
                            getcwd(testprompt,sizeof(testprompt));
                            strcpy(lastPath,testprompt);

                    if(chdir(argvList[1])==-1){
                       return -1;

                    }

                    return 1;


                }


        return -1;

}



int jobsBuiltin(int argcNum){
   if (argcNum!=0){
    return -1;
   }else{
       for(int i=0;i<4096;i++){
        if(jobslistpid[i]!=0){
          printf(JOBS_LIST_ITEM,i,jobslistname[i]);
        }
       }

       return 1;
   }

   return 1;
}


int fgBuiltin(int argcNum,char *argvList[]){


  if(argvList[1][0]!='%'){
    return -1;


   }else {
       char *temp=argvList[1]+1;
       int k=atoi(temp);
              if(k<=-1){
        return -1;
       }
       pid_t pid=jobslistpid[k];
       //kill(pid,SIGCONT);
       return pid;
   }
   return -1;

}


int killBuiltin(int argcNUM,char *argvList[]){


   if(argvList[1][0]=='%'){
       char *temp=argvList[1]+1;
       int k=atoi(temp);
       if(k<=-1){
        return -1;
       }
       pid_t pid=jobslistpid[k];

       return pid;

   }else{
       char *temp=argvList[1];
       int pid=atoi(temp);

       return pid;
   }



   return -1;
}




int colorBuiltin(int argcNUM,char *argvList[]){



 if(strcmp(argvList[1],"RED")==0){
     strcpy(colorpre,COLOR_RED);
     return 1;
 }else if(strcmp(argvList[1],"GRN")==0){
     strcpy(colorpre,COLOR_GRN);
     return 1;

 }else if(strcmp(argvList[1],"YEL")==0){
     strcpy(colorpre,COLOR_YEL);
     return 1;
 }else if(strcmp(argvList[1],"BLU")==0){
     strcpy(colorpre,COLOR_BLU);
     return 1;
 }else if(strcmp(argvList[1],"MAG")==0){
     strcpy(colorpre,COLOR_MAG);
     return 1;
 }else if(strcmp(argvList[1],"CYN")==0){
     strcpy(colorpre,COLOR_CYN);
     return 1;
 }else if(strcmp(argvList[1],"WHT")==0){
     strcpy(colorpre,COLOR_WHT);
     return 1;
 }else if(strcmp(argvList[1],"BWN")==0){
     strcpy(colorpre,COLOR_BWN);
     return 1;
 }

return -1;

}