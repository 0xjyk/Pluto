/* file: example2.c
 * description: demonstrates pluto's ability to - 
 *  (1) accept declarations of varied forms
 *  (2) motivate the parsing complexity that arrises A
 *
 * author: jasraj kripalani
 */

int main(int argc, char *argv) {
    // level 1: simple declarations     
    unsigned int i, j = 7, k;                   // unsigned int
   
    signed long long int sll1;                  // signed long long int
    long long int sll2;                         // signed long long int
    long long sll3;                             // signed long long int
    int long signed long sll4;                  // weird, but ok!

    
    // level 2: simple declarations with qualifiers
    const volatile _Bool flag;                  // const volatile _Bool
    const signed ci;                            // const signed int ci; 

    
    // level 3: pointer declarations
    int *ip;                                    // pointer to int 
    const int *cip;                             // pointer to const int
    int * const icp;                            // const pointer to int
    const int * const cicp;                     // const pointer to const int

    typedef int *int_ptr;                       // pointer to int
    const int_ptr constant_ptr;                 // const pointer to int

    
    // level 4: array declarations
    int arr[];                                  // variable array of int(s)
    char (arr2[4]);                             // array of 4 chars
    char (* restrict arr3)[4];                  // array of 4 char * restrict pointers
    
    
    // level 5: function pointers
    int *func1(int);                            // function returning a pointer to an int
    int (*func2)(int);                          // poiner to a function return an int
    int (*fpfi(int (*)(long), int))(int, ...);  // :( 

    
    // level 6: interspersed function pointers and array declarations 
    int (*apfi[3])(int *x, int *y);             // :D
    
}
