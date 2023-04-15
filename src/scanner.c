
#include "scanner.h"

#include "common.h"

/*******************************************************************************
*******************************************************************************/

static int is_prev;
static char c, prev;

static SRT_PARSER * parser;

/*******************************************************************************
*******************************************************************************/

static int scan_prev()
{
  switch(c)
  {
    case (0xa0 - 0x100):
      if(prev == 0xc2) return parser -> space_ub();
  }

  if(!parser -> other(prev)) return __FALSE;
  is_prev = __FALSE;

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int scan_main()
{
  switch(c)
  {
    case '\n':
      if(!parser -> cr()) return __FALSE;
      parser_ws.line++;
      break;

    case '\t': if(!parser -> tab()) return __FALSE; break;
    case ' ': if(!parser -> space()) return __FALSE; break;

    case (0xc2 - 0x100): is_prev = __TRUE; prev = c; break;

    default: if(!parser -> other(c)) return __FALSE;
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int scan(const char * path, FILE * file, SRT_PARSER * parser_)
{
  parser_ws.path = path;
  parser_ws.line = 1;
  parser = parser_;

  if(!parser -> start()) return __FALSE;

  is_prev = __FALSE;
  while(fread(&c, sizeof(char), 1, file) == 1)
  {
    if(is_prev) if(!scan_prev()) return parser -> stop(__FALSE);

    if(is_prev) is_prev = __FALSE;
    else if(!scan_main()) return parser -> stop(__FALSE);
  }

  if(is_prev) if(!parser -> other(prev)) return parser -> stop(__FALSE);

  return parser -> stop(__TRUE);
}
