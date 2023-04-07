#!/bin/sh

TO_RM="aclocal.m4 autom4te.cache _build compile config.guess config.rpath"
TO_RM+=" config.sub configure depcomp _install install-sh m4 missing"
TO_RM+=" $(find . -name Makefile.in | tr '\n' ' ')"
TO_RM+=" src/config.h.in"

CMD="rm -rf $TO_RM"
echo "[[$CMD]]"
eval "$CMD"
