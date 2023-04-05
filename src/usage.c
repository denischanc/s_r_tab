
#include "usage.h"

#include <getopt.h>
#include <stdlib.h>
#include "common.h"
#include "extension.h"

/*******************************************************************************
*******************************************************************************/

#define USAGE_MSG \
  "Usage: %s ([option...]) [ file ... ] [ directory ... ]\n" \
  "Options:\n" \
  "     -v, --version                      Display version number\n" \
  "     -r, --recursive                    Recursive\n" \
  "Extensions: "

/*******************************************************************************
*******************************************************************************/

static struct option long_options[] =
{
  {"version", no_argument, NULL, 'v'},
  {"recursive", no_argument, NULL, 'r'},
  {NULL, 0, NULL, 0}
};

static const char * short_options = "vr";

/*******************************************************************************
*******************************************************************************/

int dis_version = ERR, recursive = ERR;

/*******************************************************************************
*******************************************************************************/

void usage(const char * exe)
{
  fprintf(stderr, USAGE_MSG, exe);
  display_extensions(stderr);
  fprintf(stderr, "\n");
}

/*******************************************************************************
*******************************************************************************/

int parse_args(int argc, char * const argv[])
{
  int v;
  while((v = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
  {
    switch(v)
    {
      case 'v': dis_version = VAL; break;

      case 'r': recursive = VAL; break;

      default: return ERR;
    }
  }
  return VAL;
}
