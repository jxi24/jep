#ifndef hist_wrap_h
#define hist_wrap_h

#include <iosfwd>
#include <string>
#include <vector>
#include <map>

class TH1F;

class hist {
  struct binning {
    int nbins; double min, max;
    binning();
    ~binning();
  };
  static std::map<std::string,binning> binnings;
  static std::vector<const hist*> all;
  static std::string binning_name_regex_pattern;

  const binning get_binning(const std::string& hist_name);

  const binning b;
  std::pair<int,double> underflow, overflow;
  TH1F * h;

public:
  hist();
  hist(const std::string& name,const std::string& title);
  ~hist();

  void Fill(double x);
  static void read_binnings(const char* filename, const char* regex="");
  static void print_overflow();
  static void delete_all();
};

#endif
