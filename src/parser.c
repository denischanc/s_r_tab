
#include "parser.h"

#include <stdio.h>

/*******************************************************************************
*******************************************************************************/

PARSER_WS parser_ws;

/*******************************************************************************
*******************************************************************************/

int parser_cat_line(BUFFER * buffer)
{
  static char line_string[12];
  snprintf(line_string, sizeof(line_string), "%d", parser_ws.line);
  return buffer_add_string(buffer, line_string);
}
