#!/bin/bash

cd $1
for file in `ls`; do
    grep "Fail link" $file | awk '{if (str != "") str = str ";"; str=str $3;} END {print str}'
done
