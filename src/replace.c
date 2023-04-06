
#include "replace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "usage.h"

/*******************************************************************************
*******************************************************************************/

int nb_space = 2;

/*******************************************************************************
*******************************************************************************/

#define BUFFER_SIZE 10000

static int copy(const char * src, const char * dest)
{
  FILE * fileSrc = fopen(src, "r"),
       * fileDest = fopen(dest, "w");
  char buffer[BUFFER_SIZE];
  int size;

  if(!fileSrc)
  {
    if(fileDest) fclose(fileDest);
    fprintf(stderr, "Unable to open file : [%s]\n", src);
    return __FALSE;
  }
  if(!fileDest)
  {
    fclose(fileSrc);
    fprintf(stderr, "Unable to open file : [%s]\n", dest);
    return __FALSE;
  }

  while((size = fread(buffer, sizeof(char), BUFFER_SIZE, fileSrc)) != 0)
  {
    if((fwrite(buffer, sizeof(char), size, fileDest)) != size)
    {
      fprintf(stderr, "Unable to write into file : [%s]\n", dest);
      fclose(fileSrc);
      fclose(fileDest);
      return __FALSE;
    }
  }

  fclose(fileSrc);
  fclose(fileDest);
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int loop(FILE * file, FILE * pid_file, const char * pid_name,
  int * modified)
{
  char c;
  int space_tab = 0, i, error;

  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\t': space_tab += nb_space; *modified = __TRUE; break;
      case ' ': space_tab++; break;

      case '\n': if(space_tab != 0) *modified = __TRUE; space_tab = 0;

      default:
        error = __FALSE;

        for(i = 0; i < space_tab; i++)
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

static int do_result(int modified, const char * name, const char * pidName)
{
  if(modified)
  {
    if(copy(pidName, name)) fprintf(stdout, "File modified : [%s]\n", name);
    else
    {
      fprintf(stderr, "Unable to copy [%s] into [%s]\n", pidName, name);
      fprintf(stderr, "Delete [%s] yourself ...\n", pidName);
      return __FALSE;
    }
  }
  else fprintf(stdout, "File not modified : [%s]\n", name);

  remove(pidName);
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int file_replace(const char * name)
{
  FILE * file = fopen(name, "r"), * pidFile;
  char * pidName;
  int modified = __FALSE, result;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return __FALSE;
  }

  pidName = (char *)malloc((strlen(name) + 20) * sizeof(char));
  if(!pidName)
  {
    fprintf(stderr, "Not enough memory ...\n");
    fclose(file);
    return __FALSE;
  }
  sprintf(pidName, "%s.%d", name, getpid());
  pidFile = fopen(pidName, "w");
  if(!pidFile)
  {
    fprintf(stderr, "Unable to open temporary file : [%s]\n", pidName);
    fclose(file); free(pidName);
    return __FALSE;
  }

  result = loop(file, pidFile, pidName, &modified);
  fclose(file); fclose(pidFile);

  if(result) result = do_result(modified, name, pidName);
  else remove(pidName);
  free(pidName);

  return result;
}
