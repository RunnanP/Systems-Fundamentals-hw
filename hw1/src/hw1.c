#include "hw1.h"
#include <stdlib.h>

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif


unsigned short valiPsituation(int tempPArgc,char **tempPArgv);
unsigned short valiFsituation(int tempFArgc,char **tempFArgv);



unsigned short fKeyCheck(unsigned short fKT,int fKint,int fKeyCheckargc, char **fKeyCheckargv);

unsigned short pKeyCheck(unsigned short pKT,int pKint,int pKeyCheckargc, char **pKeyCheckargv);


unsigned short pAddCol(unsigned short pCT,int pCint,int pColargc, char **pColargv);

unsigned short pAddRow(unsigned short pRT,int pRint,int pRowargc, char **pRowargv);




int pDecrypt  ( unsigned short tempMode);
int pEncrypt  ( unsigned short tempMode);

int fDecrypt(unsigned short tempMode);
int fEncrypt(unsigned short tempMode);
void fEoutput(char fir,char sec,char thir);

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the program
 * and will return a unsigned short (2 bytes) that will contain the
 * information necessary for the proper execution of the program.
 *
 * IF -p is given but no (-r) ROWS or (-c) COLUMNS are specified this function
 * MUST set the lower bits to the default value of 10. If one or the other
 * (rows/columns) is specified then you MUST keep that value rather than assigning the default.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return Refer to homework document for the return value of this function.
 */
unsigned short validargs(int argc, char **argv) {



    /*mode 0 */
    if (argc==1 || argc >9){

        return 0x0000;
    }   /* too few argc NO flag situation*/        /*too many argc*/


    if(**(argv+1)!='-'){

       return 0x0000;

        }else{

            if((*(*(argv+1)+1))=='h' && (*(*(argv+1)+2 ))==0){

                        return 0x8000;     /*-h situation*/

                }

            if((*(*(argv+1)+1))=='p'&& (*(*(argv+1)+2 ))==0){
                                        /*go to p function*/

                        return valiPsituation( argc,  argv);
                }


            if((*(*(argv+1)+1))=='f'&& (*(*(argv+1)+2 ))==0){
                                        /*go to f function*/
                        return valiFsituation( argc, argv);
                }

            return 0x0000;


    }

    return 0x0000;
}
















