SHELL := /bin/bash
CPP := g++

DIRS := lib bin

BAK = BAK/backup`date +%s`.tar.gz

CFLAGS := -Wall -g -I.

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

.PHONY: all clean backup

EXE := bin/test_write bin/test_ascii bin/test_interp \
       bin/test_plot bin/test_profile \
       bin/write_data

all: $(DIRS) $(EXE)

# directories rule
$(DIRS):
	@mkdir -p $@

# jep object rule
lib/jep_common.o lib/jep_writer.o lib/jep_reader.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@

# fastjet interface
lib/jep_jet_alg.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(FJ_CFLAGS) -c $(filter %.cc,$^) -o $@

# shower graph
lib/jep_shower_graph_boost.o: lib/jep_%.o: jep/%.cc jep/%.h
	@echo -e "Compiling \E[0;49;96m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) -c $(filter %.cc,$^) -o $@ -lboost_graph

lib/jep_shower_graph_dot.o: lib/jep_%.o: jep/%.cc jep/%.h
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

lib/test_plot.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/test_profile.o: lib/%.o: test/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(FJ_CFLAGS) $(ROOT_CFLAGS) -c $(filter %.cc,$^) -o $@

lib/write_data.o: lib/%.o: write/%.cc
	@echo -e "Compiling \E[0;49;94m"$@"\E[0;0m ... "
	@$(CPP) $(CFLAGS) $(LHAPDF_FLAGS) -c $(filter %.cc,$^) -o $@

# test executable rule
bin/test_write bin/test_ascii bin/test_interp: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@

bin/test_plot: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ $(ROOT_LIBS)

bin/test_profile: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) -Wl,--no-as-needed $(filter %.o,$^) -o $@ $(ROOT_LIBS) $(FJ_LIBS)

bin/write_data: bin/%: lib/%.o
	@echo -e "Linking \E[0;49;92m"$@"\E[0;0m ... "
	@$(CPP) $(filter %.o,$^) -o $@ -lgfortran $(CERN_LIB) $(LHAPDF_LIBS)

# OBJ dependencies
lib/jep_writer.o  : jep/common.h jep/exception.h
lib/jep_reader.o  : jep/common.h jep/exception.h
lib/jep_jet_alg.o : jep/exception.h

# EXE_OBJ dependencies
lib/test_write.o  : jep/common.h jep/writer.h jep/reader.h
lib/test_interp.o : jep/common.h jep/reader.h
lib/test_ascii.o  : jep/common.h jep/reader.h
lib/test_profile.o: jep/jet_alg.h jep/shower_graph_dot.h
lib/write_data.o  : jep/common.h jep/writer.h

# EXE dependencies
bin/test_write    : lib/jep_common.o lib/jep_writer.o lib/jep_reader.o
bin/test_interp   : lib/jep_common.o lib/jep_reader.o
bin/test_ascii    : lib/jep_common.o lib/jep_reader.o
bin/test_plot     : lib/jep_common.o lib/jep_reader.o
bin/test_profile  : lib/jep_jet_alg.o lib/jep_shower_graph_dot.o
bin/write_data    : lib/jep_common.o lib/jep_writer.o lib/mod_constant.o lib/mod_terms.o

clean:
	rm -rf bin lib/jep_* lib/test_* lib/write_*

deepclean:
	rm -rf bin lib dat test.*

backup:
	@echo -e "Creating \E[0;49;93m"$(BAK)"\E[0;0m"
	@mkdir -p BAK
	@tar cvzfh $(BAK) README Makefile jep test write scripts
