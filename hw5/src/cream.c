#include "cream.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "queue.h"

#include "csapp.h"

//#include "hashmap.h"

hashmap_t *requestMap;
queue_t *requestQueue;

void serviceCheck(int *connfd);
void servicePut(int tempfd,request_header_t reqH);
void serviceGet(int tempfd,request_header_t reqH);
void serviceEvict(int tempfd,request_header_t reqH);
void serviceClear(int tempfd,request_header_t reqH);

void *threadFun(void *vargp);

void mapdes(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}




int main(int argc, char *argv[]) {


    signal(SIGPIPE,SIG_IGN);

    if(strcmp(argv[1],"-h")==0){
        printf(  "./cream [-h] NUM_WORKERS PORT_NUMBER MAX_ENTRIES\n"
                 "-h            Displays this help menu and returns EXIT_SUCCESS.\n"
                 "NUM_WORKERS   The number of worker threads used to service requests.\n"
                 "PORT_NUMBER   Port number to listen on for incoming connections.\n"
                 "MAX_ENTRIES   The maximum number of entries that can be stored in `cream`'s underlying data store.\n"
                );

        exit(EXIT_SUCCESS);

    }


    if(argc!=4){
        exit(EXIT_FAILURE);
    }

    int numWorkers=0;
    char * portchar;
    int maxEntries=0;

    numWorkers=atoi(argv[1]);
    portchar=strdup(argv[2]);
    maxEntries=atoi(argv[3]);

    if (numWorkers <1  || maxEntries <1){

        exit(EXIT_FAILURE);
    }

    pthread_t thread_ids[numWorkers];


    int listenid;
    listenid=Open_listenfd(portchar);


    requestQueue=create_queue();
    requestMap=create_map(maxEntries,jenkins_one_at_a_time_hash,mapdes);
     //requestMap=create_map(maxEntries,jj,mapdes);

    for (int i=0;i<numWorkers;i++){
        Pthread_create(&thread_ids[i],NULL,threadFun,NULL);

    }

    while(1){

      socklen_t clientlen=sizeof(struct sockaddr);


       SA *clientaddr=Calloc(1,clientlen);//free?

       int connfd;

       retry1:
       connfd=Accept(listenid,clientaddr,&clientlen);

     //  connfd=Accept(listenid,NULL,NULL);
       if(errno==EINTR){
            errno=0;
            goto retry1;
       }



       int *fdspace=Calloc(1,sizeof(int));  //free??
       *fdspace=connfd;
       enqueue(requestQueue,fdspace);
      // enqueue(requestQueue,&connfd);

    }



}




void *threadFun(void *vargp){

    Pthread_detach(pthread_self());
    while(1){
        queue_node_t *nodetemp=dequeue(requestQueue);

        int *connfd=nodetemp->item;



        serviceCheck(connfd);

        Close(*connfd);

        free(connfd);
        free(nodetemp);//free ????????????????
    }
}


void serviceCheck(int *connfd){

    int tempfd=*connfd;

    request_header_t request_header;


    Rio_readn(tempfd,&request_header,sizeof(request_header));

            if(errno==EPIPE || errno==EINTR){
            errno=0;
            return;
            }



    if (request_header.request_code==PUT){
        servicePut(tempfd,request_header);

    }else if(request_header.request_code==GET){
        serviceGet(tempfd,request_header);


    }else if (request_header.request_code==EVICT){
         serviceEvict(tempfd,request_header);

    }else if(request_header.request_code==CLEAR){

        serviceClear(tempfd,request_header);


    }else{

            response_header_t  response_header={UNSUPPORTED,0};
            Rio_writen(tempfd,&response_header,sizeof(response_header));
        if(errno==EPIPE || errno==EINTR){
            errno=0;
            return;
       }


    }

}



void servicePut(int tempfd,request_header_t reqH){


         if (reqH.key_size>MAX_KEY_SIZE || reqH.key_size <MIN_KEY_SIZE || reqH.value_size>MAX_VALUE_SIZE || reqH.value_size <MIN_VALUE_SIZE){

              response_header_t response_header= {BAD_REQUEST,0};
              Rio_writen(tempfd,&response_header,sizeof(response_header));
                            if(errno==EPIPE || errno==EINTR){
                              errno=0;
                              return;
                         }

         }else{

               int keylen=reqH.key_size;
               int vallen=reqH.value_size;

               void*keybase=Calloc(1,keylen);
               void*valbase=Calloc(1,vallen);
               Rio_readn(tempfd,keybase,keylen);
                if(errno==EPIPE || errno==EINTR){
                    free(keybase);
                    free(valbase);
                    errno=0;
                    return;
                }
               Rio_readn(tempfd,valbase,vallen);
                if(errno==EPIPE || errno==EINTR){
                    free(keybase);
                    free(valbase);
                    errno=0;
                    return;
                }

               map_key_t newkey={keybase,keylen};
               map_val_t newvalue={valbase,vallen};
               if (put(requestMap,newkey,newvalue,true) == true){//detech what is the return val

                  response_header_t response_header={OK,vallen};
                  Rio_writen(tempfd,&response_header,sizeof(response_header));



                 }else{

                response_header_t response_header={BAD_REQUEST,0};
                Rio_writen(tempfd,&response_header,sizeof(response_header));



                 }


                             if(errno==EPIPE || errno==EINTR){

                                             map_node_t deletenode=delete(requestMap,newkey);
                                             (requestMap->destroy_function)(deletenode.key,deletenode.val);

                                            errno=0;
                                            return;
                                        }

             //  free(keybase);
             //  free(valbase);




         }



     /*     for(int i=0;i<requestMap->capacity;i++){
          if((requestMap->nodes+i)->tombstone==true){
                printf("%d-%s-%strue\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }else{
            printf("%d-%s-%sfalse\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }

         }
         printf("put-------------%d\n",requestMap->size);
*/
}


