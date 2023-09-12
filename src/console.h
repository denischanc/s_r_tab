#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define TO_OUT 0
#define TO_ERR 1

void console(int to, const char * msg, ...);
void console_out(const char * msg, ...);
void console_debug(const char * msg, ...);
int console_err(const char * msg, ...);

int console_errno(const char * msg, ...);

#endif
