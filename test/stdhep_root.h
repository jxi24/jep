#include "stdhep_root.h"

stdhep::stdhep(const char* file, int MaxGenParticle)
: file( new TFile(argv[1],"READ") ),
  tree( (TTree*)file->Get("STDHEP") ),
  kMaxEvent(1), kMaxGenParticle(MaxGenParticle)
{
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
}
