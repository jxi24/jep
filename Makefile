SHELL := /bin/bash
CPP := g++

DIRS := lib bin

BAK = BAK/backup`date +%s`.tar.gz

CFLAGS := -Wall -O3 -I.
CGFLAGS := -Wall -g -I.

ROOT_CFLAGS := $(shell root-config --cflags)
ROOT_LIBS   := $(shell root-config --libs)

FJ_LIBS := -lfastjet
#FJ_LIBS := $(shell /work/raida/isaacs21/fastjet/bin/fastjet-config --libs)
#FJ_CFLAGS := $(shell /work/raida/isaacs21/fastjet/bin/fastjet-config --cxxflags)

FC := gfortran
FFLAGS := -ffixed-line-length-none -fno-automatic -O2

CERN_LIB := -lmathlib
#CERN_LIB := -L/usr/lib64/cernlib/2006-g77/lib -lmathlib

LHAPDF_INSTALL := /work/raida/isaacs21/LHAPDF/LHAPDF6
LHAPDF_CONFIG  := $(LHAPDF_INSTALL)/bin/lhapdf-config
LHAPDF_FLAGS   := $(shell $(LHAPDF_CONFIG) --cppflags)
LHAPDF_LIBS    := -Wl,-rpath=$(LHAPDF_INSTALL)/lib $(shell $(LHAPDF_CONFIG) --ldflags)

.PHONY: all clean deepclean backup

EXE := bin/test_write bin/test_ascii bin/test_interp \
       bin/test_jepfile_plot bin/test_single_event bin/test_profile \
       bin/test_statistics bin/test_stat2 bin/test_stat3 \
       bin/jet_selection \
       bin/draw_together bin/draw_stat_cmp \
#       bin/write_data

all: $(DIRS) $(EXE)

# directories rule
$(DIRS):
	@mkdir -p $@

# jep object rule
lib/jep_common.o lib/jep_writer.o lib/jep_reader.o lib/jep_statistics.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

# test object rule
lib/test_jets_file.o: lib/test_%.o: test/%.cc test/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CGFLAGS) -Itest -c $(filter %.cc,$^) -o $@

lib/test_hist_wrap.o: lib/test_%.o: test/%.cc test/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CGFLAGS) $(ROOT_CFLAGS) -Itest -c $(filter %.cc,$^) -o $@

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
lib/%.o: write/%.f90
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(FC) $(FFLAGS) -c $< -o $@ -J lib

# main object rule
lib/test_write.o lib/test_ascii.o lib/test_interp.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

lib/test_jepfile_plot.o lib/test_stat2.o lib/test_stat3.o lib/draw_together.o lib/draw_stat_cmp.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CGFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/test_single_event.o lib/test_profile.o lib/test_statistics.o lib/jet_selection.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(FJ_CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/write_data.o: lib/%.o: write/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(LHAPDF_FLAGS) -c $(filter %.cc,$^) -o $@

# executable rule
bin/test_write bin/test_ascii bin/test_interp: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@

bin/test_jepfile_plot bin/test_stat2 bin/draw_together: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS)

bin/test_stat3 bin/draw_stat_cmp: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS) -lboost_regex

bin/test_single_event bin/test_profile bin/test_statistics: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) -Wl,--no-as-needed $(filter %.o,$^) -o $@ $(ROOT_LIBS) $(FJ_LIBS)

bin/jet_selection: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) -Wl,--no-as-needed $(filter %.o,$^) -o $@ $(ROOT_LIBS) $(FJ_LIBS) -lboost_program_options

bin/write_data: bin/%: lib/%.o
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
lib/test_single_event.o: jep/jet_alg.h shower/graph_dot.h
lib/test_profile.o: jep/jet_alg.h
lib/write_data.o  : jep/common.h jep/writer.h
lib/test_statistics.o : jep/common.h jep/reader.h jep/statistics.h jep/jet_alg.h
lib/test_stat2.o  : jep/common.h jep/reader.h
lib/test_stat3.o  : jep/common.h jep/reader.h jep/stat2.h test/hist_wrap.h test/jets_file.h
lib/draw_stat_cmp.o: test/propmap.h

# EXE dependencies
bin/test_write    : lib/jep_common.o lib/jep_writer.o lib/jep_reader.o
bin/test_interp   : lib/jep_common.o lib/jep_reader.o
bin/test_ascii    : lib/jep_common.o lib/jep_reader.o
bin/test_jepfile_plot: lib/jep_common.o lib/jep_reader.o
bin/test_single_event: lib/jep_jet_alg.o lib/shower_graph_dot.o
bin/test_profile  : lib/jep_jet_alg.o
bin/write_data    : lib/jep_common.o lib/jep_writer.o lib/mod_constant.o lib/mod_terms.o
bin/test_statistics: lib/jep_common.o lib/jep_reader.o lib/jep_statistics.o lib/jep_jet_alg.o
bin/test_stat2    : lib/jep_common.o lib/jep_reader.o
bin/test_stat3    : lib/jep_common.o lib/jep_reader.o lib/test_hist_wrap.o lib/test_jets_file.o

clean:
	@rm -rf bin $(addprefix lib/, $(shell ls lib | grep -v mod))
	@echo Removed bin
	ls lib

deepclean:
	rm -rf bin lib

backup:
	@echo -e "Creating \E[0;49;93m"$(BAK)"\E[0;0m"
	@mkdir -p BAK
	@tar cvzfh $(BAK) README Makefile jep write shower test scripts cfg
