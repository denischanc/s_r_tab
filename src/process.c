
#include "process.h"

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#include "extension.h"
#include "console.h"
#include "scanner.h"
#include "common.h"
#include "replace.h"
#include "search-simple.h"
#include "search-verbose.h"
#include "usage.h"

/*******************************************************************************
*******************************************************************************/

static SRT_PARSER * parser;

/*******************************************************************************
*******************************************************************************/

static int process_file(const char * path, int lvl)
{
  console_debug("file[%s] level[%d]\n", path, lvl);

  if((lvl != 0) && !extension_valid(path)) return __TRUE;

  FILE * file = fopen(path, "r");
  if(file == NULL) return console_err("Unable to open file : [%s]\n", path);

  int result = scan(path, file, parser);

  fclose(file);

  return result;
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

static int process_path(const char * path, int lvl);

static int process_dir(const char * path, int lvl)
{
  DIR * dir;
  struct dirent * field;
  int result = __TRUE;

  console_debug("directory[%s] level[%d]\n", path, lvl);

  dir = opendir(path);
  if(dir)
  {
    while((field = readdir(dir)))
    {
      if(strcmp(field -> d_name, ".") && strcmp(field -> d_name, ".."))
      {
        char * full_path = dir_cat_path(path, field);
        if(!process_path(full_path, lvl)) result = __FALSE;
        free(full_path);
      }
    }
    closedir(dir);

    return result;
  }
  else return console_err("Unable to open directory [%s]\n", path);
}

/*******************************************************************************
*******************************************************************************/

static int process_path(const char * path, int lvl)
{
  struct stat buf;

  if(stat(path, &buf) == 0)
  {
    if(S_ISDIR(buf.st_mode))
    {
      if((lvl <= 0) || recursive) return process_dir(path, lvl + 1);
      return __TRUE;
    }
    else if(S_ISREG(buf.st_mode)) return process_file(path, lvl);
    else return console_err("[%s] not directory or regular file\n", path);
  }
  else return console_errno(__FUNCTION__);
}

/*******************************************************************************
*******************************************************************************/

static SRT_PARSER * __parser(int action)
{
  if(action == __REPLACE) return &replace_parser;
  else if((action == __SEARCH) && verbose) return &search_verbose_parser;
  else return &search_simple_parser;
}

int process_file_dir(int argc, char * argv[], int action)
{
  int i, result = __TRUE;

  parser = __parser(action);

  for(i = optind; i < argc; i++)
  {
    if(!process_path(argv[i], 0)) result = __FALSE;
  }

  return result;
}
