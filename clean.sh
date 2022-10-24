#!/bin/bash

rm -rf autom4te.cache
rm -rf build
rm -rf build-aux
rm -rf m4

rm -rf docker/vsa-docker-manager
rm -rf aclocal.m4
rm -rf autoscan.log
rm -rf config.h
rm -rf config.h.in
rm -rf config.log
rm -rf config.status
rm -rf configure
rm -rf configure.scan
rm -rf Makefile
rm -rf Makefile.in
rm -rf stamp-h1
rm -rf *.tar.gz
rm -rf *~

find -type d -name ".deps" -exec rm -rf {} \;
find -type f -name "vsa" -exec rm -rf {} \;
find -type f -name "*.pc" -exec rm -rf {} \;
find -type f -name "*.o" -exec rm -rf {} \;
find -type f -name "*.a" -exec rm -rf {} \;
find -type f -name "config.h.in" -exec rm -rf {} \;
find -type f -name "Makefile" -exec rm -rf {} \;
find -type f -name "Makefile.in" -exec rm -rf {} \;
find -type f -name "*~" -exec rm -rf {} \;
