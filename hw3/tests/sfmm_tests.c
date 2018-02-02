#include <criterion/criterion.h>
#include <errno.h>
#include <signal.h>
#include "sfmm.h"
#include <stdio.h>

int find_list_index_from_size(int sz) {
	if (sz >= LIST_1_MIN && sz <= LIST_1_MAX) return 0;
	else if (sz >= LIST_2_MIN && sz <= LIST_2_MAX) return 1;
	else if (sz >= LIST_3_MIN && sz <= LIST_3_MAX) return 2;
	else return 3;
}

Test(sf_memsuite_student, Malloc_an_Integer_check_freelist, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	int *x = sf_malloc(sizeof(int));

	cr_assert_not_null(x);

	*x = 4;

	cr_assert(*x == 4, "sf_malloc failed to give proper space for an int!");

	sf_header *header = (sf_header*)((char*)x - 8);

	/* There should be one block of size 4064 in list 3 */
	free_list *fl = &seg_free_list[find_list_index_from_size(PAGE_SZ - (header->block_size << 4))];

	cr_assert_not_null(fl, "Free list is null");

	cr_assert_not_null(fl->head, "No block in expected free list!");
	cr_assert_null(fl->head->next, "Found more blocks than expected!");
	cr_assert(fl->head->header.block_size << 4 == 4064);
	cr_assert(fl->head->header.allocated == 0);
	cr_assert(sf_errno == 0, "sf_errno is not zero!");
	cr_assert(get_heap_start() + PAGE_SZ == get_heap_end(), "Allocated more than necessary!");
}

Test(sf_memsuite_student, Malloc_over_four_pages, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	void *x = sf_malloc(PAGE_SZ << 2);

	cr_assert_null(x, "x is not NULL!");
	cr_assert(sf_errno == ENOMEM, "sf_errno is not ENOMEM!");
}

Test(sf_memsuite_student, free_double_free, .init = sf_mem_init, .fini = sf_mem_fini, .signal = SIGABRT) {
	sf_errno = 0;
	void *x = sf_malloc(sizeof(int));
	sf_free(x);
	sf_free(x);
}

Test(sf_memsuite_student, free_no_coalesce, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	/* void *x = */ sf_malloc(sizeof(long));
	void *y = sf_malloc(sizeof(double) * 10);
	/* void *z = */ sf_malloc(sizeof(char));

	sf_free(y);

	free_list *fl = &seg_free_list[find_list_index_from_size(96)];

	cr_assert_not_null(fl->head, "No block in expected free list");
	cr_assert_null(fl->head->next, "Found more blocks than expected!");
	cr_assert(fl->head->header.block_size << 4 == 96);
	cr_assert(fl->head->header.allocated == 0);
	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sf_memsuite_student, free_coalesce, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	/* void *w = */ sf_malloc(sizeof(long));
	void *x = sf_malloc(sizeof(double) * 11);
	void *y = sf_malloc(sizeof(char));
	/* void *z = */ sf_malloc(sizeof(int));

	sf_free(y);
	sf_free(x);

	free_list *fl_y = &seg_free_list[find_list_index_from_size(32)];
	free_list *fl_x = &seg_free_list[find_list_index_from_size(144)];

	cr_assert_null(fl_y->head, "Unexpected block in list!");
	cr_assert_not_null(fl_x->head, "No block in expected free list");
	cr_assert_null(fl_x->head->next, "Found more blocks than expected!");
	cr_assert(fl_x->head->header.block_size << 4 == 144);
	cr_assert(fl_x->head->header.allocated == 0);
	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sf_memsuite_student, freelist, .init = sf_mem_init, .fini = sf_mem_fini) {
	/* void *u = */ sf_malloc(1);          //32
	void *v = sf_malloc(LIST_1_MIN); //48
	void *w = sf_malloc(LIST_2_MIN); //160
	void *x = sf_malloc(LIST_3_MIN); //544
	void *y = sf_malloc(LIST_4_MIN); //2080
	/* void *z = */ sf_malloc(1); // 32

	int allocated_block_size[4] = {48, 160, 544, 2080};

	sf_free(v);
	sf_free(w);
	sf_free(x);
	sf_free(y);

	// First block in each list should be the most recently freed block
	for (int i = 0; i < FREE_LIST_COUNT; i++) {
		sf_free_header *fh = (sf_free_header *)(seg_free_list[i].head);
		cr_assert_not_null(fh, "list %d is NULL!", i);
		cr_assert(fh->header.block_size << 4 == allocated_block_size[i], "Unexpected free block size!");
		cr_assert(fh->header.allocated == 0, "Allocated bit is set!");
	}

	// There should be one free block in each list, 2 blocks in list 3 of size 544 and 1232
	for (int i = 0; i < FREE_LIST_COUNT; i++) {
		sf_free_header *fh = (sf_free_header *)(seg_free_list[i].head);
		if (i != 2)
		    cr_assert_null(fh->next, "More than 1 block in freelist [%d]!", i);
		else {
		    cr_assert_not_null(fh->next, "Less than 2 blocks in freelist [%d]!", i);
		    cr_assert_null(fh->next->next, "More than 2 blocks in freelist [%d]!", i);
		}
	}
}

