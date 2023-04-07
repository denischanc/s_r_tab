#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "buffer.h"

typedef struct
{
  void (*start)();

  int (*tab)();
  int (*space)();
  int (*cr)(const char * name, int line);
  int (*other)(char c);

  int (*stop)(const char * name, int line);
} SEARCH_SCANNER;

extern int verbose;

int search_cat_line(BUFFER * buffer);

int file_search(const char * name);

#endif
