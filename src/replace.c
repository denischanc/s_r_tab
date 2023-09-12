
#include "replace.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "console.h"
#include "usage.h"

/*******************************************************************************
*******************************************************************************/

static char * pid_name;
static FILE * pid_file;

static int space_tab, mod;

/*******************************************************************************
*******************************************************************************/

static int start()
{
  space_tab = 0;
  mod = __FALSE;

  pid_name = (char *)malloc((strlen(parser_ws.path) + 20) * sizeof(char));
  if(pid_name == NULL) return console_errno(__FUNCTION__);
  sprintf(pid_name, "%s.%d", parser_ws.path, getpid());

  pid_file = fopen(pid_name, "w");
  if(pid_file == NULL)
  {
    free(pid_name);
    return console_err("Unable to open temporary file : [%s]\n", pid_name);
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int tab()
{
  space_tab += nb_space;
  mod = __TRUE;
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int space()
{
  space_tab++;
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int space_ub()
{
  space_tab++;
  mod = __TRUE;
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int other(char c)
{
  int err = __FALSE;

  for(int i = 0; i < space_tab && !err; i++)
    if(!fprintf(pid_file, " ")) err = __TRUE;
  space_tab = 0;

  if(!err && !fprintf(pid_file, "%c", c)) err = __TRUE;

  if(err) return console_err("Unable to write into file : [%s]\n", pid_name);
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int crlf_(int is_cr)
{
  if(space_tab != 0) mod = __TRUE;
  space_tab = 0;

  if(is_cr && !other('\r')) return __FALSE;
  return other('\n');
}

static int lf()
{
  return crlf_(__FALSE);
}

static int crlf()
{
  return crlf_(__TRUE);
}

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
    return console_err("Unable to open file : [%s]\n", src);
  }
  if(!file_dst)
  {
    fclose(file_src);
    return console_err("Unable to open file : [%s]\n", dst);
  }

  while((size = fread(buffer, sizeof(char), BUFFER_SIZE, file_src)) != 0)
  {
    if((fwrite(buffer, sizeof(char), size, file_dst)) != size)
    {
      fclose(file_src);
      fclose(file_dst);
      return console_err("Unable to write into file : [%s]\n", dst);
    }
  }

  fclose(file_src);
  fclose(file_dst);

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int stop(int ok)
{
  fclose(pid_file);

  if(ok)
  {
    if(mod)
    {
      if(copy(pid_name, parser_ws.path))
      {
        console_out("File modified : [%s]\n", parser_ws.path);
      }
      else
      {
        console_err("Unable to copy [%s] into [%s]\n", pid_name,
          parser_ws.path);
        ok = __FALSE;
      }
    }
    else console_out("File not modified : [%s]\n", parser_ws.path);
  }

  remove(pid_name);
  free(pid_name);

  return ok;
}

/*******************************************************************************
*******************************************************************************/

SRT_PARSER replace_parser =
{
  start,
  tab,
  space,
  space_ub,
  lf,
  crlf,
  other,
  stop
};
