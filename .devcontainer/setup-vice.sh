#!/bin/bash

cd /tmp
VICEVERSION=$(x128 --version | sed -e 's,x128 (VICE ,,g;s,),,g')
wget https://downloads.sourceforge.net/project/vice-emu/releases/vice-${VICEVERSION?}.tar.gz
mkdir -p ~/.local/share/vice
tar xf vice-${VICEVERSION?}.tar.gz -C ~/.local/share/vice/ vice-${VICEVERSION?}/data --strip-components=2
