#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#define H 9
#define W 9
#define BLACK 'K'
#define RED 'R'
#define GREEN 'G'
#define YELLOW 'Y'
#define BLUE 'B'
#define MAGENTA 'M'
#define CYAN 'C'
#define WHITE 'W'
#define MAXSIZE 10000000
#define NUMITER 100000
#define MAXLINE 20
#define INVALID -1

struct node{
   char BOOK_case[H][W];
   int w;
   int h;
   int end;
   int parent;
};

enum bool {false, true};
typedef enum bool bool;
typedef struct node node;

/*Checks to see if shelf is full of books*/
bool isrowfull(node a, int row);
/*Checks if book is the last book on shelf*/
bool islastbook(node a, int y, int x);
/*Checks if move from y to newy is valid*/
bool validmove(node a, int y, int newy);
/*Checks if the cell is a valid coloured book*/
bool validbook(char a);
/*Counts the number of books in a shelf*/
int numbooks(node a, int y);
/*Moves last book of row y to first empty space of row newy*/
void movebook(node *a, int y, int newy);
/*Finds the cell of the book we want to move*/
int endbook_pos(node a, int y);
/*Gives the index position for where the book should move to*/
int newbook_pos(node a, int y);
/*Returns true if shelf is empty or contains books of one colour only*/
bool happyshelf(node a, int y);
/*Returns true if shelf is entirely empty*/
bool emptyshelf(node a, int y);
/*Returns true if only a single colour appears in a shelf*/
bool monocolour(node a, int y);
/*Returns true if all the books of the same colour are on the same shelf*/
bool coloursorted(node a);
/*Looks to see if a colour appears in other rows*/
bool col_elsewhere(node a, int y, char col);
/*Prints the bookcase*/
void printarr(node* a);
/*Returns true if the bookcase is happy*/
bool ishappy(node a);
/*Returns true if all characters in array are valid*/
bool allvalid(node a);
/*Returns true if two bookcases are the same*/
bool same_board(char bookcase[H][W], char bookcase2[H][W], int w, int h);
/*Copies one bookcase on to the other*/
void boardcopy(char bookcase[H][W], char bookcase2[H][W], int w, int h);
/*Initialises a bookcase to be full of nulls*/
void init_bookcase(char bookcase[H][W], int w, int h);
/*Adds the children of each parent to the array of structures*/
void find_children(node list[], long *cnt, int parent, int v);
/*Returns true if the bookcase could become happy*/
bool validstart(node a);
/*Returns true if there are too many books of one colour to fit on a shelf*/
bool excesscolours(node a);
/*Counts how many times a colour appears in a bookcase*/
int colour_cnt(node a, char c);
/*Returns true if all books in the array are grouped to the left*/
bool leftsorted(node a);
/*Prints out parents, number of moves, frees up space*/
void finish(node list[], int a, long *cnt, int v);
/*Gets parents, runs the main loop of the program*/
void becomehappy(node list[], long *cnt, int verb);
/*Interpretates the command line input, sends int based back on this*/
int interp_argc(int argc, char* argv[]);
/*Quits the program if command line input is insufficient*/
void checkargc(int verb);
/*Converting all characters from file board to capitals*/
void capitalise(char bookcase[H][W], int w, int h);
/*Putting examples of boards in structs for testing*/
void init_testnode(char bookcase[H][W], node* a, int w, int h);
/*Scans in the file and stores it in the first structure*/
void file_open(char bookcase[H][W], char *textfile, node* a);
/*Assigns height, width and parent number to a new child*/
void build_s(node *a, int h, int w, int p);
/*Checks if entire array is full*/
bool fullarray(node a);
/*Allocating space, as well as checking the return from malloc*/
void* nmalloc(int size);
/*Checks if the file pointer is not null*/
void checkptr(FILE *ptr);
/*Prints out the error, exits the program*/
void file_error(void);
/*Returns true if the bookcase in node a has been seen before in list*/
bool visitedbefore(node list[], node a, int j);
/*Reads array from file into structure*/
void read_array(FILE *fp, char str[], int w, int h);
/*If initial bookcase can be happy, starts the program*/
void checkstart(node list[], long* count, int verb);
/*Assigns the string read in from file to bookcase array*/
void assign_str(char str[], char bookcase[H][W], int w, int h);
/*Checks the top line dimensions are within 9x9 bounds*/
void inbound_check(int w, int h);
/*Assertion tests*/
void test(void);

