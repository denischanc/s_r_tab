#ifndef __BUFFER_H__
#define __BUFFER_H__

typedef struct
{
  char * buffer;
  int size, size_max;
} BUFFER;

void buffer_init(BUFFER * buffer);

int buffer_add(BUFFER * buffer, char c);
int buffer_add_string(BUFFER * buffer, const char * string);

int buffer_is_empty(BUFFER * buffer);

void buffer_clean(BUFFER * buffer);

void buffer_free(BUFFER * buffer);

#endif
