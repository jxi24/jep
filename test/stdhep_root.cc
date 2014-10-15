#ifndef stdhep_root_h
#define stdhep_root_h

class TFile;
class TTree;

class stdhep {
  TFile * const file;
  TTree * const tree;

  const int kMaxEvent;
  const int kMaxGenParticle;

public:
  stdhep(const char* file, int MaxGenParticle);
  virtual ~stdhep();

  void turn_on(const char* branch);

};

#endif