Test(sf_memsuite_student, realloc_larger_block, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(int));
	/* void *y = */ sf_malloc(10);
	x = sf_realloc(x, sizeof(int) * 10);

	free_list *fl = &seg_free_list[find_list_index_from_size(32)];

	cr_assert_not_null(x, "x is NULL!");
	cr_assert_not_null(fl->head, "No block in expected free list!");
	cr_assert(fl->head->header.block_size << 4 == 32, "Free Block size not what was expected!");

	sf_header *header = (sf_header*)((char*)x - 8);
	cr_assert(header->block_size << 4 == 64, "Realloc'ed block size not what was expected!");
	cr_assert(header->allocated == 1, "Allocated bit is not set!");
}

Test(sf_memsuite_student, realloc_smaller_block_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(int) * 8);
	void *y = sf_realloc(x, sizeof(char));

	cr_assert_not_null(y, "y is NULL!");
	cr_assert(x == y, "Payload addresses are different!");

	sf_header *header = (sf_header*)((char*)y - 8);
	cr_assert(header->allocated == 1, "Allocated bit is not set!");
	cr_assert(header->block_size << 4 == 48, "Block size not what was expected!");

	free_list *fl = &seg_free_list[find_list_index_from_size(4048)];

	// There should be only one free block of size 4048 in list 3
	cr_assert_not_null(fl->head, "No block in expected free list!");
	cr_assert(fl->head->header.allocated == 0, "Allocated bit is set!");
	cr_assert(fl->head->header.block_size << 4 == 4048, "Free block size not what was expected!");
}

Test(sf_memsuite_student, realloc_smaller_block_free_block, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(double) * 8);
	void *y = sf_realloc(x, sizeof(int));

	cr_assert_not_null(y, "y is NULL!");

	sf_header *header = (sf_header*)((char*)y - 8);
	cr_assert(header->block_size << 4 == 32, "Realloc'ed block size not what was expected!");
	cr_assert(header->allocated == 1, "Allocated bit is not set!");


	// After realloc'ing x, we can return a block of size 48 to the freelist.
	// This block will coalesce with the block of size 4016.
	free_list *fl = &seg_free_list[find_list_index_from_size(4064)];

	cr_assert_not_null(fl->head, "No block in expected free list!");
	cr_assert(fl->head->header.allocated == 0, "Allocated bit is set!");
	cr_assert(fl->head->header.block_size << 4 == 4064, "Free block size not what was expected!");
}


//############################################
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//############################################

#define UNUSED(x) (void)x


