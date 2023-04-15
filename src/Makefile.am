
bin_PROGRAMS = s_r_tab

s_r_tab_SOURCES = \
  buffer.c \
  console.c \
  extension.c \
  parser.c \
  process.c \
  replace.c \
  scanner.c \
  search-simple.c \
  search-verbose.c \
  s_r_tab.c \
  usage.c

noinst_HEADERS = \
  buffer.h \
  common.h \
  console.h \
  extension.h \
  parser.h \
  process.h \
  replace.h \
  scanner.h \
  search-simple.h \
  search-verbose.h \
  s_r_tab.h \
  usage.h

install-exec-hook:
	ln -sf s_r_tab $(DESTDIR)$(bindir)/s_tab
	ln -sf s_r_tab $(DESTDIR)$(bindir)/r_tab
