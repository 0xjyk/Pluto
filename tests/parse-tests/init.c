
int x[] = {1, 3, 5};
int z[] = {1};
int y[][] = {{1,2,3}, {1,3,5}};
int xyz[][][] = { 
    { {1,2,3}, {4,5,6} }, 
    { {7, 8, 9}, {10, 11, 12}}
};
typedef int A[]; 
A a = {1, 2}, b = {3, 4, 5};
char s[] = "abc", t[3] = "abc";
int i;
char svs[i] = "aba";
#define NULL ((void *) 0)

struct s {
    char c; 
    short s;
    int i; 
    long l;
    long long ll;
    int *ip;
};
struct st {
    int i; 
    int arr[4][2];
};

struct st s0 = {1, {[0] = {2, 3}, [1][0] = {4}, [3] = {4, 5}}};


struct sr {
    int ii;
    struct sq {
        unsigned int u;
    } i;
};

struct sr s4 = {.i.u = 0, .i = {3}, .ii=1};

struct s ss = {'a', .i=1, 2,};
struct s s1 = {.i=1, 3, .ll=5, .ip=0};
/*


// incorrect solutions 

char str[] = {"string"};

// error 

struct s s2 = {.u.s="str"};

*/
