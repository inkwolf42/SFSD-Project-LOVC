#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define STRING_MAX_LENGTH 30
#define BLOCK_MAX_SIZE 30

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


int WriteDir(FILE *f,TBlock b,int index,int offset){
   if(b.val==NULL)return 0;
   fseek(f,(index*BLOCK_MAX_SIZE)+sizeof(THeader)+offset,SEEK_SET);
   TStudent *cur = b.val;
   long c = offset;
   char g[3],n[6]; 
   while(cur!=NULL){
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
      fputc('\n',f);
      c+=(strlen(cur->id)+strlen(cur->Fname)+strlen(cur->Lname)+5+5+strlen(g));
      cur=cur->next;
   }
   return c-BLOCK_MAX_SIZE;
}

TBlock ReadDir(FILE *f,int index ,int offset){
   fseek(f,(index*BLOCK_MAX_SIZE)+sizeof(THeader)+(offset),SEEK_SET);
   TBlock b={NULL,NULL,0,offset};
   if(feof(f))return b;
   TStudent *cur;
   char g[3];
   while(b.nc<BLOCK_MAX_SIZE  && !feof(f)){
      TStudent *handler = (TStudent *)malloc(sizeof(TStudent));
      fscanf(f,"%[^|]",handler->id);
      fseek(f,1,SEEK_CUR);
      fscanf(f,"%[^|]",handler->Fname);
      fseek(f,1,SEEK_CUR);
      fscanf(f,"%[^|]",handler->Lname);
      fseek(f,1,SEEK_CUR);
      fscanf(f,"%d",&handler->group);
      fseek(f,1,SEEK_CUR);
      fscanf(f,"%f",&handler->Note);
      fseek(f,1,SEEK_CUR);
      sprintf(g,"%d",handler->group);
      b.nc+=(strlen(handler->id)+strlen(handler->Fname)+strlen(handler->Lname)+5+5+strlen(g));
      handler->next = NULL;
      b.length++;
      if(b.val==NULL){
         b.val = handler;
         cur = b.val;
      }else{
         cur->next = handler;
         cur = cur->next;
      }
   }
   return b;
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
   char g[3];
   strcpy(st->id,id);
   strcpy(st->Fname,Fname);
   strcpy(st->Lname,Lname);
   st->next = NULL;
   st->Note=note;
   st->group=group;
   sprintf(g,"%d",st->group);
   *s+=(strlen(st->id)+strlen(st->Fname)+strlen(st->Lname)+5+5+strlen(g));
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
         b.nc = of;
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
   printf("Records : %d\nBlocks : %d\n%d\n",h.NumberOfRecordes,h.NumberOfBlocks,b.nc);
   SetHeader(f,&h);
}

void display(TStudent *s){
   printf("\nID : %s\nFirst Name : %s\nLast Name : %s\nGroup : %d\nNote : %05.2f\n_______________\n",s->id,s->Fname,s->Lname,s->group,s->Note);
}

void PrintAll(FILE *f){
   THeader h = Header(f);
   printf("\n%d\n",h.NumberOfBlocks);
   printf("\n%d\n",h.NumberOfRecordes);
   int blocks = 0;
   int offset = 0;
   TBlock b;
   int r = 0;
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      b = ReadDir(f,blocks,offset);
      blocks++;
      offset = b.nc-BLOCK_MAX_SIZE;
      while(b.val!=NULL && r<h.NumberOfRecordes){
         display(b.val);
         r++;
         TStudent *tmp = b.val;
         b.val = b.val->next;
         free(tmp);
      }
   }
}


int main(){
   FILE *f = fopen("test.txt","w");
      
   init(f);

   fclose(f);
   f = fopen("test.txt","r");
   PrintAll(f);

   return 0;
}
