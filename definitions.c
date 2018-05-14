#if defined(__linux__)
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
#define ZEROING_ARG "-z"
#define HELP_ARG "-h"

#define HELP_PRINT_TO stderr

#if defined(_WIN32)
    #define DELIM "\\"
#else
    #define DELIM "/"
#endif