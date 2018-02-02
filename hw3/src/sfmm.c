/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define True 1
#define False 0
//#define UNUSED(x) (void)x
/**
 * You should store the heads of your free lists in these variables.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */

size_t addNewPage();

void breakTheBlock(size_t freeListIndex,size_t size, size_t roundSize);
void putTotheListFirst(sf_free_header* nowIndex,size_t freeListIndex);
size_t roundTo16(size_t size);
size_t find_list_from_size(size_t sz);


free_list seg_free_list[4] = {
    {NULL, LIST_1_MIN, LIST_1_MAX},
    {NULL, LIST_2_MIN, LIST_2_MAX},
    {NULL, LIST_3_MIN, LIST_3_MAX},
    {NULL, LIST_4_MIN, LIST_4_MAX}
};

int sf_errno = 0;

void *sf_malloc(size_t size) {
    sf_errno = 0;


    if (size==0 || size> PAGE_SZ *4 ){    //include header and footer
      sf_errno=EINVAL;
      return NULL;
    }


    if ( size> PAGE_SZ *4 -(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8){
      sf_errno=ENOMEM;
      return NULL;
    }


    size_t roundSize=roundTo16(size);
    size_t freeListIndex=0;
    size_t freeListIndexBackup=0;
    for (freeListIndex=0; freeListIndex<FREE_LIST_COUNT; freeListIndex++){
        if (roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8>=seg_free_list[freeListIndex].min && roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8<=seg_free_list[freeListIndex].max){
        break;
        }
    }

    freeListIndexBackup=freeListIndex;

    while (seg_free_list[freeListIndexBackup].head==NULL){
         freeListIndexBackup++;

               if (freeListIndexBackup==FREE_LIST_COUNT){
                  int t1=get_heap_end()-get_heap_start();
                  size_t record=addNewPage();
                  if(record==-1){///?????
                      sf_errno=ENOMEM;
                      return NULL;
                  }

                  if(t1==(get_heap_end()-get_heap_start())){
                      sf_errno=ENOMEM;
                      return NULL;
                  }
                  //freeListIndexBackup--;
                    freeListIndexBackup=record;

                    if ((get_heap_end()-get_heap_start()) > 4 * PAGE_SZ){
                      sf_errno=ENOMEM;
                      return NULL;
                  }

                  break;

               }
    }



    int findBool=False;
    sf_free_header* nowIndex=seg_free_list[freeListIndexBackup].head;   // it will not null


    while (findBool==False){

         flag2:


         while((((nowIndex->header).block_size)<<4) <roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8){
                         if ((get_heap_end()-get_heap_start())> 4 * PAGE_SZ){       //check the one step is 1 byte or not
                              sf_errno=ENOMEM;
                              return NULL;
                        }
               nowIndex=nowIndex->next;

               if (nowIndex==NULL){
                    flag1:

                              if (freeListIndexBackup==FREE_LIST_COUNT-1){
                                            int t1=get_heap_end()-get_heap_start();


                                             size_t record=addNewPage();
                                             if(record==-1){
                                                sf_errno=ENOMEM;
                                              return NULL;
                                              }

                                                 if (t1==get_heap_end()-get_heap_start()){
                                                      sf_errno=ENOMEM;

                                                     return NULL;
                                                 }
                                                 if ((get_heap_end()-get_heap_start())> 4 * PAGE_SZ){       //check the one step is 1 byte or not

                                                     sf_errno=ENOMEM;

                                                     return NULL;
                                                   }
                                             // }else{
                                              nowIndex=seg_free_list[record].head;

                                              if (nowIndex ==NULL){
                                                  sf_errno=ENOMEM;

                                                  return NULL;
                                              }
                                              continue;
                      //}

                              }else{
                                          freeListIndexBackup++;
                                          nowIndex=seg_free_list[freeListIndexBackup].head;

                                          if(nowIndex==NULL){
                                               goto flag1;
                                           }else{
                                               goto flag2;
                                           }

                              }

               }

         }


          putTotheListFirst(nowIndex,freeListIndexBackup);

          breakTheBlock(freeListIndexBackup,size,roundSize);

          return (((void*)nowIndex)+SF_HEADER_SIZE/8);


    }


sf_errno=ENOMEM;
return NULL;

}




size_t roundTo16(size_t size){
    size_t k=size;
    if ( size%16 !=0){
       k=(size/16+1)*16;
    }
    return k;

}

void putTotheListFirst(sf_free_header* nowIndex,size_t freeListIndex){

  if ((nowIndex->prev==NULL) && (nowIndex->next==NULL)){

    return;
  }else if((nowIndex->prev==NULL) && (nowIndex->next!=NULL)){
    return;
  }else if((nowIndex->prev!=NULL) && (nowIndex->next==NULL)){
    (nowIndex->prev)->next=NULL;
    nowIndex->prev=NULL;
    nowIndex->next=seg_free_list[freeListIndex].head;
    seg_free_list[freeListIndex].head->prev=nowIndex;
    seg_free_list[freeListIndex].head=nowIndex;
    return;
  }else{

    (nowIndex->prev)->next=nowIndex->next;
    (nowIndex->next)->prev=nowIndex->prev;
    nowIndex->prev=NULL;
    nowIndex->next=seg_free_list[freeListIndex].head;
    seg_free_list[freeListIndex].head->prev=nowIndex;
    seg_free_list[freeListIndex].head=nowIndex;
    return;
  }

}




size_t addNewPage(){

   if((get_heap_end()-get_heap_start()) == 4 * PAGE_SZ){
          return-1;

     }else  if( get_heap_start()==NULL){   //first add page
              size_t index=find_list_from_size(PAGE_SZ);
              if (index!=-1){
                  seg_free_list[index].head=sf_sbrk();

                  seg_free_list[index].head->prev=NULL;
                  seg_free_list[index].head->next=NULL;

                  seg_free_list[index].head->header.allocated=0x0;
                  seg_free_list[index].head->header.padded=0x0;
                  seg_free_list[index].head->header.two_zeroes=0x00;
                  seg_free_list[index].head->header.block_size=PAGE_SZ>>4;//change to 16
                  seg_free_list[index].head->header.unused=0x0;
                  //ini this free list,and have to init footer

                  void *temp=(void *)(seg_free_list[index].head);
                  temp=temp+PAGE_SZ;
                  sf_footer *footer=temp-SF_FOOTER_SIZE/8;

                  footer->allocated=0;
                  footer->padded=0;
                  footer->two_zeroes=0;
                  footer->block_size=PAGE_SZ>>4;
                  footer->requested_size=0;
                  return index;
              }else{
                sf_errno=ENOMEM;
                return -1;

              }

        }else{    //alreadly have block in the free list have to combine the new page

            sf_free_header *newtemp=sf_sbrk();

            if((get_heap_end()-get_heap_start()) > 4 * PAGE_SZ){
              return -1;
            }


            sf_footer *lastPageFooter=((void*)newtemp)-SF_FOOTER_SIZE/8;

                        if (lastPageFooter->allocated==1){

                              //no combine
                                newtemp->prev=NULL;
                                newtemp->next=NULL;
                                newtemp->header.allocated=0x0;
                                newtemp->header.padded=0x0;
                                newtemp->header.two_zeroes=0x00;
                                newtemp->header.block_size=PAGE_SZ>>4;//change to 16
                                newtemp->header.unused=0x0;
                            //ini this free list,and have to init footer

                                void *temp=(void *)(newtemp);
                                temp=temp+PAGE_SZ;
                                sf_footer *footer=temp-SF_FOOTER_SIZE/8;

                                footer->allocated=0;
                                footer->padded=0;
                                footer->two_zeroes=0;
                                footer->block_size=PAGE_SZ>>4;
                                footer->requested_size=0;



                                //put into the head of list
                                size_t index=find_list_from_size(PAGE_SZ);
                                   if (index==-1){
                                       sf_errno=ENOMEM;
                                       return -1;
                                   }

                               sf_free_header *oldtemp=seg_free_list[index].head;

                               if (oldtemp!=NULL){
                                oldtemp->prev=newtemp;
                                newtemp->next=oldtemp;
                                newtemp->prev=NULL;
                               }else{
                                  newtemp->prev=NULL;
                                  newtemp->next=NULL;

                              }
                              seg_free_list[index].head=newtemp;
                              return index;

                        }else{
                            //combine
                            size_t lastPageBlockSize=(lastPageFooter->block_size)<<4;

                            sf_free_header *lastPageHeader=((void*)newtemp)-lastPageBlockSize;

                            //remove from the free list first ................
                            if(lastPageHeader->prev!=NULL && lastPageHeader->next!=NULL){
                               (lastPageHeader->prev)->next=lastPageHeader->next;
                               (lastPageHeader->next)->prev=lastPageHeader->prev;
                            }else if(lastPageHeader->prev==NULL && lastPageHeader->next!=NULL){
                               (lastPageHeader->next)->prev=NULL;
                            }else if(lastPageHeader->prev!=NULL && lastPageHeader->next==NULL){
                               (lastPageHeader->prev)->next=NULL;
                            }else{
                              size_t tindex=find_list_from_size(lastPageBlockSize);
                              if (tindex==-1){
                                 sf_errno=ENOMEM;
                                 return -1;
                              }
                              seg_free_list[ tindex].head=NULL;

                            }

                            lastPageHeader->next=NULL;
                            lastPageHeader->prev=NULL;
                            lastPageHeader->header.allocated=0;
                            lastPageHeader->header.padded=0;
                            lastPageHeader->header.two_zeroes=0;
                            lastPageHeader->header.block_size=(lastPageBlockSize+PAGE_SZ)>>4;
                            lastPageHeader->header.unused=0;
                            sf_footer * newfooter=((void*)get_heap_end())-SF_FOOTER_SIZE/8;

                            newfooter->allocated=0;
                            newfooter->padded=0;
                            newfooter->two_zeroes=0;
                            newfooter->block_size=(lastPageBlockSize+PAGE_SZ)>>4;
                            newfooter->requested_size=0;

                            size_t index=find_list_from_size(PAGE_SZ+lastPageBlockSize);
                            if(index==-1){
                              sf_errno=ENOMEM;
                              return -1;
                            }

                            if(seg_free_list[index].head!=NULL){
                                if(((void*)seg_free_list[index].head) != ((void *)lastPageHeader)){

                                     seg_free_list[index].head->prev=lastPageHeader;

                                     lastPageHeader->next=seg_free_list[index].head;
                                     lastPageHeader->prev=NULL;

                                     seg_free_list[index].head=lastPageHeader;
                                }

                            }else{
                                seg_free_list[index].head=lastPageHeader;
                            }
                            return index;


                        }


        }

}



size_t find_list_from_size(size_t sz) {
  if (sz >= LIST_1_MIN && sz <= LIST_1_MAX) return 0;
  else if (sz >= LIST_2_MIN && sz <= LIST_2_MAX) return 1;
  else if (sz >= LIST_3_MIN && sz <= LIST_3_MAX) return 2;
  else if(sz>=LIST_4_MIN && sz<=LIST_4_MAX) return 3;
  else return -1;

}

void breakTheBlock(size_t freeListIndex,size_t size,size_t roundSize){//the block already in the head of list


     size_t blockTSize=((seg_free_list[freeListIndex].head->header.block_size)<<4);  //include 16 header and footer
     int recordTheRestBlock=-1;
     if (blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8<LIST_1_MIN){
        recordTheRestBlock=-1;
     }else if(blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 >= LIST_1_MIN && blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 <=LIST_1_MAX){
        recordTheRestBlock=0;
     }else if (blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 >= LIST_2_MIN && blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 <=LIST_2_MAX){
        recordTheRestBlock=1;
     }else if(blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 >=LIST_3_MIN && blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8<=LIST_3_MAX){
        recordTheRestBlock=2;
     }else if(blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 >=LIST_4_MIN && blockTSize-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8<=LIST_4_MAX){
        recordTheRestBlock=3;
     }


     if (recordTheRestBlock==-1){

        sf_free_header *newtemp=seg_free_list[freeListIndex].head;

        if (seg_free_list[freeListIndex].head->next !=NULL){
            seg_free_list[freeListIndex].head=newtemp->next;
            seg_free_list[freeListIndex].head->prev=NULL;
        }else{
            seg_free_list[freeListIndex].head=NULL;
        }


            newtemp->header.allocated=0x1;
            if(size==roundSize){
              newtemp->header.padded=0;
            }else{
            newtemp->header.padded=0x1;
            }

            newtemp->header.two_zeroes=0x00;
            newtemp->header.block_size=blockTSize>>4;//roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;
            newtemp->header.unused=0x0;
        //ini this free list,and have to init footer

            void *temp=newtemp;

            temp=temp+blockTSize-SF_FOOTER_SIZE/8;//roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8-SF_FOOTER_SIZE/8;
            sf_footer *footer=temp;//-SF_FOOTER_SIZE/8;



            footer->allocated=1;
            footer->padded=newtemp->header.padded;
            footer->two_zeroes=0;
            footer->block_size=blockTSize>>4;//roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;
            footer->requested_size=size;



     }else{


          sf_free_header *notFree=seg_free_list[freeListIndex].head;   //it will not null


          void *temp=(void *)(notFree);

          temp=temp+roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;
          sf_free_header *freeInOtherList=temp;


          sf_free_header *stillInList=seg_free_list[freeListIndex].head->next;



         //the another free in this list
          seg_free_list[freeListIndex].head=stillInList;

        if (stillInList!=NULL){
          stillInList->prev=NULL;
        }

        //init go to another list rest
          freeInOtherList->prev=NULL;
          freeInOtherList->next=seg_free_list[recordTheRestBlock].head;

          if(seg_free_list[recordTheRestBlock].head!=NULL){
          seg_free_list[recordTheRestBlock].head->prev=freeInOtherList;
            }
          ////

          seg_free_list[recordTheRestBlock].head=freeInOtherList;

          freeInOtherList->header.allocated=0;
          freeInOtherList->header.padded=0;
          freeInOtherList->header.two_zeroes=0;
          freeInOtherList->header.block_size=(((notFree->header).block_size<<4)-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)>>4;
          freeInOtherList->header.unused=0;

          temp=(void*) freeInOtherList;
          temp=temp+((freeInOtherList->header).block_size<<4);
          sf_footer *footer=temp-SF_FOOTER_SIZE/8;

          footer->allocated=0;
          footer->padded=0;
          footer->two_zeroes=0;
          footer->block_size=(((notFree->header).block_size<<4)-roundSize-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)>>4;
          footer->requested_size=0;

          //init not free block

          notFree->header.allocated=1;
          if (size==roundSize){
          notFree->header.padded=0;
          }else{
            notFree->header.padded=1;
          }
          notFree->header.two_zeroes=0;
          notFree->header.block_size=(roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)>>4;
          notFree->header.unused=0;


          temp=(void*) notFree;
          temp=temp+((notFree->header).block_size<<4);
          footer=(sf_footer*)(temp-SF_FOOTER_SIZE/8);

          footer->allocated=1;
          if (size==roundSize){
           footer->padded=0;
          }else{
           footer->padded=1;
          }
          footer->two_zeroes=0;
          footer->block_size=(roundSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)>>4;
          footer->requested_size=size;






     }


}











void sf_free(void *ptr) {
	if (ptr== NULL){
        abort();
    }

    if ((ptr-SF_HEADER_SIZE/8)<get_heap_start() ){//   || (ptr-SF_HEADER_SIZE/8)>get_heap_end()){
        abort();
    }

    sf_free_header *t_header=ptr-SF_HEADER_SIZE/8;


    if(((ptr-SF_HEADER_SIZE/8)+((t_header->header.block_size)<<4)) >get_heap_end()){
        abort();
    }

    sf_footer *t_footer=(ptr-SF_HEADER_SIZE/8)+((t_header->header.block_size)<<4)-SF_FOOTER_SIZE/8;



    if(t_header->header.allocated==0 || t_footer->allocated==0){
        abort();
    }

    if (t_header->header.padded!=t_footer->padded){
        abort();
    }

    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 != (t_footer->block_size)<<4)  &&  t_footer->padded ==0){
        abort();
    }

    if(((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8) == (t_footer->block_size)<<4)  &&  t_footer->padded ==1){
        abort();
    }
    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 != (t_header->header.block_size)<<4)  &&  t_header->header.padded ==0){
        abort();
    }
    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 == (t_header->header.block_size)<<4)  &&  t_header->header.padded ==1){
        abort();
    }


     sf_free_header *nextHeaderInList=((void*)t_footer)+SF_FOOTER_SIZE/8;

     if (((void*)nextHeaderInList)<get_heap_end() && nextHeaderInList->header.allocated==0){
        //combine

                            if(nextHeaderInList->next!=NULL && nextHeaderInList->prev!=NULL){
                              (nextHeaderInList->next)->prev=nextHeaderInList->prev;
                              (nextHeaderInList->prev)->next=nextHeaderInList->next;

                            }else if(nextHeaderInList->next!=NULL && nextHeaderInList->prev==NULL){
                              (nextHeaderInList->next)->prev=NULL;

                            }else if(nextHeaderInList->next==NULL && nextHeaderInList->prev!=NULL){
                              (nextHeaderInList->prev)->next=NULL;

                            }else if(nextHeaderInList->next==NULL && nextHeaderInList->prev==NULL){

                              size_t tindex=find_list_from_size((nextHeaderInList->header.block_size)<<4);
                              if(tindex==-1){
                                abort();
                              }
                              seg_free_list[tindex].head=NULL;


                            }


                            size_t newBlockSize=((nextHeaderInList->header.block_size)<<4)+((t_header->header.block_size)<<4);

                            t_header->next=NULL;
                            t_header->prev=NULL;
                            t_header->header.allocated=0;
                            t_header->header.padded=0;
                            t_header->header.two_zeroes=0;
                            t_header->header.block_size=newBlockSize>>4;
                            t_header->header.unused=0;

                            sf_footer* t_newFooter=(((void*)t_header)+newBlockSize-SF_FOOTER_SIZE/8);

                            t_newFooter->allocated=0;
                            t_newFooter->padded=0;
                            t_newFooter->two_zeroes=0;
                            t_newFooter->block_size=newBlockSize>>4;
                            t_newFooter->requested_size=0;




                            if (newBlockSize>=LIST_1_MIN && newBlockSize<=LIST_1_MAX){
                               if(seg_free_list[0].head!=NULL){
                               seg_free_list[0].head->prev=t_header;
                               t_header->next=seg_free_list[0].head;
                               t_header->prev=NULL;
                              }
                               seg_free_list[0].head=t_header;

                            }else if (newBlockSize>=LIST_2_MIN && newBlockSize<=LIST_2_MAX){
                               if(seg_free_list[1].head!=NULL){
                               seg_free_list[1].head->prev=t_header;
                               t_header->next=seg_free_list[1].head;
                               t_header->prev=NULL;
                               }
                               seg_free_list[1].head=t_header;

                            }else if (newBlockSize>=LIST_3_MIN && newBlockSize<=LIST_3_MAX){
                               if(seg_free_list[2].head!=NULL){
                               seg_free_list[2].head->prev=t_header;
                               t_header->next=seg_free_list[2].head;
                               t_header->prev=NULL;
                               }
                               seg_free_list[2].head=t_header;
                            }else if(newBlockSize>=LIST_4_MIN && newBlockSize<=LIST_4_MAX){
                                if(seg_free_list[3].head!=NULL){
                               seg_free_list[3].head->prev=t_header;
                               t_header->next=seg_free_list[3].head;
                               t_header->prev=NULL;
                               }
                               seg_free_list[3].head=t_header;
                            }else{
                              abort();
                            }



     }else{
        t_header->header.allocated=0;
        t_header->header.padded=0;
        t_header->header.two_zeroes=0;
        t_header->header.unused=0;
        t_footer->allocated=0;
        t_footer->padded=0;
        t_footer->two_zeroes=0;
        t_footer->requested_size=0;



        size_t blockSize=((t_header->header.block_size)<<4);

        if (blockSize>=LIST_1_MIN && blockSize<=LIST_1_MAX){
          if(seg_free_list[0].head!=NULL){
            seg_free_list[0].head->prev=t_header;
            t_header->next=seg_free_list[0].head;
            t_header->prev=NULL;
            seg_free_list[0].head=t_header;
          }else{
            seg_free_list[0].head=t_header;
            t_header->next=NULL;
            t_header->prev=NULL;
          }

        }else if(blockSize>=LIST_2_MIN && blockSize<=LIST_2_MAX){
           if(seg_free_list[1].head!=NULL){
            seg_free_list[1].head->prev=t_header;
            t_header->next=seg_free_list[1].head;
            t_header->prev=NULL;
            seg_free_list[1].head=t_header;
           }else{
            seg_free_list[1].head=t_header;
            t_header->next=NULL;
            t_header->prev=NULL;
           }

        }else if(blockSize>=LIST_3_MIN && blockSize<=LIST_3_MAX){
            if(seg_free_list[2].head!=NULL){
            seg_free_list[2].head->prev=t_header;
            t_header->next=seg_free_list[2].head;
            t_header->prev=NULL;
            seg_free_list[2].head=t_header;
           }else{
            seg_free_list[2].head=t_header;
            t_header->next=NULL;
            t_header->prev=NULL;
           }

        }else if(blockSize>=LIST_4_MIN && blockSize<=LIST_4_MAX){
            if(seg_free_list[3].head!=NULL){
            seg_free_list[3].head->prev=t_header;
            t_header->next=seg_free_list[3].head;
            t_header->prev=NULL;
            seg_free_list[3].head=t_header;
           }else{
            seg_free_list[3].head=t_header;
            t_header->next=NULL;
            t_header->prev=NULL;
           }
        }else {
          abort();
        }



     }




}