int roundTo16Test(size_t size){
    int k=size;
    if ( size%16 !=0){
       k=(size/16+1)*16;
    }
    return k;

}
 //have to include this two function, and my unit test base on
 /*
 LIST_1_MIN 32
 LIST_1_MAX 128
 LIST_2_MIN 129
 LIST_2_MAX 512
 LIST_3_MIN 513
 LIST_3_MAX 2048
 LIST_4_MIN 2049
 LIST_4_MAX -1
 PAGE_SZ 4096
 */

Test(sf_memsuite_student, malloc_each_size, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(1);
	void *y = sf_malloc(129);
	void *z = sf_malloc(513);
	void *w = sf_malloc(2049);
	void *e = sf_malloc(4097);





	sf_header *xheader = (x - 8);
	sf_header *yheader = (y - 8);
	sf_header *zheader = (z - 8);
	sf_header *wheader = (w - 8);
	sf_header *eheader = (e - 8);

    sf_footer *xfooter=x-8+(xheader->block_size<<4)-8;
    sf_footer *yfooter=y-8+(yheader->block_size<<4)-8;
    sf_footer *zfooter=z-8+(zheader->block_size<<4)-8;
    sf_footer *wfooter=w-8+(wheader->block_size<<4)-8;
    sf_footer *efooter=e-8+(eheader->block_size<<4)-8;

    cr_assert(xheader->allocated==1,"xheader allocated is not correct");
    cr_assert(yheader->allocated==1,"yheader allocated is not correct");
    cr_assert(zheader->allocated==1,"zheader allocated is not correct");
    cr_assert(wheader->allocated==1,"wheader allocated is not correct");
    cr_assert(eheader->allocated==1,"eheader allocated is not correct");

    cr_assert(xfooter->allocated==1,"xfooter allocated is not correct");
    cr_assert(yfooter->allocated==1,"yfooter allocated is not correct");
    cr_assert(zfooter->allocated==1,"zfooter allocated is not correct");
    cr_assert(wfooter->allocated==1,"wfooter allocated is not correct");
    cr_assert(efooter->allocated==1,"efooter allocated is not correct");

	cr_assert(xheader->block_size << 4 == 16+16, "xheader block_size is not correct");
	cr_assert(yheader->block_size << 4 == 128+16+16, "yheader block_size is not correct");
	cr_assert(zheader->block_size << 4 == 512+16+16, "zheader block_size is not correct");
	cr_assert(wheader->block_size << 4 == 2048+16+16, "wheader block_size is not correct");
	cr_assert(eheader->block_size << 4 == 4096+16+16, "eheader block_size is not correct");

	cr_assert(xfooter->block_size << 4 == 16+16, "xfooter block_size is not correct");
	cr_assert(yfooter->block_size << 4 == 128+16+16, "yfooter block_size is not correct");
	cr_assert(zfooter->block_size << 4 == 512+16+16, "zfooter block_size is not correct");
	cr_assert(wfooter->block_size << 4 == 2048+16+16, "wfooter block_size is not correct");
	cr_assert(efooter->block_size << 4 == 4096+16+16, "efooter block_size is not correct");

	cr_assert(xfooter-> requested_size == 1, "xfooter request_size is not correct");
	cr_assert(yfooter->requested_size  == 129, "yfooter request_size is not correct");
	cr_assert(zfooter->requested_size  == 513, "zfooter request_size is not correct");
	cr_assert(wfooter->requested_size  == 2049, "wfooter request_size is not correct");
	cr_assert(efooter->requested_size  == 4097, "efooter request_size is not correct");

    cr_assert(xheader-> padded == 1, "xheader padded is not correct");
    cr_assert(yheader-> padded == 1, "yheader padded is not correct");
    cr_assert(zheader-> padded == 1, "zheader padded is not correct");
    cr_assert(wheader-> padded == 1, "wheader padded is not correct");
    cr_assert(eheader-> padded == 1, "eheader padded is not correct");

    cr_assert(xfooter-> padded == 1, "xfooter padded is not correct");
    cr_assert(yfooter-> padded == 1, "yfooter padded is not correct");
    cr_assert(zfooter-> padded == 1, "zfooter padded is not correct");
    cr_assert(wfooter-> padded == 1, "wfooter padded is not correct");
    cr_assert(efooter-> padded == 1, "efooter padded is not correct");



    void *x1 = sf_malloc(16);
	void *y1 = sf_malloc(128);
	void *z1 = sf_malloc(512);
	void *w1 = sf_malloc(2048);
	void *e1 = sf_malloc(4096);





	sf_header *x1header = (x1 - 8);
	sf_header *y1header = (y1 - 8);
	sf_header *z1header = (z1 - 8);
	sf_header *w1header = (w1 - 8);
	sf_header *e1header = (e1 - 8);

    sf_footer *x1footer=x1-8+(x1header->block_size<<4)-8;
    sf_footer *y1footer=y1-8+(y1header->block_size<<4)-8;
    sf_footer *z1footer=z1-8+(z1header->block_size<<4)-8;
    sf_footer *w1footer=w1-8+(w1header->block_size<<4)-8;
    sf_footer *e1footer=e1-8+(e1header->block_size<<4)-8;

    cr_assert(x1header->allocated==1,"x1header allocated is not correct");
    cr_assert(y1header->allocated==1,"y1header allocated is not correct");
    cr_assert(z1header->allocated==1,"z1header allocated is not correct");
    cr_assert(w1header->allocated==1,"w1header allocated is not correct");
    cr_assert(e1header->allocated==1,"e1header allocated is not correct");

    cr_assert(x1footer->allocated==1,"x1footer allocated is not correct");
    cr_assert(y1footer->allocated==1,"y1footer allocated is not correct");
    cr_assert(z1footer->allocated==1,"z1footer allocated is not correct");
    cr_assert(w1footer->allocated==1,"w1footer allocated is not correct");
    cr_assert(e1footer->allocated==1,"e1footer allocated is not correct");

	cr_assert(x1header->block_size << 4 == 16+16, "x1header block_size is not correct");
	cr_assert(y1header->block_size << 4 == 128+16, "y1header block_size is not correct");
	cr_assert(z1header->block_size << 4 == 512+16, "z1header block_size is not correct");
	cr_assert(w1header->block_size << 4 == 2048+16, "w1header block_size is not correct");
	cr_assert(e1header->block_size << 4 == 4096+16, "e1header block_size is not correct");

	cr_assert(x1footer->block_size << 4 == 16+16, "x1footer block_size is not correct");
	cr_assert(y1footer->block_size << 4 == 128+16, "y1footer block_size is not correct");
	cr_assert(z1footer->block_size << 4 == 512+16, "z1footer block_size is not correct");
	cr_assert(w1footer->block_size << 4 == 2048+16, "w1footer block_size is not correct");
	cr_assert(e1footer->block_size << 4 == 4096+16, "e1footer block_size is not correct");

	cr_assert(x1footer-> requested_size == 16, "x1footer request_size is not correct");
	cr_assert(y1footer->requested_size  == 128, "y1footer request_size is not correct");
	cr_assert(z1footer->requested_size  == 512, "z1footer request_size is not correct");
	cr_assert(w1footer->requested_size  == 2048, "w1footer request_size is not correct");
	cr_assert(e1footer->requested_size  == 4096, "e1footer request_size is not correct");

    cr_assert(x1header-> padded == 0, "x1header padded is not correct");
    cr_assert(y1header-> padded == 0, "y1header padded is not correct");
    cr_assert(z1header-> padded == 0, "z1header padded is not correct");
    cr_assert(w1header-> padded == 0, "w1header padded is not correct");
    cr_assert(e1header-> padded == 0, "e1header padded is not correct");

    cr_assert(x1footer-> padded == 0, "x1footer padded is not correct");
    cr_assert(y1footer-> padded == 0, "y1footer padded is not correct");
    cr_assert(z1footer-> padded == 0, "z1footer padded is not correct");
    cr_assert(w1footer-> padded == 0, "w1footer padded is not correct");
    cr_assert(e1footer-> padded == 0, "e1footer padded is not correct");


}






