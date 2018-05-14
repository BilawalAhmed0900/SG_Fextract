#ifndef STRUCT_X
#define STRUCT_X

#include <stdint.h>

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
    int32_t zero_out_file;
    char *input_file;
    char *output_dir;
} parameters;
#pragma pack(pop)

#endif /* STRUCT_X */