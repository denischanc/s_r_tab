
#include "search-simple.h"

#include <stdio.h>
#include "common.h"
#include "buffer.h"

/*******************************************************************************
*******************************************************************************/

static int space_tab;

static BUFFER tab_msg_buf, endline_msg_buf;
static int buffer_init_ok = __FALSE;

/*******************************************************************************
*******************************************************************************/

static void start()
{
  space_tab = 0;

  if(!buffer_init_ok)
  {
    buffer_init_ok = __TRUE;
    buffer_init(&tab_msg_buf);
    buffer_init(&endline_msg_buf);
  }
  else
  {
    buffer_clean(&tab_msg_buf);
    buffer_clean(&endline_msg_buf);
  }
}

/*******************************************************************************
*******************************************************************************/

static int this_cat_line(BUFFER * buffer)
{
  if(!buffer_is_empty(buffer) && !buffer_add_string(buffer, ", "))
    return __FALSE;
  else return search_cat_line(buffer);
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

static int cr(const char * name, int line)
{
  if((space_tab != 0) && !this_cat_line(&endline_msg_buf)) return __FALSE;
  space_tab = 0;
  return __TRUE;
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

static int stop(const char * name, int line)
{
  if(!cr(name, line)) return __FALSE;

  if(!buffer_is_empty(&tab_msg_buf) || !buffer_is_empty(&endline_msg_buf))
  {
    printf("-----[%s]-----\n", name);
    if(!buffer_is_empty(&tab_msg_buf))
      printf("\tTab: %s\n", tab_msg_buf.buffer);
    if(!buffer_is_empty(&endline_msg_buf))
      printf("\tEnd line: %s\n", endline_msg_buf.buffer);
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

SEARCH_SCANNER scanner_simple =
{
  start,
  tab,
  space,
  cr,
  other,
  stop
};
