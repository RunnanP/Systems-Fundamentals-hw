#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <memory.h>

#define MAP_KEY(base, len) (map_key_t) {.key_base = base, .key_len = len}
#define MAP_VAL(base, len) (map_val_t) {.val_base = base, .val_len = len}
#define MAP_NODE(key_arg, val_arg, tombstone_arg) (map_node_t) {.key = key_arg, .val = val_arg, .tombstone = tombstone_arg}

bool keyequal(map_key_t key1,map_key_t key2){
    if( key1.key_len != key2.key_len){
        return false;
    }else{
       if( memcmp(key1.key_base,key2.key_base,key1.key_len)!=0){

          return false;
      }else {
        return true;
      }

    }

}

bool valequal(map_val_t val1,map_val_t val2){
    if(val1.val_len != val2.val_len){
        return false;
    }else{
         if( memcmp(val1.val_base,val2.val_base,val1.val_len)!=0){
          return false;
      }else {
        return true;
      }
    }

}


bool mapnodeequal(map_node_t node1,map_node_t node2){
    return false;

}




hashmap_t *create_map(uint32_t capacity, hash_func_f hash_function, destructor_f destroy_function) {
    if(capacity<=0 || hash_function==NULL || destroy_function==NULL){

        errno=EINVAL;
        return NULL;
    }


    hashmap_t *newMap=calloc(1,sizeof(hashmap_t));
    if (newMap==NULL){
        return NULL;
    }



    newMap->capacity=capacity;
    newMap->size=0;
    newMap->nodes=calloc(capacity,sizeof(  map_node_t  ));//error??   free??

    if(newMap->nodes == NULL){
        free(newMap);
        return NULL;
    }


    //init nodes
    for (int dur=0;dur<capacity;dur++){


        map_node_t *tempmapnode=newMap->nodes+dur;
        tempmapnode->key=MAP_KEY(NULL, 0);
        tempmapnode->val=MAP_VAL(NULL, 0);
        tempmapnode->tombstone=false;

    }

    newMap->hash_function=hash_function;
    newMap->destroy_function=destroy_function;
    newMap->num_readers=0;
   if( pthread_mutex_init(&(newMap->write_lock),NULL) !=0){
        free(newMap->nodes);
        free(newMap);
        return NULL;
   }

   if( pthread_mutex_init(&(newMap->fields_lock),NULL) !=0){
        free(newMap->nodes);
        free(newMap);
        return NULL;
   }

    newMap->invalid=false;


    return newMap;
}








