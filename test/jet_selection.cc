#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace fastjet;
using namespace std;
namespace po = boost::program_options;

struct sort_by_r {
  const PseudoJet& jet;
  sort_by_r(const PseudoJet& jet): jet(jet) { }
  bool operator() (const PseudoJet& i, const PseudoJet& j) {
    return ( jet.delta_R(i) < jet.delta_R(j) );
  }
};

ofstream& operator<<(ofstream& dat, const PseudoJet& jet)
{
  static const streamsize
    mom_size = sizeof(double)*4,
    int_size = sizeof(int),
    szt_size = sizeof(size_t);

  double mom[4] = { jet.E(), jet.px(), jet.py(), jet.pz() };
  dat.write((char*)&mom, mom_size);

  // get jet constituents ( returns object, not reference )
  vector<PseudoJet> constituents( jet.constituents() );

  // sort constituents by delta_R in ascending order
  sort(constituents.begin(), constituents.end(), sort_by_r(jet));

  const size_t size = constituents.size();
  dat.write((char*)&size, szt_size);

  if (size>0) for (size_t i=0; i<size; ++i) {
    double mom[4] = { constituents[i].E (), constituents[i].px(),
                      constituents[i].py(), constituents[i].pz() };
    dat.write((char*)&mom, mom_size);

    int pid = constituents[i].user_index();
    dat.write((char*)&pid, int_size);
  } else {
    cerr << "Jet has no constituents" << endl;
  }

  return dat;
}

enum method_t { leading_jet };