Test(sf_memsuite_student, free_each_size, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(1);
	void *y = sf_malloc(129);
	void *z = sf_malloc(513);
	void *w = sf_malloc(2049);
	void *e = sf_malloc(4097);







    sf_free(x);
    sf_free(y);
    sf_free(z);
    sf_free(w);
    sf_free(e);



	free_list *xfl = &seg_free_list[find_list_index_from_size(roundTo16Test(1))];
	free_list *yfl = &seg_free_list[find_list_index_from_size(roundTo16Test(129))];
	free_list *zfl = &seg_free_list[find_list_index_from_size(roundTo16Test(513))];
	free_list *wfl = &seg_free_list[find_list_index_from_size(roundTo16Test(2049))];
	free_list *efl = &seg_free_list[find_list_index_from_size(roundTo16Test(4097))];

	cr_assert_not_null(xfl->head, "x is not correct");
	cr_assert_not_null(yfl->head, "y is not correct");
	cr_assert_not_null(zfl->head, "z is not correct");
	cr_assert_not_null(wfl->head, "w is not correct");
	cr_assert_not_null(efl->head, "e is not correct");







	sf_header *xheader = (x - 8);
	sf_header *yheader = (y - 8);
	sf_header *zheader = (z - 8);
	sf_header *wheader = (w - 8);
	sf_header *eheader = (e - 8);

    sf_footer *xfooter=x-8+(xheader->block_size<<4)-8;
    sf_footer *yfooter=y-8+(yheader->block_size<<4)-8;
    sf_footer *zfooter=z-8+(zheader->block_size<<4)-8;
    sf_footer *wfooter=w-8+(wheader->block_size<<4)-8;
    sf_footer *efooter=e-8+(eheader->block_size<<4)-8;

    cr_assert(xheader->allocated==0,"xheader allocated is not correct");
    cr_assert(yheader->allocated==0,"yheader allocated is not correct");
    cr_assert(zheader->allocated==0,"zheader allocated is not correct");
    cr_assert(wheader->allocated==0,"wheader allocated is not correct");
    cr_assert(eheader->allocated==0,"eheader allocated is not correct");

    cr_assert(xfooter->allocated==0,"xfooter allocated is not correct");
    cr_assert(yfooter->allocated==0,"yfooter allocated is not correct");
    cr_assert(zfooter->allocated==0,"zfooter allocated is not correct");
    cr_assert(wfooter->allocated==0,"wfooter allocated is not correct");
    cr_assert(efooter->allocated==0,"efooter allocated is not correct");

	cr_assert(xheader->block_size << 4 == 16+16, "xheader block_size is not correct");
	cr_assert(yheader->block_size << 4 == 128+16+16, "yheader block_size is not correct");
	cr_assert(zheader->block_size << 4 == 512+16+16, "zheader block_size is not correct");
	cr_assert(wheader->block_size << 4 == 2048+16+16, "wheader block_size is not correct");
    cr_assert(eheader->block_size << 4 == 4096+4096-32-160-544-2080, "eheader block_size is not correct");


	cr_assert(xfooter->block_size << 4 == 16+16, "xfooter block_size is not correct");
	cr_assert(yfooter->block_size << 4 == 128+16+16, "yfooter block_size is not correct");
	cr_assert(zfooter->block_size << 4 == 512+16+16, "zfooter block_size is not correct");
	cr_assert(wfooter->block_size << 4 == 2048+16+16, "wfooter block_size is not correct");
	cr_assert(((sf_footer*)( get_heap_end()-8))->block_size << 4 == 4096+4096-32-160-544-2080, "efooter block_size is not correct");

}





