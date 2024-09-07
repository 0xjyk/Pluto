enum {PERM=0, FUNC, STMT};
enum {
#define TOKEN(a,b,c) a=b,
#define KEY(a,b,c) a=b,
#include "include/token.h"
#undef TOKEN
#undef KEY
};