unsigned short valiPsituation(int tempPArgc,char **tempPArgv){
     unsigned short temp=0x00AA;


     if (tempPArgc>=3){

           if(**(tempPArgv+2)!='-'){

                return 0x0000;

            }else{

                    if((*(*(tempPArgv+2)+1))=='e'&& (*(*(tempPArgv+2)+2 ))==0){

                    temp=temp | 0x0000;      /*default and the e flag*/

                    }else if((*(*(tempPArgv+2)+1))=='d'&& (*(*(tempPArgv+2)+2 ))==0){

                    temp=temp | 0x2000;    /*default and the d flag*/

                    }else return 0x0000;

                }



     }else return 0x0000;



     switch(tempPArgc){

        case 2:    /*   bin/hw1 -p*/
                    return 0x0000;
                    break;
        case 3:   /*    bin/hw1 -p*/


                    if((*(*(tempPArgv+2)+1))=='e'&& (*(*(tempPArgv+2)+2 ))==0){

                    temp=temp | 0x00AA;      /*default and the e flag*/
                    return temp;
                    }



                    if((*(*(tempPArgv+2)+1))=='d'&& (*(*(tempPArgv+2)+2 ))==0){

                    temp=temp | 0x00AA;    /*default and the d flag*/
                    return temp;

                    }


                    return 0x0000;
                    break;

        case 4:
                    return 0x0000;
                    break;

        case 5:
                    if(**(tempPArgv+3)!='-'){

                    return 0x0000;

                    }else {

                                if ((*(*(tempPArgv+3)+1))=='k'&& (*(*(tempPArgv+3)+2 ))==0){


                                    temp=pKeyCheck(temp,4,tempPArgc,tempPArgv);

                                    return temp;

                                }


                                if ((*(*(tempPArgv+3)+1))=='r'&& (*(*(tempPArgv+3)+2 ))==0){


                                    int c=atoi(*(tempPArgv+3+1));
                                    if (c<9 || c>15){
                                        return 0x0000;
                                    }



                                    int signNum=0;
                                    while(*(polybius_alphabet+signNum)!=0){
                                            signNum++;
                                        }
                                    if( c*10< signNum){
                                            return 0x0000;
                                        }


                                    temp=pAddRow(temp,4,tempPArgc,tempPArgv);
                                    return temp;

                                }



                                if ((*(*(tempPArgv+3)+1))=='c'&& (*(*(tempPArgv+3)+2 ))==0){

                                        int c=atoi(*(tempPArgv+3+1));
                                        if (c<9 || c>15){
                                        return 0x0000;
                                }

                                int signNum=0;
                                while(*(polybius_alphabet+signNum)!=0){
                                            signNum++;
                                        }
                                if( c*10< signNum){
                                        return 0x0000;
                                }

                             temp=pAddCol(temp,4,tempPArgc,tempPArgv);
                             return temp;

                                }



                            return 0x0000;



                        }
                        break;


        case 6:
                        return 0x0000;
                        break;
        case 7:

                        if(**(tempPArgv+3)!='-' ||   **(tempPArgv+5)!='-' ||  (*(*(tempPArgv+3)+1))==(*(*(tempPArgv+5)+1))    ){

                                    return 0x0000;

                        }else {
                                    int r=10;
                                    int c=10;

                                    for (int i=3 ;i <=5;i+=2 ){

                                                        if( (*(*(tempPArgv+i)+1))=='k'){

                                                                    temp=pKeyCheck(temp,i+1,tempPArgc,tempPArgv);
                                                                    if(temp==0x0000){
                                                                                return temp;
                                                                    }

                                                        }else if( (*(*(tempPArgv+i)+1))=='r'){

                                                                    r=atoi(*(tempPArgv+i+1));
                                                                    if (r<9 || r>15){
                                                                                return 0x0000;
                                                                    }

                                                                    temp=pAddRow(temp,i+1,tempPArgc,tempPArgv);

                                                        }else if( (*(*(tempPArgv+i)+1))=='c'){

                                                                    c=atoi(*(tempPArgv+i+1));
                                                                    if (c<9 || c>15){
                                                                                return 0x0000;
                                                                    }



                                                                    temp=pAddCol(temp,i+1,tempPArgc,tempPArgv);

                                                        }else return 0x0000;



                                    }

                                    int signNum=0;
                                    while(*(polybius_alphabet+signNum)!=0){
                                            signNum++;
                                        }

                                    if(r*c<signNum){
                                                return 0x0000;
                                    }else  return temp;



                                 }
                         break;




        case 8:
                        return 0x0000;
                        break;
        case 9:
                        if(**(tempPArgv+3)!='-' ||   **(tempPArgv+5)!='-' ||  **(tempPArgv+7)!='-'){


                                    return 0x0000;


                        }




                        if (    (*(*(tempPArgv+3)+1))==(*(*(tempPArgv+5)+1))       ||      (*(*(tempPArgv+3)+1))==(*(*(tempPArgv+7)+1))     ||     (*(*(tempPArgv+5)+1))==(*(*(tempPArgv+7)+1))    ){


                                    return 0x0000;
                            }



                        int r=10;
                        int c=10;




                        for (int i=3 ;i <=7;i+=2 ){

                        if( (*(*(tempPArgv+i)+1))=='k'){

                                    temp=pKeyCheck(temp,i+1,tempPArgc,tempPArgv);
                                    if(temp==0x0000){
                                        return temp;
                                    }

                        }else if( (*(*(tempPArgv+i)+1))=='r'){


                                    r=atoi(*(tempPArgv+i+1));
                                    if (r<9 || r>15){
                                        return 0x0000;
                                    }

                                    temp=pAddRow(temp,i+1,tempPArgc,tempPArgv);


                     }else if( (*(*(tempPArgv+i)+1))=='c'){

                                    c=atoi(*(tempPArgv+i+1));
                                    if (c<9 || c>15){
                                         return 0x0000;
                                     }
                                    temp=pAddCol(temp,i+1,tempPArgc,tempPArgv);

                     }else return 0x0000;



                    }

                    int signNum=0;
                    while(*(polybius_alphabet+signNum)!=0){
                        signNum++;
                    }
                    if(r*c<signNum){
                        return 0x0000;
                    }else  return temp;
                    break;


                }
                return temp;


}






