Test(sf_memsuite_student, realloc_big_size, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(1);
	void *y = sf_malloc(129);
	void *z = sf_malloc(513);
	void *w = sf_malloc(2049);
	void *e = sf_malloc(1);

    sf_header *yH=y-8;
    sf_header *wH=w-8;
    UNUSED(x);
    UNUSED(y);
    UNUSED(z);
    UNUSED(w);
    UNUSED(e);

	void *temp=sf_realloc(y,543);

	void *temp2=sf_realloc(w,5000);

	UNUSED(temp);

    sf_header *tempH=temp-8;
    sf_header *temp2H=temp2-8;



    cr_assert(tempH->allocated==1,"y realloc is not correct");
    cr_assert(yH->allocated==0,"y realloc is not correct");
    cr_assert(temp2H->allocated==1,"w realloc is not correct");
    cr_assert(wH->allocated==0,"w realloc is not correct");

    cr_assert(tempH->block_size<<4==528+16+16,"y block size is not correct");

    cr_assert(temp2H->block_size<<4==4992+16+16,"w block size is not correct");

    sf_footer *tempF=temp-8+(tempH->block_size<<4)-8;
    sf_footer *temp2F=temp2-8+(temp2H->block_size<<4)-8;

    cr_assert(tempF->allocated==1,"y realloc is not correct");
    cr_assert(temp2F->allocated==1,"w realloc is not correct");
    cr_assert(tempF->requested_size==543,"y realloc is not correct");
    cr_assert(temp2F->requested_size==5000,"w realloc is not correct");
    cr_assert(tempF->block_size<<4==528+16+16,"y block size is not correct");
    cr_assert(temp2F->block_size<<4==4992+16+16,"w block size is not correct");




}



