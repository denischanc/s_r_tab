#ifndef __PARSER_H__
#define __PARSER_H__

#include "buffer.h"

typedef struct
{
  int (*start)();

  int (*tab)();
  int (*space)();
  int (*space_ub)();
  int (*lf)();
  int (*crlf)();
  int (*other)(char c);

  int (*stop)(int ok);
} SRT_PARSER;

typedef struct
{
  const char * path;
  int line;
} PARSER_WS;

extern PARSER_WS parser_ws;

int parser_cat_line(BUFFER * buffer);

#endif
