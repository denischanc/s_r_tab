
#include "replace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

int nb_space = 2;

/*******************************************************************************
*******************************************************************************/

#define BUFFER_SIZE 10000

static int copy(const char * src, const char * dst)
{
  FILE * file_src = fopen(src, "r"), * file_dst = fopen(dst, "w");
  char buffer[BUFFER_SIZE];
  int size;

  if(!file_src)
  {
    if(file_dst) fclose(file_dst);
    fprintf(stderr, "Unable to open file : [%s]\n", src);
    return __FALSE;
  }
  if(!file_dst)
  {
    fclose(file_src);
    fprintf(stderr, "Unable to open file : [%s]\n", dst);
    return __FALSE;
  }

  while((size = fread(buffer, sizeof(char), BUFFER_SIZE, file_src)) != 0)
  {
    if((fwrite(buffer, sizeof(char), size, file_dst)) != size)
    {
      fprintf(stderr, "Unable to write into file : [%s]\n", dst);
      fclose(file_src);
      fclose(file_dst);
      return __FALSE;
    }
  }

  fclose(file_src);
  fclose(file_dst);

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int loop(FILE * file, FILE * pid_file, const char * pid_name,
  int * modified)
{
  char c;
  int space_tab = 0;

  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\t': space_tab += nb_space; *modified = __TRUE; break;
      case ' ': space_tab++; break;

      case '\n': if(space_tab != 0) *modified = __TRUE; space_tab = 0;

      default:
        int error = __FALSE;

        for(int i = 0; i < space_tab; i++)
          if(!fprintf(pid_file, " ")) error = __TRUE;
        space_tab = 0;
        if(!fprintf(pid_file, "%c", c)) error = __TRUE;

        if(error)
        {
          fprintf(stderr, "Unable to write into file : [%s]\n", pid_name);
          return __FALSE;
        }
    }
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int end_replace(int modified, const char * name, const char * pid_name)
{
  if(modified)
  {
    if(copy(pid_name, name)) fprintf(stdout, "File modified : [%s]\n", name);
    else
    {
      fprintf(stderr, "Unable to copy [%s] into [%s]\n", pid_name, name);
      fprintf(stderr, "Delete [%s] yourself ...\n", pid_name);
      return __FALSE;
    }
  }
  else fprintf(stdout, "File not modified : [%s]\n", name);

  remove(pid_name);

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int file_replace(const char * name)
{
  FILE * file = fopen(name, "r"), * pid_file;
  char * pid_name;
  int modified = __FALSE, result;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return __FALSE;
  }

  pid_name = (char *)malloc((strlen(name) + 20) * sizeof(char));
  if(!pid_name)
  {
    fprintf(stderr, "Not enough memory ...\n");
    fclose(file);
    return __FALSE;
  }
  sprintf(pid_name, "%s.%d", name, getpid());
  pid_file = fopen(pid_name, "w");
  if(!pid_file)
  {
    fprintf(stderr, "Unable to open temporary file : [%s]\n", pid_name);
    fclose(file); free(pid_name);
    return __FALSE;
  }

  result = loop(file, pid_file, pid_name, &modified);
  fclose(file); fclose(pid_file);

  if(result) result = end_replace(modified, name, pid_name);
  else remove(pid_name);
  free(pid_name);

  return result;
}
