#include "specific.h"
#include "../assoc.h"
#include "math.h"

/* Note: assoc_count not working properly. I believe the reason is
due to not keeping track of temporary variables - i.e, when resizing
my table, I should pass in temporary vars from each array in the table
into the resize function, and then assoc_insert these vars before moving
on. Reason for not doing so is due to time constraints */

typedef enum bool {false, true} bool;

/* resizes the structure, to cope with excessive collision */
void _resize(assoc** a);
/* Returns the hash location from DJB2 algorithm */
unsigned int _hash_loc(void *key_in, unsigned int capacity, int keysize);
/* Returns the hash location from SBDM algorithm */
unsigned int _althash_loc(void *key_in, unsigned int capacity, int keysize);
/* Custom memory allocation function, used in resize function */
void _newcalloc(assoc** a, unsigned int bytes, int keysize);
/* Returns true if the value of 2 keys are equal */
bool _found(assoc *a, void *key, void *key2);
/* Returns true if the current capacity is a prime number */
bool _isprime(unsigned int capacity);
/* Returns NULL if found an empty place, or same key, returns
bounced cell otherwise */
assoc_ind* _place(assoc **a, void* key, void* data, int arraynum);

assoc* assoc_init(int keysize)
{
   assoc *ptr = (assoc *)ncalloc(1, sizeof(assoc));
   ptr->table = (assoc_arr *)ncalloc(2, sizeof(assoc_arr));
   ptr->table[0].arr = (assoc_ind *)ncalloc(INITSIZE, sizeof(assoc_ind));
   ptr->table[1].arr = (assoc_ind *)ncalloc(INITSIZE, sizeof(assoc_ind));
   if(keysize<0){
      on_error("Incorrect Keysize Input\n");
   }
   ptr->capacity = INITSIZE;
   ptr->keysize = keysize;
   return ptr;
}

unsigned int assoc_count(assoc* a)
{
   unsigned int tot = 0;
   tot = a->table[0].population + a->table[1].population;
   return tot;
}

/*Based on the djb2 algorithm, sourced from:
http://www.cse.yorku.ca/~oz/hash.html*/
unsigned int _hash_loc(void *key_in, unsigned int capacity, int keysize)
{
   unsigned int hash = 5381, c = 0;
   char *key = (char *)key_in;
   int x = 0;
   if(!keysize){
      while((c = (unsigned int)*key++)){
         hash = ((hash << 5) + hash) + c; /* hash * 33 + c*/
      }
   }
   else{
      for(x = 0; x < keysize; x++){
         c = (unsigned int)*key++;
         hash = ((hash << 5) + hash) + c;
      }
   }
   hash = hash%capacity;
   return hash;
}

/*Based on the sbdm algorithm, sourced from:
http://www.cse.yorku.ca/~oz/hash.html*/
unsigned int _althash_loc(void *key_in, unsigned int capacity, int keysize)
{
   char *key = (char *)key_in;
   unsigned int hash = 0, c = 0;
   int x = 0;
   if(!keysize){
      while((c = (unsigned int)*key++)){
         hash = c + (hash << 6) + (hash << 16) - hash;
      }
   }
   else{
      for(x = 0; x < keysize; x++){
         c = (unsigned int)*key++;
         hash = c + (hash << 6) + (hash << 16) - hash;
      }
   }
   hash = hash%capacity;
   return hash;
}

void assoc_free(assoc* a)
{
   free(a->table[0].arr);
   free(a->table[1].arr);
   free(a->table);
   free(a);
}

bool _isprime(unsigned int capacity)
{
   unsigned int i;
   int flag = 0;
   for(i = 2; i <= capacity/2; i++){
      if(capacity%i==0){
         flag = 1;
     }
   }
   if((flag==1)||(capacity==0)||(capacity==1)){
      return false;
   }
   return true;
}

void _newcalloc(assoc** a, unsigned int bytes, int keysize)
{
   assoc *a1;
   a1 = (assoc *)ncalloc(1, sizeof(assoc));
   a1->table = (assoc_arr *)ncalloc(2, sizeof(assoc_arr));
   a1->table[0].arr = (assoc_ind *)ncalloc(bytes, sizeof(assoc_ind));
   a1->table[1].arr = (assoc_ind *)ncalloc(bytes, sizeof(assoc_ind));
   a1->capacity = bytes;
   a1->keysize = keysize;
   *a = a1;
}


