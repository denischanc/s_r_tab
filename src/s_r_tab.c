
#include "s_r_tab.h"

#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "common.h"
#include "usage.h"
#include "process.h"
#include "config.h"

/*******************************************************************************
*******************************************************************************/

#define EXE_SEARCH "s_tab"
#define EXE_REPLACE "r_tab"

/*******************************************************************************
*******************************************************************************/

int dis_version = __FALSE;

/*******************************************************************************
*******************************************************************************/

int main(int argc, char * argv[])
{
  int action = __UNDEFINE;
  char * basename_ = (char *)basename(argv[0]);

  if(!strcmp(basename_, EXE_SEARCH)) action = __SEARCH;
  else if(!strcmp(basename_, EXE_REPLACE)) action = __REPLACE;
  else
  {
    fprintf(stderr, "Bad executable name : [%s]\n", basename_);
    return EXIT_FAILURE;
  }

  if((argc == 1) || !parse_args(argc, argv))
  {
    usage(basename_);
    return EXIT_FAILURE;
  }

  if(dis_version) fprintf(stdout, "Version: " PACKAGE_VERSION "\n");

  return process_file_dir(argc, argv, action) ? EXIT_SUCCESS : EXIT_FAILURE;
}
