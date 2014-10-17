#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#include "jep/jet_alg.h"
#include "shower/graph_dot.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace fastjet;
using namespace std;

int main(int argc, char *argv[]){

  if ( argc!=3 ) {
    cout << "Usage: "<<argv[0]<<" event file.root" << endl;
    exit(0);
  }

  // ****************************************************************
  // Reading input root file argv[1] with tree STDHEP
  // ****************************************************************

  TFile *file = new TFile(argv[2],"READ");
  TTree *tree = (TTree*)file->Get("STDHEP");

  const Int_t kMaxEvent = 1;
  const Int_t kMaxGenParticle = 1205;

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
  tree->SetBranchStatus("GenParticle.PT",1);

  tree->SetBranchStatus("GenParticle.M1",1);
  tree->SetBranchStatus("GenParticle.M2",1);

  // ****************************************************************
  // Loop over input root file entries
  // ****************************************************************

//  const Long64_t nEnt = tree->GetEntriesFast();
//  for (Long64_t ent=2; ent<nEnt; ++ent) {
//    tree->GetEntry(ent);

  tree->GetEntry(atoi(argv[1]));
  if (GenParticle_) { // if there are particles in the event

    // Collect Final State particles
    vector<PseudoJet> particles; // unclustered final state particles

    // store info about shower to pass to FastJet
    jep::shower_info::init(GenParticle_);

    for (Int_t i=0; i<GenParticle_; ++i) {
/*      cout << setw(4) << i << ' '
           << setw(6) << GenParticle_PID[i] << ' '
           << setw(4) << GenParticle_M1[i] << ' '
           << setw(4) << GenParticle_M2[i] << ' '
           << setw(4) << GenParticle_D1[i] << ' '
           << setw(4) << GenParticle_D2[i] << endl;
*/

      jep::shower_info *user_info =
      jep::shower_info::add(i, GenParticle_PID[i], GenParticle_Status[i],
                            GenParticle_M1[i], GenParticle_M2[i]);

      // skip if not final state particle
      if ( GenParticle_Status[i] != 1 ) continue;

      PseudoJet particle(
        GenParticle_Px[i],GenParticle_Py[i],GenParticle_Pz[i],GenParticle_E[i]
      );
      particle.set_user_index(i);
      particle.set_user_info (user_info);
      particles.push_back( particle );
    }

    // AntiKt4 Clustering Algorithm
    ClusterSequence cs(particles, JetDefinition(antikt_algorithm, 0.7) );

    // Sort jets by Pt
    vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

    // Loop over 5 leading jets
    for (size_t i=0; i<5; ++i) {

      // Validate jet
      jep::jet_validator jv(jets[i],5);
      bool from_higgs_bb = jv.is_from_higgs_bb();
      bool from_gluons = jv.is_from_gluons();
      bool from_quarks = jv.is_from_quarks();
      cout << "Jet "<<(int)i+1<<": Et = " << jets[i].Et();
      if (from_higgs_bb) cout << " from H->bb";
      if (from_gluons) cout << " from gluons";
      if (from_quarks) cout << " from quarks";
      cout << endl;

      // Print jet profile
      if (from_higgs_bb || from_gluons || from_quarks) {
        bool within_cone = true;
        vector<double> prof = jep::profile(jets[i], 0.7, 0.1, 7, within_cone);

        cout <<left<<setw(5)<<'r'<<"  "<<'E'<< endl;
        for (size_t i=0; i<7; ++i) {
          cout << setw(5) << 0.1+i*0.1 << "  "
               << setw(7) << prof[i] << endl;
        }
        cout << endl;
      }
    }
    cout << endl;

    // Draw shower graph
    shower_graph_dot g;

    for (Int_t i=0; i<GenParticle_; ++i) {
      g.add_particle(i,GenParticle_PID[i],GenParticle_Status[i], GenParticle_PT[i]);

      Int_t mothers[2] = { GenParticle_M1[i], GenParticle_M2[i] };
      for (short j=0;j<2;++j)
        if (mothers[j]!=-1) g.add_edge(mothers[j],i);
    }

    for (size_t j=0, size=min(jets.size(),5lu); j<size; ++j) {
      vector<int> jet_particles;
      const vector<PseudoJet> constituents
        = sorted_by_pt(jets[j].constituents());

      for (size_t i=0, size=constituents.size(); i<size; ++i)
        jet_particles.push_back(constituents[i].user_index());

      stringstream ss;
      ss << "Jet " << j+1;
      g.add_jet(ss.str().c_str(),jet_particles);
    }

    g.save("graph.gv");

    // clear shower info for the event
    jep::shower_info::clear();

  } else {
    cout << "Selected event " << argv[1] << " has no particles" << endl;
  }

  file->Close();
  delete file;

  return 0;
}