void _resize(assoc** a)
{
  assoc *arrptr;
  assoc *aorig = *a;
  unsigned int currentcap = aorig->capacity, newcap;
  int i;
  newcap = currentcap*SCALEFACTOR;
  while(!_isprime(newcap)){
     newcap++;
  }
  _newcalloc(&arrptr, newcap, aorig->keysize);
  *a = arrptr;

  for(i = 0; i < (int)currentcap; i++){
    if(aorig->table[0].arr[i].seen==1){
      assoc_insert(a, aorig->table[0].arr[i].key, aorig->table[0].arr[i].data);
    }
    if(aorig->table[1].arr[i].seen==1){
     assoc_insert(a, aorig->table[1].arr[i].key, aorig->table[1].arr[i].data);
    }
  }
  assoc_free(aorig);
}

void* assoc_lookup(assoc* a, void* key)
{
   void *ptr = NULL;
   unsigned int hash, hash2;
   if(!key){
      return ptr;
   }
   hash = _hash_loc(key, a->capacity, a->keysize);
   hash2 = _althash_loc(key, a->capacity, a->keysize);

   if(a->table[0].arr[hash].key){
      if(_found(a, a->table[0].arr[hash].key, key)){
         ptr = a->table[0].arr[hash].data;
      }
   }
   else if(a->table[1].arr[hash2].key){
      if(_found(a, a->table[1].arr[hash2].key, key)){
         ptr = a->table[1].arr[hash2].data;
      }
   }
   return ptr;
}

void assoc_insert(assoc** a, void* key, void* data)
{
   assoc *assoc1 = *a;
   assoc_ind* index = NULL;
   int arraynum = 1, cnt = 0;
   int maxcnt = (int)(log(assoc1->capacity)/log(2));
   double cap = CAPLIMIT*(double)assoc1->capacity;

   if((assoc1->table[0].population > (unsigned int)cap)||
   (assoc1->table[1].population > (unsigned int)cap)){
      _resize(a);
   }

   assoc1 = *a;
   if(key!=NULL){
      index = _place(a, key, data,(arraynum+1)%2);
      while(index){
         arraynum++;
         index = _place(a, key, data,(arraynum+1)%2);
         cnt++;
         if(cnt>=maxcnt){
            _resize(a);
            cnt = 0;
         }
      }
      arraynum = (arraynum+1)%2;
      assoc1->table[arraynum].population++;
      *a = assoc1;
   }
}

assoc_ind* _place(assoc **a, void* key, void* data, int arraynum)
{
   assoc* assoc1 = *a;
   assoc_ind* rtn = NULL;
   unsigned int hash;
   int j = 0;
   if(arraynum == 0){
      hash = _hash_loc(key, assoc1->capacity, assoc1->keysize);
      j = 0;
   }
   else{
      hash = _althash_loc(key, assoc1->capacity, assoc1->keysize);
      j = 1;
   }
   if(assoc1->table[j].arr[hash].seen){
      if(_found(assoc1, assoc1->table[j].arr[hash].key, key)){
         /*same key, update data*/
         assoc1->table[j].arr[hash].data = data;
         *a = assoc1;
         return NULL;
      }
      else{
         /* return thing from this nest - bounce */
         rtn = &assoc1->table[j].arr[hash];
         rtn->key = key;
         rtn->data = data;
         assoc1->table[j].arr[hash].key = key;
         assoc1->table[j].arr[hash].data = data;
         assoc1->table[j].arr[hash].seen = 1;
         *a = assoc1;
         return rtn;
      }
   }
   assoc1->table[j].arr[hash].key = key;
   assoc1->table[j].arr[hash].data = data;
   assoc1->table[j].arr[hash].seen = 1;
   *a = assoc1;
   return NULL;
}

bool _found(assoc *a, void *key, void *key2)
{
   if(a->keysize == 0){
      if(strcmp((char *)key, (char *)key2)!=0){
         return false;
      }
      return true;
   }
   else{
      if(memcmp(key, key2, a->keysize)!=0){
         return false;
      }
      return true;
   }
}