void *sf_realloc(void *ptr, size_t size) {
if (ptr== NULL){
       // sf_errno=EINVAL;
      //  ptr=NULL;
        //return NULL;
  abort();
    }

    if ((ptr-SF_HEADER_SIZE/8)<get_heap_start()){  // || (ptr-SF_HEADER_SIZE/8)>get_heap_end()){
      //  sf_errno=EINVAL;
      //  return NULL;
      abort();
    }

 sf_free_header *t_header=ptr-SF_HEADER_SIZE/8;
    if(((ptr-SF_HEADER_SIZE/8)+((t_header->header.block_size)<<4)) >get_heap_end()){
       //   sf_errno=EINVAL;
        //return NULL;
      abort();
    }

    sf_footer *t_footer=(ptr-SF_HEADER_SIZE/8)+((t_header->header.block_size)<<4)-SF_FOOTER_SIZE/8;


    if(t_header->header.allocated==0 || t_footer->allocated==0){
      //  sf_errno=EINVAL;
       // return NULL;
      abort();
    }
    if (t_header->header.padded!=t_footer->padded){
      //  sf_errno=EINVAL;
      //  return NULL;
      abort();
    }

    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 != (t_footer->block_size)<<4)  &&  t_footer->padded ==0){
       // sf_errno=EINVAL;
        //return NULL;
      abort();
    }

    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 == (t_footer->block_size)<<4)  &&  t_footer->padded ==1){
       // sf_errno=EINVAL;
      //  return NULL;
      abort();
    }
    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 != (t_header->header.block_size)<<4)  &&  t_header->header.padded ==0){
        //sf_errno=EINVAL;
        //return NULL;
      abort();
    }
    if((t_footer->requested_size+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8 == (t_header->header.block_size)<<4)  &&  t_header->header.padded ==1){
        //sf_errno=EINVAL;
        //return NULL;
      abort();
    }


    if(size==0){
        sf_free(ptr);
        return NULL;
    }



  size_t oldSize=((((sf_header*)(ptr-SF_HEADER_SIZE/8))->block_size)<<4)-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;//old payload,not header and footer
   if (oldSize==roundTo16(size)){
      //no split
      if(size==roundTo16(size)){
        sf_free_header * tempHeader=ptr-SF_HEADER_SIZE/8;
        tempHeader->header.padded=0;

        sf_footer *tempfooter=ptr-SF_HEADER_SIZE/8+oldSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;
        tempfooter->padded=0;
        tempfooter->requested_size=size;

      }else{
        sf_free_header * tempHeader=ptr-SF_HEADER_SIZE/8;
        tempHeader->header.padded=1;

        sf_footer *tempfooter=ptr-SF_HEADER_SIZE/8+oldSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;
        tempfooter->padded=1;
        tempfooter->requested_size=size;



      }

      return ptr;
   }else if(size>oldSize){
//re bigsize

        void* newMeo=sf_malloc(size);

        if (newMeo==NULL){
            return NULL;
        }

        memcpy(newMeo,ptr,oldSize);

        sf_free(ptr);

        return newMeo;
   }else{

//re small size
        if  ( (oldSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)-(roundTo16(size)+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)<32){

        //not split
              sf_free_header* nheader=ptr-SF_HEADER_SIZE/8;
              sf_footer* nfooter=(((void*)nheader)+oldSize+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)-SF_FOOTER_SIZE/8;

              if (oldSize==size){
              nheader->header.padded=0;
              nfooter->padded=0;

              }else{
              nheader->header.padded=1;
              nfooter->padded=1;
              }
              nfooter->requested_size=size;
            return ptr;
        }else{//split

            sf_header *oldTempHeader=ptr-SF_HEADER_SIZE/8;

            size_t newBlockSize=roundTo16(size)+(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;

            sf_header *returnHeader=oldTempHeader;
            sf_footer *returnFooter=((void*)returnHeader)+newBlockSize-SF_FOOTER_SIZE/8;
            sf_header *backListHeader=((void*)returnHeader)+newBlockSize;

            sf_footer *backListFooter=((void*)backListHeader)+(oldSize-roundTo16(size))-SF_FOOTER_SIZE/8;






            backListHeader->allocated=1;
            if(oldSize-roundTo16(size)-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8!=roundTo16(oldSize-roundTo16(size)-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8)){
             backListHeader->padded=1;
            }else{
            backListHeader->padded=0;
            }
            backListHeader->two_zeroes=0;
       backListHeader->block_size=(oldSize-roundTo16(size))>>4;

            backListHeader->unused=0;

            backListFooter->allocated=1;
            backListFooter->padded=backListHeader->padded;
            backListFooter->two_zeroes=0;
         backListFooter->block_size=(oldSize-roundTo16(size))>>4;
           backListFooter->requested_size=oldSize-roundTo16(size)-(SF_HEADER_SIZE+SF_FOOTER_SIZE)/8;

            sf_free(((void*)backListHeader)+SF_HEADER_SIZE/8);






   ///the return ptr
            returnHeader->allocated=1;
            if(roundTo16(size)!=size){
             returnHeader->padded=1;
            }else{
            returnHeader->padded=0;
            }
            returnHeader->two_zeroes=0;

            returnHeader->block_size=newBlockSize>>4;
            returnHeader->unused=0;

            returnFooter->allocated=1;
            returnFooter->padded=returnHeader->padded;
            returnFooter->two_zeroes=0;

            returnFooter->block_size=newBlockSize>>4;
            returnFooter->requested_size=size;

            return ((void*)returnHeader)+SF_HEADER_SIZE/8;



        }


   }
   return ptr;

}
