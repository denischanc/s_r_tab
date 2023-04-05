
#include "extension.h"

#include <string.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

static char * extensions[] =
{
  "c", "C", "cpp", "cxx", "cc", "java", "h", "pl", "php", "e", "l", "y", "js",
  "xml", "json", NULL
};

/*******************************************************************************
*******************************************************************************/

void display_extensions(FILE * f)
{
  char ** extension;
  for(extension = extensions; *extension != NULL; extension++)
  {
    if(extension == extensions) fprintf(f, *extension);
    else fprintf(f, ",%s", *extension);
  }
}

/*******************************************************************************
*******************************************************************************/

int extension_valid(const char * name)
{
  int size, j, diff;
  char ** extension;

  for(extension = extensions; *extension != NULL; extension++)
  {
    size = strlen(*extension);
    diff = strlen(name) - size;
    if(diff > 1)
    {
      if(name[diff - 1] == '.')
      {
        j = 0;
        while((name[diff + j] == *extension[j]) && (j < size)) j++;
        if(j == size) return VAL;
      }
    }
  }
  return ERR;
}
