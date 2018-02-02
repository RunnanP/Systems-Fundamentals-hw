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
void commandsub(char* argvList[],char *input){
    if (checkTimes(input,'`')!=2){
      printf(SYNTAX_ERROR ,"COMMAND_SUBSITUTION: Not many `\n");
      return;
    }

    runCommandsub(argvList,input);
    return;

}

void redirectionAndPipe(char*argvList[],char *input){
       int index=0;
      /* while(input[index]!=0){
         if (input[index]=='&'){
          input[index]=' ';
         }
         index++;
       }*/
if(argvList[0][0]=='<' || argvList[0][0]=='>' || argvList[0][0]=='|'){
      printf(SYNTAX_ERROR,"Can't put < > | in the first place\n");
      return;
}
       if(strstr(input,"|")!=NULL){

            if(strstr(input,"&")==NULL){

              pipeFun(argvList,input);
            }else if(checkTimes(input,'&')!=1){
               printf(SYNTAX_ERROR,"Pipe: too many &\n");
               return;
            }else {

                 while(input[index]!=0){
                       if (input[index]=='&'){
                            input[index]=' ';
                         }
                       index++;
                      }

                  pipeFunBACK(argvList,input);
            }



       }else{
            if(strstr(input,"&")==NULL){

               rediectionFun(input);
            }else if(checkTimes(input,'&')!=1){
               printf(SYNTAX_ERROR,"Redirection: too many &\n");
               return;
            }else {

                 while(input[index]!=0){
                       if (input[index]=='&'){
                            input[index]=' ';
                         }
                       index++;
                      }

                  rediectionFun(input);
            }

       }
       return;


}







void getouttargetfd(int *record,char *input){//>
 //printf("kkk\n");
                                      //  char *firstPartAdd=input;
                                        char *secondPartAdd=strstr(input,">")+1;
                                      //  printf("%s\n",secondPartAdd);
                                        *(strstr(input,">"))=0;

                                       // char *argvfirstList[4096];
                                        char *argvsecondList[4096];
                                       // char firstInput[4096];
                                        char secInput[4096];
                                       // strcpy(firstInput,firstPartAdd);
                                     //   printf("kkk\n");
                                        strcpy(secInput,secondPartAdd);
                                       // int firstNum=splitInput(argvfirstList,firstPartAdd);
                                      //  printf("kkk\n");
                                        int secNum=splitInput(argvsecondList,secondPartAdd);
                                       // UNUSED(firstNum);
                                        UNUSED(secNum);

                                     // int fisrtfd=dup(STDOUT_FILENO);
                                    //    printf("kkk\n");
                                      *record=open(argvsecondList[0],O_RDWR|O_CREAT,S_IRWXU);
                                     // *(strstr(input," "))=0;
                                   //   printf("kkk\n");





}




void getinsourcefd(int* record,char *input){//<
    //  char *firstPartAdd=input;
                                        char *secondPartAdd=strstr(input,"<")+1;
                                        *(strstr(input,"<"))=0;

                                        //char *argvfirstList[4096];
                                        char *argvsecondList[4096];

                                       // char firstInput[4096];
                                        char secInput[4096];
                                       // strcpy(firstInput,firstPartAdd);
                                        strcpy(secInput,secondPartAdd);

                                      //  int firstNum=splitInput(argvfirstList,firstPartAdd);
                                        int secNum=splitInput(argvsecondList,secondPartAdd);
                                        //UNUSED(firstNum);
                                        UNUSED(secNum);

                                     // int fisrtfd=dup(STDIN_FILENO);
                                      *record=open(argvsecondList[0],O_RDWR,S_IRWXU);



}