void serviceGet(int tempfd,request_header_t reqH){
   // response_header_t response_header;

     if (reqH.key_size>MAX_KEY_SIZE || reqH.key_size <MIN_KEY_SIZE ){
              response_header_t response_header={BAD_REQUEST,0};
              Rio_writen(tempfd,&response_header,sizeof(response_header));

                if(errno==EPIPE || errno==EINTR){
                    errno=0;
                    return;
               }

         }else{

             int keylen=reqH.key_size;
             void *keybase=Calloc(1,keylen);
             Rio_readn(tempfd,keybase,keylen);
                  if(errno==EPIPE || errno==EINTR){
                    free(keybase);
                    errno=0;
                    return;
                }

             map_key_t newkey={keybase,keylen};

             map_val_t newvalue=get(requestMap,newkey);
              free(keybase);//??

             if(newvalue.val_base==NULL || newvalue.val_len == 0){
                // printf("rrr\n");
                 response_header_t response_header={NOT_FOUND,0};
                  Rio_writen(tempfd,&response_header,sizeof(response_header));

                                  if(errno==EPIPE || errno==EINTR){

                                            errno=0;
                                            return;
                                        }
             }else{
                 response_header_t response_header={OK,newvalue.val_len};
                  Rio_writen(tempfd,&response_header,sizeof(response_header));

                                if(errno==EPIPE || errno==EINTR){

                                            errno=0;
                                            return;
                                        }

                  Rio_writen(tempfd,newvalue.val_base,newvalue.val_len);

                                   if(errno==EPIPE || errno==EINTR){

                                        errno=0;
                                        return;
                                    }


             }

           //  free(keybase);


         }

 /*        for(int i=0;i<requestMap->capacity;i++){
          if((requestMap->nodes+i)->tombstone==true){
                printf("%d-%s-%strue\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }else{
            printf("%d-%s-%sfalse\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }

         }
         printf("get-------------%d\n",requestMap->size);
*/
}

void serviceEvict(int tempfd,request_header_t reqH){
    //response_header_t response_header;

    if (reqH.key_size>MAX_KEY_SIZE || reqH.key_size <MIN_KEY_SIZE ){
            response_header_t  response_header={BAD_REQUEST,0};
              Rio_writen(tempfd,&response_header,sizeof(response_header));
         }else {

             int keylen=reqH.key_size;
             void *keybase=Calloc(1,keylen);
             Rio_readn(tempfd,keybase,keylen);
                if(errno==EPIPE || errno==EINTR){
                    free(keybase);

                    errno=0;
                    return;
                }

             map_key_t newkey={keybase,keylen};

             map_node_t deletenode=delete(requestMap,newkey);
             (requestMap->destroy_function)(deletenode.key,deletenode.val);//????
                free(keybase);
            //free( deletenode.key.key_base);//?????
           // free(deletenode.val.val_base);//??????

            response_header_t response_header={OK,0};
             Rio_writen(tempfd,&response_header,sizeof(response_header));


         }


               if(errno==EPIPE || errno==EINTR){
                    errno=0;
                    return;
                }



   /*       for(int i=0;i<requestMap->capacity;i++){
          if((requestMap->nodes+i)->tombstone==true){
                printf("%d-%s-%strue\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }else{
            printf("%d-%s-%sfalse\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }

         }
         printf("evict-------------%d\n",requestMap->size);
*/
}

void serviceClear(int tempfd,request_header_t reqH){


             if (clear_map(requestMap)==true){
                response_header_t  response_header={OK,0};
                Rio_writen(tempfd,&response_header,sizeof(response_header));
             }else{

              response_header_t  response_header={BAD_REQUEST,0};
                Rio_writen(tempfd,&response_header,sizeof(response_header));
             }

               if(errno==EPIPE || errno==EINTR){
                    errno=0;
                    return;
                }

/*
         for(int i=0;i<requestMap->capacity;i++){
          if((requestMap->nodes+i)->tombstone==true){
                printf("%d-%s-%strue\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }else{
            printf("%d-%s-%sfalse\n",i,(char*)((requestMap->nodes+i)->key.key_base),(char*)((requestMap->nodes+i)->val.val_base));
          }

         }
         printf("clear-------------%d\n",requestMap->size);*/
}






