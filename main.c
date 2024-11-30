#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define STRING_MAX_LENGTH 30
#define BLOCK_MAX_SIZE 200

//Struct For student
typedef struct S{
   char id[STRING_MAX_LENGTH];
   char Fname[STRING_MAX_LENGTH];
   char Lname[STRING_MAX_LENGTH];
   int  group;
   float Note;
   struct S *next;
}TStudent;

//Struct For Block
typedef struct node{
   TStudent *val;
   struct node *next;
   int length;
   int nc;//Number of charachters
}TBlock;

//Struct For Header
typedef struct{
   int NumberOfBlocks;
   int NumberOfRecordes;
   int curId;
}THeader;


//Function for getting a header
THeader Header(FILE *f){
   fseek(f,0,SEEK_SET);
   THeader h;
   fread(&h,sizeof(THeader),1,f);
   return h;
}

//Function for setting a header
void SetHeader(FILE *f,THeader *h){
   fseek(f,0,SEEK_SET);
   fwrite(h,sizeof(THeader),1,f);
}


//Calculate the size of a Student Data
int sizeOfRecord(TStudent *s){
   return strlen(s->Fname)+strlen(s->Lname)+5+strlen(s->id)+(s->group>=100?3:(s->group>=10?2:1))+5;
}

//Function to Write a Block into a file
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

//Function to Read a Block from a file
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

//Function to Read a Block from a file (without Offset)
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

//Function to create a Student
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

//Search for student with specific ID ,Return -1 if not found (Can't emplement the binary Search)
int searchId(FILE *f,char id[STRING_MAX_LENGTH],int *r){
   THeader h = Header(f);
   
   int blocks = 0;
   TBlock b = ReadDirNoOffset(f,0);
   int pb = 0;
   (*r)=0;
   while(blocks<h.NumberOfBlocks && *r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      (*r)=0;
      while(b.val!=NULL && (*r)<h.NumberOfRecordes){
         if(strcmp(b.val->id,id)==0){
            return pb;
         }
         (*r)++;
         TStudent *tmp = b.val;
         pb=blocks;
         b.val = b.val->next;
         free(tmp);
      }
      blocks++;
   }
   (*r)--;
   return -1;
}

//Search for student with First Name ,Return -1 if not found (Binary Search Emplmented)
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

//Find the the place where the student should be inserted
int FindTheSpot(FILE *f,char fname[STRING_MAX_LENGTH],int *r){
   THeader h = Header(f);
   
   int blocks = 0;
   TBlock b = ReadDirNoOffset(f,0);
   TStudent *prev=NULL;
   int pb = 0;
   (*r)=0;
   if((b.val==NULL)||(strcmp(b.val->Fname,fname)>0))return 0;
   while(blocks<h.NumberOfBlocks && *r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
      (*r)=0;
      while(b.val!=NULL && *r<h.NumberOfRecordes){
         if(prev!=NULL && strcmp(prev->Fname,fname)<0 && strcmp(fname,b.val->Fname)<0){
            return pb-1;
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
   return pb;
}

//Clear the Block form student
void clearBlock(TBlock *b){
   TStudent *tmp;
   b->val=NULL;
   return;
   while (b->val!=NULL) {
      tmp = b->val;
      b->val=b->val->next;
      free(tmp);
   }
}


//Insert A Student in the correct order
void Insert(FILE *f,TStudent *s){
   int a=0;
   int bi = FindTheSpot(f,s->Fname,&a);
   THeader h = Header(f);
   int blocks = 0;
   TBlock b,buf={NULL,NULL,0,0};
   int offset = 0;
   int r = 0,j = 0;
   int chars = 0;
   TStudent *p=NULL;
   int dd = 0;
   THeader header={0,0,h.curId+1};
   b = ReadDirNoOffset(f,blocks);
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      blocks++;
      j=0;
      while(b.val!=NULL && r<h.NumberOfRecordes){
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
               p=NULL;
               chars=offset;
               clearBlock(&buf);
               header.NumberOfBlocks++;
            }
            dd=1;
            continue;
         }
         if(buf.val==NULL){
            buf.val = b.val;
            p=b.val;
            b.val = b.val->next;
            p->next=NULL;
            header.NumberOfRecordes++;
         }else{
            p->next = b.val;
            p=b.val;
            b.val = b.val->next;
            p->next=NULL;
            header.NumberOfRecordes++;
         }
         chars+=sizeOfRecord(p);
         if(chars>=BLOCK_MAX_SIZE){
            offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
            chars=offset;
            p=NULL;
            clearBlock(&buf);
            header.NumberOfBlocks++;
         }
         j++;
         r++;
      }
      b = ReadDirNoOffset(f,blocks);
   }
   

   if(dd==0){
      if(buf.val==NULL){
         buf.val = s;
         p=buf.val;
         header.NumberOfRecordes++;
      }else{
         p->next = s;
         p=p->next;
         header.NumberOfRecordes++;
      }
      chars+=sizeOfRecord(s);
      if(chars>=BLOCK_MAX_SIZE){
         offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
         clearBlock(&buf);
         header.NumberOfBlocks++;
      }
   }
   if(buf.val!=NULL){
      offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
   }
   header.NumberOfBlocks++;
   SetHeader(f,&header);
   
}