void rediectionFun(char* input){
//printf("56\n");
 char inputBackup[4096];
  strcpy(inputBackup,input);
 // printf("78\n");
       if (checkTimes(input,'>')>1 || checkTimes(input,'<')>1){
                    printf(SYNTAX_ERROR,"Redirection: too many  < or >\n");
                    return;//wrong
                  }
//printf("cc\n");


    if (strstr(input,"<")==NULL  && strstr(input,">")!=NULL){//>
//printf("GG\n");

                                        char *firstPartAdd=input;
                                        char *secondPartAdd=strstr(input,">")+1;
                                        *(strstr(input,">"))=0;

                                        char *argvfirstList[4096];
                                        char *argvsecondList[4096];
                                        char firstInput[4096];
                                        char secInput[4096];
                                        strcpy(firstInput,firstPartAdd);
                                        strcpy(secInput,secondPartAdd);
                                        int firstNum=splitInput(argvfirstList,firstPartAdd);
                                        int secNum=splitInput(argvsecondList,secondPartAdd);
                                        UNUSED(firstNum);
                                        UNUSED(secNum);

                                      int fisrtfd=dup(STDOUT_FILENO);
                                      int secfd=open(argvsecondList[0],O_RDWR|O_CREAT,S_IRWXU);
                                      if (secfd==-1){
                                       printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                       close(secfd);//???????
                                       close(fisrtfd);
                                       return;
                                      }


                                      dup2(secfd,STDOUT_FILENO);

                                                  exeFGRED(firstNum,argvfirstList);
                                                                  dup2(fisrtfd,STDOUT_FILENO);
                                                                    close(secfd);
                                                                    close(fisrtfd);






                                                               return;




    }else if(strstr(input,"<")!=NULL  && strstr(input,">")==NULL){//<

                                         char *firstPartAdd=input;
                                        char *secondPartAdd=strstr(input,"<")+1;
                                        *(strstr(input,"<"))=0;

                                        char *argvfirstList[4096];
                                        char *argvsecondList[4096];

                                        char firstInput[4096];
                                        char secInput[4096];
                                        strcpy(firstInput,firstPartAdd);
                                        strcpy(secInput,secondPartAdd);

                                        int firstNum=splitInput(argvfirstList,firstPartAdd);
                                        int secNum=splitInput(argvsecondList,secondPartAdd);
                                        UNUSED(firstNum);
                                        UNUSED(secNum);

                                      int fisrtfd=dup(STDIN_FILENO);
                                      int secfd=open(argvsecondList[0],O_RDWR,S_IRWXU);
                                      if (secfd==-1){
                                       printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                       close(secfd);//??o
                                       close(fisrtfd);
                                       return;
                                      }

                                      UNUSED(secfd);
                                      //  int secfd=open("77.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
                                    //  printf("kkkkkkk");
                                     dup2(secfd,STDIN_FILENO);



                                                  exeFGRED(firstNum,argvfirstList);
                                                        dup2(fisrtfd,STDIN_FILENO);
                                                                close(secfd);
                                                              close(fisrtfd);



                                                           return;

    }else if(strstr(input,"<")<strstr(input,">") ){//<>
                           char *firstPartAdd=input;
                           char *secondPartAdd=strstr(input,"<")+1;
                           char *thirdPartAdd=strstr(input,">")+1;
                           *(strstr(input,"<"))=0;
                           *(thirdPartAdd-1)=0;

                           char *argvfirstList[4096];
                           char *argvsecondList[4096];
                           char *argvthirdList[4096];

                            char firstInput[4096];
                            char secInput[4096];
                            char thirdInput[4096];

                            strcpy(firstInput,firstPartAdd);
                            strcpy(secInput,secondPartAdd);
                            strcpy(thirdInput,thirdPartAdd);


                            int firstNum=splitInput(argvfirstList,firstPartAdd);
                            int secNum=splitInput(argvsecondList,secondPartAdd);
                            int thirdNum=splitInput(argvthirdList,thirdPartAdd);

                          //  printf("%s\n",firstPartAdd);
                           // printf("%s\n",secondPartAdd);
                           // printf("%s\n",thirdPartAdd);
                            UNUSED(firstNum);
                            UNUSED(secNum);
                            UNUSED(thirdNum);




                               int infd=dup(STDIN_FILENO);
                               int outfd=dup(STDOUT_FILENO);

                               int secfd=open(argvsecondList[0],O_RDWR,S_IRWXU);
                               if(secfd==-1){
                                close(secfd);
                                close(infd);
                                    close(outfd);
                                     printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                       return;
                               }

                               int thirdfd=open(argvthirdList[0],O_RDWR|O_CREAT,S_IRWXU);
                              if (thirdfd==-1){
                              close(infd);
                                    close(outfd);
                                close(thirdfd);//????
                                    printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                       return;
                               }


                                      //  int secfd=open("77.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
                                    //  printf("kkkkkkk");
                                     dup2(secfd,STDIN_FILENO);
                                     dup2(thirdfd,STDOUT_FILENO);
                                    //  printf("ppp");
                                      //fflush(stdout);
                                     exeFGRED(firstNum,argvfirstList);//error later


                                     dup2(infd,STDIN_FILENO);
                                     dup2(outfd,STDOUT_FILENO);
                                    close(infd);
                                    close(outfd);
                                    close(secfd);
                                    close(thirdfd);




                                    return;







    }else if(strstr(input,">")<strstr(input,"<") ){    //><

                            char *firstPartAdd=input;
                           char *secondPartAdd=strstr(input,">")+1;
                           char *thirdPartAdd=strstr(input,"<")+1;
                           *(strstr(input,">"))=0;
                           *(thirdPartAdd-1)=0;

                           char *argvfirstList[4096];
                           char *argvsecondList[4096];
                           char *argvthirdList[4096];

                            char firstInput[4096];
                            char secInput[4096];
                            char thirdInput[4096];

                            strcpy(firstInput,firstPartAdd);
                            strcpy(secInput,secondPartAdd);
                            strcpy(thirdInput,thirdPartAdd);


                            int firstNum=splitInput(argvfirstList,firstPartAdd);
                            int secNum=splitInput(argvsecondList,secondPartAdd);
                            int thirdNum=splitInput(argvthirdList,thirdPartAdd);


                            UNUSED(firstNum);
                            UNUSED(secNum);
                            UNUSED(thirdNum);





                               int infd=dup(STDIN_FILENO);
                               int outfd=dup(STDOUT_FILENO);

                               int secfd=open(argvsecondList[0],O_RDWR|O_CREAT,S_IRWXU);
                               if(secfd==-1){
                                close(infd);
                                close(outfd);
                                close(secfd);
                                printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                return;
                               }
                               int thirdfd=open(argvthirdList[0],O_RDWR,S_IRWXU);
                               if ( thirdfd==-1){
                                close(infd);
                                close(outfd);

                                close(thirdfd);
                                       printf(SYNTAX_ERROR,"Redirection: wrong doc file\n");
                                       return;
                               }

                                      //  int secfd=open("77.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
                                    //  printf("kkkkkkk");
                                     dup2(thirdfd,STDIN_FILENO);
                                     dup2(secfd,STDOUT_FILENO);
                                    //  printf("ppp");
                                      //fflush(stdout);
                                     exeFGRED(firstNum,argvfirstList);//error later

                                    dup2(infd,STDIN_FILENO);
                                    dup2(outfd,STDOUT_FILENO);
                                    close(infd);
                                    close(outfd);
                                    close(secfd);
                                    close(thirdfd);




                                    return;



    }


printf(SYNTAX_ERROR,"Redirection: wrong arg\n");
return;
}




