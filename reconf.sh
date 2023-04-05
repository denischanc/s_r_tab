#!/bin/sh

touch ./config.rpath

mkdir -p m4
autoreconf -i -v
