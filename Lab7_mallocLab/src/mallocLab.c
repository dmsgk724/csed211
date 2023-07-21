 /*
 segregation free list with seg_prev& seg_next
 segregation free list is almostly same with explicit free list
 except for placing block according to size

 when free block's size is given we divide it into 2 repeatedly until size is less or equal than 1
 number of using divide is used to determine where of the segmentation list the block is added

when find segmentation list index, it is inserted by ascending order
when I implement malloc or realloc I used first fit to find available free block
it is the same effect of best fit because i inseterd free list of order.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*define Macros*/
#define WORD 4
#define DWORD 8
#define ALLOCATED 1
#define FREE 0

#define CHUNKSIZE (1<<12)
#define SEG_LIM_NUM 20
#define GET(ptr) (*(unsigned int *)(ptr))
#define PUT(ptr, value) (*(unsigned int *)(ptr) = (value))

#define UPDATE(p, ptr) (*(unsigned int *)(p) = (unsigned int)(ptr))

#define PACK(size, alloc) ((size) | (alloc))
#define GET_SIZE(ptr) (GET(ptr) & ~0x7)
#define GET_ALLOC(ptr) (GET(ptr) & 0x1)
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define HDRP(bp) ((char*)(bp)-WORD)
#define FTRP(bp) ((char*)(bp)+GET_SIZE(HDRP(bp))-DWORD)
#define NEXT_BLOCK_PTR(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WORD)))
#define PREV_BLOCK_PTR(bp) ((char *)(bp) -GET_SIZE(((char *)(bp) - DWORD)))

#define PRED_FREE_PTR(bp)  ((char*)bp)//it is used to implement explict free list
#define NEXT_FREE_PTR(bp)  ((char*)(bp)+WORD)
#define SEG_PREV(bp) (*(char**)bp)//it is used to implement seg list 
#define SEG_NEXT(bp) (*(char**)(NEXT_FREE_PTR(bp)))

static void * seg_list;
static char * heap_list;
char *epilogue_block = 0;
int free_count = 0;

#define SEG_LIST(index) (*((char **)seg_list + index))

/*function declaration*/
static void * extend_heap(size_t size);
static void * find_fit(size_t asize);
static void insert_block(void * block, size_t size);
static void delete_block(void * block);
static void * place(void * block, size_t asize);
static void * coalesce(void * block);
// static int mm_check(void);

// static int checkInFreelist(void);
// static int checkOverlap(void);
// static int checklistfree_coalesce(void);
// static int valid_heap(void);

/* 
 * mm_init - initialize the malloc package.
 * reference from textbook
 init heap_list. put prologue header,footer,padding,epilogueheader
 & init seg_list
 */
int mm_init(void)
{
seg_list=mem_sbrk(SEG_LIM_NUM*WORD);

int i=0;
while(i<SEG_LIM_NUM)
{
    SEG_LIST(i)=NULL;
    i++;
}

if((heap_list=mem_sbrk(4*WORD))==(void*)-1)
{
    return -1;
}
    PUT(heap_list, 0);//padding         
    PUT(heap_list + (1 * WORD), PACK(DWORD, ALLOCATED)); //prologue HDRP
    PUT(heap_list + (2 * WORD), PACK(DWORD, ALLOCATED)); //prologue FTRP
    PUT(heap_list + (3 * WORD), PACK(0, ALLOCATED));
    epilogue_block = (heap_list + (3*WORD));  //epilogue 
    heap_list += (2 * WORD);

    if(extend_heap(1<<7)==NULL)//init heap size
    {
        return -1;    
    }

    return 0;
}
/* 
 * mm_malloc - Allocate a block by fitting(first fit)
 *  
 */
void *mm_malloc(size_t size)
{
   
    size_t asize;
    size_t extendsize;
    void * block;
    void * ptr=NULL;

    if(size==0)
    {return NULL;}

    asize= ALIGN(size+DWORD);
    if((block = find_fit(asize)) != NULL)//if available free block exits
    {
        ptr= place(block, asize); //place
        return ptr;
    }
    
    extendsize=MAX(asize,CHUNKSIZE);//if not 
    if((block=extend_heap(extendsize))==NULL)//extend heap
    return NULL;
     ptr= place(block, asize);//place
        return ptr;
    

}

/*
 * mm_free - Freeing a block by updating alloc bits, and call coalsce
 */

void mm_free(void *ptr)
{
    
    size_t size=GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr),PACK(size,FREE));
    PUT(FTRP(ptr),PACK(size,FREE));
    insert_block(ptr,size);//insert free block
    coalesce(ptr);
    //mm_check();
    return;
}