bool put(hashmap_t *self, map_key_t key, map_val_t val, bool force) {

    if(self==NULL){

        errno=EINVAL;
        return false;
    }


    pthread_mutex_lock(&(self->write_lock));

    if(key.key_base==NULL || key.key_len==0 || val.val_base == NULL || val.val_len==0){
        errno=EINVAL;
        pthread_mutex_unlock(&(self->write_lock));

        return false;

    }

    if (self->invalid==true){

        errno=EINVAL;
        pthread_mutex_unlock(&(self->write_lock));
        return false;
    }


    if(self->capacity==self->size){//force have to be consider   //full hashmap

                  if(force==false){

                    errno=ENOMEM;
                    pthread_mutex_unlock(&(self->write_lock));
                    return false;

                  }else{//force true
                    int startIndex=get_index(self,key);  //need linear pro??...to find the exactly space

                    map_node_t *tempmapnode=(self->nodes)+startIndex;

                    int loopmark=0;
                    while(loopmark!=self->capacity){


                                 if( keyequal( tempmapnode->key,key) ==true){

                                     break;
                                 }



                                if(startIndex<self->capacity){
                                startIndex++;
                                loopmark++;
                               }else{
                                startIndex=0;
                                loopmark++;
                               }


                               tempmapnode=(self->nodes)+startIndex;


                    }



                   if(loopmark==self->capacity){
                        startIndex=get_index(self,key);
                        tempmapnode=(self->nodes)+startIndex;
                   }



                    (self->destroy_function)(tempmapnode->key,tempmapnode->val);

                     tempmapnode->key =key;
                     tempmapnode->val =val;
                     tempmapnode->tombstone=false;

                  //  self->size=(self->size)+1;
                    pthread_mutex_unlock(&(self->write_lock));
                    return true;

                  }






    }else{  //ignore force... not full

           int startIndex=get_index(self,key);
           map_node_t *temp;
           int loopmark=0;
           while( loopmark!=self->capacity ){//index 0???

                     temp = (self->nodes)   +startIndex;

                     if( keyequal(temp ->key,MAP_KEY(NULL, 0) ) ){

                        break;
                     }

                   /*  if(temp ->tombstone ==true){


                      //  (self->destroy_function)(   temp ->key,   temp ->val);
                        break;
                     }*/

                     if(keyequal(temp ->key,key ) && temp ->tombstone ==false ){

                         (self->destroy_function)(   temp ->key,   temp ->val);
                        break;


                     }     //overwrite the same key.........

                       if(startIndex<self->capacity){
                        startIndex++;
                        loopmark++;
                       }else{
                        startIndex=0;
                        loopmark++;
                       }

           }

                if( keyequal(temp ->key,MAP_KEY(NULL, 0) ) || loopmark==self->capacity  ){

                          startIndex=get_index(self,key);

                           while(1){
                            temp = (self->nodes)   +startIndex;

                                      if( keyequal(temp ->key,MAP_KEY(NULL, 0) ) ){
                                             self->size=(self->size)+1;
                                               break;
                                       }

                                      if(temp ->tombstone ==true){
                                            self->size=(self->size)+1;
                                       break;
                                      }

                                           if(startIndex<self->capacity){
                                                startIndex++;

                                               }else{
                                                startIndex=0;

                                               }

                           }
                }


          temp->key =key;
          temp->val =val;
          temp ->tombstone =false;

            //self->size=(self->size)+1;


            pthread_mutex_unlock(&(self->write_lock));
         return true;

    }






    pthread_mutex_unlock(&(self->write_lock));

    return false;
}











map_node_t delete(hashmap_t *self, map_key_t key) {


    if(self==NULL){
        errno=EINVAL;
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
    }

    pthread_mutex_lock(&(self->write_lock));
    if(key.key_base==NULL || key.key_len==0 || self->invalid==true){
         errno=EINVAL;
         pthread_mutex_unlock(&(self->write_lock));
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);

    }


        map_node_t *returnvalue;
        int tempindex=get_index(self,key);
        returnvalue= (self->nodes)+tempindex;
      //  for (int tempindex=0; tempindex<capacity; tempindex++){
        int loopmark=0;
        while(1){
              // returnvalue= (self->nodes)+tempindex;
                  if(  returnvalue->tombstone==false){
                         if(   keyequal(returnvalue->key,key ) ) {

                        //      if(returnvalue->tombstone==true){//seach next
                         //       pthread_mutex_unlock(&(self->write_lock));
                         //       return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
                         //     }else{
                                returnvalue->tombstone=true;
                                self->size--;

                                pthread_mutex_unlock(&(self->write_lock));
                                return *returnvalue;

                         //     }
                         }else if(  keyequal(returnvalue->key,MAP_KEY(NULL, 0) )){

                                 pthread_mutex_unlock(&(self->write_lock));
                                 return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);

                         }


                     }//else tombsone=true


                        if(tempindex < self->capacity){//???

                           tempindex++;
                           loopmark++;

                         }else{
                            tempindex=0;
                            loopmark++;
                         }
                        returnvalue= (self->nodes)+tempindex;

                        if(loopmark>=self->capacity){
                            break;
                        }

          }


        pthread_mutex_unlock(&(self->write_lock));
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);

    //return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
}