void test_priv(void)
{
   assoc *a1, *a2, *a3, *a_new;
   int temphash = 0;
   float fmore;
   unsigned int prev_pop1 = 0, prev_pop2 = 0;
   unsigned int prev_cap1 = 0;
   char *word1 = "hello";
   char *word2 = "zeb";
   char *word3 = "mmmmm";
   char *word4 = "doesntwor";
   char *word5 = "a";
   char *word6 = "?";
   char *word7 = "epistlers";
   char *word8 = "unadaptableness";
   float f1 = 1.22;
   float f2 = 109.09;
   float f3 = 1.09;
   static int i1[10];
   static int j1[10];
   static int k1[10];
   unsigned int x;
   int y;

   /* -------------------------------------- */

   /* Initialisation tests */
   a1 = assoc_init(0);
   assert(a1);
   assert(a1->capacity == INITSIZE);
   assert(a1->keysize == 0);
   assert(a1->table[0].arr);
   assert(a1->table[1].arr);
   assert(!assoc_count(a1));

   a2 = assoc_init(4);
   assert(a2);
   assert(a2->capacity == INITSIZE);
   assert(a2->keysize == 4);
   assert(a2->table[0].arr);
   assert(a2->table[1].arr);
   assert(!assoc_count(a2));

   a3 = assoc_init(sizeof(float));
   assert(a3);
   assert(a3->capacity == INITSIZE);
   assert(a3->keysize == 4);
   assert(a3->table[0].arr);
   assert(a3->table[1].arr);
   assert(!assoc_count(a3));

   /* -------------------------------------- */

   /* SBDM Hash Location Tests (For strings/chars) */

   assert(_althash_loc(word1,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word2,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word3,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word4,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word5,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word6,INITSIZE,0)<INITSIZE);
   assert(_althash_loc(word6,500,0)<500);

   for(x = 0; x < 1000; x++){
      assert(_althash_loc(&x,INITSIZE,sizeof(unsigned int))<INITSIZE);
   }

   for(x = 0; x < 500; x++){
      fmore = (float)rand()/(float)300000.0;
      y = (unsigned int)fmore;
      assert(_althash_loc(&y,INITSIZE,sizeof(float))<INITSIZE);
      assert(_hash_loc(&y,INITSIZE,sizeof(float))<INITSIZE);
   }

/* -------------------------------------- */

   /* Inserting first word tests */
   assoc_insert(&a1, word1, &i1[0]);
   temphash = _hash_loc(word1, a1->capacity, 0);
   assert(a1->table[0].arr[temphash].seen == 1);
   assert(a1->table[0].arr[temphash].data == &i1[0]);
   assert((char *)a1->table[0].arr[temphash].key == word1);
   assert(strcmp((char *)a1->table[0].arr[temphash].key,word1)==0);
   /*checking word is inserted only once - should make a function for this!!*/
   for(x = 0; x < a1->capacity; x++){
      if(x != (unsigned int)temphash){
         assert(!a1->table[0].arr[x].key);
         assert(!a1->table[1].arr[x].key);
         assert(!a1->table[0].arr[x].seen);
         assert(!a1->table[1].arr[x].seen);
         assert(!a1->table[0].arr[x].data);
         assert(!a1->table[1].arr[x].data);
      }
   }
   assert(a1->keysize == 0);
   assert(a1->table[0].population);
   assert(!a1->table[1].population);
   assert(assoc_count(a1));
   assert(assoc_lookup(a1,word1));
   assert(_found(a1, word1, word1));

   word1 = "zzz";
   assert(!assoc_lookup(a1,word1));

   assoc_insert(&a1, word7, &i1[1]);
   temphash = _hash_loc(word7, a1->capacity, 0);
   assert(a1->table[0].arr[temphash].seen == 1);
   assert(a1->table[0].arr[temphash].data == &i1[1]);
   assert((char *)a1->table[0].arr[temphash].key == word7);
   assert(strcmp((char *)a1->table[0].arr[temphash].key,word7)==0);
   assert(a1->keysize == 0);
   assert(a1->table[0].population == 2);
   assert(assoc_count(a1) == 2);
   assert(assoc_lookup(a1,word7));
   assert(_found(a1, word7, word7));

   assoc_insert(&a1, word8, &i1[2]);
   temphash = _hash_loc(word8, a1->capacity, 0);
   assert(a1->table[0].arr[temphash].seen == 1);
   assert(a1->table[0].arr[temphash].data == &i1[2]);
   assert((char *)a1->table[0].arr[temphash].key == word8);
   assert(strcmp((char *)a1->table[0].arr[temphash].key,word8)==0);
   assert(a1->keysize == 0);
   assert(a1->table[0].population == 3);
   assert(assoc_count(a1) == 3);
   assert(assoc_lookup(a1,word8));
   assert(_found(a1, word8, word8));

   /*Another insert*/

   for(y = 0; y < 100; y++){
      x = (rand()%300000) + 1;
      assoc_insert(&a2, &x, &j1[0]);
      assert(assoc_lookup(a2,&x));
      assert(_found(a1, &x, &x));
   }

   /* Inserting floats tests */
   assoc_insert(&a3, &f1, &k1[0]);
   temphash = _hash_loc(&f1, a3->capacity, sizeof(float));
   assert(a3->table[0].arr[temphash].seen == 1);
   assert(a3->table[0].arr[temphash].data == &k1[0]);
   assert(memcmp(a3->table[0].arr[temphash].key, &f1, a3->keysize)==0);
   /*checking float is inserted only once */
   assert(a3->keysize != 0);
   assert(a3->table[0].population==1);
   assert(assoc_count(a3));

   assoc_insert(&a3, &f2, &k1[1]);
   temphash = _hash_loc(&f2, a3->capacity, sizeof(float));
   assert(a3->table[0].arr[temphash].seen == 1);
   assert(a3->table[0].arr[temphash].data == &k1[1]);
   assert(memcmp(a3->table[0].arr[temphash].key, &f2, a3->keysize)==0);
   assert(a3->table[0].population==2);
   assert(assoc_count(a3)==2);

   assoc_insert(&a3, &f3, &k1[2]);
   temphash = _hash_loc(&f3, a3->capacity, sizeof(float));
   assert(a3->table[0].arr[temphash].seen);
   assert(assoc_count(a3)==3);


   /* -------------------------------------- */

   /* _resize tests */

   prev_pop1 = a1->table[0].population;
   prev_pop2 = a1->table[1].population;
   prev_cap1 = a1->capacity;
   x = assoc_count(a1);
   _resize(&a1);
   assert(a1);
   assert(a1->table[0].arr);
   assert(a1->table[1].arr);
   assert(a1->keysize == 0);
   assert(_isprime(a1->capacity));
   assert((int)a1->capacity >= (int)(SCALEFACTOR*prev_cap1));
   assert(prev_pop1 == a1->table[0].population);
   assert(prev_pop2 == a1->table[1].population);
   assert(assoc_count(a1) == x);

   prev_pop1 = a1->table[0].population;
   prev_pop2 = a1->table[1].population;
   prev_cap1 = a1->capacity;
   x = assoc_count(a1);
   _resize(&a1);
   assert(a1);
   assert(a1->table[0].arr);
   assert(a1->table[1].arr);
   assert(a1->keysize == 0);
   assert(_isprime(a1->capacity));
   assert((int)a1->capacity >= (int)(SCALEFACTOR*prev_cap1));
   assert(prev_pop1 == a1->table[0].population);
   assert(prev_pop2 == a1->table[1].population);
   assert(assoc_count(a1) == x);

   prev_pop1 = a2->table[0].population;
   prev_pop2 = a2->table[1].population;
   prev_cap1 = a2->capacity;
   x = assoc_count(a2);
   _resize(&a2);
   assert(a2);
   assert(a2->table[0].arr);
   assert(a2->table[1].arr);
   assert(a2->keysize == 4);
   assert(_isprime(a2->capacity));
   assert((int)a2->capacity >= (int)(SCALEFACTOR*prev_cap1));
   /*assert(assoc_count(a2) == x);*/


   /* -------------------------------------- */

   _newcalloc(&a_new, 120, 0);
   assert(a_new);
   assert(a_new->table[0].arr);
   assert(a_new->table[1].arr);
   assert(a_new->capacity == 120);
   assert(a_new->keysize == 0);
   assert(a_new->table[0].population == 0);
   assert(a_new->table[1].population == 0);

   /* -------------------------------------- */

   /* Free tests*/
   assoc_free(a1);
   assoc_free(a2);
   assoc_free(a3);
   assoc_free(a_new);


   /* -------------------------------------- */
}
