
#include "search-verbose.h"

#include <stdio.h>
#include "common.h"
#include "buffer.h"
#include "console.h"
#include "usage.h"

/*******************************************************************************
*******************************************************************************/

static BUFFER line_buf, notmod_buf, mod_buf;
static int buffer_init_ok = __FALSE;

static int is_mod;

/*******************************************************************************
*******************************************************************************/

static int start()
{
  is_mod = __FALSE;

  if(!buffer_init_ok)
  {
    buffer_init_ok = __TRUE;
    buffer_init(&line_buf);
    buffer_init(&notmod_buf);
    buffer_init(&mod_buf);
  }
  else
  {
    buffer_clean(&line_buf);
    buffer_clean(&notmod_buf);
    buffer_clean(&mod_buf);
  }

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int tab()
{
  if(!buffer_add_string(&mod_buf, "\e[44m")) return __FALSE;
  for(int i = 0; i < nb_space; i++)
  {
    if(!buffer_add(&mod_buf, ' ')) return __FALSE;
    if(!buffer_add(&notmod_buf, ' ')) return __FALSE;
  }
  if(!buffer_add_string(&mod_buf, "\e[0m")) return __FALSE;

  is_mod = __TRUE;

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int space()
{
  return buffer_add(&mod_buf, ' ') && buffer_add(&notmod_buf, ' ');
}

/*******************************************************************************
*******************************************************************************/

static int space_ub()
{
  if(!buffer_add_string(&mod_buf, "\e[44m")) return __FALSE;
  if(!buffer_add(&mod_buf, ' ')) return __FALSE;
  if(!buffer_add(&notmod_buf, ' ')) return __FALSE;
  if(!buffer_add_string(&mod_buf, "\e[0m")) return __FALSE;

  is_mod = __TRUE;

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int cr()
{
  if(!buffer_is_empty(&notmod_buf) || is_mod)
  {
    console_out("[%s:%d] ", parser_ws.path, parser_ws.line);
    if(!buffer_is_empty(&line_buf)) console_out("%s", line_buf.buffer);
    if(!buffer_is_empty(&notmod_buf))
      console_out("\e[41m%s\e[0m", notmod_buf.buffer);
    console_out("\n");
  }

  is_mod = __FALSE;

  buffer_clean(&mod_buf);
  buffer_clean(&notmod_buf);
  buffer_clean(&line_buf);

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

static int other(char c)
{
  if(!buffer_add_string(&line_buf, mod_buf.buffer)) return __FALSE;

  buffer_clean(&mod_buf);
  buffer_clean(&notmod_buf);

  return buffer_add(&line_buf, c);
}

/*******************************************************************************
*******************************************************************************/

static int stop(int ok)
{
  return !ok ? __FALSE : cr();
}

/*******************************************************************************
*******************************************************************************/

SRT_PARSER search_verbose_parser =
{
  start,
  tab,
  space,
  space_ub,
  cr,
  other,
  stop
};