int main(int argc, char* argv[])
{
   int verb = 0;
   char* textfile = argv[1];
   char bookcase[H][W];
   static node list[MAXSIZE];
   long* count = (long* )nmalloc(sizeof(long));
   test();
   init_bookcase(bookcase,H,W);
   verb = interp_argc(argc, argv);
   checkargc(verb);
   file_open(bookcase, textfile, &list[0]);
   checkstart(list,count,verb);
   return 0;
}

void checkstart(node list[], long* count, int verb)
{
   if(validstart(list[0])){
      becomehappy(list,count,verb);
   }
   else{
      printf("No solution!\n");
   }
   free(count);
}

void* nmalloc(int size)
{
   void* v = malloc(size);
   if(v==NULL){
      printf("Cannot malloc() space");
      exit(EXIT_FAILURE);
   }
   return v;
}

int interp_argc(int argc, char* argv[])
{
   int verb = 0;
   char verbose[MAXLINE] = "verbose";
   if(argc<=1){
      verb = INVALID;
   }
   if(argc==3){
      if(strcmp(argv[2],verbose)==0){
         verb = 1;
      }
   }
   if(argc>3){
      verb = INVALID;
   }
   return verb;
}

void checkargc(int verb)
{
   if(verb == INVALID){
      printf("Invalid command line.\n");
      exit(EXIT_FAILURE);
   }
}

void file_open(char bookcase[H][W], char *textfile, node* a)
{
   FILE *fp;
   int h = 0, w = 0;
   char numbers[MAXLINE];
   char str[(H*W)+1];
   fp=fopen(textfile,"r");
   checkptr(fp);
   if(fgets(numbers, MAXLINE, fp)==NULL){
      file_error();
   }
   if(sscanf(numbers, "%d %d", &h, &w)!=2){
      file_error();
   }
   a->h = h;
   a->w = w;
   inbound_check(w, h);
   read_array(fp, str, w, h);
   assign_str(str,bookcase,w,h);
   boardcopy(bookcase,a->BOOK_case,w,h);
   fclose(fp);
}

void assign_str(char str[], char bookcase[H][W], int w, int h)
{
   int i = 0, y, x;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         bookcase[y][x]=str[i++];
      }
   }
   capitalise(bookcase,w,h);
}

void inbound_check(int w, int h)
{
   if((h>H)||(w>W)||(h<=0)||(w<=0)){
      file_error();
   }
}

void read_array(FILE *fp, char str[], int w, int h)
{
   char c, xcnt = 0, ycnt = 0;
   int i = 0;
   while((c=fgetc(fp))!=EOF){
      if(c!='\n'){
         str[i++] = c;
         ++xcnt;
      }
      else if(c=='\n'){
         if(xcnt!=w){
            file_error();
         }
         xcnt = 0;
         ++ycnt;
      }
   }
   if(ycnt!=h){
      file_error();
   }
}

void checkptr(FILE *fp)
{
   if(fp == NULL){
      printf("Error reading file?\n");
      exit(EXIT_FAILURE);
   }
}

void file_error(void)
{
   printf("Invalid File!\n");
   exit(EXIT_FAILURE);
}

void capitalise(char bookcase[H][W], int w, int h)
{
   int y, x;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         bookcase[y][x]=toupper(bookcase[y][x]);
      }
   }
}

void boardcopy(char bookcase[H][W], char bookcase2[H][W], int w, int h)
{
   int y, x;
   init_bookcase(bookcase2,w,h);
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         bookcase2[y][x] = bookcase[y][x];
      }
   }
}

void init_bookcase(char bookcase[H][W], int w, int h)
{
   int y, x;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         bookcase[y][x] = '\0';
      }
   }
}

bool validstart(node a)
{
   int flag = 0;
   if((!ishappy(a))&&(!fullarray(a))){
      flag = 1;
   }
   if(ishappy(a)){
      flag = 1;
   }
   if((excesscolours(a))||(!allvalid(a))||(!leftsorted(a))){
      flag = 0;
   }
   if(flag==1){
      return true;
   }
   return false;
}

