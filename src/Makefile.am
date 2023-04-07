
bin_PROGRAMS = s_r_tab

s_r_tab_SOURCES = \
  s_r_tab.c \
  extension.c \
  usage.c \
  process.c \
  search.c \
  search-simple.c \
  search-verbose.c \
  replace.c \
  buffer.c \
  console.c

noinst_HEADERS = \
  common.h \
  s_r_tab.h \
  extension.h \
  usage.h \
  process.h \
  search.h \
  search-simple.h \
  search-verbose.h \
  replace.h \
  buffer.h \
  console.h

install-exec-hook:
	ln -sf s_r_tab $(DESTDIR)$(bindir)/s_tab
	ln -sf s_r_tab $(DESTDIR)$(bindir)/r_tab
