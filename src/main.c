// SPDX-FileCopyrightText: 2022 Gavin Distaso
// SPDX-License-Identifier: MIT License

#define UNICODE
#define _UNICODE 

#include <main.h>

#include <cli/cli-parser.h>
#include <cli/cli-logger.h>

#include <builder/bld-lexer.h>

#include <stdbool.h>

/*
* The command line arguments are as follows
*                                   [Description]
* --- FLAGS ---
* `-I`                       -->    Sets to interperet mode
* --- KEYS ---
* `-o [relative filepath]`   -->    specifies output filepath (required)
* `-c [language]`            -->    Sets to convert mode for specific language
* `-i` [filepath]            -->    specifies input filepath (required)
*/
const char* TAGS[] = {"-I", "-o", "-c", "-i"};
const int TAGS_LEN = sizeof(TAGS) / sizeof(char*);

static inline void retriveBuildInfo(CLI_CTX*,SETTINGS_CTX*,char**,char**);

//TODO: refactor all this crap that is below: idk, use macros or smth
int main(int argc, char** argv){
    CLI_CTX cli = {argv, argc};
    SETTINGS_CTX settings = SETTINGS_CTX();

    // check for unknown tags
    int tagI;
    if((tagI = CLI_findUnknownTags(&cli, TAGS, TAGS_LEN)) != -1)
        CLI_logStatus(STATUS_FATAL, "unknown tag '%s'", argv[tagI]);
    
    char* outputFile, *inputFile;
    
    retriveBuildInfo(&cli, &settings, &outputFile, &inputFile);
    
    /* ===== Tokenize File ===== */

    LEXER_CTX lexer;
    if(!BLD_readSource(&lexer, inputFile))
        CLI_logStatus(STATUS_FATAL, "input file not found ['%s']", inputFile);

    BLD_freeLexer(&lexer);
}

static inline void retriveBuildInfo(
    CLI_CTX* cli, 
    SETTINGS_CTX* settings,
    char** outputFile,
    char** inputFile
){
    // get build mode

    bool 
        interpret = CLI_findTag(cli, "-I") != -1,
        convert = CLI_findTag(cli, "-c") != -1,
        compile = 1 & !interpret & !convert;

    if(interpret && convert){
        CLI_logStatus(STATUS_FATAL, "can only convert OR interpret");
    } else if(interpret && !!(settings->buildMode = 1)){
        CLI_logStatus(STATUS_LOG, "setting build mode to interpret");
    } else if(convert && !!(settings->buildMode = 2)){
        CLI_logStatus(STATUS_FATAL, "convert isnt ready yet!");
    } else if(compile)
        CLI_logStatus(STATUS_FATAL, "compile isnt ready yet!");

    // get build file locations

    if(!CLI_getKeyPair(cli, "-o", outputFile) && !interpret)
        CLI_logStatus(STATUS_FATAL, "output file not specified! try `-o [filepath]`");

    if(!CLI_getKeyPair(cli, "-i", inputFile))
        CLI_logStatus(STATUS_FATAL, "input file not specified! try `-i [filepath]`");
}