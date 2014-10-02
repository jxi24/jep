#ifndef shower_graph_dot_h
#define shower_graph_dot_h

#include <vector>

class _graph_impl;

class shower_graph_dot {

  _graph_impl *graph;

public:
  shower_graph_dot();
  virtual ~shower_graph_dot();

  virtual void add_particle(int i, int pid, int status, float E);
  virtual void add_edge(int from, int to);
  virtual void add_jet(const char* name, const std::vector<int>& particles);
  virtual void save(const char* filename) const;
};

#endif