unsigned short valiFsituation(int tempFArgc,char **tempFArgv){
     unsigned short temp=0x4000;


    if (tempFArgc==2 || tempFArgc >5){

        return 0x0000;
    }


      if(**(tempFArgv+2)!='-' || (*(*(tempFArgv+2)+2 ))!=0){

       return 0x0000;

    }



        if((*(*(tempFArgv+2)+1))=='e'){



                    temp=temp | 0x0000;

                    if (tempFArgc==3){
                        return temp;
                       }else{

                                    if(**(tempFArgv+3)!='-' || *(*(tempFArgv+3)+1) !='k' ||  *(*(tempFArgv+3)+2) !=0      ){

                                    return 0x0000;

                                    }else {

                                        return fKeyCheck(temp,4,tempFArgc,tempFArgv);

                                          }



                    }



        }else if((*(*(tempFArgv+2)+1))=='d'){

                    temp=temp | 0x2000;


                     if (tempFArgc==3){
                        return temp;
                     }else{


                                    if(**(tempFArgv+3)!='-' || *(*(tempFArgv+3)+1 )  !='k' ||  *(*(tempFArgv+3)+2) !=0  ){

                                    return 0x0000;

                                    }else {

                                        return fKeyCheck(temp,4,tempFArgc,tempFArgv);


                                          }

                      }
                    }

       else return 0x0000;


}






unsigned short fKeyCheck(unsigned short fKT,int fKint,int fKeyCheckargc, char **fKeyCheckargv){
       int sign=0;
       char *fKeyAddress=*(fKeyCheckargv+fKint);



       while ((*   ( fKeyAddress+sign) )!=0){
        sign++;

       }


           for (int i=0;i<sign;i++) {

                for (int j=i+1;j<sign;j++){


                 if ((*( fKeyAddress+i)) ==  ((*  (  fKeyAddress+j)))  ){
                        return 0x0000;
                     }

                }
          }




     for (int i=0;i<sign;i++){
       int kCorrectFlag=0;
            for (int j=0;j<26;j++){

                  if   ( (*( fm_alphabet+j))== (*  ( fKeyAddress+i))) {



                    kCorrectFlag=1;

                  }

            }
      if (kCorrectFlag==0){
            return 0x0000;

      }

     }


    key=  fKeyAddress;

     return fKT;


 }






unsigned short pKeyCheck(unsigned short pKT,int pKint,int pKeyCheckargc, char **pKeyCheckargv){


       int sign=0;
       int polybiusLenth=0;
       char *pKeyAddress=*(pKeyCheckargv+pKint);
       while (*( pKeyAddress+sign) != 0){
        sign++;
       }

        while(*(polybius_alphabet+polybiusLenth)!=0){
            polybiusLenth++;
        }

           for (int i=0;i<sign;i++) {

                for (int j=i+1;j<sign;j++){


                 if ((*( pKeyAddress+i) )==  (*  (  pKeyAddress+j))){

                        return 0x0000;
                     }

                }
          }




     for (int i=0;i<sign;i++){
       int kCorrectFlag=0;
       //for (int j=0;j<94;j++){
            for (int j=0;j<polybiusLenth;j++){


                  if   ( (*( polybius_alphabet+j))  == * (pKeyAddress+i) ){
                    kCorrectFlag=1;

                  }

            }
      if (kCorrectFlag==0){

            return 0x0000;

      }

     }




    key=  pKeyAddress;
    return pKT;


}

unsigned short pAddCol(unsigned short pCT,int pCint,int pColargc, char **pColargv){
        unsigned short temp=pCT & 0xFFF0;
        int c=atoi(*(pColargv+pCint));

        switch(c){
            case 9:
                   temp=temp | 0x0009;
                   break;
            case 10:
                   temp=temp | 0x000A;
                   break;
            case 11:
                   temp=temp | 0x000B;
                   break;
            case 12:
                   temp=temp | 0x000C;
                   break;
            case 13:
                   temp=temp | 0x000D;
                   break;
            case 14:
                   temp=temp | 0x000E;
                   break;
            case 15:
                   temp=temp | 0x000F;
                   break;


        }

         return temp;
}


