#include "queue.h"
#include <errno.h>


queue_t *create_queue(void) {
     queue_t *newQueue;

     if((newQueue=calloc(1,sizeof(queue_t)))==NULL){
            return NULL;

     }

    newQueue->front=NULL;
    newQueue->rear=NULL;

    if (pthread_mutex_init(&(newQueue->lock),NULL) !=0){
           free(newQueue);
           return NULL;
    }


    if (sem_init(&(newQueue-> items),0,0)  == -1 ){ // track the number item in queue
           free(newQueue);
           return NULL;
    }


    newQueue->invalid=false;
    return newQueue;
}




bool invalidate_queue(queue_t *self, item_destructor_f destroy_function) {

    if(self==NULL || destroy_function==NULL){
        errno=EINVAL;
        return false;
    }

    if(self->invalid==true){
       errno=EINVAL;
       return false;
   }



    pthread_mutex_lock(&(self->lock));//error 0 for correct


    int serecord=0;
    sem_getvalue(&(self->items),&serecord);

    while(serecord!=0){
        queue_node_t *temp=self->front;
        self->front=temp->next;
        destroy_function(temp);
        //free(temp);//????????????????????
        sem_trywait(&(self->items));
        sem_getvalue(&(self->items),&serecord);

    }



    self->invalid=true;


  pthread_mutex_unlock(&(self->lock));


    return true;
}











bool enqueue(queue_t *self, void *item) {  //insert to rear   //error return false

   if(self ==NULL || item==NULL){
     errno=EINVAL;
     return false;
   }

   if(self->invalid==true){
    errno=EINVAL;
    return false;
   }

   pthread_mutex_lock(&(self->lock));
   sem_post(&(self->items));

            queue_node_t *newNode=calloc(1,sizeof(queue_node_t));  //error?
            if(newNode==NULL){
              return false;
            }

            newNode-> item=item;
            newNode-> next=NULL;

            if (self->rear==NULL){ //new queue
                self->front=newNode;
                self->rear=newNode;
            }else{
                (self->rear)->next=newNode;
                self->rear=newNode;
            }

    self->invalid=false;



   pthread_mutex_unlock(&(self->lock));
   return true;
}





void *dequeue(queue_t *self) {

   if(self ==NULL){
     errno=EINVAL;
     return NULL;

   }


  if(self->invalid==true){
    errno=EINVAL;
    return false;
   }



  // pthread_mutex_lock(&(self->lock));
   sem_wait(&(self->items));  //it should have item
   pthread_mutex_lock(&(self->lock));


       queue_node_t *temp=self->front;

       if(self->front == self->rear){
           self->front=NULL;
           self->rear=NULL;
       }else{
           self->front=temp->next;
       }




       self->invalid=false;



     pthread_mutex_unlock(&(self->lock));
//free(temp);//?????
return temp;
}