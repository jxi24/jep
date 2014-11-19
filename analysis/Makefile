SHELL := /bin/bash

BIN := ../bin
MC  := mc

JEP_CFG := R7_CT10nnlo
JALG := antikt

P_MC := gluon quark higgs

.PHONY: all theory stat_cmp profiles $(P_MC) jets mc_unc_est

.SECONDARY: %.root %.jets

all: stat_cmp

theory: config/*.jepcfg
	mkdir -p theory
	$(foreach cfg,$^,$(BIN)/write_theory -c $(cfg) --dir=theory;)

out/%_theory_profiles.root: theory/%_$(JEP_CFG).jep
	@echo -e "\E[0;49;96mGenerating theoretical profile histograms for" $* "\E[0;0m"
	@$(BIN)/test_avg_theory_prof $^ config/pt_bins.txt $@

out/%_mc.jets: $(MC)/%.root
	@echo -e "\E[0;49;96mSelecting Monte Carlo jets\E[0;0m"
	@$(BIN)/jet_selection -i $< -o $@ -m leading-jet -r 0.7 --cluster-alg=$(JALG) -n 50000

out/%_mc_hists.root: out/%_mc.jets
	@echo -e "\E[0;49;96mGenerating MC statistics histograms\E[0;0m"
	@$(BIN)/test_stat3 $< config/$*.bins $@ \
		gluon:theory/gluon_$(JEP_CFG).jep \
		quark:theory/quark_$(JEP_CFG).jep \
		higgs:theory/higgs_$(JEP_CFG).jep

stat_cmp: out/*_mc_hists.root
	@echo -e "\E[0;49;96mPlotting statistics comparisons for\E[0;0m"
	@echo $^
	@mkdir -p out/stat_cmp
	@$(BIN)/draw_stat_cmp out/stat_cmp $^

profiles: out/*_mc_hists.root $(patsubst %,out/%_theory_profiles.root,$(P_MC))
	@echo -e "\E[0;49;96mPlotting profiles for\E[0;0m"
	@echo $^
	@mkdir -p out/stat_cmp
	$(BIN)/draw_profile_cmp out/avg_prof.pdf $^

mc_unc_est_%: out/%_mc.jets
	$(BIN)/mc_profile -c config/R7_mc_profile.jepcfg -i $< -o out/$*_mc_profile

$(P_MC): %: out/%_mc_hists.root

jets: $(patsubst %, out/%_mc.jets, $(P_MC))