unsigned short pAddRow  (unsigned short pRT,int pRint,int pRowargc, char **pRowargv){

    unsigned short temp=pRT & 0xFF0F;
    int c=atoi(*(pRowargv+pRint));


        switch(c){
            case 9:
                   temp=temp | 0x0090;
                   break;
            case 10:
                   temp=temp | 0x00A0;
                   break;
            case 11:
                   temp=temp | 0x00B0;
                   break;
            case 12:
                   temp=temp | 0x00C0;
                   break;
            case 13:
                   temp=temp | 0x00D0;
                   break;
            case 14:
                   temp=temp | 0x00E0;
                   break;
            case 15:
                   temp=temp | 0x00F0;
                   break;


        }

         return temp;


}


int fCipherCall(int fCipherTempArgc,char **fCipheTempArgv,unsigned short fMode){


       if( NULL==key){

            for(int i=0;i<26;i++){

            *(fm_key+i)=*(fm_alphabet+i);
                }




     }else{

               int keyCount=0;
                    while (*( key+keyCount) != 0){
                            keyCount++;
                         }


                  for (int i=0;i<keyCount;i++){

                        *(fm_key+i)=*(key+i);

                            }






                 int alphabetFlag=0;
                 int tableFlag=0;


                    while(alphabetFlag<26){

                      int bo=-1;


                        for (int m=0;m<keyCount;m++){           //check key loop


                            if (   *(key+m)== *(fm_alphabet+alphabetFlag)){



                                      bo=m;


                           }
                    }


                      if (bo!=-1){


                              alphabetFlag++;



                      }else{

                             *(fm_key+keyCount+tableFlag)=*(fm_alphabet+alphabetFlag);
                                   alphabetFlag++;
                                   tableFlag++;



                      }



                }




        }




          unsigned short deMode=fMode & 0x2000;
          if (deMode==0x2000){

             return fDecrypt(fMode);

          }else {



             return fEncrypt(fMode);
          }











return EXIT_SUCCESS;
}









int fDecrypt(unsigned short tempMode){


     char inputC=9;
     int bufferFlag=0;

     long space;
    char * buffer=(char*)&space;    //every chacter buffer ,will renew every time per character

    int newlineFlag=0;
     while(inputC!=EOF){

        inputC=getchar();
        if(inputC!=-1){

            while (newlineFlag!=0){

                printf("\n");
                newlineFlag--;
            }
        }
        if(inputC==-1){

            break;
        }

        if(inputC=='\n') {
            newlineFlag++;
            continue;
          }
          for (int i=0;i<27;i++){
                 if (inputC==*(fm_key+i) )  {

                  *( polybius_table+bufferFlag)=* ( * (  fractionated_table+i)+0);

                  *(polybius_table+bufferFlag +1  )=* ( * (  fractionated_table+i)+1);
                  *(polybius_table+bufferFlag +2  )=* ( * (  fractionated_table+i)+2);
                  bufferFlag+=3;

                 }

          }


       int    bufferSign =0;
        while (*( polybius_table+bufferSign) != 0){
              bufferSign++;
             }

        int xFlag=0;
        for (int i=0;i<bufferSign;i++){

            if(*(polybius_table+i)=='x'){
                xFlag=1;
            }
        }

        if(xFlag==0){
           continue;
        }


        for (int i=0;i<bufferSign;i++){

                if(*( polybius_table+i) ==  'x' ){

                         if ( i==  0){
                              printf("%c",32);

                              for(int j=0;j<bufferFlag-1;j++){

                                     *(polybius_table+j)=*(polybius_table+j+1);

                              }
                              *(polybius_table+bufferFlag-1)=0;
                              bufferFlag--;
                              break;




                         }else{


                                 for (int j=0;j<90;j++){
                                         if(  **( morse_table+j)!=0){

                                                    int boolTemp=0;
                                                    for(int m=0;m<i;m++){
                                                    if(  *( polybius_table+m)==   *( *( morse_table+j)+m) ){

                                                                     boolTemp++;
                                                    }else{
                                                                     boolTemp--;
                                                         }


                                                    }

                                                    if(boolTemp==i  && ( *( *( morse_table+j)+i)==0)      ){

                                                                   printf("%c",* ( polybius_alphabet+j)  );


                                                                                        for(int f=0;f<bufferSign;f++){

                                                                                            *(polybius_table+f)=*(polybius_table+f+i+1);

                                                                                         }
                                                                                         *(polybius_table+bufferSign-i-1)=0;

                                                                                         bufferFlag=bufferFlag-i-1;

                                                                                         break;

                                                     }

                                         }


                                 }






                             }


                }


        }




     }



          int    bufferSign =0;


                                        while (*( polybius_table+bufferSign) != 0){
                                                    bufferSign++;
                                                    }

                                    if(*(polybius_table+bufferSign-1)=='x'){
                                            if(*(polybius_table+bufferSign)=='x'){
                                                bufferSign-=2;

                                            }else{
                                                bufferSign--;
                                            }

                                    }

                  for (int j=0;j<90;j++){
                                         if(  **( morse_table+j)!=0){

                                                    int boolTemp=0;
                                                    for(int m=0;m<bufferSign;m++){
                                                    if(  *( polybius_table+m)==   *( *( morse_table+j)+m) ){

                                                                     boolTemp++;
                                                    }else{
                                                                     boolTemp--;
                                                         }


                                                    }

                                                    if(boolTemp==bufferSign  && *( *( morse_table+j)+bufferSign)==0)        {

                                                                    printf("%c",* ( polybius_alphabet+j)  );




                                                     }
                                            }
                                }

   while (newlineFlag!=0){

                printf("\n");
                newlineFlag--;
            }


return EXIT_SUCCESS;
}



































