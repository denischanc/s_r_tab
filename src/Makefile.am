
bin_PROGRAMS = s_r_tab

s_r_tab_SOURCES = s_r_tab.c extension.c usage.c process.c search.c replace.c

noinst_HEADERS = common.h extension.h usage.h process.h search.h replace.h

install-exec-hook:
	ln -sf s_r_tab $(DESTDIR)$(bindir)/s_tab
	ln -sf s_r_tab $(DESTDIR)$(bindir)/r_tab
