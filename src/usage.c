
#include "usage.h"

#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"
#include "extension.h"

/*******************************************************************************
*******************************************************************************/

static struct option long_options[] =
{
  {"version", no_argument, NULL, 'v'},
  {"recursive", no_argument, NULL, 'r'},
  {"space", required_argument, NULL, 's'},
  {"extensions", required_argument, NULL, 'e'},
  {NULL, 0, NULL, 0}
};

static const char * short_options = "vrs:e:";

/*******************************************************************************
*******************************************************************************/

int dis_version = ERR, recursive = ERR, nb_space = 2;

/*******************************************************************************
*******************************************************************************/

#define USAGE_MSG \
  "Usage: %s ([option...]) [ file ... ] [ directory ... ]\n" \
  "Options:\n" \
  "     -v, --version                      Display version number\n" \
  "     -r, --recursive                    Recursive\n" \
  "     -s, --space                        Space nb for tab (%d)\n" \
  "     -e, --extensions                   Other extensions (ext1:...)\n" \
  "Extensions: "

void usage(const char * exe)
{
  fprintf(stderr, USAGE_MSG, exe, nb_space);
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

      case 's':
        nb_space = atoi(optarg);
        if(nb_space <= 0)
        {
          fprintf(stderr, "Invalid number : [%s]\n", optarg);
          return ERR;
        }
        break;

      case 'e': add_extensions(optarg); break;

      default: return ERR;
    }
  }
  return VAL;
}
