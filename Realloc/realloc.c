#include "specific.h"
#include "../assoc.h"

typedef enum bool {false, true} bool;

/* Resizes the structure, to cope with excessive collision */
void _resize(assoc** a);
/* Returns the hash location from DJB2 algorithm */
unsigned int _hash_loc(void *key_in, unsigned int capacity, int keysize);
/* Returns the probe size, using DJB2 algorithm */
unsigned int _step_hash(void* key_in, unsigned int capacity, int keysize);
/* Returns the new hash location, after stepping by probe size */
unsigned int _new_hash(void* key_in, int capacity, unsigned int currenthash, int keysize);
/* Custom memory allocation function, used in resize function */
void _newcalloc(assoc** a, unsigned int bytes, int keysize);
/* Returns true if the value of 2 keys are equal */
bool _found(assoc *a, void *key, void *key2);
/* Returns true if the current capacity is a prime number */
bool _isprime(unsigned int capacity);
/* Private test function */
void test_priv(void);

assoc* assoc_init(int keysize)
{
   assoc *ptr = (assoc *)ncalloc(1, sizeof(assoc));
   ptr->arr = (assoc_ind *)ncalloc(INITSIZE, sizeof(assoc_ind));
   if(keysize<0){
      on_error("Incorrect Keysize Input\n");
   }
   ptr->capacity = INITSIZE;
   ptr->keysize = keysize;
   return ptr;
}

unsigned int assoc_count(assoc* a)
{
   return a->population;
}

/*Based on the djb2 algorithm, sourced from:
http://www.cse.yorku.ca/~oz/hash.html*/
unsigned int _step_hash(void* key_in, unsigned int capacity, int keysize)
{
   unsigned int hash = 5381, c = 0;
   int x = 0;
   char *key = (char *)key_in;
   if(!keysize){
      while((c = (unsigned int)*key++)){
         hash = ((hash << 5) + hash) + c;
      }
   }
   else{
      for(x = 0; x < keysize; x++){
         c = (unsigned int)*key++;
         hash = ((hash << 5) + hash) + c;
      }
   }
   hash = hash/capacity;
   hash = hash%capacity;
   if(hash < 1){
      hash = 1;
   }
   return hash;
}

