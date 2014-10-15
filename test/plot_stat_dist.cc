#ifndef jep_statistics_h
#define jep_statistics_h

#include <vector>
#include <map>

class stat_dist {
  
  std::map<std::string, std::vector<float> > dist;

public:
  stat_dist();
  ~stat_dist();

  void fill(const std::string& name, float x);
  void save(const char* name) const;

};

#endif
