# SG_Fextract
Steins;Gate MPK file extractor

# Usage

    SG_Fopener.exe [-h, -l] Input Output

    Required parameters:
       Input        : Input file to read from
       Output       : Output dir to write to, nul for current one

    Optional parameters:
       -h           : Print this help
       -l           : List file from input only

# Compilation
Compile it using

    gcc.exe --pedantic -ansi -Wall -Werror -Ofast -o SG_Fextract.exe SG_Fextract.c

# MPK file
MPK file is a file of simple structure, you can find detail [here](https://github.com/Dragneel1234/SG_Fextract/blob/master/MPKFILE.md)
