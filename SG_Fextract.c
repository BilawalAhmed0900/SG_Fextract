#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#if defined(__linux__)
    #include <sys/stat.h>
    #include <alloca.h>
#endif

#include "structs.c"
#include "definitions.c"
#include "errors_enum.c"
#include "functions.c"

#if 0
    /* Print the help section of the program */
    void print_help(void);

    /* A simple wrapper around malloc, exiting the program if buffer is not allocated */
    void *xalloc(size_t size);
    /* if (ptr != NULL) free(ptr); */
    void xfree(void *ptr);

    /* Check if the content in mhdr is from the correct MPK file */
    int32_t verify_mainheader(mainheader *mhdr);

    /* Fill the param by reading the argv */
    void populate(parameters *params, int32_t argc, char *argv[]);

    /* A wrapper around fopen, exiting the program if fopen returns NULL */
    FILE *file_open(char *filename, const char *mode);

    /* If dir exists */
    int32_t dir_exists(const char *dir_name);

    /* Print each member of fhdr */
    void print_fileheader(fileheader *fhdr);

    /* Used by extract_all */
    void extract_one(FILE *inptr, char *outdir, fileheader *fhdr, int32_t zero_out_file);

    /* Extract or re-add all files from inptr, with info from fhdr_array */
    void extract_all(FILE *inptr, char *outdir, fileheader *fhdr_array, int32_t count, int32_t zero_out_file);

    /* Process on inptr */
    void process(FILE *inptr, char *outdir, int32_t list_only, int32_t zero_out_file);
#endif

int32_t main(int32_t argc, char *argv[])
{
    FILE *inptr;
    parameters params;
    
    fprintf(HELP_PRINT_TO, "SG_Fextract - Made by Dragneel1234@Github\n");
    
    memset(&params, 0, sizeof(parameters));
    populate(&params, argc, argv);
    if (strcmp(params.output_dir, "nul") != 0 && dir_exists(params.output_dir) && params.reput_to_file == 0)
    {
        fprintf(stderr, "'%s' already exists\n", params.output_dir);
        exit(DIR_ALREADY_EXISTS);
    }
    
    inptr = file_open(params.input_file, "rb+");
    process(inptr, params.output_dir, params.list_only, params.zero_out_file, params.reput_to_file);
    
    fclose(inptr);
    
    return 0;
}