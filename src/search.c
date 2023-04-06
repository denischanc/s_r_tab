
#include "search.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

static int concat_line(char ** msg, int line)
{
  static char buffer[12];
  char * tmp;
  int size = 0;

  sprintf(buffer, "%d", line);
  size = strlen(buffer);

  if(*msg != 0)
  {
    size += strlen(*msg) + 3;
    tmp = (char *)malloc(size * sizeof(char));
    if(!tmp)
    {
      fprintf(stderr, "Not enough memory ...\n");
      return __FALSE;
    }
    tmp[0] = 0;
    strcat(tmp, *msg);
    strcat(tmp, ", ");
    strcat(tmp, buffer);
    free(*msg);
  }
  else
  {
    size += 1;
    tmp = (char *)malloc(size * sizeof(char));
    if(!tmp)
    {
      fprintf(stderr, "Not enough memory ...\n");
      return __FALSE;
    }
    tmp[0] = 0;
    strcat(tmp, buffer);
  }

  *msg = tmp;
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int loop(FILE * file, char ** tabMsg, char ** endlineMsg)
{
  char c;
  int line = 1, spaceTab = 0;

  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\n':
        if(spaceTab)
        {
          if(!concat_line(endlineMsg, line)) return __FALSE;
        }
        spaceTab = 0;
        line++;
        break;

      case '\t':
        spaceTab++;
        if(!concat_line(tabMsg, line)) return __FALSE;
        break;

      case ' ': spaceTab++; break;

      default: spaceTab = 0;
    }
  }
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int file_search(const char * name)
{
  FILE * file = fopen(name, "r");
  char * tabMsg = 0, * endlineMsg = 0;
  int result = __TRUE;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return __FALSE;
  }

  if(loop(file, &tabMsg, &endlineMsg))
  {
    if(tabMsg || endlineMsg)
    {
      fprintf(stdout, "-----[%s]-----\n", name);
      if(tabMsg)
        fprintf(stdout, "\tTab: %s\n", tabMsg);
      if(endlineMsg)
        fprintf(stdout, "\tEnd line: %s\n", endlineMsg);
    }
  }
  else result = __FALSE;

  if(tabMsg) free(tabMsg);
  if(endlineMsg) free(endlineMsg);
  fclose(file);

  return result;
}

