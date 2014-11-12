#!/bin/bash

# Create theory jep files

mkdir -p data
mkdir -p data/theory

if [ $# -eq 0 ]
then
  ./bin/write_data -c config/R0.7.cfg
else
  ./bin/write_data -c config/R0.7.cfg --pdf="$1"
fi

