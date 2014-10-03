#ifndef shower_graph_boost_h
#define shower_graph_boost_h

class shower_graph_boost {
  typedef unsigned short num_t;

  class _graph_impl;
  _graph_impl *graph;

public:
  shower_graph_boost(num_t n); // n - number of vertexes
  virtual ~shower_graph_boost();

  virtual void add_edge(num_t from, num_t to);
  virtual void save_dot(const char* filename) const;
};

#endif
