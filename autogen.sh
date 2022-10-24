#!/bin/bash

./clean.sh
autoreconf -i
./configure $*
make
