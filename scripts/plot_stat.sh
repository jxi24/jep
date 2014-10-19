#!/bin/bash

for stat in chi2_I chi2_d
do
  ./bin/draw_together gluon.root $stat"_1.pdf" $stat $stat"_gluon" $stat"_quark" $stat"_higgs"
  ./bin/draw_together gluon.root $stat"_2.pdf" $stat $stat"_higgs" $stat"_quark" $stat"_gluon"
done

for stat in like_I like_d
do
  ./bin/draw_together gluon.root $stat".pdf" $stat $stat"_gluon" $stat"_quark" $stat"_higgs"
done
