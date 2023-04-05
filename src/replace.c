
#include "replace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

#define SPACE 2

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
    return ERR;
  }
  if(!fileDest)
  {
    fclose(fileSrc);
    fprintf(stderr, "Unable to open file : [%s]\n", dest);
    return ERR;
  }

  while((size = fread(buffer, sizeof(char), BUFFER_SIZE, fileSrc)) != 0)
  {
    if((fwrite(buffer, sizeof(char), size, fileDest)) != size)
    {
      fprintf(stderr, "Unable to write into file : [%s]\n", dest);
      fclose(fileSrc);
      fclose(fileDest);
      return ERR;
    }
  }

  fclose(fileSrc);
  fclose(fileDest);
  return VAL;
}

/*******************************************************************************
*******************************************************************************/

static int loop(FILE * file, FILE * pidFile, const char * pidName,
  int * modified)
{
  char c;
  int spaceTab = 0, i, error;

  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    switch(c)
    {
      case '\t': spaceTab += SPACE; *modified = VAL; break;
      case ' ': spaceTab++; break;

      case '\n': if(spaceTab != 0) *modified = VAL; spaceTab = 0;

      default:
        error = ERR;

        for(i = 0; i < spaceTab; i++)
          if(!fprintf(pidFile, " ")) error = VAL;
        spaceTab = 0;
        if(!fprintf(pidFile, "%c", c)) error = VAL;

        if(error)
        {
          fprintf(stderr, "Unable to write into file : [%s]\n", pidName);
          return ERR;
        }
    }
  }
  return VAL;
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
      return ERR;
    }
  }
  else fprintf(stdout, "File not modified : [%s]\n", name);

  remove(pidName);
  return VAL;
}

/*******************************************************************************
*******************************************************************************/

int file_replace(const char * name)
{
  FILE * file = fopen(name, "r"), * pidFile;
  char * pidName;
  int modified = ERR, result;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return ERR;
  }

  pidName = (char *)malloc((strlen(name) + 10) * sizeof(char));
  if(!pidName)
  {
    fprintf(stderr, "Not enough memory ...\n");
    fclose(file);
    return ERR;
  }
  sprintf(pidName, "%s.%d", name, getpid());
  pidFile = fopen(pidName, "w");
  if(!pidFile)
  {
    fprintf(stderr, "Unable to open temporary file : [%s]\n", pidName);
    fclose(file); free(pidName);
    return ERR;
  }

  result = loop(file, pidFile, pidName, &modified);
  fclose(file); fclose(pidFile);

  if(result) result = do_result(modified, name, pidName);
  else remove(pidName);
  free(pidName);

  return result;
}
