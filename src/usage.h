#ifndef __USAGE_H__
#define __USAGE_H__

extern int dis_version, recursive, nb_space, verbose;

void usage(const char * exe);

int parse_args(int argc, char * const argv[]);

#endif
