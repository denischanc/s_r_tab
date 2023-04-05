
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include "config.h"

#define SPACE 2

#define __UNDEFINE 0
#define __SEARCH 1
#define __REPLACE 2

#define EXE_SEARCH "s_tab"
#define EXE_REPLACE "r_tab"

#define ERR (0 == 1) /* error */
#define VAL (0 == 0) /* valid */

char * extensions[] =
{
  "c", "C", "cpp", "cxx", "cc", "java", "h", "pl", "php", "e", "l", "y", "js",
  "xml", "json", NULL
};

/*******************************************************************************
*******************************************************************************/

#define USAGE_MSG \
  "Usage: %s ([option...]) [ file ... ] [ directory ... ]\n" \
  "Options:\n" \
  "     -V, --version                      Display version number\n" \
  "     -R, --recursive                    Recursive\n" \
  "     -S, --space [nb]                  Tab space number\n" \
  "Extensions: "

void display_extensions(FILE * f)
{
  char ** extension;
  for(extension = extensions; *extension != NULL; extension++)
  {
    if(extension == extensions) fprintf(f, *extension);
    else fprintf(f, ",%s", *extension);
  }
}

void usage(const char * exe)
{
  fprintf(stderr, USAGE_MSG, exe);
  display_extensions(stderr);
  fprintf(stderr, "\n");
}

/*******************************************************************************
*******************************************************************************/

int concatLine(char ** msg, int line)
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
      return ERR;
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
      return ERR;
    }
    tmp[0] = 0;
    strcat(tmp, buffer);
  }

  *msg = tmp;
  return VAL;
}

int fileSearchLoop(FILE * file, char ** tabMsg, char ** endlineMsg)
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
          if(!concatLine(endlineMsg, line)) return ERR;
        }
        spaceTab = 0;
        line++;
        break;

      case '\t':
        spaceTab++;
        if(!concatLine(tabMsg, line)) return ERR;
        break;

      case ' ': spaceTab++; break;

      default: spaceTab = 0;
    }
  }
  return VAL;
}

int fileSearch(const char * name)
{
  FILE * file = fopen(name, "r");
  char * tabMsg = 0, * endlineMsg = 0;
  int result = VAL;

  if(!file)
  {
    fprintf(stderr, "Unable to open file : [%s]\n", name);
    return ERR;
  }

  if(fileSearchLoop(file, &tabMsg, &endlineMsg))
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
  else result = ERR;

  if(tabMsg) free(tabMsg);
  if(endlineMsg) free(endlineMsg);
  fclose(file);

  return result;
}

/*******************************************************************************
*******************************************************************************/

#define BUFFER_SIZE 10000

int copy(const char * src, const char * dest)
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

int fileReplaceLoop(FILE * file, FILE * pidFile, const char * pidName,
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

int fileReplaceResult(int modified, const char * name, const char * pidName)
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

int fileReplace(const char * name)
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

  result = fileReplaceLoop(file, pidFile, pidName, &modified);
  fclose(file); fclose(pidFile);

  if(result) result = fileReplaceResult(modified, name, pidName);
  else remove(pidName);
  free(pidName);

  return result;
}

/*******************************************************************************
*******************************************************************************/

int extensionValid(const char * name)
{
  int i, size, j, diff;
  char * extension = extensions[0];

  for(i = 0; extension != NULL; extension = extensions[++i])
  {
    size = strlen(extension);
    diff = strlen(name) - size;
    if(diff > 1)
    {
      if(name[diff - 1] == '.')
      {
        j = 0;
        while((name[diff + j] == extension[j]) && (j < size)) j++;
        if(j == size) return VAL;
      }
    }
  }
  return ERR;
}

/*******************************************************************************
*******************************************************************************/

int processFile(const char * path, int action)
{
  if(extensionValid(path))
  {
    switch(action)
    {
      case __SEARCH: return fileSearch(path);
      case __REPLACE: return fileReplace(path);
    }
  }
  return VAL;
}

char * dirCatPath(const char * path, const struct dirent * field)
{
  int size;
  char * resName;

  size = strlen(path) + strlen(field -> d_name) + 2;
  resName = (char *)malloc(size * sizeof(char));
  resName[0] = 0;
  strcat(resName, path);
  strcat(resName, "/");
  strcat(resName, field -> d_name);

  return resName;
}

int processPath(const char * path, int action, int recursive, int lvl);

int processDirectory(const char * path, int action, int recursive, int lvl)
{
  DIR * dir;
  struct dirent * field;
  char * fullPath;
  int result = VAL;

  dir = opendir(path);
  if(dir)
  {
    while((field = readdir(dir)))
    {
      if(strcmp(field -> d_name, ".") && strcmp(field -> d_name, ".."))
      {
        fullPath = dirCatPath(path, field);
        if(!processPath(fullPath, action, recursive, lvl)) result = ERR;
        free(fullPath);
      }
    }
    closedir(dir);
    return result;
  }
  else
  {
    fprintf(stderr, "Unable to open directory [%s]\n", path);
    return ERR;
  }
}

int processPath(const char * path, int action, int recursive, int lvl)
{
  struct stat buf;

  if(stat(path, &buf) == 0)
  {
    if(S_ISDIR(buf.st_mode))
    {
      if((lvl <= 0) || recursive)
      {
        return processDirectory(path, action, recursive, lvl + 1);
      }
      return VAL;
    }
    else if(S_ISREG(buf.st_mode))
    {
      return processFile(path, action);
    }
    else
    {
      fprintf(stderr, "[%s] not directory or regular file\n", path);
    }
  }
  else
  {
    perror(__FUNCTION__);
  }
  return ERR;
}

int processArg(int argc, char * argv[], int action)
{
  int i, result = EXIT_SUCCESS, recursive = ERR;

  for(i = 1; i < argc; i++)
  {
    if(!strcmp(argv[i], "-V") || !strcmp(argv[i], "--version"))
    {
      fprintf(stdout, "Version: " PACKAGE_VERSION "\n");
    }
    else if(!strcmp(argv[i], "-R") || !strcmp(argv[i], "--recursive"))
    {
      recursive = VAL;
    }
    else if(!processPath(argv[i], action, recursive, 0))
    {
      result = EXIT_FAILURE;
    }
  }
  return result;
}

int main(int argc, char * argv[])
{
  int action = __UNDEFINE;
  char * basenameSelf = (char *)basename(argv[0]);

  if(!strcmp(basenameSelf, EXE_SEARCH)) action = __SEARCH;
  else if(!strcmp(basenameSelf, EXE_REPLACE)) action = __REPLACE;
  else
  {
    fprintf(stderr, "Bad executable name : [%s]\n", basenameSelf);
    return EXIT_FAILURE;
  }

  if(argc == 1)
  {
    switch(action)
    {
      case __SEARCH: usage(EXE_SEARCH); break;
      case __REPLACE: usage(EXE_REPLACE); break;
    }
    return EXIT_FAILURE;
  }

  return processArg(argc, argv, action);
}
