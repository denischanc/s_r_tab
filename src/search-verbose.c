
#include "search-verbose.h"

#include <stdio.h>
#include "common.h"
#include "buffer.h"
#include "replace.h"
#include "console.h"

/*******************************************************************************
*******************************************************************************/

static BUFFER line_buf, space_buf, tab_buf;
static int buffer_init_ok = __FALSE;

static int nb_tab;

/*******************************************************************************
*******************************************************************************/

static void start()
{
  nb_tab = 0;

  if(!buffer_init_ok)
  {
    buffer_init_ok = __TRUE;
    buffer_init(&line_buf);
    buffer_init(&space_buf);
    buffer_init(&tab_buf);
  }
  else
  {
    buffer_clean(&line_buf);
    buffer_clean(&space_buf);
    buffer_clean(&tab_buf);
  }
}

/*******************************************************************************
*******************************************************************************/

static int tab()
{
  if(!buffer_add_string(&tab_buf, "\e[44m")) return __FALSE;
  for(int i = 0; i < nb_space; i++)
  {
    if(!buffer_add(&tab_buf, ' ')) return __FALSE;
    if(!buffer_add(&space_buf, ' ')) return __FALSE;
  }
  if(!buffer_add_string(&tab_buf, "\e[0m")) return __FALSE;

  nb_tab++;

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int space()
{
  return buffer_add(&tab_buf, ' ') && buffer_add(&space_buf, ' ');
}

/*******************************************************************************
*******************************************************************************/

static int cr(const char * name, int line)
{
  if(!buffer_is_empty(&space_buf) || (nb_tab > 0))
  {
    console_out("[%s:%d] ", name, line);
    if(!buffer_is_empty(&line_buf)) console_out(line_buf.buffer);
    if(!buffer_is_empty(&space_buf))
      console_out("\e[41m%s\e[0m", space_buf.buffer);
    console_out("\n");
  }

  nb_tab = 0;

  buffer_clean(&tab_buf);
  buffer_clean(&space_buf);
  buffer_clean(&line_buf);

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int other(char c)
{
  if(!buffer_add_string(&line_buf, tab_buf.buffer)) return __FALSE;

  buffer_clean(&tab_buf);
  buffer_clean(&space_buf);

  return buffer_add(&line_buf, c);
}

/*******************************************************************************
*******************************************************************************/

static int stop(const char * name, int line)
{
  return cr(name, line);
}

/*******************************************************************************
*******************************************************************************/

SEARCH_SCANNER scanner_verbose =
{
  start,
  tab,
  space,
  cr,
  other,
  stop
};
