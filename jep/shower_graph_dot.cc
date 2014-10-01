#include "jep/shower_graph_dot.h"

#include <vector>
//#include <string>
#include <fstream>

using namespace std;

struct vertex {
  int i, pid, status;
  vertex(int i, int pid, int status) : i(i), pid(pid), status(status) { }
};

ofstream& operator<<(ofstream& f, const vertex& v) {
  f << v.i << " [label=\"" << v.i<<':'<< v.pid << "\" color=";
  switch (v.status) {
    case  1: f << "green";  break;
    case  2: f << "blue";   break;
    case  3: f << "red";    break;
    case  4: f << "yellow"; break;
    default: f << "black";  break;
  }
  f << "];";
  return f;
}

struct edge {
  int from, to;
  edge(int from, int to) : from(from), to(to) { }
};

struct shower_graph_dot::_graph_impl {
  vector<vertex> vertices;
  vector<edge> edges;

  _graph_impl() { }
  virtual ~_graph_impl() { }

};

shower_graph_dot::shower_graph_dot()
: graph( new _graph_impl() ) { }

shower_graph_dot::~shower_graph_dot() {
  delete graph;
}

void shower_graph_dot::add_vertex(int i, int pid, int status) {
  graph->vertices.push_back(vertex(i, pid, status));
}
void shower_graph_dot::add_edge(int from, int to) {
  graph->edges.push_back(edge(from,to));
}

void shower_graph_dot::save(const char* filename) const {
  ofstream f(filename);
  f << "digraph {" << endl;

  for (vector<vertex>::iterator it=graph->vertices.begin(), end=graph->vertices.end();
       it!=end; ++it)
  {
    f << (*it) << endl;
  }

  for (vector<edge>::iterator it=graph->edges.begin(), end=graph->edges.end();
       it!=end; ++it)
  {
    f << it->from << "->" << it->to << ';' << endl;
  }

  f << "}" << endl;
  f.close();
}
