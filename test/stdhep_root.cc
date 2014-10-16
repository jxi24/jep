#ifndef stdhep_root_h
#define stdhep_root_h

class TFile;
class TTree;

class stdhep {
  TFile * const file;
  TTree * const tree;

  const int kMaxEvent;
  const int kMaxGenParticle;

  Int_t            Event_;
  UInt_t          *Event_fUniqueID;
  UInt_t          *Event_fBits;
  Long64_t        *Event_Number;
  Int_t           *Event_Nparticles;
  Int_t           *Event_ProcessID;
  Double_t        *Event_Weight;
  Double_t        *Event_ScalePDF;
  Double_t        *Event_CouplingQED;
  Double_t        *Event_CouplingQCD;
  Int_t            Event_size;
  Int_t            GenParticle_;
  UInt_t          *GenParticle_fUniqueID;
  UInt_t          *GenParticle_fBits;
  Int_t           *GenParticle_PID;
  Int_t           *GenParticle_Status;
  Int_t           *GenParticle_M1;
  Int_t           *GenParticle_M2;
  Int_t           *GenParticle_D1;
  Int_t           *GenParticle_D2;
  Double_t        *GenParticle_E;
  Double_t        *GenParticle_Px;
  Double_t        *GenParticle_Py;
  Double_t        *GenParticle_Pz;
  Double_t        *GenParticle_PT;
  Double_t        *GenParticle_Eta;
  Double_t        *GenParticle_Phi;
  Double_t        *GenParticle_Rapidity;
  Double_t        *GenParticle_T;
  Double_t        *GenParticle_X;
  Double_t        *GenParticle_Y;
  Double_t        *GenParticle_Z;
  Int_t            GenParticle_size;

public:
  stdhep(const char* file, int MaxGenParticle);
  virtual ~stdhep();

  void turn_on(const char* branch);

};

#endif
