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
  ofstream dat(output_file.c_str(), ofstream::binary);

  dat.write((char*)&cone_r, sizeof(float)); // cone radius

  { // placeholders, filled later
    size_t blank = 0;
    dat.write((char*)&blank, sizeof(size_t)); // number of jets
  }

  // ****************************************************************
  // Read input root file with tree STDHEP
  // ****************************************************************

  TFile *fin = new TFile(input_file.c_str(),"READ");
  if (fin->IsZombie()) return 1;

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

    if (!GenParticle_) {
      cerr << "Event " << ent << " has no particles" << endl;
      continue;
    }

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
