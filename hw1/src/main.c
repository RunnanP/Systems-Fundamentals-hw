#include <stdlib.h>
#include <stdio.h>

#include "hw1.h"
#include "debug.h"




#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    unsigned short mode;


    mode = validargs(argc, argv);


    debug("Mode: 0x%X", mode);

    if(mode == 0x0000){
        USAGE(*argv,EXIT_FAILURE);
        return EXIT_FAILURE;
    }    /*FILURE SITUATION*/


    if(mode & 0x8000) {
        USAGE(*argv, EXIT_SUCCESS);
        return EXIT_SUCCESS;
    }                /*-h situation*/


    if(mode & 0x4000) {
        return fCipherCall(argc,argv,mode);


      }else {
        return pCipherCall(argc,argv,mode);
    }

    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */