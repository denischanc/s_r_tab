
#include "usage.h"

#include <getopt.h>
#include <stdlib.h>
#include "common.h"
#include "extension.h"
#include "console.h"
#include "s_r_tab.h"

/*******************************************************************************
*******************************************************************************/

static struct option long_options[] =
{
  {"version", no_argument, NULL, 'v'},
  {"norecursion", no_argument, NULL, 'r'},
  {"space", required_argument, NULL, 's'},
  {"extensions", required_argument, NULL, 'e'},
  {"verbose", no_argument, NULL, 'V'},
  {NULL, 0, NULL, 0}
};

static const char * short_options = "vrs:e:V";

int dis_version = __FALSE, recursive = __TRUE, nb_space = 2,
  verbose = __FALSE;

/*******************************************************************************
*******************************************************************************/

#define USAGE_MSG \
  "Usage: %s ([option...]) [file...] [directory...]\n" \
  "Options:\n" \
  "     -v, --version                      Display version number\n" \
  "     -r, --norecursion                  No recursion\n" \
  "     -s, --space                        Space nb for tab (%d)\n" \
  "     -e, --extensions                   Other extensions (ext1:...)\n" \
  "     -V, --verbose                      Verbose\n" \
  "Extensions: "

void usage(const char * exe)
{
  print_version(TO_ERR);
  console_err(USAGE_MSG, exe, nb_space);
  display_extensions();
  console_err("\n");
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
      case 'v': dis_version = __TRUE; break;

      case 'r': recursive = __FALSE; break;

      case 's':
        nb_space = atoi(optarg);
        if(nb_space <= 0)
          return console_err("Invalid number : [%s]\n", optarg);
        break;

      case 'e': add_extensions(optarg); break;

      case 'V': verbose = __TRUE; break;

      default: return __FALSE;
    }
  }
  return __TRUE;
}
