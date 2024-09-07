#include "pluto.h"
int num_files;
char **infiles;
Vector tempfiles;
Vector ppfiles;
unsigned char JOBSCOPE;

void parseoptions(int argc, char **argv) {
    if (argc == 1) {
        usage();
        exit(EXIT_FAILURE);
    }
    JOBSCOPE = CGEN;
    int opt;
    // cli args define the scope of work for pluto
    while ((opt = getopt(argc, argv, "lptcho:")) != -1) {
        switch (opt) {
        case 'l':
            JOBSCOPE = LEX;
            break;
        case 'p':
            JOBSCOPE = PARSE;
            break;
        case 't':
            JOBSCOPE = TYPECHECK;
            break;
        case 'c':
            JOBSCOPE = CGEN;
            break;
        case 'o':
            outfile = optarg;
            break;
        case 'h': default:
            usage();
            exit(EXIT_FAILURE);
        }
    }
    assert(JOBSCOPE != 0 && JOBSCOPE <= 0x7);
    // extract files
    num_files = argc - optind;
    infiles = argv + optind;
    // setup 
    // retrieve the current working directory
    cwd = getcwd(NULL, 0);
    assert(cwd != NULL);
    // create a temporary directory in the current working directory
    strcat(temp_dir, cwd);
    strcat(temp_dir, temp_dir_name);
    assert(strlen(temp_dir) <= PATH_MAX);
}

void usage() {
    fprintf(stdout,
        "usage: pluto [scope options] files [-o file]\n"
        "\n"
        "scope options can be one of -\n"
        " -l          preprocess and lex files\n"
        " -p          preprocess, lex and parse files\n"
        " -t          preprocess, lex, parse and typecheck files\n"
        " -c          preprocess, lex, parse, typecheck and generate code files (default)\n"
        " -h          print this message\n"
        "\n"
        "note: if more than one options are provided, the last option is considered\n"
        "      except '-h' which leads to termination.\n"
        "      files is a list of one or more space separated files. each file is \n"
        "      expected to be a relative path to the current working directory.\n"
        "\n"
        "optionally, an outfile file can be provided using -\n"
        " -o file     (creates/overwrites) file with the output\n"
        "note: file is expected to be a relative path to the current working directory.\n"
         
        );
}


// preprocess each file, save preprocessed output to current working directory
void preprocess() {
    tempfiles = vec_init(num_files);
    ppfiles = vec_init(num_files);
    for (int i = 0; i < num_files; i++) {
        // prepare file to hold pp output 
        char pp_file[PATH_MAX+1]; 
        *pp_file ='\0';
        strcat(pp_file, basename(infiles[i]));
        strcat(pp_file, pp_suffix);
        vec_pushback(tempfiles, strdup(pp_file));
        vec_pushback(ppfiles, strdup(pp_file));
        
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        int fd = open(pp_file, O_WRONLY | O_CREAT | O_TRUNC, mode);
        if (fd <= -1) {
            printf("couldn't open %s\n", pp_file);
            exit(EXIT_FAILURE);
        }
        // Utilise GNU cpp to preprocess 
        char *pp[] = {"cpp", 
            "-E",
            "-I", 
            "/afs/.ir/users/j/k/jk10/lib/musl/include/", 
            "-std=c11", infiles[i], "-o", pp_file, NULL};
        pid_t pid = fork(); 
        if (pid < 0) 
            fprintf(stderr, "coudln't preprocess 0 %s\n", infiles[i]);
        if (pid == 0) {
            execvp("cpp", pp);
            fprintf(stderr, "coudn't preprocess 1%s\n", infiles[i]);
            exit(EXIT_FAILURE);
        }
        int status; 
        waitpid(pid, &status, 0);
        if (status < 0) {
            fprintf(stderr, "coudn't preprocess %s\n", infiles[i]);
            exit(EXIT_FAILURE);
        } 
    }
}

void process() {
    for (int i = 0; i < ppfiles->size; i++) {
        parse((char *)vec_get(ppfiles, i));
    }
}

void remove_tempfiles() {
    if (!tempfiles)
        return;
    for (int i = 0; i < tempfiles->size; i++) {
        char *file = vec_get(tempfiles, i);
        unlink(file);
        free(file);
    }
    vec_free(tempfiles);
}
