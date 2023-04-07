
#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

/*******************************************************************************
*******************************************************************************/

#define INC 1024

/*******************************************************************************
*******************************************************************************/

void buffer_init(BUFFER * buffer)
{
  buffer -> buffer = NULL;
  buffer -> size = 0;
  buffer -> size_max = 0;
}

/*******************************************************************************
*******************************************************************************/

int buffer_add(BUFFER * buffer, char c)
{
  if(buffer -> size >= buffer -> size_max)
  {
    buffer -> size_max += INC;
    buffer -> buffer = (char *)realloc(buffer -> buffer, buffer -> size_max);

    if(buffer -> buffer == NULL)
    {
      perror(__FUNCTION__);
      return __FALSE;
    }
  }

  if(buffer -> size == 0) buffer -> size = 1;

  buffer -> buffer[buffer -> size - 1] = c;
  buffer -> buffer[buffer -> size++] = 0;

  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int buffer_add_string(BUFFER * buffer, const char * string)
{
  if(string != NULL) for(const char * cur = string; *cur != 0; cur++)
  {
    if(!buffer_add(buffer, *cur)) return __FALSE;
  }
  return __TRUE;
}

/*******************************************************************************
*******************************************************************************/

int buffer_is_empty(BUFFER * buffer)
{
  return (buffer -> size <= 1);
}

/*******************************************************************************
*******************************************************************************/

void buffer_clean(BUFFER * buffer)
{
  if(buffer -> size != 0)
  {
    buffer -> size = 1;
    buffer -> buffer[0] = 0;
  }
}

/*******************************************************************************
*******************************************************************************/

void buffer_free(BUFFER * buffer)
{
  free(buffer -> buffer);

  buffer_init(buffer);
}