/*
 * mm_realloc -there is many case according to block size and aligned new size
 if there is no more space consider next block. if next block space is not enough call extend heap.
 if there isno next block, call malloc.
 */
void *mm_realloc(void *ptr, size_t size)
{
   void * new_ptr;
    size_t new_size=ALIGN(size);
    size_t old_size=GET_SIZE(HDRP(ptr))-DWORD;
    int left;
    int extendsize;

    if(ptr==NULL)//when ptr is NULL call malloc
    {
        return mm_malloc(size);
    }

    if(size==0)//call free
    {
    mm_free(ptr);
    return NULL;
    }

    int sub_b=old_size-new_size;
    
    if(sub_b<0)//if oldsize<neWORD -> size expanding
    {
        void * nextptr = NEXT_BLOCK_PTR(ptr);
        int next_size=GET_SIZE(HDRP(nextptr)); 
        if(nextptr!=NULL&&(GET_ALLOC(HDRP(nextptr))==FREE))//when next block is free
        {
            left=old_size+next_size-new_size; 
            if(left<0)//it needs to extend heap size
            {
                extendsize=MAX(-left,CHUNKSIZE);
                if(extend_heap(extendsize)==NULL)
                {return NULL;}
                left+=extendsize;

            }

            delete_block(nextptr);//delete next free block
            if(left<=DWORD)//if substraction is to small not split
            {PUT(HDRP(ptr), PACK(new_size+left+DWORD, 1)); 
            PUT(FTRP(ptr), PACK(new_size+left+DWORD, 1)); 
            return ptr;
            }
            else//split
            {
              PUT(HDRP(ptr), PACK(new_size+DWORD, 1)); 
            PUT(FTRP(ptr), PACK(new_size+DWORD, 1)); 
            new_ptr=ptr;
            ptr=NEXT_BLOCK_PTR(new_ptr);
            PUT(HDRP(ptr), PACK(left, FREE)); 
            PUT(FTRP(ptr), PACK(left, FREE)); 
            insert_block(ptr,GET_SIZE(HDRP(ptr)));
            coalesce(ptr);
            return new_ptr;
            }

        }
        //next block is not free
        else{
            new_ptr=mm_malloc(size);//call malloc
            if(new_ptr==NULL)
            return NULL;
            memcpy(new_ptr,ptr,old_size);//copy ptr to new_ptr
            mm_free(ptr);//free old ptr
            return new_ptr;
        }
    }

    else if(sub_b==0)//if block size is same with requested size
    {return ptr;}
    else//if oldsize>new size shrink size
    {
        if(old_size-new_size-DWORD<=DWORD)
        {return ptr;}//not enough size difference to split 
        PUT(HDRP(ptr),PACK(new_size+DWORD,ALLOCATED));//split
        PUT(FTRP(ptr),PACK(new_size+DWORD,ALLOCATED));
        new_ptr=ptr;
        ptr=NEXT_BLOCK_PTR(new_ptr);
        PUT((HDRP(ptr)),PACK(old_size-new_size,0));
        PUT((FTRP(ptr)),PACK(old_size-new_size,0));
        insert_block(ptr,GET_SIZE(HDRP(ptr)));
        coalesce(ptr);
        return new_ptr;
    }

   
}








/*helper functions*/

/*extend_heap -
extend heap by call mem_sbrk
*/
static void * extend_heap(size_t size) 
{
    void *ptr;                   
    size_t asize;               
    
    asize = ALIGN(size);
    
    if ((ptr = mem_sbrk(asize)) == (void *)-1)
        return NULL;
    
    PUT(HDRP(ptr), PACK(asize, 0));  
    PUT(FTRP(ptr), PACK(asize, 0));   
    PUT(HDRP(NEXT_BLOCK_PTR(ptr)), PACK(0, 1)); //new eplogue header
    insert_block(ptr, asize);

    ptr=coalesce(ptr);
    //mm_check();
    return ptr;

}

/*
find_fit -
find block in seg_list by first_fit
*/
static void * find_fit(size_t asize)
{
    int index=0;
    int tempsize=asize;
    void * list=NULL;

    while(index<SEG_LIM_NUM)//search for segregated list 
    {
    if((tempsize<=1)&&(SEG_LIST(index)!=NULL))//
    {
    list=SEG_LIST(index);

    while((list!=NULL)&&(asize>GET_SIZE(HDRP(list))))//first fit
    {
        list=SEG_PREV(list);
    }
    
    if(list!=NULL)
    return list;

    }
    tempsize>>=1;
    index++;

    if(index==SEG_LIM_NUM-1)
    return list;
    }
    return NULL;
}

