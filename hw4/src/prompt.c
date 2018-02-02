
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





//#include <unctl.h>

#define UNUSED(x) (void)x

void makeprompt(char *buffer){


//time
  time_t rawtime;
  struct tm * timeinfo;
  char timebuffer [4096];
  UNUSED(timebuffer);

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (timebuffer,4096,STRFTIME_RPRMT,timeinfo);




    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);


 int x = 0, y = 0;
 UNUSED(x);
 UNUSED(y);

x=size.ws_col-strlen(timebuffer);
y=size.ws_row;


char strbuf[4096];
  sprintf(strbuf,"\e[%dG",x-5);


        write(1, "\e[s", strlen("\e[s"));

        write(1,strbuf,strlen(strbuf));

        write(1, timebuffer, strlen(timebuffer));
        write(1,"\e[0G",strlen("\e[0G"));
        write(1, "\e[u", strlen("\e[u"));
        fflush(stdout);

  //      char rightprompt[128];
    //    strcpy(rightprompt,"\e[s");
       // strcat(rightprompt,strbuf);
      //  strcat(rightprompt,"\e[u");



//git
          int gitnumber=getgit();

  char gitbuf[4096]="";
  if( gitnumber!=0){
    sprintf(gitbuf," %d ",gitnumber);
  }


//creat prompt
                                            char prompt[4096];
                                            getcwd(prompt,sizeof(prompt));

                                            char *homelocation=getenv("HOME");
                                            UNUSED(homelocation);
                                            if(strstr(prompt,homelocation)==NULL){
                                                strcpy(buffer,gitbuf);
                                                strcat(buffer,prompt);
                                                strcat(buffer,USER_SUFFIX);
                                            }else{
                                                strcpy(buffer,gitbuf);
                                                strcat(buffer,"~");

                                                strcat(buffer,strstr(prompt,homelocation)+strlen(getenv("HOME")));
                                                strcpy(prompt,buffer);
                                                strcat(buffer,USER_SUFFIX);

                                            }
                                           wrapcolor(buffer);
                                        //   strcat(rightprompt,buffer);
                                          // strcpy(buffer,rightprompt);
}





void wrapcolor(char * buffer){

  char temp[4096];
  strcpy(temp,colorpre);
  strcat(temp,buffer);
  strcat(temp,COLOR_RESET);
  strcpy(buffer,temp);
}



int getgit(){

 char *gitarg[]={"git","status",NULL};
 //char *greparg[]={"grep","modifided:",NULL};

                            int erfd=dup(STDERR_FILENO);
                            int outfd=dup(STDOUT_FILENO);

                            int garbage=open("/dev/null",O_WRONLY );
                            dup2(garbage,STDERR_FILENO);



                                 int p[2];
                                      pid_t pid;
                                      int infd=0;
                                      //int i=0;

                                          pipe(p);


     sigset_t try;
        sigfillset(&try);
        sigdelset(&try,SIGCHLD);

                                          if( (pid=fork() )==-1){
                                            exit(0);
                                          }else if(pid == 0){

                                               // printf("%s\n", getenv("HOME"));
                                             // dup2(infd, STDIN_FILENO);//dup2(fd_in,0);

                                             // if(argvList[i+1]!=NULL){
                                                dup2(p[1],STDOUT_FILENO);
                                              //}

                                              close(p[0]);
                                              //
                                              execvp("git",gitarg);
                                              //exeFG(1,gitarg);

                                              exit(0);

                                          }else{
                                            //int staus;
                                            signal(SIGCHLD,gitHandler);
                                            sigsuspend(&try);
                                            //waitpid(-1,&staus,0);
                                            signal(SIGCHLD,sigchld_handler);
                                            close(p[1]);
                                            infd=p[0];


                                          }


                                     char *gitBuffer=calloc(4,1028);

                                     char *bufferhelp=gitBuffer;
                                    // commandsubBuffer=malloc(4096*10);
                                     //pipe(p);
                                     //close(p[1]);
                                     read(infd,gitBuffer,4096);
                                    // fflush(stdin);
                              // printf("%s\n",gitBuffer);

                                    int modirecord=0;
                                    char* gitincresebuffer;
                                    while((gitincresebuffer=strstr(gitBuffer,"modified:"))!=NULL){
                                      modirecord++;
                                      gitBuffer=gitincresebuffer+1;

                                    }
                                //    printf("EEEEEEE%d\n",modirecord);

                                     dup2(erfd,STDERR_FILENO);
                                     dup2(outfd,STDOUT_FILENO);
                                     //  printf("-------%s\n",bufferhelp);
                                     free(bufferhelp);


                                    return modirecord;


}



void gitHandler(int s){
  int staus;
  waitpid(-1,&staus,0);



}


