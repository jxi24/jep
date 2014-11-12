#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

#include <TFile.h>
#include <TH1.h>

#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

struct hist {
  jep::val_t Emin, Emax;
  TH1* h;
  hist(const pair<jep::val_t,jep::val_t>& E, const jep::header& head)
  : Emin(E.first), Emax(E.second),
    h( new TH1F(Form("h_E_%.0f_%.0f",Emin,Emax),"",head.r_num,head.r_min-head.r_step,head.r_max()) )
  {
    h->SetTitle((string(h->GetName())+";#psi;r").c_str());
  }
};

int main(int argc, char** argv)
{
  if (argc!=4) {
    cout << "Usage: " << argv[0] << " data.jep bins.txt out.root" << endl;
    return 1;
  }

  jep::reader* jf = new jep::reader(argv[1]);
  const jep::header& head = jf->get_header();
  cout << "Reading " << argv[1] << ": " << jep::pid_name(head.pid) << endl;

  ifstream fbins(argv[2]);

  TFile* rf = new TFile(argv[3],"recreate");

  pair<jep::val_t,jep::val_t> E;
  fbins >> E.first;
  while (fbins >> E.second) {
    hist h(E,head);
    E.first = E.second;

    vector<jep::val_t> psi = jf->psi_avg(10,E.first,E.second);
    for (jep::num_t i=0;i<head.r_num;++i) {
      h.h->SetBinContent(i+1,psi[i]);
    }
  }

  fbins.close();
  rf->Write();
  rf->Close();
  delete rf;
  delete jf;

  return 0;
}