Test(sf_memsuite_student, malloc_edge, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(1);
	void *y = sf_malloc(129);
	void *z = sf_malloc(513);
	void *w = sf_malloc(2049);
	void *e = sf_malloc(1);

    sf_header *yH=y-8;
    sf_header *wH=w-8;
    UNUSED(x);
    UNUSED(y);
    UNUSED(z);
    UNUSED(w);
    UNUSED(e);

	void *temp=sf_realloc(y,543);

	void *temp2=sf_realloc(w,5000);

	UNUSED(temp);

    sf_header *tempH=temp-8;
    sf_header *temp2H=temp2-8;



    cr_assert(tempH->allocated==1,"y realloc is not correct");
    cr_assert(yH->allocated==0,"y realloc is not correct");
    cr_assert(temp2H->allocated==1,"w realloc is not correct");
    cr_assert(wH->allocated==0,"w realloc is not correct");

    cr_assert(tempH->block_size<<4==528+16+16,"y block size is not correct");

    cr_assert(temp2H->block_size<<4==4992+16+16,"w block size is not correct");

    sf_footer *tempF=temp-8+(tempH->block_size<<4)-8;
    sf_footer *temp2F=temp2-8+(temp2H->block_size<<4)-8;

    cr_assert(tempF->allocated==1,"y realloc is not correct");
    cr_assert(temp2F->allocated==1,"w realloc is not correct");
    cr_assert(tempF->requested_size==543,"y realloc is not correct");
    cr_assert(temp2F->requested_size==5000,"w realloc is not correct");
    cr_assert(tempF->block_size<<4==528+16+16,"y block size is not correct");
    cr_assert(temp2F->block_size<<4==4992+16+16,"w block size is not correct");




}






