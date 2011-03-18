#pragma once

#include <fstream>

typedef unsigned char uint8_t;

extern int yyparse();
extern int yydebug;
extern FILE* yyin;
extern const char* yyin_filename;

extern int parse(const char* filename);