/*Based on the djb2 algorithm, sourced from:
http://www.cse.yorku.ca/~oz/hash.html*/
unsigned int _hash_loc(void *key_in, unsigned int capacity, int keysize)
{
   unsigned int hash = 5381, c = 0;
   int x = 0;
   char *key = (char *)key_in;
   if(!keysize){
      while((c = (unsigned int)*key++)){
         hash = ((hash << 5) + hash) + c;
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

void assoc_free(assoc* a)
{
   free(a->arr);
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
   a1->arr = (assoc_ind *)ncalloc(bytes, sizeof(assoc_ind));
   a1->capacity = bytes;
   a1->keysize = keysize;
   *a = a1;
}

void _resize(assoc** a)
{
   assoc *a_new;
   assoc *aorig = *a;
   unsigned int currentcap = aorig->capacity, newcap, i;
   newcap = currentcap*SCALEFACTOR;
   while(!_isprime(newcap)){
      newcap++;
   }
   _newcalloc(&a_new, newcap, aorig->keysize);
   *a = a_new;
   for(i = 0; i < currentcap; i++){
      if(aorig->arr[i].seen==1){
         assoc_insert(a, aorig->arr[i].key, aorig->arr[i].data);
      }
   }
   assoc_free(aorig);
}

unsigned int _new_hash(void* key_in, int capacity, unsigned int currhash, int keysize)
{
   unsigned int hash = 0;
   char *key = (char *)key_in;
   int diff = (int)(currhash - _step_hash(key, capacity, keysize));
   if(diff<0){
      hash = currhash - _step_hash(key, capacity, keysize);
      hash = hash + capacity;
      hash = hash%capacity;
   }
   else{
      hash = currhash - _step_hash(key, capacity, keysize);
   }
   return hash;
}

void* assoc_lookup(assoc* a, void* key)
{
   void *ptr = NULL;
   unsigned int hash;
   if(key){
      hash = _hash_loc(key, a->capacity, a->keysize);
      if(a->arr[hash].key){
         while(!_found(a, a->arr[hash].key, key)){
            hash = _new_hash(key, a->capacity, hash, a->keysize);
            if(!a->arr[hash].key){
               return NULL;
            }
         }
         ptr = a->arr[hash].data;
      }
   }
   return ptr;
}

void assoc_insert(assoc** a, void* key, void* data)
{
   unsigned int hash = 0, seen_at = 0;
   int seen_flag = 0;
   assoc *assoc1 = *a;
   double cap = CAPLIMIT*(double)assoc1->capacity;
   if(assoc1->population > (unsigned int)cap){
      _resize(a);
      assoc1 = *a;
   }
   hash = _hash_loc(key, assoc1->capacity, assoc1->keysize);
   while((assoc1->arr[hash].key)&&(seen_flag==0)){
      if(_found(assoc1, assoc1->arr[hash].key, key)){
         seen_flag = 1;
         seen_at = hash;
      }
      hash = _new_hash(key, assoc1->capacity, hash, assoc1->keysize);
   }
   if(seen_flag == 0){
      assoc1->population++;
      assoc1->arr[hash].seen = 1;
      assoc1->arr[hash].key = key;
      assoc1->arr[hash].data = data;
   }
   else{
      assoc1->arr[seen_at].data = data;
   }
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
   unsigned int prev_cap = 0, x, temphash = 0, prev_pop, y;
   float fmore;
   char *word1 = "hello";
   char *word2 = "zeb";
   char *word3 = "mmmmm";
   char *word4 = "doesntwor";
   char *word5 = "a";
   char *word6 = "?";
   char *word9 = "farandoles";
   char *word10 = NULL;
   int d1 = 1;
   int d2 = 17;
   int d3 = 18;
   int d4 = 100;
   int d5 = 10;
   int d6 = -1;
   float f1 = 11.22;
   float f2 = 1.09;
   float f3 = 0.09;
   unsigned int step1;
   static int i1[10];
   static int j1[10];
   static int k1[10];

   /* -------------------------------------- */

   a1 = assoc_init(0);
   assert(a1);
   assert(a1->capacity == INITSIZE);
   assert(a1->keysize == 0);
   assert(a1->arr);
   assert(!assoc_count(a1));

   a2 = assoc_init(1);
   assert(a2);
   assert(a2->capacity == INITSIZE);
   assert(a2->keysize == 1);
   assert(a2->arr);
   assert(!assoc_count(a2));

   a3 = assoc_init(sizeof(float));
   assert(a3);
   assert(a3->capacity == INITSIZE);
   assert(a3->arr);
   assert(!assoc_count(a3));

   /* -------------------------------------- */

   assert(_step_hash(word1,INITSIZE,0)!=0);
   assert(_step_hash(word1,INITSIZE,0)<INITSIZE);
   assert(_step_hash(word2,INITSIZE,0)!=0);
   assert(_step_hash(word2,INITSIZE,0)<INITSIZE);
   assert(_step_hash(word3,INITSIZE,0)!=0);
   assert(_step_hash(word3,INITSIZE,0)<INITSIZE);
   assert(_step_hash(word4,INITSIZE,0)!=0);
   assert(_step_hash(word4,INITSIZE,0)<INITSIZE);
   assert(_step_hash(word5,INITSIZE,0)!=0);
   assert(_step_hash(word5,INITSIZE,0)<INITSIZE);
   assert(_step_hash(word6,INITSIZE,0)!=0);
   assert(_step_hash(word6,INITSIZE,0)<INITSIZE);

   for(x = 0; x < 500; x++){
      x = (rand()%300000)+1;
      assert(_step_hash(&x,INITSIZE,sizeof(unsigned int))!=0);
      assert(_step_hash(&x,INITSIZE,sizeof(unsigned int))<INITSIZE);
   }

   for(x = 0; x < 500; x++){
      fmore = (float)rand()/(float)300000.0;
      y = (unsigned int)fmore;
      assert(_step_hash(&y,INITSIZE,sizeof(float))!=0);
      assert(_step_hash(&y,INITSIZE,sizeof(float))<INITSIZE);
   }

   /* -------------------------------------- */

   assert(_isprime(0)==false);
   assert(_isprime(1)==false);
   assert(_isprime(2)==true);
   assert(_isprime(10)==false);
   assert(_isprime(139)==true);
   assert(_isprime(198197)==true);
   assert(_isprime(763771)==true);
   assert(_isprime(763772)==false);

   /* -------------------------------------- */

   /* Inserting first word tests */
   assoc_insert(&a1, word1, &i1[0]);
   temphash = _hash_loc(word1, a1->capacity, a1->keysize);
   assert(a1->arr[temphash].seen == 1);
   assert(a1->arr[temphash].data == &i1[0]);
   assert((char *)a1->arr[temphash].key == word1);
   assert(strcmp((char *)a1->arr[temphash].key,word1)==0);
   /*checking word is inserted only once - should make a function for this!!*/
   for(x = 0; x < a1->capacity; x++){
      if(x != temphash){
         assert(!a1->arr[x].key);
         assert(!a1->arr[x].seen);
         assert(!a1->arr[x].data);
      }
   }
   assert(a1->keysize == 0);
   assert(a1->population);
   assert(assoc_count(a1));

   /* Inserting second word tests */
   assoc_insert(&a1, word9, &i1[1]);
   temphash = _hash_loc(word9, a1->capacity, a1->keysize);
   assert(a1->arr[temphash].seen == 1);
   assert(a1->arr[temphash].data == &i1[1]);
   assert((char *)a1->arr[temphash].key == word9);
   assert(strcmp((char *)a1->arr[temphash].key,word9)==0);

   /* Inserting first number tests */
   assoc_insert(&a2, &d4, &j1[0]);
   temphash = _hash_loc(&d4, a2->capacity, a2->keysize);
   assert(a2->arr[temphash].seen == 1);
   assert(a2->arr[temphash].data == &j1[0]);
   assert(*(int *)a2->arr[temphash].key == d4);
   assert(memcmp(a2->arr[temphash].key, &d4, a1->keysize)==0);
   /*checking number is inserted only once */
   for(x = 0; x < a2->capacity; x++){
      if(x != temphash){
         assert(!a2->arr[x].key);
         assert(!a2->arr[x].seen);
         assert(!a2->arr[x].data);
      }
   }
   assert(a2->keysize != 0);
   assert(a2->population);
   assert(assoc_count(a2));
   prev_pop = a2->population;

   assoc_insert(&a2, &d5, &j1[1]);
   temphash = _hash_loc(&d5, a2->capacity, a2->keysize);
   assert(a2->arr[temphash].seen == 1);
   assert(a2->arr[temphash].data == &j1[1]);
   assert(*(int *)a2->arr[temphash].key == d5);
   assert(a2->population == prev_pop+1);
   prev_pop = a2->population;

   /* Checking no duplicates */
   assoc_insert(&a2, &d4, &j1[0]);
   assert(a2->population == prev_pop);

   /* Inserting negative number tests */
   assoc_insert(&a2, &d6, &j1[2]);
   temphash = _hash_loc(&d6, a2->capacity, sizeof(int));
   assert(a2->population == prev_pop+1);
   prev_pop = a2->population;

   /* Inserting floats tests */
   assoc_insert(&a3, &f1, &k1[0]);
   temphash = _hash_loc(&f1, a3->capacity, sizeof(float));
   assert(a3->arr[temphash].seen == 1);
   assert(a3->arr[temphash].data == &k1[0]);
   assert(memcmp(a3->arr[temphash].key, &f1, a3->keysize)==0);
   /*checking float is inserted only once */
   assert(a3->keysize != 0);
   assert(a3->population==1);
   assert(assoc_count(a3));

   assoc_insert(&a3, &f2, &k1[1]);
   temphash = _hash_loc(&f2, a3->capacity, sizeof(float));
   assert(a3->arr[temphash].seen == 1);
   assert(a3->arr[temphash].data == &k1[1]);
   assert(memcmp(a3->arr[temphash].key, &f2, a3->keysize)==0);
   assert(a3->population==2);
   assert(assoc_count(a3));

   assoc_insert(&a3, &f3, &k1[2]);
   temphash = _hash_loc(&f3, a3->capacity, sizeof(float));
   assert(a3->arr[temphash].seen);
   assert(a3->population==3);
   assert(assoc_count(a3));

   /* -------------------------------------- */

   /* Resize tests */
   prev_cap = a1->capacity;
   _resize(&a1);
   assert(a1);
   assert(a1->arr);
   assert(a1->keysize == 0);
   assert(_isprime(a1->capacity));
   assert((int)a1->capacity >= (int)(SCALEFACTOR*prev_cap));
   for(x = 0; x < a1->capacity; x++){
      if(a1->arr[x].seen==1){
         assert((strcmp((char *)a1->arr[x].key,word9)==0) ||
         (strcmp((char *)a1->arr[x].key,word1)==0));
      }
   }

   prev_cap = a2->capacity;
   _resize(&a2);
   assert(a2);
   assert(a2->arr);
   assert(a2->keysize != 0);
   assert(_isprime(a2->capacity));
   assert((int)a2->capacity >= (int)(SCALEFACTOR*prev_cap));

   prev_cap = a3->capacity;
   _resize(&a3);
   assert(a3);
   assert(a3->arr);
   assert(a3->keysize != 0);
   assert(_isprime(a3->capacity));
   assert((int)a3->capacity >= (int)(SCALEFACTOR*prev_cap));

   /* -------------------------------------- */
   /* New hash char tests */
   step1 = _step_hash(word1, INITSIZE,0);
   assert(_new_hash(word1, INITSIZE, 17, 0)==17-step1);
   step1 = _step_hash(word2, INITSIZE, 0);
   assert(_new_hash(word2, INITSIZE, 15, 0)==15-step1);
   step1 = _step_hash(word4, INITSIZE, 0);
   assert(_new_hash(word4, INITSIZE, 15, 0)==15-step1);
   step1 = _step_hash(word1, INITSIZE, 0);
   assert(_new_hash(word1, INITSIZE, 5, 0)>0);
   assert(_new_hash(word1, INITSIZE, 5, 0)<INITSIZE);

   assert(_new_hash(word3, INITSIZE, 5, 0)<INITSIZE);
   assert(_new_hash(word1, INITSIZE, 1, 0)<INITSIZE);
   assert(_new_hash(word6, 6, 5, 0)<INITSIZE);

   /* -------------------------------------- */

   /* New hash int tests */
   step1 = _step_hash(&d1, INITSIZE, sizeof(int));
   assert(_new_hash(&d1, INITSIZE, 15, sizeof(int))==15-step1);
   step1 = _step_hash(&d2, INITSIZE, sizeof(int));
   assert(_new_hash(&d2, INITSIZE, 15, sizeof(int))==15-step1);
   step1 = _step_hash(&d3, INITSIZE, sizeof(int));
   assert(_new_hash(&d3, INITSIZE, 15, sizeof(int))==15-step1);
   step1 = _step_hash(&d4, INITSIZE, sizeof(int));
   assert(_new_hash(&d4, INITSIZE, 15, sizeof(int))==15-step1);
   step1 = _step_hash(&d4, INITSIZE, sizeof(int));
   assert(_new_hash(&d4, INITSIZE, 5, sizeof(int))<INITSIZE);

   for(x = 0; x < 500; x++){
      x = (rand()%300000)+1;
      assert(_new_hash(&x, INITSIZE, 5, sizeof(unsigned int))<INITSIZE);
   }

   /* -------------------------------------- */

   /* Assoc lookup tests */
   assert(!assoc_lookup(a2, &d1));
   assert(!assoc_lookup(a2, &d2));
   assert(!assoc_lookup(a2, &d3));
   assert(assoc_lookup(a2, &d4));
   assert(assoc_lookup(a2, &d5));
   assoc_insert(&a2, &d3, &j1[2]);
   assert(assoc_lookup(a2, &d3));

   assert(assoc_lookup(a1,word1));
   assert(assoc_lookup(a1,word9));
   assert(assoc_lookup(a1,word3)==NULL);
   assert(!assoc_lookup(a1,word10));

   /* -------------------------------------- */

   _newcalloc(&a_new, 120, 0);
   assert(a_new);
   assert(a_new->arr);
   assert(a_new->capacity == 120);
   assert(a_new->keysize == 0);
   assert(a_new->population == 0);

   /* -------------------------------------- */

   /* Free tests*/
   assoc_free(a1);
   assoc_free(a2);
   assoc_free(a3);
   assoc_free(a_new);

}
