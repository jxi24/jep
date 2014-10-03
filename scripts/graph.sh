#!/bin/bash

#./bin/test_profile $1 ~/Public/test_higgs.root
./bin/test_profile $1 ../test_higgs.root
dot -Tsvg jet.gv -o jet.svg
