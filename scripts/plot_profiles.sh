#!/bin/bash

# Draw jet profiles
# Overlay theory and Monte Carlo

./bin/draw_profile_cmp out/avg_prof.pdf \
  out/*_mc_hists.root \
  out/*_theory_profiles.root
