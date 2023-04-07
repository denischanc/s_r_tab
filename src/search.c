
#include "search.h"

#include <stdio.h>
#include "common.h"
#include "search-simple.h"
#include "search-verbose.h"

/*******************************************************************************
*******************************************************************************/

int verbose = __FALSE;

static SEARCH_SCANNER * scanner;

static int line;

/*******************************************************************************
*******************************************************************************/

int search_cat_line(BUFFER * buffer)
{
  static char line_string[12];
  snprintf(line_string, sizeof(line_string), "%d", line);
  return buffer_add_string(buffer, line_string);
}

/*******************************************************************************
*******************************************************************************/

static int loop(const char * name, FILE * file)
{
  line = 1;

  scanner -> start();

  char c;
  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\n': if(!scanner -> cr(name, line)) return __FALSE; line++; break;
      case '\t': if(!scanner -> tab()) return __FALSE; break;
      case ' ': if(!scanner -> space()) return __FALSE; break;
      default: if(!scanner -> other(c)) return __FALSE;
    }
  }

  return scanner -> stop(name, line);
}

/*******************************************************************************
*******************************************************************************/

int file_search(const char * name)
{
  FILE * file = fopen(name, "r");

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return __FALSE;
  }

  scanner = verbose ? &scanner_verbose : &scanner_simple;

  int result = loop(name, file);

  fclose(file);

  return result;
}
