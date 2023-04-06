
#include "extension.h"

#include <string.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

static const char * extensions[] =
{
  "c", "C", "cpp", "cxx", "cc", "java", "h", "pl", "php", "e", "l", "y", "js",
  "xml", "json", NULL
};

#define MAX_EXT 100
static const char * other_extensions[MAX_EXT + 1] = {};

/*******************************************************************************
*******************************************************************************/

void display_extensions(FILE * f)
{
  const char ** extension;
  for(extension = extensions; *extension != NULL; extension++)
  {
    if(extension == extensions) fprintf(f, *extension);
    else fprintf(f, ",%s", *extension);
  }
}

/*******************************************************************************
*******************************************************************************/

static int extension_valid_help(const char * name, const char ** extensions)
{
  int size, j, diff;
  const char ** extension;

  for(extension = extensions; *extension != NULL; extension++)
  {
    size = strlen(*extension);
    diff = strlen(name) - size;
    if(diff > 1)
    {
      if(name[diff - 1] == '.')
      {
        j = 0;
        while((name[diff + j] == (*extension)[j]) && (j < size)) j++;
        if(j == size) return VAL;
      }
    }
  }
  return ERR;
}

int extension_valid(const char * name)
{
  return (extension_valid_help(name, extensions) == VAL) ? VAL :
    extension_valid_help(name, other_extensions);
}

/*******************************************************************************
*******************************************************************************/

void add_extensions(char * extensions)
{
  char * start = extensions, * cur = extensions;
  int i = 0, done = ERR;

  while(!done)
  {
    if((*cur == ':') || (*cur == 0))
    {
      if(*cur == 0) done = VAL;

      if(cur > start)
      {
        *cur = 0;
        if(i < MAX_EXT) other_extensions[i++] = strdup(start);
      }
      start = cur + 1;
    }

    cur++;
  }
}
