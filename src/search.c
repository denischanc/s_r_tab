
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

  if(*msg != NULL)
  {
    size += strlen(*msg) + 3;
    tmp = (char *)malloc(size * sizeof(char));
    if(tmp == NULL)
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
    if(tmp == NULL)
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

static int loop(FILE * file, char ** tab_msg, char ** endline_msg)
{
  char c;
  int line = 1, space_tab = 0;

  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\n':
        if((space_tab != 0) && !concat_line(endline_msg, line)) return __FALSE;
        space_tab = 0;
        line++;
        break;

      case '\t':
        space_tab++;
        if(!concat_line(tab_msg, line)) return __FALSE;
        break;

      case ' ': space_tab++; break;

      default: space_tab = 0;
    }
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int file_search(const char * name)
{
  FILE * file = fopen(name, "r");
  char * tab_msg = NULL, * endline_msg = NULL;
  int result = __TRUE;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return __FALSE;
  }

  if(loop(file, &tab_msg, &endline_msg))
  {
    if(tab_msg || endline_msg)
    {
      fprintf(stdout, "-----[%s]-----\n", name);
      if(tab_msg)
        fprintf(stdout, "\tTab: %s\n", tab_msg);
      if(endline_msg)
        fprintf(stdout, "\tEnd line: %s\n", endline_msg);
    }
  }
  else result = __FALSE;

  if(tab_msg) free(tab_msg);
  if(endline_msg) free(endline_msg);
  fclose(file);

  return result;
}

