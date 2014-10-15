#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#include "jep/jet_alg.h"
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
  int exception = 0;

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

  tree->SetBranchStatus("GenParticle.M1",1);
  tree->SetBranchStatus("GenParticle.M2",1);

  // ****************************************************************
  // Loop over input root file entries
  // ****************************************************************

  const Long64_t nEnt = tree->GetEntriesFast();
  int chi2[3] = {0};
  int dchi2[3] = {0};
  int log[3] = {0};
  int dlog[3] = {0};
  int nHiggs = 0, nGluon = 0, nQuark = 0;
  int gFalse[4]={0}, qFalse[4]={0}, hFalse[4]={0};
  for (Long64_t ent=0; ent<nEnt; ++ent) {
    try{
        tree->GetEntry(ent);
    
        // Collect Final State particles
        vector<PseudoJet> particles; // unclustered final state particles
    
        // store info about shower to pass to FastJet
        jep::shower_info::init(GenParticle_);

        for (Int_t i=0; i<GenParticle_; ++i) {
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
        ClusterSequence cs(particles, JetDefinition(antikt_algorithm, 1.0) );
    
        // Sort jets by Pt
        vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());

        int goodcnt = 0;
    
        for(unsigned int k=0; k < 5; k++){
            if(jets[k].Et() < 100) continue;
            jep::jet_validator jv(jets[k],5);
            bool is_higgs = jv.is_from_higgs_bb();
//            if(!is_higgs) continue;
            bool within_cone = true;
            vector<double> prof = jep::profile(jets[k], 1.0, 0.1, 10, within_cone, 0.1,false);
        
            for (unsigned char i=0; i<10; ++i) {
              prof[i] = prof[i]/prof[prof.size()-1];
            }
        
/*            if(is_gluon) {
                nGluon++;
            } else if(is_quark) {
                nQuark++;
            } else if(is_higgs) {
                nHiggs++;
            }*/
            if(is_higgs) {
                nHiggs++;
            }
            for(int j=1; j<5; j++){
                vector<val_t> stats = statistics(j, prof, jets[k].Et(), 1.0);
                // Gluon is best
                if(stats[0] < stats[1] && stats[0] < stats[2]) {
                    switch (j) {
                        case 1:
                            if(is_higgs) gFalse[0]++;
                            chi2[0]++;
                            break;
                        case 2:
                            if(is_higgs) gFalse[1]++;
                            dchi2[0]++;
                            break;
                        case 3:
                            if(is_higgs) gFalse[2]++;
                            log[0]++;
                            break;
                        case 4:
                            if(is_higgs) gFalse[3]++;
                            dlog[0]++;
                            break;
                    }
                // Quark is best
                } else if(stats[1] < stats[0] && stats[1] < stats[2]) {
                    switch (j) {
                        case 1:
                            if(is_higgs) qFalse[0]++;
                            chi2[1]++;
                            break;
                        case 2:
                            if(is_higgs) qFalse[1]++;
                            dchi2[1]++;
                            break;
                        case 3:
                            if(is_higgs) qFalse[2]++;
                            log[1]++;
                            break;
                        case 4:
                            if(is_higgs) qFalse[3]++;
                            dlog[1]++;
                            break;
                    }
                // Higgs is best
                } else if(stats[2] < stats[1] && stats[2] < stats[0]) {
                    switch (j) {
                        case 1:
                            if(!is_higgs) {
                                hFalse[0]++;
                            }
                            chi2[2]++;
                            break;
                        case 2:
                            if(!is_higgs) {
                                hFalse[1]++;
                            }
                            dchi2[2]++;
                            break;
                        case 3:
                            if(!is_higgs) {
                                hFalse[2]++;
                            }
                            log[2]++;
                            break;
                        case 4:
                            if(!is_higgs) {
                                hFalse[3]++;
                            }
                            dlog[2]++;
                            break;
                    }
                } else {
                    cout << "NO BEST for stat test " << j << endl;
                    cout << "Gluon: " << stats[0] << endl;
                    cout << "Quark: " << stats[1] << endl;
                    cout << "Higgs: " << stats[2] << endl;
                }
            }
        }
    }
    catch (jep::jepex e) {
        exception++;
    }

    // clear shower info for the event
    jep::shower_info::clear();
  }

  cout << "+-----------------------------------------------------+" << endl;
  cout << "|" << setw(21) << "|" << setw(10) << "Gluon  " << "|" << setw(10) << "Quark  " << "|" << setw(10) << "Higgs  " << "|" << endl;
  cout << "+-----------------------------------------------------+" << endl;
  cout << "|" << setw(20) << "chi^2 " << "|" << setw(10) << chi2[0] << "|" << setw(10) << chi2[1] << "|" << setw(10) << chi2[2] << "|" << endl;
  cout << "|" << setw(20) << "dchi^2 " <<"|" << setw(10) << dchi2[0] << "|" << setw(10) << dchi2[1] << "|" << setw(10) << dchi2[2] << "|" << endl;
  cout << "|" << setw(20) << "log " << "|" << setw(10) << log[0] << "|" << setw(10) << log[1] << "|" << setw(10) << log[2] << "|" << endl;
  cout << "|" << setw(20) << "dlog " << "|" << setw(10) << dlog[0] << "|" << setw(10) << dlog[1] << "|" << setw(10) << dlog[2] << "|" << endl;
  cout << "|" << setw(20) << "valid " << "|" << setw(10) << nGluon << "|" << setw(10) << nQuark << "|" << setw(10) << nHiggs << "|" << endl;
  cout << "|" << setw(20) << "false h c^2 " << "|" << setw(10) << gFalse[0] << "|" << setw(10) << qFalse[0] << "|" << setw(10) << hFalse[0] << "|" << endl;
  cout << "|" << setw(20) << "false h dc^2 " << "|" << setw(10) << gFalse[1] << "|" << setw(10) << qFalse[1] << "|" << setw(10) << hFalse[1] << "|" << endl;
  cout << "|" << setw(20) << "false h log " << "|" << setw(10) << gFalse[2] << "|" << setw(10) << qFalse[2] << "|" << setw(10) << hFalse[2] << "|" << endl;
  cout << "|" << setw(20) << "false h dlog " << "|" << setw(10) << gFalse[3] << "|" << setw(10) << qFalse[3] << "|" << setw(10) << hFalse[3] << "|" << endl;
  cout << "+-----------------------------------------------------+" << endl << endl;
  cout << "exceptions: " << exception << endl;

  file->Close();
  delete file;

  return 0;
}
