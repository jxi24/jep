SHELL := /bin/bash
CPP := g++

DIRS := lib bin

CFLAGS := -Wall -O3 -I. -Itools -Isrc

ROOT_CFLAGS := $(shell root-config --cflags)
ROOT_LIBS   := $(shell root-config --libs)

FJ_LIBS := -lfastjet
#FJ_LIBS := $(shell /work/raida/isaacs21/fastjet/bin/fastjet-config --libs)
#FJ_CFLAGS := $(shell /work/raida/isaacs21/fastjet/bin/fastjet-config --cxxflags)

FC := gfortran
FFLAGS := -ffixed-line-length-none -fno-automatic -O2

CERN_LIB := -lmathlib
#CERN_LIB := -L/usr/lib64/cernlib/2006-g77/lib -lmathlib

#LHAPDF_INSTALL := /work/raida/isaacs21/LHAPDF/LHAPDF6
#LHAPDF_CONFIG  := $(LHAPDF_INSTALL)/bin/lhapdf-config
#LHAPDF_CFLAGS  := $(shell $(LHAPDF_CONFIG) --cppflags)
#LHAPDF_LIBS    := -Wl,-rpath=$(LHAPDF_INSTALL)/lib $(shell $(LHAPDF_CONFIG) --ldflags)
LHAPDF_CFLAGS  := $(shell lhapdf-config --cppflags)
LHAPDF_LIBS    := $(shell lhapdf-config --ldflags)

.PHONY: all test clean deepclean backup

all:  $(DIRS) \
			bin/test_jepfile_plot bin/test_avg_theory_prof \
			bin/test_single_event bin/test_profile \
			bin/test_stat3 \
			bin/jet_selection \
			bin/draw_together bin/draw_stat_cmp bin/draw_profile_cmp \
			bin/write_theory \
			bin/mc_profile \
			bin/test_binner bin/profile_uncert

test: bin/test_write bin/test_ascii bin/test_interp


# directories rule
$(DIRS):
	@mkdir -p $@

# jep object rule
lib/jep_common.o lib/jep_writer.o lib/jep_reader.o lib/jep_statistics.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

# object rule
lib/jets_file.o: lib/%.o: src/%.cc src/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/running_stat.o: lib/%.o: tools/%.cc tools/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/hist_wrap.o: lib/%.o: src/%.cc src/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

# fastjet interface
lib/jep_jet_alg.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(FJ_CFLAGS) -c $(filter %.cc,$^) -o $@

# shower graph
lib/shower_graph_boost.o: lib/shower_%.o: shower/%.cc shower/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/shower_graph_dot.o: lib/shower_%.o: shower/%.cc shower/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

# fortran object rule
lib/%.o: theory/%.f90
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(FC) $(FFLAGS) -c $< -o $@ -J lib

# main object rule
lib/test_write.o lib/test_ascii.o lib/test_interp.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/profile_uncert.o: lib/%.o: src/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/test_jepfile_plot.o lib/test_avg_theory_prof.o lib/test_stat3.o lib/draw_together.o lib/draw_stat_cmp.o lib/draw_profile_cmp.o lib/mc_profile.o: lib/%.o: src/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/test_single_event.o lib/test_profile.o lib/test_statistics.o lib/jet_selection.o: lib/%.o: src/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(FJ_CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/write_theory.o: lib/%.o: theory/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(LHAPDF_CFLAGS) -c $(filter %.cc,$^) -o $@

# executable rule
bin/test_write bin/test_ascii bin/test_interp bin/profile_uncert: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@

bin/test_binner: bin/%: test/%.cc tools/binner.h
	@echo -e "Compiling \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(ROOT_CFLAGS) $< -o $@ $(ROOT_LIBS)

bin/test_jepfile_plot bin/test_avg_theory_prof bin/draw_together: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS)

bin/mc_profile: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS) -lboost_program_options

bin/test_stat3 bin/draw_stat_cmp bin/draw_profile_cmp: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS) -lboost_regex

bin/test_single_event bin/test_profile bin/test_statistics: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) -Wl,--no-as-needed $(filter %.o,$^) -o $@ $(ROOT_LIBS) $(FJ_LIBS)

bin/jet_selection: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) -Wl,--no-as-needed $(filter %.o,$^) -o $@ $(ROOT_LIBS) $(FJ_LIBS) -lboost_program_options

bin/write_theory: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ -lgfortran $(CERN_LIB) $(LHAPDF_LIBS) -lboost_program_options

# OBJ dependencies
lib/jep_writer.o  : jep/common.h jep/exception.h
lib/jep_reader.o  : jep/common.h jep/exception.h
lib/jep_jet_alg.o : jep/exception.h
lib/jep_statistics.o : jep/common.h jep/exception.h jep/reader.h

# EXE_OBJ dependencies
lib/test_write.o  : jep/common.h jep/writer.h jep/reader.h
lib/test_interp.o : jep/common.h jep/reader.h
lib/test_ascii.o  : jep/common.h jep/reader.h
lib/test_jepfile_plot.o: jep/common.h jep/reader.h
lib/test_avg_theory_prof.o: jep/common.h jep/reader.h
lib/test_single_event.o: jep/jet_alg.h shower/graph_dot.h
lib/test_profile.o: jep/jet_alg.h
lib/write_theory.o: jep/common.h jep/writer.h
lib/test_statistics.o : jep/common.h jep/reader.h jep/statistics.h jep/jet_alg.h
lib/test_stat3.o  : jep/common.h jep/reader.h jep/stat2.h src/hist_wrap.h src/jets_file.h
lib/draw_stat_cmp.o: tools/propmap.h
lib/draw_profile_cmp.o: tools/propmap.h
lib/mc_profile.o  : jep/common.h jep/writer.h src/jets_file.h tools/running_stat.h
lib/profile_uncert.o: src/jets_file.h tools/running_stat.h tools/binner.h

# EXE dependencies
bin/test_write    : lib/jep_common.o lib/jep_writer.o lib/jep_reader.o
bin/test_interp   : lib/jep_common.o lib/jep_reader.o
bin/test_ascii    : lib/jep_common.o lib/jep_reader.o
bin/test_jepfile_plot: lib/jep_common.o lib/jep_reader.o
bin/test_avg_theory_prof: lib/jep_common.o lib/jep_reader.o
bin/test_single_event: lib/jep_jet_alg.o lib/shower_graph_dot.o
bin/test_profile  : lib/jep_jet_alg.o
bin/write_theory  : lib/jep_common.o lib/jep_writer.o lib/mod_constant.o lib/mod_terms.o
bin/test_statistics: lib/jep_common.o lib/jep_reader.o lib/jep_statistics.o lib/jep_jet_alg.o
bin/test_stat3    : lib/jep_common.o lib/jep_reader.o lib/hist_wrap.o lib/jets_file.o
bin/mc_profile    : lib/jep_common.o lib/jep_writer.o lib/jets_file.o lib/running_stat.o
bin/profile_uncert: lib/jets_file.o lib/running_stat.o

clean:
	@rm -rf bin $(addprefix lib/, $(shell ls lib | grep -v mod))
	@echo Removed bin
	ls lib

deepclean:
	rm -rf bin lib
