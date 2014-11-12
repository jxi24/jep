#!/bin/bash

./bin/test_stat3 \
  out/"$1"_mc.jets config/$1.bins out/"$1"_mc_hists.root \
  gluon:data/theory/gluon_R0.7.jep \
  quark:data/theory/quark_R0.7.jep \
  higgs:data/theory/higgs_R0.7.jep
