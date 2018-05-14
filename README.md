# SG_Fextract
Steins;Gate MPK file extractor

# Usage

    SG_Fextract - Made by Dragneel1234@Github
    Usage: SG_Fextract [-h, -l, -z] Input Output

    Required parameters:
       Input        : Input file to read from
       Output       : Output dir to write to, nul for current one

    Optional parameters:
       -h           : Print this help
       -l           : List file from input only
       -z           : Zero out content of raw data inside file

    Note: -z will have no effect if -l is used

# Compilation
Compile it using

    gcc --pedantic -ansi -Wall -Werror -Ofast -o SG_Fextract SG_Fextract.c

# MPK file
MPK file is a file of simple structure, you can find detail [here](https://github.com/Dragneel1234/SG_Fextract/blob/master/MPKFILE.md)
