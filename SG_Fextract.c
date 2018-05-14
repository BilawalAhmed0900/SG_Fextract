#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
    #include <sys/stat.h>
    #include <alloca.h>
    #include <unistd.h>
    #define _mkdir(name) mkdir(name, 0777)
#else
    #define _mkdir(name) mkdir(name)
#endif

#define min(a, b)\
    (a < b) ? a : b

#define LIST_ARG "-l"
#define HELP_ARG "-h"

#define HELP_PRINT_TO stderr

#ifdef _WIN32
    #define DELIM "\\"
#else
    #define DELIM "/"
#endif

enum ERRORS
{
    EXTRA_PARAMS = 1, NOT_ENOUGH_PARAMS, 
    FILE_ERROR, 
    WRONG_ARGUMENTS, 
    INVALID_FILE, 
    NOT_ENOUGH_MEMORY,
    DIR_ALREADY_EXISTS,
    INCOMPLETE_FILE,
    OPTIONAL_PARAMETER_PLACEMENT
};

#pragma pack(push, 1)
typedef struct _mainheader
{
    char identifier[4];
    int16_t UNKNOWN1;
    int16_t UNKNOWN2;
} mainheader;

typedef struct _filecount
{
    int64_t filecount;
    int64_t UNKNOWN1[6];
    int32_t UNKNOWN2;
} filecount;

typedef struct _fileheader
{
    int32_t fileindex;
    int64_t position;
    int64_t filesize;
    int64_t filesize_again;
    char filename[228];
} fileheader;

typedef struct _parameters
{
    int32_t list_only;
    char *input_file;
    char *output_dir;
} parameters;
#pragma pack(pop)

void print_help(void)
{
    fprintf(HELP_PRINT_TO, "SG_Fextract.exe [-h, -l] Input Output\n");
    fprintf(HELP_PRINT_TO, "\n");
    fprintf(HELP_PRINT_TO, "Required parameters:\n");
    fprintf(HELP_PRINT_TO, "   Input \t: Input file to read from\n");
    fprintf(HELP_PRINT_TO, "   Output\t: Output dir to write to, nul for current one\n");
    fprintf(HELP_PRINT_TO, "\n");
    fprintf(HELP_PRINT_TO, "Optional parameters:\n");
    fprintf(HELP_PRINT_TO, "   -h    \t: Print this help\n");
    fprintf(HELP_PRINT_TO, "   -l    \t: List file from input only\n");
    fprintf(HELP_PRINT_TO, "\n");
}

void *xalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, "Error: Can't allocate memory of size %zu\n", size);
        exit(NOT_ENOUGH_MEMORY);
    }
    
    return ptr;
}

