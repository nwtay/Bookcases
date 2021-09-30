#include <string.h>

void test_priv(void);

typedef int datatype;
#define INITSIZE 17
#define CAPLIMIT 0.6
#define SCALEFACTOR 4

struct assoc_ind {
   int seen;
   void *key;
   void *data;
};

typedef struct assoc_ind assoc_ind;

struct assoc {
   assoc_ind *arr;
   int keysize;
   unsigned int capacity;
   unsigned int population;
};

typedef struct assoc assoc;
