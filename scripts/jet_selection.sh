#!/bin/bash

./bin/jet_selection -i ~/Public/jep_test_mc/gluon.root \
                    -o out/gluon.jets \
                    -m leading-jet \
                    -r 0.7