Test(sf_memsuite_student, realloc_smaller_size, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(1);
	void *y = sf_malloc(129);
	void *z = sf_malloc(513);
	void *w = sf_malloc(20);
	void *e = sf_malloc(4097);
    UNUSED(x);
    UNUSED(y);
    UNUSED(z);
    UNUSED(w);
    UNUSED(e);

	void *temp=sf_realloc(z,132);

	sf_header* tempH=temp-8;
	sf_footer* tempF=temp-8+(tempH->block_size<<4)-8;
    cr_assert(tempH->allocated==1,"z realloc is not correct");
    cr_assert(tempH->block_size<<4==144+16,"z realloc block_size is not correct");
    cr_assert(tempH->padded==1,"z realloc padded is not correct");
    cr_assert(tempF->allocated==1,"z realloc is not correct");
    cr_assert(tempF->block_size<<4==144+16,"z realloc block_size is not correct");
    cr_assert(tempF->padded==1,"z realloc padded is not correct");
    cr_assert(tempF->requested_size==132,"z requested_size is not correct");

    sf_header * nextheader=temp-8+(tempH->block_size<<4);
    sf_footer * nextfooter=temp-8+(tempH->block_size<<4)+384-8;

    cr_assert(nextheader->allocated==0,"z realloc is not correct");
    cr_assert(nextheader->block_size<<4==384,"z realloc block_size is not correct");

    cr_assert(nextfooter->allocated==0,"z realloc is not correct");
    cr_assert(nextfooter->block_size<<4==384,"z realloc block_size is not correct");




    free_list *zfl = &seg_free_list[find_list_index_from_size(roundTo16Test(384))];

    cr_assert(((void*)(zfl->head))==(void*)nextheader,"z is not correct");

}



/*
Test(sf_memsuite_student, reallocToBIGcheckValue, .init = sf_mem_init, .fini = sf_mem_fini) {
  int * a =sf_malloc(158);
   *a=140948883;
   int * test=sf_realloc(a,54);
   cr_assert(*test==140948883);


}



Test(sf_memsuite_student, check_error1, .init = sf_mem_init, .fini = sf_mem_fini) {
    char temp[3]={'q','w'};
    sf_realloc(&temp,10);
    cr_assert(sf_errno==EINVAL,"wrong errno");
}

Test(sf_memsuite_student, check_error2, .init = sf_mem_init, .fini = sf_mem_fini) {
    sf_malloc(4096*4-16);

    cr_assert(sf_errno==0,"wrong errno");
    sf_malloc(1);
    cr_assert(sf_errno==ENOMEM,"wrong errno");


}

Test(sf_memsuite_student, check_error3, .init = sf_mem_init, .fini = sf_mem_fini) {
    sf_malloc(4096*4-15);

    cr_assert(sf_errno==ENOMEM,"wrong errno");
    sf_errno=0;
    sf_malloc(1);
    cr_assert(sf_errno==0,"wrong errno");



}

Test(sf_memsuite_student, check_error4, .init = sf_mem_init, .fini = sf_mem_fini) {
    sf_malloc(4096*2-16);

    cr_assert(sf_errno==0,"wrong errno");
    sf_malloc(4096*2-16);
    cr_assert(sf_errno==0,"wrong errno");

    sf_malloc(1);
    cr_assert(sf_errno==ENOMEM,"wrong errno");


}


Test(sf_memsuite_student, realloc_padded_to_not_padded, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *a=sf_malloc(160-16);
    sf_free_header *aHeader=a-8;
    cr_assert(aHeader->header.padded==0,"a padded is not correct");


    void *b=sf_realloc(a,160-20);
    sf_free_header *bHeader=b-8;
    cr_assert(bHeader->header.padded==1,"b padded is not correct");
    UNUSED(b);



}


Test(sf_memsuite_student, realloc_not_padded_to_padded, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *a=sf_malloc(145);
    sf_free_header *aHeader=a-8;
    cr_assert(aHeader->header.padded==1,"a padded is not correct");


    void *b=sf_realloc(a,160);
    sf_free_header *bHeader=b-8;
    cr_assert(bHeader->header.padded==0,"b padded is not correct");
    UNUSED(b);



}


*/


//cccccccccccccccccccccccccccccccccc
