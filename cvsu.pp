# 1 "tests/parse-tests/cvsu"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "tests/parse-tests/cvsu"
struct s { int i ; const int ci ; };
struct s s;
const struct s cs;
volatile struct s vs;

int main(int argc, char **argv) {

    s.i;

    s.ci;

    cs.i;

    cs.ci;

    vs.i;

    vs.ci;

}
