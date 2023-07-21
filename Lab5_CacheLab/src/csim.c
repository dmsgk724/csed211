//박은하 20200445 dmsgk724
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

int h_mod, v_mod=0;
int num_hits, num_miss, num_evict=0;

int S,s,E,b,t;
char filename[1000];
FILE *fp=NULL;

typedef struct{
    int valid_bit;
    unsigned tag;
    int LRU;

}line;

line**cache=NULL;

void update(unsigned long address)
{
    int max_LRU=INT_MIN;
    int max_LRU_id=-1;
    unsigned set_ind=(address>>b)&((-1UL)>>(64-s));
    unsigned t_bit=address>>(s+b);

    for(int i=0;i<E;i++)
    if (cache[set_ind][i].tag == t_bit)
      {
         cache[set_ind][i].LRU = 0;
         num_hits++;
         if(v_mod==1)
         {printf("hit\n");}
         return;
      }
        for (int i = 0; i < E; i++)
   {
      if (cache[set_ind][i].valid_bit == 0)
      {
         cache[set_ind][i].valid_bit = 1;
         cache[set_ind][i].tag = t_bit;
         cache[set_ind][i].LRU = 0;
         num_miss++;
            if(v_mod==1)
         {printf("miss\n");}
         return;
      }
   }
   
   
   for (int i = 0; i < E; i++)
   {
      if (cache[set_ind][i].LRU > max_LRU)
      {
         max_LRU = cache[set_ind][i].LRU;
         max_LRU_id = i;
      }
      
   }
   cache[set_ind][max_LRU_id].tag = t_bit;
   cache[set_ind][max_LRU_id].LRU = 0;
   num_miss++;
   num_evict++;
      if(v_mod==1)
         {printf("miss eviction\n");}
   return;
}

void update_unused()
{
   for (int i = 0; i < S; i++)
   {
      for (int j = 0; j < E; j++)
      {
         if (cache[i][j].valid_bit == 1)
            cache[i][j].LRU++;
      }
   }
}


int main(int argc, char * argv[])
{
  int opt,temp;
   char LMS;
   unsigned long address;
   num_hits = num_miss = num_evict = 0;
   
   
   while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
   {
      switch (opt)
      {
      case 'h':h_mod = 1;
         break;
      case 'v':v_mod = 1;
         break;
      case 's':s=atoi(optarg);
         S = 1 << s;
         break;
      case 'E':E= atoi(optarg);
         break;
      case'b':b = atoi(optarg);
         break;
      case 't':fp = fopen(optarg, "r");
         break;
      }
   }
   if (h_mod == 1)
   {
      printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n-h         Print this help message.\n-v         Optional verbose flag.\n-s <num>   Number of set index bits.\n-E <num>   Number of lines per set.\n-b <num>   Number of block offset bits.\n-t <file>  Trace file.\n\nExamples:\n  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
      exit(0);
   }
   
   if (fp == NULL)
   {
      fprintf(stderr, "File is wrong!\n");
      exit(-1);

   }
   cache = (line**)malloc(sizeof(line*) * S);
   for (int i = 0; i < S; i++)
   {
      *(cache+i) = (line*)malloc(sizeof(line) * E);
   }
   for (int i = 0; i < S; i++)
   {
      for (int j = 0; j < E; j++)
      {
         cache[i][j].valid_bit = 0;
         cache[i][j].tag = -1;
         cache[i][j].LRU = -1;
      }
   }
   while (fscanf(fp ,"%c %lx, %d\n", &LMS, &address, &temp)>0)
   {
      
      switch (LMS)
      {
      case 'I':continue;   
      case 'M':
      if((v_mod==1))
      {printf("%c %lx, %d", LMS, address, temp);}
      update(address);
      case 'L':
      if((v_mod==1))
      {printf("%c %lx, %d", LMS, address, temp);}
      update(address);
         break;
      case 'S':
      if((v_mod==1))
      {printf("%c %lx, %d", LMS, address, temp);}
      update(address);
         break;
      }
      update_unused();
   }
   fclose(fp);
   for (int i = 0; i < S; i++)
      free(cache[i]);
   free(cache);

   printSummary(num_hits, num_miss, num_evict);
   return 0;

}