/*
place - at free block put size
if sub is big split

to make reallocation optimization
if asize is bigger than 100,
free block is placed not in next block ptr, but in current ptr
*/
static void * place(void * block, size_t asize)/////////////okay!
{

    size_t b_size=GET_SIZE(HDRP(block));
    size_t sub=b_size-asize;
    
    delete_block(block);//old block delete
    if(sub<=DWORD)//if(block size - asize is smaller than DWORD)
    {
        PUT(HDRP(block), PACK(b_size, ALLOCATED)); 
        PUT(FTRP(block), PACK(b_size, ALLOCATED)); 
    }
    //else split
   else if(asize>100)//asize is bigger than 100 insert small sub block first to reallocate well
    {
        PUT(HDRP(block), PACK(sub, 0));
        PUT(FTRP(block), PACK(sub, 0));
        PUT(HDRP(NEXT_BLOCK_PTR(block)), PACK(asize, 1));
        PUT(FTRP(NEXT_BLOCK_PTR(block)), PACK(asize, 1));
        insert_block(block, sub);
        return NEXT_BLOCK_PTR(block);
    }

    else{
        PUT(HDRP(block), PACK(asize, ALLOCATED));
        PUT(FTRP(block), PACK(asize,ALLOCATED));
        PUT(HDRP(NEXT_BLOCK_PTR(block)), PACK(sub, FREE));
        PUT(FTRP(NEXT_BLOCK_PTR(block)), PACK(sub, FREE));
        insert_block(NEXT_BLOCK_PTR(block),sub);
    }

    return block;

}

/*
insert_block-
insert block into segregated free lists
it is inserted in ascending order(8(precedence)<-6<-4)
update connection between seg_list element
*/
static void insert_block(void *block, size_t size) //segregated free list에 insert
{    
  int i=0;
    void *prev_ptr=NULL;
    void *next_ptr=NULL;
    while((i<SEG_LIM_NUM-1)&&(size>1))
    {   
        size>>=1;
        i++;
    //searching segregated list;
    }

    prev_ptr=SEG_LIST(i);
    while((prev_ptr!=NULL)&&(size>GET_SIZE(HDRP(prev_ptr))))
    {
    next_ptr=prev_ptr;
    prev_ptr=SEG_PREV(prev_ptr);
    }

    if(next_ptr==NULL)
    {
        if(prev_ptr==NULL)
        { UPDATE(PRED_FREE_PTR(block),NULL);//update seg list
                UPDATE(NEXT_FREE_PTR(block),NULL);
                SEG_LIST(i)=block; }
        else{
            UPDATE(PRED_FREE_PTR(block),prev_ptr);
            UPDATE(NEXT_FREE_PTR(prev_ptr),block);
            UPDATE(NEXT_FREE_PTR(block),NULL);
            SEG_LIST(i)=block;
        }
    }
    else{
        if(prev_ptr==NULL){
             
            UPDATE(PRED_FREE_PTR(block), NULL);
            UPDATE(NEXT_FREE_PTR(block), next_ptr);
            UPDATE(PRED_FREE_PTR(next_ptr), block);
        }
        else{
             UPDATE(PRED_FREE_PTR(block),prev_ptr);
            UPDATE(NEXT_FREE_PTR(prev_ptr),block);//위 두개와 아래 두개 연결되어있음
            UPDATE(NEXT_FREE_PTR(block),next_ptr);
            UPDATE(PRED_FREE_PTR(next_ptr),block);

        }
    }
return;
}

/*
delete_block
delete block and update connection between seg_list
*/

static void delete_block(void *block) 
{
int list=0;
size_t size =GET_SIZE(HDRP(block));
while ((list < SEG_LIM_NUM-1) && (size > 1)) {//use first fit
        size >>= 1;
        list++;
}

if(SEG_NEXT(block)==NULL)
{
    if(SEG_PREV(block)==NULL)
    {SEG_LIST(list)=NULL;}
    else
    {UPDATE(NEXT_FREE_PTR(SEG_PREV(block)),NULL);
        SEG_LIST(list)=SEG_PREV(block);}
}
else
{
    if(SEG_PREV(block)==NULL)
    {UPDATE(PRED_FREE_PTR(SEG_NEXT(block)),NULL);}
    else
    {  UPDATE(NEXT_FREE_PTR(SEG_PREV(block)),SEG_NEXT(block));
    UPDATE(PRED_FREE_PTR(SEG_NEXT(block)),SEG_PREV(block));}
}

return;

}

