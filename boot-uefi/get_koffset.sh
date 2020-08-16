#!/bin/bash

line="$(readelf $1 -SW | grep kernel_text)"

regex='.*.kernel_text\s+\w+\s+[a-f0-9]+\s+([a-f0-9]+).*'
sedcmd='s/'+$regex+'/\1/'
offset="$(echo $line | sed -E 's/.*.kernel_text\s+\w+\s+[a-f0-9]+\s+([a-f0-9]+).*/\1/')"

cflag="-DKERNEL_START_OFFSET=0x$offset"

echo $cflag
