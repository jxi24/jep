#!/bin/bash

#./bin/test_profile $1 ~/Public/test_higgs.root
./bin/test_single_event $1 ../data/gluon.root
dot -Tsvg graph.gv -o graph.svg
