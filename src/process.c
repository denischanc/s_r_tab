
#include "process.h"

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "common.h"
#include "search.h"
#include "replace.h"
#include "extension.h"
#include "usage.h"

/*******************************************************************************
*******************************************************************************/

static int process_file(const char * path, int action, int lvl)
{
  if((lvl == 0) || extension_valid(path))
  {
    switch(action)
    {
      case __SEARCH: return file_search(path);
      case __REPLACE: return file_replace(path);
    }
  }
  return VAL;
}

/*******************************************************************************
*******************************************************************************/

static char * dir_cat_path(const char * path, const struct dirent * field)
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

static int process_path(const char * path, int action, int recursive, int lvl);

static int process_dir(const char * path, int action, int recursive, int lvl)
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
        fullPath = dir_cat_path(path, field);
        if(!process_path(fullPath, action, recursive, lvl)) result = ERR;
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

/*******************************************************************************
*******************************************************************************/

static int process_path(const char * path, int action, int recursive, int lvl)
{
  struct stat buf;

  if(stat(path, &buf) == 0)
  {
    if(S_ISDIR(buf.st_mode))
    {
      if((lvl <= 0) || recursive)
      {
        return process_dir(path, action, recursive, lvl + 1);
      }
      return VAL;
    }
    else if(S_ISREG(buf.st_mode))
    {
      return process_file(path, action, lvl);
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

/*******************************************************************************
*******************************************************************************/

int process_file_dir(int argc, char * argv[], int action)
{
  int i, result = EXIT_SUCCESS;

  for(i = optind; i < argc; i++)
  {
    if(!process_path(argv[i], action, recursive, 0))
    {
      result = EXIT_FAILURE;
    }
  }

  return result;
}
