#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#include "jep/profile.h"
#include "jep/statistics.h"
#include "jep/common.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace fastjet;
using namespace std;
using namespace jep;

int main(int argc, char *argv[]){

  if ( argc!=2 ) {
    cout << "Usage: "<<argv[0]<<" file.root" << endl;
    exit(0);
  }

  // ****************************************************************
  // Reading input root file argv[1] with tree STDHEP
  // ****************************************************************

  TFile *file = new TFile(argv[1],"READ");
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

  // ****************************************************************
  // Loop over input root file entries
  // ****************************************************************

  const Long64_t nEnt = tree->GetEntriesFast();
  for (Long64_t ent=0; ent<nEnt; ++ent) {
    tree->GetEntry(ent);

    // Collect Final State particles
    vector<PseudoJet> particles; // unclustered final state particles

    for (Int_t i=0; i<GenParticle_; ++i) {
      // skip if not final state particle
      if ( GenParticle_Status[i] != 1 ) continue;

      PseudoJet jet(
        GenParticle_Px[i],GenParticle_Py[i],GenParticle_Pz[i],GenParticle_E[i]
      );
      jet.set_user_index(GenParticle_PID[i]);
      particles.push_back( jet );
    }

    // AntiKt4 Clustering Algorithm
    ClusterSequence cs(particles, JetDefinition(antikt_algorithm, 1.0) );

    // Sort jets by Pt
    vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

    for (unsigned char i=0; i<5; ++i)
      cout << "Jet "<<(int)i<<": Et = " << jets[i].Et() << endl;
    cout << endl;

    for(unsigned char k=0; k < 5; k++){
    //    vector<double> prof = jep::profile(&jets[0], 0.4, 0.025, 13);
        vector<double> prof = jep::profile(&jets[k], 1.0, 0.1, 10);
    
        cout << "r\tE" << endl;
        for (unsigned char i=0; i<10; ++i) {
    //      cout << 0.1+i*0.025 << '\t' << prof[i] << endl;
          prof[i] = prof[i]/prof[prof.size()-1];
          cout << 0.1+i*0.1 << '\t' << prof[i] << endl;
        }
    
        for(int j=1; j<5; j++){
            vector<val_t> stats = statistics(j, prof, jets[0].Et(), 1.0);
            for(unsigned char i=0; i<stats.size(); i++) {
                cout << stats[i] << endl;
            }
        }
    
      }
    break;
  }

  file->Close();
  delete file;

  return 0;
}
