
#include "search-simple.h"

#include <stdio.h>
#include "common.h"
#include "buffer.h"
#include "console.h"

/*******************************************************************************
*******************************************************************************/

static int space_tab;

static BUFFER tab_msg_buf, endline_msg_buf, space_ub_msg_buf;
static int buffer_init_ok = __FALSE;

/*******************************************************************************
*******************************************************************************/

static int start()
{
  space_tab = 0;

  if(!buffer_init_ok)
  {
    buffer_init_ok = __TRUE;
    buffer_init(&tab_msg_buf);
    buffer_init(&endline_msg_buf);
    buffer_init(&space_ub_msg_buf);
  }
  else
  {
    buffer_clean(&tab_msg_buf);
    buffer_clean(&endline_msg_buf);
    buffer_clean(&space_ub_msg_buf);
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int this_cat_line(BUFFER * buffer)
{
  if(!buffer_is_empty(buffer) && !buffer_add_string(buffer, ", "))
    return __FALSE;
  else return parser_cat_line(buffer);
}

/*******************************************************************************
*******************************************************************************/

static int tab()
{
  space_tab++;
  return this_cat_line(&tab_msg_buf);
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
  return this_cat_line(&space_ub_msg_buf);
}

/*******************************************************************************
*******************************************************************************/

static int lf()
{
  if((space_tab != 0) && !this_cat_line(&endline_msg_buf)) return __FALSE;
  space_tab = 0;
  return __TRUE;
}

static int crlf()
{
  return lf();
}

/*******************************************************************************
*******************************************************************************/

static int other(char c)
{
  space_tab = 0;
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int stop(int ok)
{
  if(!ok || !lf()) return __FALSE;

  if(!buffer_is_empty(&tab_msg_buf) || !buffer_is_empty(&endline_msg_buf) ||
     !buffer_is_empty(&space_ub_msg_buf))
  {
    console_out("-----[%s]-----\n", parser_ws.path);
    if(!buffer_is_empty(&tab_msg_buf))
      console_out("\tTab: %s\n", tab_msg_buf.buffer);
    if(!buffer_is_empty(&endline_msg_buf))
      console_out("\tEnd line: %s\n", endline_msg_buf.buffer);
    if(!buffer_is_empty(&space_ub_msg_buf))
      console_out("\tUnbreakable space: %s\n", space_ub_msg_buf.buffer);
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

SRT_PARSER search_simple_parser =
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