/*
coalesce-
colaesce between adjacent free blocks
*/
static void *coalesce(void *block){

    size_t palloc=GET_ALLOC(HDRP(PREV_BLOCK_PTR(block)));
    size_t nalloc=GET_ALLOC(HDRP(NEXT_BLOCK_PTR(block)));
    size_t size=GET_SIZE(HDRP(block));

    if (palloc &&  nalloc) {             //case1
        return block;
    }
   
    else if(!palloc&& nalloc) //case2
    {
    delete_block(block);
    delete_block(PREV_BLOCK_PTR(block));
    size += GET_SIZE(HDRP(PREV_BLOCK_PTR(block)));
    PUT(FTRP(block), PACK(size, 0));
    PUT(HDRP(PREV_BLOCK_PTR(block)), PACK(size, 0));
    block=PREV_BLOCK_PTR(block);

    }
    else if(palloc&&! nalloc) //case3
    {
        delete_block(block);
        delete_block(NEXT_BLOCK_PTR(block));
        size+=GET_SIZE(HDRP(NEXT_BLOCK_PTR(block)));
        PUT(HDRP(block), PACK(size, 0));
        PUT(FTRP(block), PACK(size, 0));

    }
    else //case4
    {
        delete_block(PREV_BLOCK_PTR(block));
        delete_block(block);
        delete_block(NEXT_BLOCK_PTR(block));
        size+=GET_SIZE(HDRP(PREV_BLOCK_PTR(block)))+GET_SIZE(FTRP(NEXT_BLOCK_PTR(block)));
        PUT(HDRP(PREV_BLOCK_PTR(block)),PACK(size,0));
        PUT(FTRP(NEXT_BLOCK_PTR(block)),PACK(size,0));
        block=PREV_BLOCK_PTR(block);

    }

    insert_block(block,size);
    return block;

}





// static int mm_check(void)  
// { 
//     if (checklistfree_coalesce() == 0){
//         return 0;
//     }
//     if (checkInFreelist() == 0){
//         return 0;
//     }
//     if (checkOverlap() == 0){
//         return 0;
//     }
//     if (valid_heap() == 0){
//         return 0;
//     }
//     return 1;
// }

// /* check heap size is double word alignment
// & check heap block point to valid heap address
//  */
// static int valid_heap(void){
//     char *temp_block=NEXT_BLOCK_PTR(heap_list);

//     while(temp_block<epilogue_block)
//      {
//         if((HDRP(temp_block) < HDRP(NEXT_BLOCK_PTR(heap_list))) || (HDRP(temp_block) > epilogue_block) || GET_SIZE(HDRP(temp_block))%DWORD != 0) {
//             printf("not valid address: %p\n", temp_block);
//             return 0;
//         }
//         temp_block=NEXT_BLOCK_PTR(temp_block);
//     }
//     return 1;
// }

// /* 
//  * check every block in the free list marked as free
//  check are there any contiguous free blacks that somehow escaped coalescing
//  */
// static int checklistfree_coalesce(void)
// {

// int index=0;
// void * current=NULL;

//     while(index<SEG_LIM_NUM)
//     {
//     current=SEG_LIST(index);
//    while(current!=NULL)
//      {
//          if (GET_ALLOC(HDRP(current)) || GET_ALLOC(FTRP(current))){
//             return 0; 
//         }
//          if (PREV_BLOCK_PTR(current) != NULL && !GET_ALLOC(HDRP(PREV_BLOCK_PTR(current))))  {
//             return 0; 
//          }
//         if (NEXT_BLOCK_PTR(current) != NULL && !GET_ALLOC(HDRP(NEXT_BLOCK_PTR(current)))){
//             return 0;  
//         }
        
//         current = SEG_PREV(current);
//     }
//     index++;
// }
// return 1;
// }

// /*
// *  check any allocated blocks overlap
// */
// static int checkOverlap(void){
//     void *current = heap_list; 
//     while(current != NULL && GET_SIZE(HDRP(current))!=0){  
//         if(GET_ALLOC(HDRP(current))){  
//             void *next = NEXT_BLOCK_PTR(current);  
//             if (current + GET_SIZE(HDRP(current))>= next)
//                 return 0;
//         }
//         current = NEXT_BLOCK_PTR(current);
//     }
//     return 1;
// }
// /*
// *  check every free block is in free list
// */
// static int checkInFreelist(void){
//     void *current = heap_list;
//     void * list=NULL;
//     int i=0; // pointer to the beginning of heap
//     while (current != NULL && GET_SIZE(HDRP(current)) != 0){
        
//         if (GET_ALLOC(HDRP(current)) == 0){ //if it finds a free block
//            while(i<SEG_LIM_NUM)
//            {
//                list=SEG_LIST(i);
//                while(list!=NULL)
//                {
//                    if(list==current)
//                    break;
//                    list=SEG_PREV(list);
//                }

//                if(list==current)
//                break;
//                i++;
//            }
            
//         }
//         if(list==NULL)
//            return 0;
//         current = NEXT_BLOCK_PTR(current);
//     }
//     return 1;
// }