//ca

 int fEncrypt(unsigned short tempMode){



    char inputC=9;

     char firstC;
     char secondC;
     char thirdC;

     int threeTimesC=0;


     int newlineFlag=0;


     int endRecord=0;
     while (inputC!=EOF){
                    inputC=getc(stdin);
                    if (inputC==-1){

                        break;
                    }

                    if (inputC==10){



                        if(threeTimesC==0){
                            printf("%c",10 );
                        }else {
                            newlineFlag++;
                            }



                    } else if(inputC==32 || inputC==9){

                         switch(threeTimesC){
                             case 0:
                                if(endRecord<2){
                                endRecord++;
                           //     printf("%d",endRecord);
                                firstC='x';
                                threeTimesC++;
                            }
                                break;
                            case 1:
                              if(endRecord<2){
                            endRecord++;
                         //   printf("%d",endRecord);
                                secondC='x';
                                threeTimesC++;
                            }
                                break;
                            case 2:
                              if(endRecord<2){
                            endRecord++;
                         //   printf("%d",endRecord);
                                thirdC='x';
                                threeTimesC=0;
                                fEoutput(firstC,secondC,thirdC);
                                while(newlineFlag!=0){
                                    printf("%c",10);
                                    newlineFlag--;
                                }
                            }
                                break;



                         }



                 }else if(inputC<33 || inputC>122   ){



                                 return EXIT_FAILURE;



                }else {
                    endRecord=0;
                  //  printf("%d",endRecord);
                      int   tempInputC=inputC-33;
                                    if(   **( morse_table+ tempInputC)==0){     //empty character

                                        return EXIT_FAILURE;
                                     }else{

                                        int fflag=0;
                                        while(*(*(morse_table+tempInputC)+fflag)!=0){



                                                    switch(threeTimesC){
                                                                 case 0:
                                                                     firstC=*(*(morse_table+tempInputC)+fflag);
                                                                     fflag++;
                                                                     threeTimesC++;
                                                                     break;
                                                                case 1:
                                                                     secondC=*(*(morse_table+tempInputC)+fflag);
                                                                     fflag++;
                                                                      threeTimesC++;
                                                                      break;
                                                                case 2:
                                                                     thirdC=*(*(morse_table+tempInputC)+fflag);
                                                                     fflag++;
                                                                    threeTimesC=0;
                                                                     fEoutput(firstC,secondC,thirdC);
                                                                        while(newlineFlag!=0){
                                                                        printf("%c",10);
                                                                        newlineFlag--;
                                                                         }
                                                                      break;



                                                                     }






                                        }

                                      switch(threeTimesC){
                                                        case 0:
                                                        endRecord++;
                                                      //  printf("%d",endRecord);
                                                            firstC='x';
                                                            threeTimesC++;
                                                             break;
                                                        case 1:
                                                        endRecord++;
                                                       // printf("%d",endRecord);
                                                            secondC='x';
                                                             threeTimesC++;
                                                              break;
                                                        case 2:
                                                        endRecord++;
                                                      //  printf("%d",endRecord);
                                                            thirdC='x';
                                                             threeTimesC=0;
                                                             fEoutput(firstC,secondC,thirdC);
                                                              while(newlineFlag!=0){
                                                                printf("%c",10);
                                                                newlineFlag--;
                                                                 }
                                                              break;

                                                            }

                                     }

                    }







        }



        switch(threeTimesC){
                             case 0:
                              while(newlineFlag!=0){
                                    printf("%c",10);
                                    newlineFlag--;
                                }

                                break;
                            case 1:
                             while(newlineFlag!=0){
                                    printf("%c",10);
                                    newlineFlag--;
                                }

                                break;
                            case 2:

                                thirdC='x';
                                threeTimesC=0;
                                fEoutput(firstC,secondC,thirdC);
                                while(newlineFlag!=0){
                                    printf("%c",10);
                                    newlineFlag--;
                                }
                                break;
                            }
        return EXIT_SUCCESS;


}





