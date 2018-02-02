#ifndef SFISH_H
#define SFISH_H

/* Format Strings */
#define EXEC_NOT_FOUND "sfish: %s: command not found\n" //
#define JOBS_LIST_ITEM "[%d] %s\n"
#define STRFTIME_RPRMT "%a %b %e, %I:%M%p"



#define BUILTIN_ERROR  "sfish builtin error: %s\n"   //builti
#define SYNTAX_ERROR   "sfish syntax error: %s\n"  //pipe
#define EXEC_ERROR     "sfish exec error: %s\n"    //exec



//new
#define USER_SUFFIX " :: runpan >>"

//#define DIR_NOT_FOUND "sfish: %s: No such file or directory\n"




#define USAGE(input)                                                           \
  do {                                                                         \
    printf("%s",                                                             \
            "\n"                                                               \
            "sfish shell Builtin function\n"                                   \
            "\n"                                                               \
            "function list:\n\n"                                               \
            "help          show this help menu.\n"                             \
            "exit          exit the shell.\n"                                  \
            "cd            change the location.\n"                             \
                     ".    stand for current location\n"                       \
                     "..   stand for father location\n"                        \
                     "-    stand for last location\n"                          \
            "pwd           show current location.\n"                           \
            "color         change the prompt color\n"                          \
                     "RED,GRN,YEL,BLU,MAG,CYN,WHT,BWN\n"                       \
            "fg            make background to foreground\n"                    \
            "jobs          show all jobs in background or foreground\n"        \
            "kill          kill process\n"                                     \
            "\n"                                                               \
            );                                                                 \
  } while (0)






#define COLOR_RED   "\033[31m"
#define COLOR_GRN   "\033[32m"
#define COLOR_YEL   "\033[33m"
#define COLOR_BLU   "\033[34m"
#define COLOR_MAG   "\033[35m"
#define COLOR_CYN   "\033[36m"
#define COLOR_WHT   "\033[37m"
#define COLOR_BWN   "\x1b[38;5;130m"
#define COLOR_RESET "\x1b[0m"

#endif





/*

typedef struct{
    pid_t jobpid;
    char * name;
} jobitem;

jobitem  joblist[512];
*/
char *jobslistname[4096];
int jobslistpid[4096];
int loopindex;

char colorpre[4096];

//bool fg=true;
//char *readlinetemp;
//prompt
void makeprompt(char *buffer);
void wrapcolor(char *buffer);
int getgit();
void gitHandler(int s);

//execute

void execute(int *argcNUM,char *argvList[]);
void exeFG(int argcNUM,char *argvList[]);
void exeBG(int argcNUM,char *argvList[]);
void exeFGRED(int argcNUM,char *argvList[]);
void exeBGRED(int argcNUM,char *argvList[]);
void exeFGPIPE(int argcNUM,char *argvList[]);

bool sonIsFg(int *argcNUM,char *argvList[]);
int findpath(int argcNUM,char *argvList[]);




//helper
int splitInput(char * argvList[], char * input);

void sigchld_handler(int s);


int addjob(pid_t pid,char* name);
int deljobbypid(pid_t pid);
int deljobbyindex(int index);

int splitInputfornewline(char * argvList[], char * input);



//builtin
bool isBuiltIn(char *argvList[]);
void executeBuiltIn(int argcNUM,char *argvList[],char lastpath[]);

int checkBuiltIn(char * argvList[]);
int pwdBuiltin(int argcNum,char * argvList[]);
int cdBuiltin(int argcNum,char *argvList[],char lastPath[]);
int jobsBuiltin(int argcNum);
int fgBuiltin(int argcNum,char *argvList[]);
int  killBuiltin(int argcNUM,char *argvList[]);
int colorBuiltin(int argcNUM,char *argvList[]);


//pipe redir
void commandsub(char* argvList[],char *input);
void runCommandsub(char*argvList[],char *input);


void redirectionAndPipe(char*argvList[],char *input);
void pipeFun(char*argvList[],char *input);
void pipeFunBACK(char*argvList[],char *input);


void rediectionFun(char* input);
int splitPip(char*argvList[],char *input);
void execuCmd(char* cmd);
int checkTimes(char *input,char token);


void getouttargetfd(int *record,char *input);
void getinsourcefd(int* record,char *input);