//Delete the Student with a specific ID
void Delete(FILE *f , char id[STRING_MAX_LENGTH]){
   int a=0;
   int bi = searchId(f,id,&a);
   THeader h = Header(f);
   int blocks = 0;
   TBlock b,buf={NULL,NULL,0,0};
   int offset = 0;
   int r = 0,j = 0;
   int chars = 0;
   TStudent *p=NULL;
   THeader header={1,0,h.curId};
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
      j=0;
      while(b.val!=NULL && r<h.NumberOfRecordes){
         if(b.val!=NULL && (j!=a || blocks!=bi+1)){
            if(buf.val==NULL){
               buf.val = b.val;
               p=b.val;
               b.val = b.val->next;
               p->next=NULL;
               header.NumberOfRecordes++;
            }else{
               p->next = b.val;
               p=b.val;
               b.val = b.val->next;
               p->next=NULL;
               header.NumberOfRecordes++;
            }
            chars+=sizeOfRecord(b.val);
            if(chars>=BLOCK_MAX_SIZE){
               offset = WriteDir(f,buf,header.NumberOfBlocks,offset);
               chars=offset;
               clearBlock(&buf);
               header.NumberOfBlocks++;
            }
         }else{
            b.val=b.val->next;
         }
         j++;
         r++;
      }
   }
   if(buf.val!=NULL){
      offset = WriteDir(f,buf,header.NumberOfBlocks-1,offset);
   }
   SetHeader(f,&header);
}

//Put some starting data
void init(FILE *f){
   THeader h = {0,0,0};
   SetHeader(f,&h);
   TStudent p;
   for(int i = 15 ; i>=0 ; i--){
      
      snprintf(p.id,STRING_MAX_LENGTH,"Id_%03d",i);
      snprintf(p.Fname,STRING_MAX_LENGTH,"Name%03d",15-i);
      snprintf(p.Lname,STRING_MAX_LENGTH,"SOm%03d",i);
      p.Note=10;
      p.group=5;
      
      Insert(f,&p);
      
   }
}

//Put the First data
void initFirst(FILE *f,TStudent *s){
   THeader h = {0,0,1};
   TBlock b={NULL,NULL,0,0};
   int of = 0;
   TStudent *p = NULL;

   b.val = s;
   p=b.val;
   b.length++;
   
   WriteDir(f,b,h.NumberOfBlocks,of);
   h.NumberOfRecordes+=b.length;
   h.NumberOfBlocks++;
   SetHeader(f,&h);
}

//Display one Student
void display(TStudent *s){
   printf("\nID : %s\nFirst Name : %s\nLast Name : %s\nGroup : %d\nNote : %05.2f\n_______________\n",s->id,s->Fname,s->Lname,s->group,s->Note);
}

