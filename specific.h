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

struct assoc_arr {
   assoc_ind *arr;
   unsigned int population;
};

typedef struct assoc_arr assoc_arr;

struct assoc {
   assoc_arr *table;
   int keysize;
   unsigned int capacity;
};
typedef struct assoc assoc;
