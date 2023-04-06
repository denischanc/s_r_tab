
#include "extension.h"

#include <string.h>
#include <stdlib.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

static char * extensions_decl =
  "c:C:cpp:cxx:cc:java:h:pl:php:e:l:y:js:xml:json";

#define EXT_MAX 50
static const char * extensions[EXT_MAX];
static int extensions_size = 0;
static int extensions_ok = __FALSE;

/*******************************************************************************
*******************************************************************************/

void add_extensions(char * extensions_decl);

static const char ** __extensions()
{
  if(!extensions_ok)
  {
    extensions_ok = __TRUE;

    add_extensions(extensions_decl);
  }
  return extensions;
}

/*******************************************************************************
*******************************************************************************/

void display_extensions(FILE * f)
{
  const char ** extensions = __extensions();
  for(int i = 0; i < extensions_size; i++)
  {
    if(i == 0) fprintf(f, *extensions++);
    else fprintf(f, ":%s", *extensions++);
  }
}

/*******************************************************************************
*******************************************************************************/

int extension_valid(const char * name)
{
  int size, j, diff;
  const char ** extensions = __extensions();

  for(int i = 0; i < extensions_size; i++, extensions++)
  {
    size = strlen(*extensions);
    diff = strlen(name) - size;
    if(diff > 1)
    {
      if(name[diff - 1] == '.')
      {
        j = 0;
        while((name[diff + j] == (*extensions)[j]) && (j < size)) j++;
        if(j == size) return __TRUE;
      }
    }
  }
  return __FALSE;
}

/*******************************************************************************
*******************************************************************************/

void add_extensions(char * extensions_decl)
{
  char * start = extensions_decl, * cur = start;
  const char ** extensions = __extensions() + extensions_size;
  int done = __FALSE;

  while(!done && (extensions_size < EXT_MAX))
  {
    if((*cur == ':') || (*cur == 0))
    {
      if(*cur == 0) done = __TRUE;

      if(cur > start)
      {
        int size = cur - start;
        char * tmp = (char *)malloc((size + 1) * sizeof(char));
        strncpy(tmp, start, size);
        tmp[size] = 0;

        *(extensions++) = tmp;
        extensions_size++;
      }

      start = cur + 1;
    }

    cur++;
  }
}