//Display All Student
void PrintAll(FILE *f){
   THeader h = Header(f);
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

//Display All Student with certen Group
void PrintWithGroup(FILE *f,int g){
   THeader h = Header(f);
   int blocks = 0;
   TBlock b;
   int r = 0;
   while(blocks<h.NumberOfBlocks && r<h.NumberOfRecordes){
      b = ReadDirNoOffset(f,blocks);
      blocks++;
      while(b.val!=NULL && r<h.NumberOfRecordes){
         if(g==b.val->group)display(b.val);
         r++;
         TStudent *tmp = b.val;
         b.val = b.val->next;
         free(tmp);
      }
   }
}

//Function To ask the user to select one of the options
void ask(int *a){
   printf("\nSelect an option :\n");
   printf("\t0) exit\n");
   printf("\t1) Display all students\n");
   printf("\t2) Display all students in specific group\n");
   printf("\t3) Search for specific Student with ID\n");
   printf("\t4) Search for specific Student with First Name\n");
   printf("\t5) Insert a new student\n");
   printf("\t6) Delete a student via Id\n");
   printf("\n> ");
   scanf("%d",a);
}

//Helper Function to ask the use for a Int Value
int AskInt(char stmt[50]){
   int r;
   printf("%s\n\t> ",stmt);
   scanf("%d",&r);
   system("clear");
   return r;
}

//Helper Function to ask the use for a Float Value
int AskFloat(char stmt[50]){
   float r;
   printf("%s\n\t> ",stmt);
   scanf("%f",&r);
   system("clear");
   return r;
}

//Helper Function to ask the use for a String Value
void AskString(char stmt[50],char r[STRING_MAX_LENGTH]){
   printf("%s\n\t> ",stmt);
   scanf("%s",r);
   system("clear");
}

int main(){
   //open file if it exist
   FILE *f = fopen("test.txt","r+");
   int newF = 0;
   
   //This will be used to check if the file opened or created
   if(f==NULL || 1){
      f = fopen("test.txt","w+");
      init(f);
      newF = 1;
   }
   int run = 1;
   
   //Variables to store user inputs
   int ans,h,bi;
   char Cans[STRING_MAX_LENGTH];
   TStudent snew = {"","","",0,0,NULL};
   THeader header;

   //Store the Questions used here 
   char Questions[8][50]={
      "Enter The Group Number:",
      "Enter The Id:",
      "Enter First Name:",
      "Enter The First Name :",
      "Enter The Last Name :",
      "Enter Group Number:",
      "Enter Note:",
      "Enter The Id:"
   };

   //Clear the first time to look better
   system("clear");
   while(run){

      //Ask the user to select an option
      ask(&ans);

      //clear the previos Question
      system("clear");

      //see witch option the user choese
      switch (ans) {

         //exit
         case 0:
            run=0;
            break;

         //Print all student
         case 1:
            PrintAll(f);
            break;

         //Print all student in a specific group
         case 2:
            PrintWithGroup(f,AskInt(Questions[0]));
            break;

         //Print specific student via Id
         case 3:
            AskString(Questions[1],Cans);
            h=0;
            bi = searchId(f,Cans,&h);
            printf("\nb : %d,r : %d\n",bi,h);
            if(bi==-1){
               printf("No Student with that ID!\n");
            }else{
               TBlock block = ReadDirNoOffset(f,bi);
               while(h>0){
                  block.val=block.val->next;
                  h--;
               }
               display(block.val);
            }
            break;

         //Print specific student via First name
         case 4:
            AskString(Questions[2],Cans);
            h=0;
            bi = search(f,Cans,&h);
            if(bi==-1){
               printf("No Student with that Name!\n");
            }else{
               TBlock block = ReadDirNoOffset(f,bi);
               while(h>0)block.val=block.val->next;
               display(block.val);
            }
            break;

         //Create and Insert a new student
         case 5:
            header = Header(f);
            AskString(Questions[3],snew.Fname);
            AskString(Questions[4],snew.Lname);
            snew.group=AskInt(Questions[5]);
            snew.Note=AskFloat(Questions[6]);
            snew.next=NULL;
            if(newF==0){
               snprintf(snew.id,STRING_MAX_LENGTH,"Id_%03d",header.curId);
               Insert(f,&snew);
            }else{
               snprintf(snew.id,STRING_MAX_LENGTH,"Id_000");
               initFirst(f,&snew);
            }
            break;
         
         //Delete an Student via Id
         case 6:
            AskString(Questions[7],Cans);
            h=0;
            bi = searchId(f,Cans,&h);
            if(bi==-1){
               printf("No Student with that ID!\n");
            }else{
               Delete(f,Cans);
            }
            break;
         
         //The Input is not valid
         default:
            printf("Invalid Option!!\n");
      }
   }
   
   //Close the File
   fclose(f);
   return 0;
}
