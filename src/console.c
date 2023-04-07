
#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

static void __console(int to, const char * msg, va_list ap)
{
  vfprintf((to == TO_ERR) ? stderr : stdout, msg, ap);
}

/*******************************************************************************
*******************************************************************************/

void console(int to, const char * msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  __console(to, msg, ap);
  va_end(ap);
}

/*******************************************************************************
*******************************************************************************/

void console_out(const char * msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  __console(TO_OUT, msg, ap);
  va_end(ap);
}

/*******************************************************************************
*******************************************************************************/

int console_err(const char * msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  __console(TO_ERR, msg, ap);
  va_end(ap);

  return __FALSE;
}

/*******************************************************************************
*******************************************************************************/

int console_errno(const char * msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  __console(TO_ERR, msg, ap);
  va_end(ap);

  return console_err(" : %s.\n", strerror(errno));
}
