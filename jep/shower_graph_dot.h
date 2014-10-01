#ifndef shower_graph_dot_h
#define shower_graph_dot_h

class shower_graph_dot {

  class _graph_impl;
  _graph_impl *graph;

public:
  shower_graph_dot();
  virtual ~shower_graph_dot();

  virtual void add_vertex(int i, int pid, int status);
  virtual void add_edge(int from, int to);
  virtual void save(const char* filename) const;
};

#endif
