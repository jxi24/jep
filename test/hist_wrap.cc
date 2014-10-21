#include "hist_wrap.h"

#include <iostream>
#include <fstream>

#include <TH1.h>

using namespace std;

// binning ************************************************

hist::binning::binning(): nbins(100), min(0.), max(100.) { }
hist::binning::~binning() { }

// hist ***************************************************

hist::hist(): b(), underflow(0,0.), overflow(0,0.), h(NULL) { }
hist::~hist() { } // doesn't delete TH1F* because TFile will do that

hist::hist(const string& name,const string& title)
: b(binnings[name]), underflow(0,b.min), overflow(0,b.max),
  h( new TH1F(name.c_str(),title.c_str(),b.nbins,b.min,b.max) )
{
  all.push_back(this);
}
void hist::Fill(double x) {
  h->Fill(x);
  if (x>=b.max) {
    ++overflow.first;
    if (x>overflow.second) overflow.second = x;
  }
  else if (x<b.min) {
    ++underflow.first;
    if (x<underflow.second) underflow.second = x;
  }
}

void hist::read_binnings(const char* filename) {
  ifstream binsfile(filename);

  string hname;
  binning b;
  while ( binsfile >> hname ) {
    binsfile >> b.nbins;
    binsfile >> b.min;
    binsfile >> b.max;

    binnings[hname] = b;
  }
}

void hist::finish() {
  for (vector<const hist*>::iterator it=all.begin(), end=all.end(); it<end; ++it) {
    const hist* h = *it;
    if (h->underflow.first) {
      cout << "Underflow in " << h->h->GetName()
           << ": N="<<h->underflow.first << " min="<<h->underflow.second << endl;
    }
    if (h->overflow.first) {
      cout << "Overflow in " << h->h->GetName()
           << ": N="<<h->overflow.first << " max="<<h->overflow.second << endl;
    }
    delete h;
  }
}

map<string,hist::binning> hist::binnings;
vector<const hist*> hist::all;