map_val_t get(hashmap_t *self, map_key_t key) {//first writer or first reader
    if(self==NULL){
        errno=EINVAL;
        return MAP_VAL(NULL, 0);
    }

    if(self->invalid==true){
        errno=EINVAL;
        return MAP_VAL(NULL, 0);
    }

    pthread_mutex_lock(&(self->fields_lock));
    self->num_readers++;

    if(self->num_readers==1){

        pthread_mutex_lock(&(self->write_lock));
    }

    pthread_mutex_unlock(&(self->fields_lock));


        map_node_t *returnvalue;
        int tempindex=get_index(self,key);

        returnvalue= (self->nodes)+tempindex;
        int loopmark=0;
        while (1){

            if(     keyequal(returnvalue->key,key )   ){

                        if(returnvalue->tombstone==true){
                                pthread_mutex_lock(&(self->fields_lock));
                                 self->num_readers--;
                                     if(self->num_readers==0){

                                           pthread_mutex_unlock(&(self->write_lock));
                                      }
                                pthread_mutex_unlock(&(self->fields_lock));
                                return MAP_VAL(NULL, 0);

                        }else{

                                 pthread_mutex_lock(&(self->fields_lock));
                                 self->num_readers--;
                                 if(self->num_readers==0){

                                           pthread_mutex_unlock(&(self->write_lock));
                                      }
                                pthread_mutex_unlock(&(self->fields_lock));
                                return returnvalue->val;
                        }
            }else if(keyequal(returnvalue->key,MAP_KEY(NULL, 0) )){

                           pthread_mutex_lock(&(self->fields_lock));
                           self->num_readers--;

                                    if(self->num_readers==0){

                                           pthread_mutex_unlock(&(self->write_lock));
                                      }
                           pthread_mutex_unlock(&(self->fields_lock));
                           return MAP_VAL(NULL, 0);
            }


                        if(tempindex < self->capacity){

                         tempindex++;
                         loopmark++;

                         }else{
                            tempindex=0;
                            loopmark++;
                         }
                        returnvalue= (self->nodes)+tempindex;


                        if(loopmark>self->capacity){///capcity ???size?

                            break;
                        }
        }



    pthread_mutex_lock(&(self->fields_lock));
     self->num_readers--;
                    if(self->num_readers==0){

                            pthread_mutex_unlock(&(self->write_lock));
                     }
    pthread_mutex_unlock(&(self->fields_lock));

    return MAP_VAL(NULL, 0);
}







bool clear_map(hashmap_t *self) {

     if(self==NULL){

        errno=EINVAL;
        return false;
    }




    pthread_mutex_lock(&(self->write_lock));

    if(self->invalid==true){
        errno=EINVAL;
      pthread_mutex_unlock(&(self->write_lock));
        return false;
     }

    for(int tempindex=0; tempindex<self->capacity; tempindex++){
        map_node_t *returnvalue= (self->nodes)+tempindex;

        //if( keyequal(returnvalue->key,MAP_KEY(NULL, 0)) ==false)    {
      if(returnvalue->tombstone==false && keyequal(returnvalue->key,MAP_KEY(NULL, 0)) ==false){
        (self->destroy_function)(   returnvalue ->key,   returnvalue ->val);


                    returnvalue->key=MAP_KEY(NULL, 0);
                    returnvalue->val=MAP_VAL(NULL, 0);
                    returnvalue->tombstone=false;


        }

        if(returnvalue->tombstone==true){   //??????????????

                    returnvalue->key=MAP_KEY(NULL, 0);
                    returnvalue->val=MAP_VAL(NULL, 0);
                    returnvalue->tombstone=false;


        }

    }
    self->size=0;

    pthread_mutex_unlock(&(self->write_lock));
	return true;
}






bool invalidate_map(hashmap_t *self) {
      if(self==NULL){
        errno=EINVAL;
        return false;
    }

    pthread_mutex_lock(&(self->write_lock));

    if(self->invalid==true){
        errno=EINVAL;
      pthread_mutex_unlock(&(self->write_lock));
        return false;
     }

    self->invalid=true;

    for(int tempindex=0; tempindex<self->capacity; tempindex++){
        map_node_t *returnvalue= (self->nodes)+tempindex;
        if(keyequal(returnvalue->key,MAP_KEY(NULL, 0)) ==false  &&  returnvalue->tombstone==false){
             (self->destroy_function)(   returnvalue ->key,   returnvalue ->val);//return false stituait
        }

    }

    free(self->nodes);

    pthread_mutex_unlock(&(self->write_lock));

    return true;
}


