#!/bin/sh

aclocal -I config;
libtoolize --force --copy;
autoreconf -vfi;

