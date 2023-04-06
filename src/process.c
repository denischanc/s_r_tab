
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

/*******************************************************************************
*******************************************************************************/

int recursive = __TRUE;

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
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static char * dir_cat_path(const char * path, const struct dirent * field)
{
  int size;
  char * res_name;

  size = strlen(path) + strlen(field -> d_name) + 2;
  res_name = (char *)malloc(size * sizeof(char));
  res_name[0] = 0;
  strcat(res_name, path);
  strcat(res_name, "/");
  strcat(res_name, field -> d_name);

  return res_name;
}

static int process_path(const char * path, int action, int lvl);

static int process_dir(const char * path, int action, int lvl)
{
  DIR * dir;
  struct dirent * field;
  int result = __TRUE;

  dir = opendir(path);
  if(dir)
  {
    while((field = readdir(dir)))
    {
      if(strcmp(field -> d_name, ".") && strcmp(field -> d_name, ".."))
      {
        char * full_path = dir_cat_path(path, field);
        if(!process_path(full_path, action, lvl)) result = __FALSE;
        free(full_path);
      }
    }
    closedir(dir);

    return result;
  }
  else
  {
    fprintf(stderr, "Unable to open directory [%s]\n", path);
    return __FALSE;
  }
}

/*******************************************************************************
*******************************************************************************/

static int process_path(const char * path, int action, int lvl)
{
  struct stat buf;

  if(stat(path, &buf) == 0)
  {
    if(S_ISDIR(buf.st_mode))
    {
      if((lvl <= 0) || recursive) return process_dir(path, action, lvl + 1);
      return __TRUE;
    }
    else if(S_ISREG(buf.st_mode)) return process_file(path, action, lvl);
    else fprintf(stderr, "[%s] not directory or regular file\n", path);
  }
  else perror(__FUNCTION__);

  return __FALSE;
}

/*******************************************************************************
*******************************************************************************/

int process_file_dir(int argc, char * argv[], int action)
{
  int i, result = __TRUE;

  for(i = optind; i < argc; i++)
  {
    if(!process_path(argv[i], action, 0)) result = __FALSE;
  }

  return result;
}