void xfree(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

int32_t verify_mainheader(mainheader *mhdr)
{
    if (strcmp(mhdr->identifier, "MPK") == 0)
    {
        return 1;
    }
    
    return 0;
}

void populate(parameters *params, int32_t argc, char *argv[])
{
    int32_t got_input = 0;
    int32_t got_output = 0;
    
    /* At 0, exe name exists */
    int32_t loopcounter = 1;
    
    if (argc == 1)
    {
        print_help();
        exit(EXIT_SUCCESS);
    }
    
    for (; loopcounter < argc; loopcounter++)
    {
        goto start; 

        /* Optional parameter found after one positional parameter */
        optional_placement_error:
        {
            fprintf(stderr, "Error: Optional parameters should come first\n");
            print_help();
            exit(OPTIONAL_PARAMETER_PLACEMENT);
        }
        
        start:
        if (strcmp(argv[loopcounter], HELP_ARG) == 0)
        {
            if (got_input == 1 || got_output == 1)
            {
                goto optional_placement_error;
            }
            
            print_help();
            exit(EXIT_SUCCESS);
        }
        
        if (strcmp(argv[loopcounter], LIST_ARG) == 0)
        {
            if (got_input == 1 || got_output == 1)
            {
                goto optional_placement_error;
            }
            params->list_only = 1;
            continue;
        }
        
        if (argv[loopcounter][0] == '-')
        {
            goto error;
        }
        
        if (got_input == 0)
        {
            params->input_file = argv[loopcounter];
            got_input = 1;
            continue;
        }
        
        if (got_output == 0)
        {
            params->output_dir = argv[loopcounter];
            got_output = 1;
            continue;
        }
        
        /* Extra paramter found */
        error:
        fprintf(stderr, "Error: Extra parameter '%s'\n", argv[loopcounter]);
        print_help();
        exit(EXTRA_PARAMS);
    }
    
    /* One of the positional paramter not found */
    if (got_input == 0 || got_output == 0)
    {
        fprintf(stderr, "Error: %s parameter not given\n", got_input == 0 ? "Input" : "Output");
        print_help();
        exit(NOT_ENOUGH_PARAMS);
    }
}

/* A simple wrapper around fopen */
FILE *file_open(char *filename, const char *mode)
{
    FILE *ptr;
    if (filename == NULL || mode == NULL || (strcmp(mode, "rb") != 0 && strcmp(mode, "wb")))
    {
        fprintf(stderr, "Error: Arguments given to file_open are wrong\n");
        exit(WRONG_ARGUMENTS);
    }
    
    ptr = fopen(filename, mode);
    if (ptr == NULL)
    {
        fprintf(stderr, "Error: '%s' %s\n", filename, mode[0] == 'r' ? "can't be opened" : "can't be created");
        exit(FILE_ERROR);
    }
    
    return ptr;
}

int32_t dir_exists(const char *dir_name)
{
    DIR *dir = opendir(dir_name);
    if (dir != NULL)
    {
        closedir(dir);
        return 1;
    }
    
    return 0;
}

void print_fileheader(fileheader *fhdr)
{
    printf("Index: %d\nFile Position: %d\nFile Size: %d\nFile Size again: %d\nFile Name %s\n",
        (int32_t)fhdr->fileindex, (int32_t)fhdr->position, 
        (int32_t)fhdr->filesize, (int32_t)fhdr->filesize_again, 
        fhdr->filename);
}

void extract_one(FILE *inptr, char *outdir, fileheader *fhdr)
{
    char *name;
    char *buffer;
    int32_t sizeofbuffer;
    int32_t sizeofdata;
    FILE *file;
    
    if (strcmp(outdir, "nul") == 0)
    {
        sizeofbuffer = strlen(fhdr->filename) + 1;
        name = alloca(sizeofbuffer);
        sprintf(name, "%s", fhdr->filename);
    }
    else
    {
        /* '.' + DELIM + outdir + DELIM + filename + 1 */
        sizeofbuffer = 1 + 1 + strlen(outdir) + 1 + strlen(fhdr->filename) + 1; 
        name = alloca(sizeofbuffer);
        sprintf(name, ".%s%s%s%s", DELIM, outdir, DELIM, fhdr->filename);
    }
    
    /* One of them is compressed and other is uncompressed size */
    sizeofdata = min(fhdr->filesize, fhdr->filesize_again);
    buffer = xalloc(sizeofdata);
    
    file = file_open(name, "wb");
    fseek(inptr, fhdr->position, SEEK_SET);
    if (fread(buffer, 1, sizeofdata, inptr) != sizeofdata)
    {
        fprintf(stderr, "Error: file incomplete\n");
        exit(INCOMPLETE_FILE);    
    }
    
    if (fwrite(buffer, 1, sizeofdata, file) != sizeofdata)
    {
        fprintf(stderr, "Error: file incomplete\n");
        exit(INCOMPLETE_FILE);    
    }
    
    fclose(file);
    xfree(buffer);
}

void extract_all(FILE *inptr, char *outdir, fileheader *fhdr_array, int32_t count)
{
    int32_t loopcounter;
    
    if (strcmp(outdir, "nul") != 0)
    {
        _mkdir(outdir);
    }
    
    for (loopcounter = 0; loopcounter < count; loopcounter++)
    {
        printf("Extracting: %.2f%%\r", (float)loopcounter / count * 100);
        extract_one(inptr, outdir, &fhdr_array[loopcounter]);
    }
    
    printf("Extracting: 100.00%%\n");
}

void process(FILE *inptr, char *outdir, int32_t list_only)
{
    mainheader mhdr;
    filecount  fcount;
    fileheader *fhdr;
    int64_t total_fdhrs_size;
    int64_t loopcounter;
    
    if (fread(&mhdr, 1, sizeof(mainheader), inptr) != sizeof(mainheader))
    {
        fprintf(stderr, "Error: file incomplete\n");
        exit(INCOMPLETE_FILE);
    }
    
    if (verify_mainheader(&mhdr) == 0)
    {
        fprintf(stderr, "Error: file not a valid MPK file\n");
        exit(INVALID_FILE);
    }
    
    if (fread(&fcount, 1, sizeof(filecount), inptr) != sizeof(filecount))
    {
        fprintf(stderr, "Error: file incomplete\n");
        exit(INCOMPLETE_FILE);
    }
    
    total_fdhrs_size = fcount.filecount * sizeof(fileheader);
    fhdr = xalloc((size_t)total_fdhrs_size);
    if (fread(fhdr, 1, total_fdhrs_size, inptr) != total_fdhrs_size)
    {
        fprintf(stderr, "Error: file incomplete\n");
        exit(INCOMPLETE_FILE);
    }
    
    if (list_only == 1)
    {
        for (loopcounter = 0; loopcounter < fcount.filecount; loopcounter++)
        {
            print_fileheader(&fhdr[loopcounter]);
            printf("\n");
        }
        
        goto end;
    }
    
    extract_all(inptr, outdir, fhdr, fcount.filecount);
    
    /* end of the function */
    end:
    xfree(fhdr);
}

int32_t main(int32_t argc, char *argv[])
{
    FILE *inptr;
    parameters params;
    
    fprintf(HELP_PRINT_TO, "SG_Fextract - Made by Dragneel1234@Github\n");
    
    memset(&params, 0, sizeof(parameters));
    populate(&params, argc, argv);
    if (strcmp(params.output_dir, "nul") != 0 && dir_exists(params.output_dir))
    {
        fprintf(stderr, "'%s' already exists\n", params.output_dir);
        exit(DIR_ALREADY_EXISTS);
    }
    
    inptr = file_open(params.input_file, "rb");
    process(inptr, params.output_dir, params.list_only);
    
    fclose(inptr);
    
    return 0;
}