bool ishappy(node a)
{
   int flag = 0, y;
   int h = a.h;
   for(y = 0; y < h; y++){
      if(!happyshelf(a,y)){
         flag = 1;
      }
   }
   if((flag==0)&&(coloursorted(a))&&(allvalid(a))){
      return true;
   }
   return false;
}

bool happyshelf(node a, int y)
{
   if(emptyshelf(a,y)||monocolour(a,y)){
      return true;
   }
   return false;
}

bool emptyshelf(node a, int y)
{
   int flag = 0, x;
   int w = a.w;
   for(x = 0; x < w; x++){
      if(a.BOOK_case[y][x]!='.'){
         flag = 1;
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool monocolour(node a, int y)
{
   int x, flag = 0;
   int w = a.w;
   char col;
   for(x = 0; x < w; x++){
      if(validbook(a.BOOK_case[y][x])){
         col = a.BOOK_case[y][x];
      }
   }
   for(x = 0; x < w; x++){
      if(validbook(a.BOOK_case[y][x])){
         if(a.BOOK_case[y][x]!=col){
            flag = 1;
         }
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool coloursorted(node a)
{
   int y, x, flag = 0;
   int h = a.h;
   int w = a.w;
   char col;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(validbook(a.BOOK_case[y][x])){
            col = a.BOOK_case[y][x];
            if(col_elsewhere(a, y, col)){
               flag = 1;
            }
         }
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool validbook(char a)
{
   if((a==BLACK)||(a==RED)||(a==RED)||(a==GREEN)||(a==YELLOW)||
   (a==BLUE)||(a==MAGENTA)||(a==CYAN)||(a==WHITE)){
      return true;
   }
   return false;
}

bool col_elsewhere(node a, int y, char col)
{
   int row, x, flag = 0;
   int h = a.h;
   int w = a.w;
   for(row = 0; row < h; row++){
      for(x = 0; x < w; x++){
         if(row!=y){
            if(a.BOOK_case[row][x]==col){
               flag = 1;
            }
         }
      }
   }
   if(flag==1){
      return true;
   }
   return false;
}

bool allvalid(node a)
{
   int h = a.h;
   int w = a.w;
   int y, x, flag = 0;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(a.BOOK_case[y][x]!='.'){
            if(!validbook(a.BOOK_case[y][x])){
               flag = 1;
            }
         }
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool fullarray(node a)
{
   int y, flag = 0;
   int h = a.h;
   for(y = 0; y < h; y++){
      if(!isrowfull(a,y)){
         flag = 1;
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool isrowfull(node a, int row)
{
   int x, flag = 0;
   int w = a.w;
   for(x = 0; x < w; x++){
      if(a.BOOK_case[row][x]=='.'){
         flag = 1;
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

bool excesscolours(node a)
{
   int h = a.h;
   int w = a.w;
   int y, x, flag = 0;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(validbook(a.BOOK_case[y][x])){
            if(colour_cnt(a,a.BOOK_case[y][x])>w){
               flag = 1;
            }
         }
      }
   }
   if(flag==1){
      return true;
   }
   return false;
}

int colour_cnt(node a, char c)
{
   int h = a.h;
   int w = a.w;
   int y, x, cnt = 0;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(a.BOOK_case[y][x]==c){
            cnt++;
         }
      }
   }
   return cnt;
}

bool leftsorted(node a)
{
   int h = a.h;
   int w = a.w;
   int y, x, cnt = 0;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(a.BOOK_case[y][x]!='.'){
            cnt++;
         }
      }
      for(x = 0; x < cnt; x++){
         if(a.BOOK_case[y][x]=='.'){
            return false;
         }
      }
      cnt = 0;
   }
   return true;
}

void becomehappy(node list[], long *cnt, int verb)
{
   int parent;
   *cnt = 0;
   if(!ishappy(list[0])){
      build_s(&list[0],list[0].h,list[0].w,-1);
      for(parent = 0; parent < NUMITER; parent++){
         find_children(list,cnt,parent,verb);
      }
      printf("No Solution!\n");
      free(cnt);
      exit(EXIT_SUCCESS);
   }
   else{
      if(verb==1){
         printarr(&list[0]);
      }
      printf("1\n");
      free(cnt);
      exit(EXIT_SUCCESS);
   }
}


void build_s(node *a, int h, int w, int p)
{
   a->h = h;
   a->w = w;
   a->parent = p;
}

void find_children(node list[], long *cnt, int parent, int v)
{
   int j, y, ynew;
   char original[H][W];
   int w = list[0].w;
   int h = list[0].h;
   if(parent==0){
      j=1;
   }
   else{
      j = *cnt;
   }
   boardcopy(list[parent].BOOK_case,original,w,h);
   for(y = 0; y < h; y++){
      for(ynew = 0; ynew < h; ynew++){
         if((ynew != y)&&(validmove(list[parent],y,ynew))&&(*cnt<MAXSIZE)){
            movebook(&list[parent],y,ynew);
            build_s(&list[j],h,w,parent);
            list[j].end = list[j-1].end + 1;
            *cnt = list[j].end;
            boardcopy(list[parent].BOOK_case,list[j].BOOK_case,w,h);
            boardcopy(original,list[parent].BOOK_case,w,h);
            if(ishappy(list[j])){
               finish(list, j, cnt, v);
            }
            if(visitedbefore(list,list[j],j)){
               j--;
            }
            else{
               j++;
            }
         }
      }
   }
   *cnt = *cnt + 1;
}

bool visitedbefore(node list[], node a, int j)
{
   int i;
   int w = a.w;
   int h = a.h;
   for(i = 0; i < j; i++){
      if(same_board(list[i].BOOK_case,a.BOOK_case,w,h)){
         return true;
      }
   }
   return false;
}

bool validmove(node a, int y, int newy)
{
   int x = endbook_pos(a, y);
   if(islastbook(a,y,x)){
      if(!isrowfull(a, newy)){
         return true;
      }
   }
   return false;
}

int endbook_pos(node a, int y)
{
   int x;
   int w = a.w;
   if(!emptyshelf(a,y)){
      for(x = 0; x < w; x++){
         if(islastbook(a, y, x)){
            return x;
         }
      }
   }
   return 0;
}

bool islastbook(node a, int y, int x)
{
   int col;
   int w = a.w;
   if(validbook(a.BOOK_case[y][x])){
      if(x==(w-1)){
         return true;
      }
      else if(x<(w-1)){
         for(col = x+1; col < w; col++){
            if(a.BOOK_case[y][col]!='.'){
               return false;
            }
         }
      }
      return true;
   }
   return false;
}

void movebook(node *a, int y, int newy)
{
   int end, end2;
   end = endbook_pos(*a, y);
   end2 = newbook_pos(*a, newy);
   if(validmove(*a,y,newy)){
      a->BOOK_case[newy][end2] = a->BOOK_case[y][end];
      a->BOOK_case[y][end] = '.';
   }
}

int newbook_pos(node a, int y)
{
   int end;
   if(emptyshelf(a,y)){
      end = 0;
   }
   else{
      end = endbook_pos(a, y)+1;
   }
   return end;
}

void finish(node list[], int a, long *cnt, int v)
{
   int parent, flag = 0, itercnt = 1;
   if(v==1){
      printarr(&list[a]);
   }
   parent = a;
   while(flag == 0){
      parent = list[parent].parent;
      if(v==1){
         printarr(&list[parent]);
      }
      itercnt++;
      if(parent==0){
         flag = 1;
         printf("%d\n",itercnt);
         free(cnt);
         exit(EXIT_SUCCESS);
      }
   }
}

void printarr(node* z)
{
   int y, x;
   node a = *z;
   int h = a.h;
   int w = a.w;
   for(y = 0; y < h; y++){
      printf("\n|");
      for(x = 0; x < w; x++){
         printf(" %c |",a.BOOK_case[y][x]);
      }
   }
   printf("\n\n");
}

bool same_board(char bookcase[H][W], char bookcase2[H][W], int w, int h)
{
   int y, x, flag = 0;
   for(y = 0; y < h; y++){
      for(x = 0; x < w; x++){
         if(bookcase2[y][x]!=bookcase[y][x]){
            flag = 1;
         }
      }
   }
   if(flag==1){
      return false;
   }
   return true;
}

int numbooks(node a, int y)
{
   int x, cnt = 0;
   int w = a.w;
   for(x = 0; x < w; x++){
      if(validbook(a.BOOK_case[y][x])){
         cnt++;
      }
   }
   return cnt;
}

void init_testnode(char bookcase[H][W], node* a, int w, int h)
{
   boardcopy(bookcase,a->BOOK_case,w,h);
   a->w=w;
   a->h=h;
}

void test(void)
{
   node blist[43];
   char bookcase1[H][W] = {{'C','G','.'},{'.','.','.'},{'G','G','C'}};
   char bookcase2[H][W] = {{'.','G','G'},{'C','M','K'},{'.','.','M'}};
   char bookcase3[H][W] = {{'G','.','.'},{'C','M','W'},{'.','.','M'}};
   char bookcase4[H][W] = {{'G','.','.','.'},{'C','M','B','Y'},{'Y','R','K','.'},
   {'.','.','W','W'},{'K','.','.','W'}};
   char bookcase5[H][W] = {{'G','~','Z'},{'C','M','W'},{'.','.','M'}};
   char bookcase6[H][W] = {{'C','.','.'},{'G','Y','.'},{'G','G','C'}};
   char bookcase7[H][W] = {{'G','R','Y','B'},{'C','M','B','Y'},{'Y','R','K','.'},
   {'M','M','W','W'},{'K','B','B','W'}};
   char bookcase8[H][W] = {{'G','K','R','B','.','.'},{'G','M','M','R','B','.'},
   {'G','W','W','B','W','W'},{'G','K','K','.','.','.'},{'G','B','R','W','W','.'}};
   char bookcase9[H][W] = {{'C'},{'.'}};
   char bookcase10[H][W] = {{'C','X'},{'.','.'},{'C','M'}};
   char bookcase11[H][W] = {{'R','R','.'},{'G','G','.'},{'C','C','.'},{'Y','Y','Y'}};
   char bookcase12[H][W] = {{'R','R','.'},{'R','R','.'}};
   char bookcase13[H][W] = {{'R','X','.'},{'R','R','.'}};
   char bookcase14[H][W] = {{'.','.','.'},{'.','.','.'}};
   char bookcase15[H][W] = {{'Y','Y','.'},{'.','.','.'}};
   char bookcase16[H][W] = {{'.','.','.'},{'B','B','.'}};
   char bookcase17[H][W] = {{'.','.','.'},{'B','B','M'}};
   char bookcase18[H][W] = {{'X','X','.'},{'Z','Z','.'}};
   char bookcase19[H][W] = {{'.','.','.'},{'G','R','G'},{'C','Y','R'},{'Y','C','.'}};
   char bookcase20[H][W] = {{'R','G','.'},{'R','G','.'},{'.','.','.'}};
   char bookcase21[H][W] = {{'.','.','.'},{'R','R','.'},{'G','G','.'}};
   char bookcase22[H][W] = {{'M','M','M'},{'R','R','R'},{'Y','R','Y'}};
   char bookcase23[H][W] = {{'M','M','M'},{'R','R','R'},{'Y','Y','Y'}};
   char bookcase24[H][W] = {{'R','R','R'},{'R','R','R'}};
   char bookcase25[H][W] = {{'R','G','B','.','.'},{'R','M','R','Y','K'},{'W','R','R','R','.'}};
   char bookcase26[H][W] = {{'.','.','.'},{'R','R','.'},{'G','G','.'}};
   char bookcase27[H][W] = {{'R','.','R'},{'R','R','.'},{'G','G','.'}};
   char bookcase28[H][W] = {{'R','G','Y','Y'},{'G','R','B','B'},{'K','K','M','M'},{'K','K','C','C'}};
   /*----------------------------------*/
   char bookcase31[H][W] = {{'C','G','.'},{'.','.','.'},{'G','G','C'}};
   char bookcase32[H][W] = {{'.','G','G'},{'C','M','K'},{'.','.','M'}};
   char bookcase33[H][W] = {{'G','.','.'},{'C','M','W'},{'.','.','M'}};
   char bookcase34[H][W] = {{'G','.','.','.'},{'C','M','B','Y'},{'Y','R','K','.'},
   {'.','.','W','W'},{'K','.','.','W'}};
   char bookcase35[H][W] = {{'G','~','Z'},{'C','M','W'},{'.','.','M'}};
   char bookcase36[H][W] = {{'C','.','.'},{'G','Y','.'},{'G','G','C'}};
   char bookcase37[H][W] = {{'G','R','Y','B'},{'C','M','B','Y'},{'Y','R','K','.'},
   {'M','M','W','W'},{'K','B','B','W'}};
   char bookcase38[H][W] = {{'G','K','R','B','.','.'},{'G','M','M','R','B','.'},
   {'G','W','W','B','W','W'},{'G','K','K','.','.','.'},{'G','B','R','W','W','.'}};
   char bookcase39[H][W] = {{'C'},{'.'}};
   char bookcase40[H][W] = {{'C','X'},{'.','.'},{'C','M'}};
   char bookcase41[H][W] = {{'J','J'},{'.','!'},{'J','J'}};
   char bookcase42[H][W] = {{'J','J'},{'.','!'},{'J','J'}};

/*Assigning examples to structures for testing*/
   init_testnode(bookcase1,&blist[1],3,3);
   init_testnode(bookcase2,&blist[2],3,3);
   init_testnode(bookcase3,&blist[3],3,3);
   init_testnode(bookcase4,&blist[4],4,5);
   init_testnode(bookcase5,&blist[5],3,3);
   init_testnode(bookcase6,&blist[6],3,3);
   init_testnode(bookcase7,&blist[7],4,5);
   init_testnode(bookcase8,&blist[8],6,5);
   init_testnode(bookcase9,&blist[9],1,2);
   init_testnode(bookcase10,&blist[10],2,3);
   init_testnode(bookcase11,&blist[11],3,4);
   init_testnode(bookcase12,&blist[12],3,2);
   init_testnode(bookcase13,&blist[13],3,2);
   init_testnode(bookcase14,&blist[14],3,2);
   init_testnode(bookcase15,&blist[15],3,2);
   init_testnode(bookcase16,&blist[16],3,2);
   init_testnode(bookcase17,&blist[17],3,2);
   init_testnode(bookcase18,&blist[18],3,2);
   init_testnode(bookcase19,&blist[19],3,4);
   init_testnode(bookcase20,&blist[20],3,3);
   init_testnode(bookcase21,&blist[21],3,3);
   init_testnode(bookcase22,&blist[22],3,3);
   init_testnode(bookcase23,&blist[23],3,3);
   init_testnode(bookcase24,&blist[24],3,2);
   init_testnode(bookcase25,&blist[25],5,3);
   init_testnode(bookcase26,&blist[26],3,3);
   init_testnode(bookcase27,&blist[27],3,3);
   init_testnode(bookcase28,&blist[28],4,4);
   init_testnode(bookcase31,&blist[31],3,3);
   init_testnode(bookcase32,&blist[32],3,3);
   init_testnode(bookcase33,&blist[33],3,3);
   init_testnode(bookcase34,&blist[34],4,5);
   init_testnode(bookcase35,&blist[35],3,3);
   init_testnode(bookcase36,&blist[36],3,3);
   init_testnode(bookcase37,&blist[37],4,5);
   init_testnode(bookcase38,&blist[38],6,5);
   init_testnode(bookcase39,&blist[39],1,2);
   init_testnode(bookcase40,&blist[40],2,3);
   init_testnode(bookcase41,&blist[41],2,3);
   init_testnode(bookcase42,&blist[42],2,3);

   assert(visitedbefore(blist,blist[40],40)==true);
   assert(visitedbefore(blist,blist[39],39)==true);
   assert(visitedbefore(blist,blist[38],38)==true);
   assert(visitedbefore(blist,blist[37],37)==true);
   assert(visitedbefore(blist,blist[36],36)==true);
   assert(visitedbefore(blist,blist[35],35)==true);
   assert(visitedbefore(blist,blist[34],34)==true);
   assert(visitedbefore(blist,blist[33],33)==true);
   assert(visitedbefore(blist,blist[32],32)==true);
   assert(visitedbefore(blist,blist[31],31)==true);
   assert(visitedbefore(blist,blist[1],1)==false);
   assert(visitedbefore(blist,blist[2],2)==false);
   assert(visitedbefore(blist,blist[3],3)==false);
   assert(visitedbefore(blist,blist[4],4)==false);
   assert(visitedbefore(blist,blist[5],5)==false);
   assert(visitedbefore(blist,blist[6],6)==false);
   assert(visitedbefore(blist,blist[7],7)==false);
   assert(visitedbefore(blist,blist[8],8)==false);
   assert(visitedbefore(blist,blist[41],41)==false);

}
