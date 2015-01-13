CPP := g++

CFLAGS := -Wall -g

PYTHIA8_CFLAGS := $(shell pythia8-config --cxxflags)
PYTHIA8_LIBS   := $(shell pythia8-config --libs)
PYTHIA8_XML    := $(shell pythia8-config --xmldoc)

.PHONY: all clean

EXE := generate

all: $(EXE)

$(EXE): %: %.cc
	$(CPP) $(CFLAGS) $(PYTHIA8_CFLAGS) -DXMLDIR=\"$(PYTHIA8_XML)\" $^ -o $@ $(PYTHIA8_LIBS) -lboost_program_options

clean:
	rm -f $(EXE)