int splitPip(char*argvList[],char *input){
       int argcNumber=0;
       char *startPoint;
       char *endPoint;
       if (input[0]==0){
        return -1;
       }
       startPoint=input;
       endPoint=input;


       while (endPoint[0]!=0 && endPoint[0]!='\n'){

                  if( endPoint[0]!='|'){

                    endPoint++;

                    continue;

                  }else if( endPoint[0]=='|'){

                    argvList[argcNumber]=startPoint;

                    argcNumber++;
                    endPoint[0]=0;
                    endPoint++;
                    startPoint=endPoint;

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
















void execuCmd(char* cmd){
     char *list[4096];
     int temp=splitInput(list,cmd);

         //   exeFGRED(temp,list);
     exeFGPIPE(temp,list);

  return;

}






int checkTimes(char *input,char token){
   int record=0;
   int index=0;
   while(input[index]!=0){
    if (input[index]==token){
        record++;
    }
    index++;
   }

   return record;

}









void runCommandsub(char*argvList[],char *input){
   //int record=0;
   int index=0;
   while(input[index]!=0){
    if (input[index]=='`'){
        input[index]=' ';
    }
    index++;
   }

   int argcNUM=splitInput(argvList,input);
   //   printf(SYNTAX_ERROR ,"COMMAND_SUBSITUTION: Not correct style\n");
   //   return;
  // }

  // exeFG(argcNUM-1,argvList+1);







                                 int p[2];
                                      pid_t pid;
                                      int infd=0;
                                      //int i=0;

                                          pipe(p);

                                          if( (pid=fork() )==-1){
                                            exit(0);
                                          }else if(pid == 0){

                                             // dup2(infd, STDIN_FILENO);//dup2(fd_in,0);

                                             // if(argvList[i+1]!=NULL){
                                                dup2(p[1],STDOUT_FILENO);
                                              //}

                                              close(p[0]);
                                              exeFG(argcNUM-1,argvList+1);

                                              exit(0);

                                          }else{
                                            int staus;
                                            waitpid(-1,&staus,0);

                                            close(p[1]);
                                            infd=p[0];

                                          }


                                     char commandsubBuffer[4096*10];

                                     //pipe(p);
                                     //close(p[1]);
                                     read(infd,commandsubBuffer,4096);


                                     int recordnum=splitInputfornewline(argvList+1,commandsubBuffer);

                                     exeFG(recordnum+1,argvList);
                                    // execvp(argvList[0],argvList);


}








void pipeFun(char*argvList[],char *input){


    sigset_t try;

    sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



   char stringbackup[4096];
   strcpy(stringbackup,input);



      int numofcmd=splitPip(argvList,input);


        // int insourcefd=STDIN_FILNO;
        // int outtargetfd=STDOUT_FILENO;
         if (checkTimes(argvList[0],'<')>1 || checkTimes(argvList[0],'>')!=0 || checkTimes(argvList[numofcmd],'<')!=0 || checkTimes(argvList[numofcmd],'>')>1){
             printf(SYNTAX_ERROR,"Pipe: syntax_error\n");
             return;
         }

         for (int k=1;k<numofcmd;k++){
            if(checkTimes(argvList[k],'<')!=0   || checkTimes(argvList[k],'>')!=0){
              printf(SYNTAX_ERROR,"Pipe: syntax_error\n");
              return;
            }
         }

                                  int inTback=dup(STDIN_FILENO);
                                  int outTback=dup(STDOUT_FILENO);
                                  UNUSED(inTback);
                                  UNUSED(outTback);


                                int insourcefd=STDIN_FILENO;
                                int outtargetfd=STDOUT_FILENO;



                                       bool markin=false;
                                       bool markout=false;
                                       UNUSED(markin);
                                       UNUSED(markout);
                                       if(strstr(argvList[0],"<")!=NULL){
                                          getinsourcefd(&insourcefd,argvList[0]);
                                          if(insourcefd==-1){
                                            printf(SYNTAX_ERROR,"Pipe: open file error\n");
                                            return;
                                          }
                                          markin=true;
                                       }

                                       if(strstr(argvList[numofcmd],">")!=NULL){
                                      //  printf("2\n");
                                          getouttargetfd(&outtargetfd,argvList[numofcmd]);
                                        //  printf("%d\n",outtargetfd);
                                          if(outtargetfd==-1){
                                            printf(SYNTAX_ERROR,"Pipe: open file error\n" );
                                            return;
                                          }
                                          markout=true;  //mark to close
                                       }





    int sonpid=fork();


      if (sonpid==0){
                setpgid(0,0);
                int groupid=getpid();

                                  int pipelist[numofcmd*2];
                                  for (int k=0;k<numofcmd;k++){
                                    pipe(pipelist+k*2);
                                  }


                                //  dup2(insourcefd,STDIN_FILENO);
                                //  dup2(pipelist[1],STDOUT_FILENO);
                                //  close(pipelist[0]);
                                //  execuCmd(argvList[0]);




                                  int i=0;    //record the argvList
                                  int j=0;    //record pipe
                                  pid_t pid=0;



 // printf("%s\n",argvList[0]);
 //  printf("%s\n",argvList[1]);
 //  printf("sss\n");
 //   printf("%s\n",argvList[2]);
 //   printf("llllll\n");
 // exit(0);
                                      while(argvList[i]!=NULL){
                                                 // pipe(pipelist+(i*2));
                                          pid=fork();
                                          if(pid == 0){



                                                      setpgid(getpid(),groupid);

                                                if (i!=0){
                                                      dup2(pipelist[j-2], STDIN_FILENO);//dup2(fd_in,0);
                                                    }else if(i==0){
                                                      dup2(insourcefd,STDIN_FILENO);
                                                   //   setpid()
                                                    }


                                                      if(argvList[i+1]!=NULL){
                                                         dup2(pipelist[j+1],STDOUT_FILENO);
                                                      }else if(argvList[i+1]==NULL){
                                                         dup2(outtargetfd,STDOUT_FILENO);
                                                        // read(outtargetfd,)

                                                      }



                                                     // close(pipelist[i*2]);


                                                      for(int l = 0; l < 2*numofcmd; l++){
                                                              close(pipelist[l]);
                                                      }


                                                      execuCmd(argvList[i]);

                                                      //exit(0);

                                                  }else{

                                                    setpgid(pid,groupid);

                                                    i++;
                                                    j+=2;
                                                  }


                                              }

 // printf("%s\n",argvList[0]);
 //  printf("%s\n",argvList[1]);
 //  printf("sss\n");
 //   printf("%s\n",argvList[2]);
  //  printf("llllll\n");
  //exit(0);

                                                for(int p = 0; p < 2 * numofcmd; p++){
                                                           close(pipelist[p]);
                                                   }

                                                for(int p = 0; p < numofcmd + 1; p++){
                                                          wait(NULL);
                                                        }
                                                tcsetpgrp(0,getppid());
                                                exit(0);



                }else{
                       setpgid(sonpid,0);
                       addjob(sonpid,stringbackup);
                       //printf("dpp\n");
                       tcsetpgrp(0,sonpid);
                       //son have to died in totally
                      // signal(SIGCHLD,sigchld_handler2);
                      // waitpid(sonpid,NULL,WUNTRACED);
                      // tcsetpgrp(0,getpid());
                       sigsuspend(&try);
                      // signal(SIGCHLD,sigchld_handler);
                       return;



                }







}














void pipeFunBACK(char*argvList[],char *input){
  //printf("1\n");

    sigset_t try;

    sigfillset(&try);
        sigdelset(&try,SIGCHLD);
        sigdelset(&try,SIGCONT);



   char stringbackup[4096];
   strcpy(stringbackup,input);



      int numofcmd=splitPip(argvList,input);


        // int insourcefd=STDIN_FILNO;
        // int outtargetfd=STDOUT_FILENO;
         if (checkTimes(argvList[0],'<')>1 || checkTimes(argvList[0],'>')!=0 || checkTimes(argvList[numofcmd],'<')!=0 || checkTimes(argvList[numofcmd],'>')>1){
             printf(SYNTAX_ERROR,"Pipe: syntax_error\n");
             return;
         }

         for (int k=1;k<numofcmd;k++){
            if(checkTimes(argvList[k],'<')!=0   || checkTimes(argvList[k],'>')!=0){
              printf(SYNTAX_ERROR,"Pipe: syntax_error\n");
              return;
            }
         }

                                  int inTback=dup(STDIN_FILENO);
                                  int outTback=dup(STDOUT_FILENO);
                                  UNUSED(inTback);
                                  UNUSED(outTback);


                                int insourcefd=STDIN_FILENO;
                                int outtargetfd=STDOUT_FILENO;



                                       bool markin=false;
                                       bool markout=false;
                                       UNUSED(markin);
                                       UNUSED(markout);
                                       if(strstr(argvList[0],"<")!=NULL){
                                          getinsourcefd(&insourcefd,argvList[0]);
                                          if(insourcefd==-1){
                                            printf(SYNTAX_ERROR,"Pipe: open file error\n");
                                            return;
                                          }
                                          markin=true;
                                       }

                                       if(strstr(argvList[numofcmd],">")!=NULL){

                                          getouttargetfd(&outtargetfd,argvList[numofcmd]);

                                          if(outtargetfd==-1){
                                            printf(SYNTAX_ERROR,"Pipe: open file error\n" );
                                            return;
                                          }
                                          markout=true;  //mark to close
                                       }







    int sonpid=fork();


      if (sonpid==0){
                setpgid(0,0);
                int groupid=getpid();

                                  int pipelist[numofcmd*2];
                                  for (int k=0;k<numofcmd;k++){
                                    pipe(pipelist+k*2);
                                  }


                                  int i=0;    //record the argvList
                                  int j=0;    //record pipe
                                  pid_t pid=0;




                                      while(argvList[i]!=NULL){
                                                 // pipe(pipelist+(i*2));
                                          pid=fork();
                                          if(pid == 0){
                                                      setpgid(getpid(),groupid);

                                                if (i!=0){
                                                      dup2(pipelist[j-2], STDIN_FILENO);//dup2(fd_in,0);
                                                    }else if(i==0){
                                                      dup2(insourcefd,STDIN_FILENO);
                                                    }


                                                      if(argvList[i+1]!=NULL){
                                                         dup2(pipelist[j+1],STDOUT_FILENO);
                                                      }else if(argvList[i+1]==NULL){
                                                         dup2(outtargetfd,STDOUT_FILENO);
                                                        // read(outtargetfd,)

                                                      }



                                                     // close(pipelist[i*2]);


                                                      for(int l = 0; l < 2*numofcmd; l++){
                                                              close(pipelist[l]);
                                                      }


                                                      execuCmd(argvList[i]);

                                                      //exit(0);

                                                  }else{

                                                    setpgid(pid,groupid);

                                                    i++;
                                                    j+=2;
                                                  }


                                              }

                                                 // dup2(pipelist[numofcmd*2-2],outtargetfd);
                                                 // close(pipelist)

                                                for(int p = 0; p < 2 * numofcmd; p++){
                                                           close(pipelist[p]);
                                                   }

                                                for(int p = 0; p < numofcmd + 1; p++)
                                                          wait(NULL);



                }else{
                       setpgid(sonpid,0);
                       addjob(sonpid,stringbackup);
                       //printf("dpp\n");
                       //block child
                       signal(SIGCHLD,SIG_IGN);
                       kill(sonpid,SIGTSTP);
                       signal(SIGCHLD,sigchld_handler);
                       //tcsetpgrp(0,);

                      // sigsuspend(&try);

                       return;



                }







}

