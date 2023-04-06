#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include <stdio.h>

void display_extensions(FILE * f);

int extension_valid(const char * name);

void add_extensions(char * extensions_decl);

#endif