int main(int argc, char *argv[])
{
  // START OPTIONS **************************************************
  string input_file, output_file, method_str;
  int _kMaxGenParticle;
  float cone_r;
  method_t method;

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("input,i", po::value<string>(&input_file),
       "input stdhep root file (from showering)")
      ("output,o", po::value<string>(&output_file),
       "output binary file")
      ("method,m", po::value<string>(&method_str),
       "jet selection method")
      ("cone-radius,r", po::value<float>(&cone_r),
       "antikt jet clustering cone radius")

      ("max-gen", po::value<int>(&_kMaxGenParticle)->default_value(2000),
       "maximum GenParticle_* arrays size in input file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_opt), vm);
    po::notify(vm);

    // Options Properties ---------------------------------
    if (argc==1 || vm.count("help")) {
      cout << all_opt << endl;
      cout << "Selection methods:" << endl;
      cout << "  leading-jet: select leading pt jet in every event" << endl;
      cout << endl;
      return 0;
    }

    if (!method_str.compare("leading-jet")) method = leading_jet;
    else {
      cerr << "Undefined jet selection method: " << method_str << endl;
      return 1;
    }

    // Necessary options ----------------------------------
    if (!vm.count("input"))
    { cerr << "Missing command --input,-i" << endl; return 1; }
    if (!vm.count("output"))
    { cerr << "Missing command --output,-o" << endl; return 1; }
    if (!vm.count("method"))
    { cerr << "Missing command --method,-m" << endl; return 1; }
    if (!vm.count("cone-radius"))
    { cerr << "Missing command --cone-radius,-r" << endl; return 1; }

  } catch(exception& e) {
    cerr << "Arguments error: " <<  e.what() << endl;
    return 1;
  }
  // END OPTIONS ****************************************************

  // output file stream
  ofstream dat(output_file, ofstream::binary);

  dat.write((char*)&cone_r, sizeof(float)); // cone radius

  { // placeholders, filled later
    size_t blank = 0;
    dat.write((char*)&blank, sizeof(size_t)); // number of jets
  }

  // ****************************************************************
  // Read input root file with tree STDHEP
  // ****************************************************************

  TFile *fin = new TFile(input_file.c_str(),"READ");
  TTree *tree = (TTree*)fin->Get("STDHEP");

  const Int_t kMaxEvent = 1;
  const Int_t kMaxGenParticle = _kMaxGenParticle;

  // Variables ******************************************************

  Int_t           Event_;
  UInt_t          Event_fUniqueID[kMaxEvent];   //[Event_]
  UInt_t          Event_fBits[kMaxEvent];   //[Event_]
  Long64_t        Event_Number[kMaxEvent];   //[Event_]
  Int_t           Event_Nparticles[kMaxEvent];   //[Event_]
  Int_t           Event_ProcessID[kMaxEvent];   //[Event_]
  Double_t        Event_Weight[kMaxEvent];   //[Event_]
  Double_t        Event_ScalePDF[kMaxEvent];   //[Event_]
  Double_t        Event_CouplingQED[kMaxEvent];   //[Event_]
  Double_t        Event_CouplingQCD[kMaxEvent];   //[Event_]
  Int_t           Event_size;
  Int_t           GenParticle_;
  UInt_t          GenParticle_fUniqueID[kMaxGenParticle];   //[GenParticle_]
  UInt_t          GenParticle_fBits[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_PID[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_Status[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_M1[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_M2[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_D1[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_D2[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_E[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Px[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Py[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Pz[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_PT[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Eta[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Phi[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Rapidity[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_T[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_X[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Y[kMaxGenParticle];   //[GenParticle_]
  Double_t        GenParticle_Z[kMaxGenParticle];   //[GenParticle_]
  Int_t           GenParticle_size;

  // Branches *******************************************************

  tree->SetMakeClass(1); // Necessary here !!!
  tree->SetBranchAddress("Event",                &Event_);
  tree->SetBranchAddress("Event.fUniqueID",      &Event_fUniqueID);
  tree->SetBranchAddress("Event.fBits",          &Event_fBits);
  tree->SetBranchAddress("Event.Number",         &Event_Number);
  tree->SetBranchAddress("Event.Nparticles",     &Event_Nparticles);
  tree->SetBranchAddress("Event.ProcessID",      &Event_ProcessID);
  tree->SetBranchAddress("Event.Weight",         &Event_Weight);
  tree->SetBranchAddress("Event.ScalePDF",       &Event_ScalePDF);
  tree->SetBranchAddress("Event.CouplingQED",    &Event_CouplingQED);
  tree->SetBranchAddress("Event.CouplingQCD",    &Event_CouplingQCD);
  tree->SetBranchAddress("Event_size",           &Event_size);
  tree->SetBranchAddress("GenParticle",          &GenParticle_);
  tree->SetBranchAddress("GenParticle.fUniqueID",&GenParticle_fUniqueID);
  tree->SetBranchAddress("GenParticle.fBits",    &GenParticle_fBits);
  tree->SetBranchAddress("GenParticle.PID",      &GenParticle_PID);
  tree->SetBranchAddress("GenParticle.Status",   &GenParticle_Status);
  tree->SetBranchAddress("GenParticle.M1",       &GenParticle_M1);
  tree->SetBranchAddress("GenParticle.M2",       &GenParticle_M2);
  tree->SetBranchAddress("GenParticle.D1",       &GenParticle_D1);
  tree->SetBranchAddress("GenParticle.D2",       &GenParticle_D2);
  tree->SetBranchAddress("GenParticle.E",        &GenParticle_E);
  tree->SetBranchAddress("GenParticle.Px",       &GenParticle_Px);
  tree->SetBranchAddress("GenParticle.Py",       &GenParticle_Py);
  tree->SetBranchAddress("GenParticle.Pz",       &GenParticle_Pz);
  tree->SetBranchAddress("GenParticle.PT",       &GenParticle_PT);
  tree->SetBranchAddress("GenParticle.Eta",      &GenParticle_Eta);
  tree->SetBranchAddress("GenParticle.Phi",      &GenParticle_Phi);
  tree->SetBranchAddress("GenParticle.Rapidity", &GenParticle_Rapidity);
  tree->SetBranchAddress("GenParticle.T",        &GenParticle_T);
  tree->SetBranchAddress("GenParticle.X",        &GenParticle_X);
  tree->SetBranchAddress("GenParticle.Y",        &GenParticle_Y);
  tree->SetBranchAddress("GenParticle.Z",        &GenParticle_Z);
  tree->SetBranchAddress("GenParticle_size",     &GenParticle_size);

  // Note: Must set addresses for all these branches and SetMakeClass(1)
  // because of how the root file was written

  // Turn on only needed branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("GenParticle",1);
  tree->SetBranchStatus("GenParticle.PID",1);
  tree->SetBranchStatus("GenParticle.Status",1);
  tree->SetBranchStatus("GenParticle.E" ,1);
  tree->SetBranchStatus("GenParticle.Px",1);
  tree->SetBranchStatus("GenParticle.Py",1);
  tree->SetBranchStatus("GenParticle.Pz",1);

//  tree->SetBranchStatus("GenParticle.M1",1);
//  tree->SetBranchStatus("GenParticle.M2",1);

  // ****************************************************************
  // Loop over input root file entries
  // ****************************************************************

  clock_t last_time = clock();
  short num_sec = 0; // seconds elapsed

  size_t selected = 0;
  const Long64_t nEnt = tree->GetEntriesFast();
  cout << "Events: " << nEnt << endl;

  for (Long64_t ent=0; ent<nEnt; ++ent) {
    tree->GetEntry(ent);

    // Collect Final State particles
    vector<PseudoJet> particles; // unclustered final state particles

    for (Int_t i=0; i<GenParticle_; ++i) {
      // skip if not final state particle
      if ( GenParticle_Status[i] != 1 ) continue;

      PseudoJet particle(
        GenParticle_Px[i],GenParticle_Py[i],GenParticle_Pz[i],GenParticle_E[i]
      );
      particle.set_user_index(GenParticle_PID[i]);
      particles.push_back( particle );
    }

    // AntiKt4 Clustering Algorithm
    ClusterSequence cs(particles, JetDefinition(antikt_algorithm, cone_r) );

    // Sort jets by Pt
    vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

    // Do jet selection ***********************************

    if (method == leading_jet) {

      dat << jets[0];
      ++selected;

    }
    

    // ****************************************************

    // timed counter
    if ( (clock()-last_time)/CLOCKS_PER_SEC > 1 ) {
      ++num_sec;
      cout << setw(10) << ent
           << setw( 7) << num_sec << 's';
      cout.flush();
      for (char i=0;i<19;++i) cout << '\b';
      last_time = clock();
    }

  }
  cout << setw(10) << nEnt
       << setw( 7) << num_sec << 's' << endl << endl;

  fin->Close();
  delete fin;

  dat.flush();

  cout << selected << " jets selected" << endl;
  dat.seekp(sizeof(float), std::ios::beg);
  dat.write((char*)&selected, sizeof(size_t)); // number of jets

  dat.close();

  return 0;
}
