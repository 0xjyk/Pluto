/* file: init1.c
 * description: demonstrates pluto's ability to - 
 * `(1) declare structures 
 *  (2) associate aliases via type definitions
 *  (3) initialise structs using compound literals 
 * 
 * author: jasraj kripalani
 */

#include "example1.h"

// a few global declarations
red         cr;
green       cg;
blue        cb;
teal        ct;
lavender    cl;


int main(int argc, char **argv) {
    // initialise a new struct using a initializer list
    struct s1 palette1 = { 1, &cr, &cg, &cb, &ct, &cl};
    
    // a compound literal creates a new lval obect
    struct s1 *palette2 = &(struct s1) {1, &cr, &cg, &cb, &ct, &cl};
    
    // initialise only a subset of the members 
    struct s2 *palette3 = &(struct s2) {.flag = 1, &cr, .g = &cg};
    
    // reassign palette1 using palette2 - compatible types
    palette1 = *palette2;

    // since struct s1 is not compatible with struct 2 it is
    // an error to assign the content of palette3 to palette1
    palette1 = *palette3;

    // a really complicated initialisation
    struct ComplexStruct obj = {
        .geometry.point = {10, 20},
        .flags = {.is_visible = 1, .is_active = 1, .type = 'a'},
        .on_update = NULL,
        .next = NULL
    };
}
