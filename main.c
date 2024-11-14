#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define STRING_MAX_LENGTH 30
#define BLOCK_MAX_SIZE 200

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



int sizeOfRecord(TStudent *s){
   return strlen(s->Fname)+strlen(s->Lname)+5+strlen(s->id)+(s->group>=100?3:(s->group>=10?2:1))+5;
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

TBlock ReadDirNoOffset(FILE *f,int index ){
   fseek(f,(index*BLOCK_MAX_SIZE)+sizeof(THeader),SEEK_SET);
   int offset = 0;
   if(index!=0){
      while(!feof(f) && getc(f)!='\n')offset++;
   }
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


char names[7][STRING_MAX_LENGTH]={
   "Amar",
   "Benyoucef",
   "Dio",
   "Maan",
   "Mahmoud",
   "Salah",
   "Zoro"
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

int searchId(FILE *f,char id[STRING_MAX_LENGTH],int *i){
   THeader h = Header(f);
   int min = 0;
   int max = h.NumberOfBlocks-1;
   printf("%d,%d\n",min,max);
   int found = -1;
   char minID[STRING_MAX_LENGTH];
   while(found==-1 && min<=max){
      int mid = (min+max)/2;
      TBlock b = ReadDirNoOffset(f,mid);
      *i=0;
      if(b.val!=NULL){
         strcpy(minID,b.val->id);
         while(b.val!=NULL){
            printf("%s\n",b.val->id);
            if(strcmp(id,b.val->id)==0){
               printf("ID : %s\n",b.val->id);
               return mid;
               break;
            }
            (*i)++;
            TStudent *tmp = b.val;
            b.val = b.val->next;
            free(tmp);
         }
      }else return -1;
      if(strcmp(id,minID)>0){
         min = mid+1;
      }else{
         max = mid-1;
      }
   }
   return found;
}

int search(FILE *f,char fname[STRING_MAX_LENGTH],int *i){
   THeader h = Header(f);
   int min = 0;
   int max = h.NumberOfBlocks-1;
   int found = -1;
   char minID[STRING_MAX_LENGTH];
   while(found==-1 && min<=max){
      int mid = (min+max)/2;
      TBlock b = ReadDirNoOffset(f,mid);
      *i=0;
      if(b.val!=NULL){
         strcpy(minID,b.val->Fname);
         while(b.val!=NULL){
            if(strcmp(fname,b.val->Fname)==0){
               printf("ID : %s\n",b.val->id);
               return mid;
               break;
            }
            (*i)++;
            TStudent *tmp = b.val;
            b.val = b.val->next;
            free(tmp);
         }
      }else return -1;
      if(strcmp(fname,minID)>0){
         min = mid+1;
      }else{
         max = mid-1;
      }
   }
   return found;
}

int FindTheSpot(FILE *f,char fname[STRING_MAX_LENGTH],int *r){
   THeader h = Header(f);
   
   int blocks = 0;
   TBlock b;
   TStudent *prev=NULL;
   int pb = 0;
   while(blocks<h.NumberOfBlocks && *r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
      (*r)=0;
      while(b.val!=NULL && *r<h.NumberOfRecordes){
         if(prev!=NULL && strcmp(prev->Fname,fname)<0 && strcmp(fname,b.val->Fname)<0){
            return pb-1;
         }else if(prev!=NULL && strcmp(fname,b.val->Fname)<0){
            return 0;
         }
         (*r)++;
         TStudent *tmp = prev;
         prev = b.val;
         pb=blocks;
         b.val = b.val->next;
         free(tmp);
      }
   }
   (*r)--;
   return pb-1;
}



void Insert(FILE *f,TStudent *s){
   int a;
   int bi = FindTheSpot(f,s->Fname,&a);
   printf("%d,%d\n",bi,a);
   THeader h = Header(f);
   int blocks = 0;
   TBlock b,buf={NULL,NULL,0,0};
   int offset = 0;
   int r = 0,j = 0;
   int chars = 0;
   TStudent *p=NULL;
   int dd = 0;
   THeader header={0,0};
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
      j=0;
      while(b.val!=NULL && r<h.NumberOfRecordes){
         printf("%d,%d\n",blocks,j);
         j++;
         if(dd==0 && blocks==bi+1 && j==a){

            if(buf.val==NULL){
               buf.val = s;
               p=s;
               header.NumberOfRecordes++;
            }else{
               p->next = s;
               p=s;
               header.NumberOfRecordes++;
            }
            chars+=sizeOfRecord(s);
            if(chars>=BLOCK_MAX_SIZE){
               offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
               header.NumberOfBlocks++;
            }
            dd=1;
            continue;
         }
         if(buf.val==NULL){
            buf.val = b.val;
            p=b.val;
            header.NumberOfRecordes++;
         }else{
            p->next = b.val;
            p=b.val;
            header.NumberOfRecordes++;
         }
         chars+=sizeOfRecord(b.val);
         if(chars>=BLOCK_MAX_SIZE){
            offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
            header.NumberOfBlocks++;
         }
         r++;
         b.val = b.val->next;
      }
   }
   if(buf.val!=NULL){
      offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
      SetHeader(f,&header);
   }
   
}

void Delete(FILE *f , char id[STRING_MAX_LENGTH]){
   int a;
   int bi = searchId(f,id,&a);
   if(bi!=-1){
      TBlock b = ReadDirNoOffset(f,bi);
   }
}

void init(FILE *f){
   THeader h = {0,0};
   TBlock b={NULL,NULL,0,0};
   int of = 0;
   TStudent *p = NULL;
   for(int i = 0 ; i<7 ; i++){
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
         b.val = createStudent(&b.nc,id,names[i%7],names[(i+1)%7],1,(7*i%80)/4.0);
         p=b.val;
      }else{
         p->next = createStudent(&b.nc,id,names[i%7],names[(i+1)%7],1,(7*i%80)/4.0);
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
   TBlock b;
   int r = 0;
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
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
   /*int a = 0;*/
   /*TStudent s = *createStudent(&a,"Test","Mahmoud","Benyoucef",2,10.0);*/
   fclose(f);
   f = fopen("test.txt","r+");
   TStudent newS = {"Id_069","Mahmoud","Benyoucef",2,10,NULL};
   display(&newS);
   Insert(f,&newS);
   PrintAll(f);
   char Fid[STRING_MAX_LENGTH] = "Mahmoud"; 
   printf("\nthe Student that has id '%s' is : \n\n",Fid);
   int i = 0;
   int bi = FindTheSpot(f,Fid,&i);
   printf("%d,%d\n",bi,i);
   if(bi!=-1){
      TBlock b = ReadDirNoOffset(f,bi);
      printf("%d,%d\n",bi,i);
      while(i>0){
         i--;
         b.val = b.val->next;
      }
      display(b.val);
   }
   return 0;
}
