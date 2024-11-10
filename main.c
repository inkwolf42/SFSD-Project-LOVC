#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define STRING_MAX_LENGTH 30
#define BLOCK_MAX_SIZE 50

typedef struct S{
   char id[STRING_MAX_LENGTH];
   char Fname[STRING_MAX_LENGTH];
   char Lname[STRING_MAX_LENGTH];
   int  group;
   float Note;
   struct S *next;
}TStudent;

typedef struct node{
   TStudent *val;
   struct node *next;
   int length;
   int nc;//Number of charachters
}TBlock;

typedef struct{
   int NumberOfBlocks;
   int NumberOfRecordes;
}THeader;



THeader Header(FILE *f){
   fseek(f,0,SEEK_SET);
   THeader h;
   fread(&h,sizeof(THeader),1,f);
   return h;
}

void SetHeader(FILE *f,THeader *h){
   fseek(f,0,SEEK_SET);
   fwrite(h,sizeof(THeader),1,f);
}

int WriteDir(FILE *f,TBlock b,int index ,int offset){
   if(b.val==NULL)return 0;
   fseek(f,(index*BLOCK_MAX_SIZE)+sizeof(THeader)+offset,SEEK_SET);
   TStudent *cur = b.val;
   long c = 1;
   char g[3],n[6]; 
   while(cur!=NULL){
      fputc('$',f);
      fputs(cur->id,f);
      fputc('|',f);
      fputs(cur->Fname,f);
      fputc('|',f);
      fputs(cur->Lname,f);
      fputc('|',f);
      sprintf(g,"%d",cur->group);
      fputs(g,f);
      fputc('|',f);
      sprintf(n,"%05.2f",cur->Note);
      fputs(n,f);
      c+=(strlen(cur->id)+strlen(cur->Fname)+strlen(cur->Lname)+4+5+strlen(g));
      cur=cur->next;
   }
   return c-BLOCK_MAX_SIZE;
}

char names[5][STRING_MAX_LENGTH]={
   "Hello",
   "World",
   "AAA",
   "D",
   "OvO"
};

TStudent* createStudent(int *s,char id[STRING_MAX_LENGTH],char Fname[STRING_MAX_LENGTH],char Lname[STRING_MAX_LENGTH],int group,float note){
   TStudent *st = (TStudent *) malloc(sizeof(TStudent));
   strcpy(st->id,id);
   strcpy(st->Fname,Fname);
   strcpy(st->Lname,Lname);
   st->next = NULL;
   st->Note=note;
   st->group=group;
   *s+=strlen(id)+strlen(Fname)+strlen(Lname)+5+(group>=100?3:(group>=10?2:1));
   return st;
}

void init(FILE *f){
   THeader h = {0,0};
   TBlock b={NULL,NULL,0,0};
   int of = 0;
   TStudent *p = NULL;
   for(int i = 0 ; i<10 ; i++){
      if(b.nc>BLOCK_MAX_SIZE){
         of = WriteDir(f,b,h.NumberOfBlocks,of);
         h.NumberOfRecordes+=b.length;
         h.NumberOfBlocks++;
         b.length=0;
         b.val=NULL;
         b.nc=0;
      }
      char id[STRING_MAX_LENGTH];
      sprintf(id,"Id_%03d",i);
      if(b.val==NULL){
         b.val = createStudent(&b.nc,id,names[i%5],names[(i+1)%5],1,(3.25*i));
         p=b.val;
      }else{
         p->next = createStudent(&b.nc,id,names[i%5],names[(i+1)%5],1,(3.25*i));
         p=p->next;
      }
      b.length++;
   }
   WriteDir(f,b,h.NumberOfBlocks,of);
   h.NumberOfRecordes+=b.length;
   h.NumberOfBlocks++;
   printf("Records : %d\nBlocks : %d\n",h.NumberOfRecordes,h.NumberOfBlocks);
   SetHeader(f,&h);
}


void add(int *a){
   (*a)++;
}

int main(){
   FILE *f = fopen("test.txt","w");
      
   init(f);
   int t = 0;
   add(&t);
   printf("%d",t);

   return 0;
}