void fEoutput(char fir,char sec,char thir){


for(int i=0;i<26;i++){
        if(  *(*(fractionated_table+i)+0) ==fir   &&  *(*(fractionated_table+i)+1) == sec    &&  *(*(fractionated_table+i)+2) ==thir){

          printf("%c",*( fm_key+i));
            break;
        }


   }


}
























int pCipherCall(int pCipherTempArgc,char **pCipheTempArgv,unsigned short pMode){


    int rowC;
    int colC;


    unsigned short rowMode;
    unsigned short colMode;


    rowMode=pMode & 0x00F0;
    colMode=pMode & 0x000F;


    switch(rowMode){

        case 0x0090:
            rowC=9;
            break;
        case 0x00A0:
            rowC=10;
            break;
        case 0x00B0:
            rowC=11;
            break;
        case 0x00C0:
            rowC=12;
            break;
        case 0x00D0:
            rowC=13;
            break;
        case 0x00E0:
            rowC=14;
            break;
        case 0x00F0:
            rowC=15;
            break;




    }




     switch(colMode){

        case 0x0009:
            colC=9;
            break;
        case 0x000A:
            colC=10;
            break;
        case 0x000B:
            colC=11;
            break;
        case 0x000C:
            colC=12;
            break;
        case 0x000D:
            colC=13;
            break;
        case 0x000E:
            colC=14;
            break;
        case 0x000F:
            colC=15;
            break;




    }


    int tableSize=rowC*colC;




  if( NULL==key){


            for(int i=0;i<94;i++){

            *(polybius_table+i)=*(polybius_alphabet+i);
                }

     }else{

                int keyCount=0;
                    while (*( key+keyCount) != 0){
                            keyCount++;
                         }





                for (int i=0;i<keyCount;i++){

                        *(polybius_table+i)=*(key+i);

                            }





                     int alphabetFlag=0;
                     int tableFlag=0;

                    int arrayNumber=0;
                    while (*(polybius_alphabet+arrayNumber)!=0){
                        arrayNumber++;
                    }
                    while(alphabetFlag<arrayNumber){

                      int bo=-1;


                        for (int m=0;m<keyCount;m++){           //check key loop


                            if (   *(key+m)== *(polybius_alphabet+alphabetFlag)){



                                      bo=m;


                           }
                    }


                      if (bo!=-1){


                              alphabetFlag++;



                      }else{

                             *(polybius_table+keyCount+tableFlag)=*(polybius_alphabet+alphabetFlag);
                                   alphabetFlag++;
                                   tableFlag++;



                      }



                }


            }


        for (int i=94;i<257;i++){
                   *(polybius_table+i)=0;




             }




      unsigned short deMode=pMode & 0x2000;
          if (deMode==0x2000){

               return pDecrypt(pMode);
          }else {

               return pEncrypt(pMode);
          }




    return EXIT_SUCCESS;


}












 int pDecrypt  ( unsigned short tempMode){


        int rowC;
    int colC;


    unsigned short rowMode;
    unsigned short colMode;


    rowMode= tempMode & 0x00F0;
    colMode= tempMode & 0x000F;


    switch(rowMode){

        case 0x0090:
            rowC=9;
            break;
        case 0x00A0:
            rowC=10;
            break;
        case 0x00B0:
            rowC=11;
            break;
        case 0x00C0:
            rowC=12;
            break;
        case 0x00D0:
            rowC=13;
            break;
        case 0x00E0:
            rowC=14;
            break;
        case 0x00F0:
            rowC=15;
            break;




    }




     switch(colMode){

        case 0x0009:
            colC=9;
            break;
        case 0x000A:
            colC=10;
            break;
        case 0x000B:
            colC=11;
            break;
        case 0x000C:
            colC=12;
            break;
        case 0x000D:
            colC=13;
            break;
        case 0x000E:
            colC=14;
            break;
        case 0x000F:
            colC=15;
            break;




    }


    int tableSize=rowC*colC;





    char inputC=9;
    char inputC1;
    char inputC2;
    while    ( inputC!=EOF){


                            inputC=getc(stdin);

                            if(inputC==-1){
                                break;
                            }
                            if(inputC==' '){
                               printf(" ");

                            }else if (inputC== '\n'){

                                    printf("\n");


                             } else if(inputC==9){
                                printf("%c",9);
                             }else{


                                       inputC1=inputC;

                                       inputC2=getc(stdin);

                                       if (inputC1>=65){
                                             inputC1=inputC1-55;

                                       }else {

                                              inputC1=inputC1-48;
                                       }


                                        if (inputC2>=65){
                                             inputC2=inputC2-55;

                                       }else {

                                              inputC2=inputC2-48;
                                       }


                                        int countFlag;

                                        countFlag=inputC1*colC+inputC2;
                                        printf("%c",*(polybius_table+countFlag));




                                    }



          }








return EXIT_SUCCESS;

}




































