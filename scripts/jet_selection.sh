#!/bin/bash

#data_dir=../data
data_dir=~/Public/jep_test_mc

./bin/jet_selection \
  -i $data_dir/$1.root \
  -o out/$1.jets \
  -m leading-jet \
  -r 0.7

