#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cctype>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#include "stdhep.h"
#include "timed_counter.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace fastjet;
using namespace std;
namespace po = boost::program_options;

struct constit {
  double r, Et;
  short pid;
  constit(double r, double Et, short pid)
  : r(r), Et(Et), pid(pid) { }
  // for sorting by r is ascending order
  bool operator<(const constit& other) const {
    return ( r < other.r );
  }
};

ofstream& operator<<(ofstream& dat, const PseudoJet& jet)
{
  static const streamsize
    dbl_size = sizeof(double),
    srt_size = sizeof(short),
    szt_size = sizeof(size_t);

  // write jet Et
  double jet_Et = jet.Et();
  dat.write((char*)&jet_Et, dbl_size);

  // get jet constituents ( returns object, not reference )
  vector<constit> constituents;
  {
    vector<PseudoJet> c( jet.constituents() );
    for (vector<PseudoJet>::iterator it=c.begin(), end=c.end();
         it<end; ++it)
    {
      constituents.push_back( constit(
        it->delta_R(jet), it->Et(), it->user_index()
      ) );
    }
  }

  // sort constituents by delta_R in ascending order
  sort(constituents.begin(), constituents.end());

  const size_t size = constituents.size();
  dat.write((char*)&size, szt_size); // write number of constituents

  // write constituents
  if (size>0) for (vector<constit>::iterator it=constituents.begin(),
    end=constituents.end(); it<end; ++it)
  {
    dat.write((char*)&it->r,   dbl_size);
    dat.write((char*)&it->Et,  dbl_size);
    dat.write((char*)&it->pid, srt_size);
  } else {
    cerr << "Jet has no constituents" << endl;
  }

  return dat;
}

enum method_t { leading_jet };
istream& operator>>(istream& in, method_t& m) {
  string str;
  in >> str;
  if (!str.compare("leading")) m = leading_jet;
  else throw runtime_error("Undefined jet selection method: "+str);
  return in;
}

struct cluster_alg { fastjet::JetAlgorithm alg; float r; };
istream& operator>>(istream& in, cluster_alg& alg) {
  string str;
  in >> str;
  string::iterator it = --str.end();
  while (isdigit(*it)) --it;
  ++it;
  string name;
  transform(str.begin(), it, back_inserter(name), ::tolower);
  if (!name.compare("antikt")) alg.alg = antikt_algorithm;
  else if (!name.compare("kt")) alg.alg = kt_algorithm;
  else if (!name.compare("cambridge")) alg.alg = cambridge_algorithm;
  else throw runtime_error("Undefined jet clustering algorithm: "+name);
  alg.r = atof( string(it,str.end()).c_str() )/10.;
  return in;
}

int main(int argc, char *argv[])
{
  // START OPTIONS **************************************************
  string input_file, output_file;
  method_t method;
  Long64_t num_ent=0;

  cluster_alg jet_alg;
  jet_alg.alg = antikt_algorithm;
  jet_alg.r = 0.4;

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("input,i", po::value<string>(&input_file)->required(),
       "input showered event file in stdhep root format")
      ("output,o", po::value<string>(&output_file)->required(),
       "output binary file")
      ("method,m", po::value<method_t>(&method)->default_value(leading_jet,"leading"),
       "jet selection method")
      ("cluster,c", po::value<cluster_alg>(&jet_alg),
       "jet clustering algorithm:\ne.g. antikt4 (default), kt6")
      ("num-events,n", po::value<Long64_t>(&num_ent),
       "read only first n events in root file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_opt), vm);
    if (argc==1 || vm.count("help")) {
      cout << all_opt << endl;
      cout << "Selection methods:" << endl;
      cout << "  leading: select leading pt jet in every event" << endl;
      cout << endl;
      return 0;
    }
    po::notify(vm);
  } catch(exception& e) {
    cerr << "Arguments error: " <<  e.what() << endl;
    return 1;
  }
  // END OPTIONS ****************************************************

  // output file stream
  ofstream dat(output_file.c_str(), ofstream::binary);

  dat.write((char*)&jet_alg.r, sizeof(float)); // cone radius

  { // placeholders, filled later
    size_t blank = 0;
    dat.write((char*)&blank, sizeof(size_t)); // number of jets
  }

  // ****************************************************************
  // Read input root file with tree STDHEP
  // ****************************************************************

  TFile *fin = new TFile(input_file.c_str(),"READ");
  if (fin->IsZombie()) return 1;

  // Set branches
  stdhep<> data((TTree*)fin->Get("STDHEP"),stdhep_jets);

  // Jet Clustering Algorithm
  const JetDefinition jet_def(jet_alg.alg,jet_alg.r);
  cout << "\nClustering with " << jet_def.description() << endl << endl;

  // ****************************************************************
  // Loop over input root file entries
  // ****************************************************************

  size_t selected = 0;
  Long64_t nEnt = data.tree->GetEntriesFast();
  if (num_ent>0 && num_ent<nEnt) nEnt = num_ent;
  cout << "Events: " << nEnt << endl;
  timed_counter counter;

  for (Long64_t ent=0; ent<nEnt; ++ent) {
    counter(ent);
    data.tree->GetEntry(ent);

    if (!data.GenParticle_) {
      cerr << "Event " << ent << " has no particles" << endl;
      continue;
    }

    // Collect Final State particles
    vector<PseudoJet> particles; // unclustered final state particles

    for (Int_t i=0; i<data.GenParticle_; ++i) {
      // skip if not final state particle
      if ( data.GenParticle_Status[i] != 1 ) continue;

      PseudoJet particle(
        data.GenParticle_Px[i],data.GenParticle_Py[i],
        data.GenParticle_Pz[i],data.GenParticle_E[i]
      );
      particle.set_user_index(data.GenParticle_PID[i]);
      particles.push_back( particle );
    }

    // Perform clustering
    ClusterSequence cs(particles, jet_def);

    // Sort jets by Pt
    vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

    // Do jet selection ***********************************

    if (method == leading_jet) {

      dat << jets[0];
      ++selected;

    }
  }
  counter.prt(nEnt);
  cout << endl;

  fin->Close();
  delete fin;

  dat.flush();

  cout << selected << " jets selected" << endl;
  dat.seekp(sizeof(float), std::ios::beg);
  dat.write((char*)&selected, sizeof(size_t)); // number of jets

  dat.close();

  return 0;
}