int pEncrypt  ( unsigned short tempMode){
     int rowC;
    int colC;


    unsigned short rowMode;
    unsigned short colMode;


    rowMode= tempMode & 0x00F0;
    colMode= tempMode & 0x000F;


    switch(rowMode){

        case 0x0090:
            rowC=9;
            break;
        case 0x00A0:
            rowC=10;
            break;
        case 0x00B0:
            rowC=11;
            break;
        case 0x00C0:
            rowC=12;
            break;
        case 0x00D0:
            rowC=13;
            break;
        case 0x00E0:
            rowC=14;
            break;
        case 0x00F0:
            rowC=15;
            break;




    }




     switch(colMode){

        case 0x0009:
            colC=9;
            break;
        case 0x000A:
            colC=10;
            break;
        case 0x000B:
            colC=11;
            break;
        case 0x000C:
            colC=12;
            break;
        case 0x000D:
            colC=13;
            break;
        case 0x000E:
            colC=14;
            break;
        case 0x000F:
            colC=15;
            break;




    }


    int tableSize=rowC*colC;














   char inputC=9;

     int inputFlag;
    while    ( inputC!=EOF){

                            inputC=getc(stdin);
                            if(inputC==-1){
                                break;
                            }

                            if(inputC==' '){
                               printf(" ");

                            }else if (inputC=='\n'){

                                    printf("\n");


                             } else if(inputC==9){
                                printf("%c",9);
                             }else{

                                         int arrayNumber=0;
                                         int recordReal=0;
                                        while (*(polybius_alphabet+arrayNumber)!=0){
                                       arrayNumber++;
                                       }
                                        for(int i=0;i<arrayNumber;i++){
                                                if(*(polybius_table+i)==inputC){

                                                        int tempC=i/colC;
                                                        int tempR=i%colC;

                                                        recordReal++;


                                                        printf("%X%X",tempC,tempR);


                                                         }
                                         }


                                        if(recordReal==0){
                                         return EXIT_FAILURE;
                                        }





                                 }
                          }


        return EXIT_SUCCESS;

}