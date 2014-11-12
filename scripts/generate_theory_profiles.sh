#!/bin/bash

# Create histograms of theoretical jet profiles
# for pt bins
# averaged within subcones
# from jep files

./bin/test_avg_theory_prof \
  data/theory/"$1"_R0.7.jep config/pt_bins.txt out/"$1"_theory_profiles.root
