#include "jep/shower_graph_dot.h"

#include <string>
#include <fstream>
#include <map>

using namespace std;

struct particle {
  int i, pid, status;
  particle(int i, int pid, int status) : i(i), pid(pid), status(status) { }
};

ofstream& operator<<(ofstream& f, const particle& v) {
  f << v.i << " [label=\"";
  switch (v.pid) {
    case     1: f << 'd';        break;
    case     2: f << 'u';        break;
    case     3: f << 's';        break;
    case     4: f << 'c';        break;
    case     5: f << 'b';        break;
    case     6: f << 't';        break;
    case    -1: f << "-d";       break;
    case    -2: f << "-u";       break;
    case    -3: f << "-s";       break;
    case    -4: f << "-c";       break;
    case    -5: f << "-b";       break;
    case    -6: f << "-t";       break;
    case    11: f << "e-";       break;
    case   -11: f << "e+";       break;
    case    13: f << "μ-";       break;
    case   -13: f << "μ-";       break;
    case    15: f << "τ-";       break;
    case   -15: f << "τ+";       break;
    case    12: f << "νe-";      break;
    case   -12: f << "νe+";      break;
    case    14: f << "νμ-";      break;
    case   -14: f << "νμ-";      break;
    case    16: f << "ντ-";      break;
    case   -16: f << "ντ+";      break;
    case    21: f << 'g';        break;
    case    22: f << "γ";        break;
    case    23: f << 'Z';        break;
    case    24: f << "W+";       break;
    case   -24: f << "W-";       break;
    case    25: f << 'H';        break;
    case   111: f << "π0";       break;
    case   211: f << "π+";       break;
    case  -211: f << "π-";       break;
    case  2212: f << 'p';        break;
    case -2212: f << "-p";       break;
    case  2112: f << 'n';        break;
    case -2112: f << "-n";       break;
    case  2224: f << "∆++";      break;
    case -2224: f << "-∆++";     break;
    case  2214: f << "∆+";       break;
    case -2214: f << "-∆+";      break;
    case  2114: f << "∆0";       break;
    case -2114: f << "-∆0";      break;
    case  1114: f << "∆-";       break;
    case  3122: f << "Λ";        break;
    case -3122: f << "-Λ";       break;
    case   113: f << "ρ0";       break;
    case   213: f << "ρ+";       break;
    case  -213: f << "ρ-";       break;
    case   221: f << "η";        break;
    case   223: f << "ω";        break;
    case   331: f << "η'";       break;
    case  -331: f << "-η'";      break;
    case   311: f << "K0";       break;
    case  -311: f << "-K0";      break;
    case   321: f << "K+";       break;
    case  -321: f << "K-";       break;
    case   130: f << "K0L";      break;
    case   310: f << "K0S";      break;
    case   313: f << "K*0";      break;
    case  -313: f << "-K*0";     break;
    case   323: f << "K*+";      break;
    case  -323: f << "K*-";      break;
    case   511: f << "B0";       break;
    case  -511: f << "-B0";      break;
    case   521: f << "B+";       break;
    case  -521: f << "B-";       break;
    case   513: f << "B*0";      break;
    case   523: f << "B*+";      break;
    case  -513: f << "-B*0";     break;
    case  -523: f << "-B*+";     break;
    case   421: f << "D0";       break;
    case  -421: f << "-D0";      break;
    case   411: f << "D+";       break;
    case  -411: f << "D-";       break;
    case   413: f << "D*+";      break;
    case  2203: f << "(uu)1";    break;
    case  2101: f << "(ud)0";    break;
    default: f << v.pid;
  }

  f<< "\", color=";
  switch (v.status) {
    case  1: f << "green";  break;
    case  2: f << "blue";   break;
    case  3: f << "yellow"; break;
    default: f << "black";  break;
  }
  f << ", style=filled];";
  return f;
}

struct edge {
  int from, to;
  edge(int from, int to) : from(from), to(to) { }
};

ofstream& operator<<(ofstream& f, const edge& e) {
  f << e.from << "->" << e.to << " [penwidth=1];";
  return f;
}

struct shower_graph_dot::_graph_impl {
  vector<particle> particles;
  vector<edge> edges;
  map<string, vector<int> > jets;

  _graph_impl() { }
  virtual ~_graph_impl() { }

};

shower_graph_dot::shower_graph_dot()
: graph( new _graph_impl() ) { }

shower_graph_dot::~shower_graph_dot() {
  delete graph;
}

void shower_graph_dot::add_particle(int i, int pid, int status) {
  graph->particles.push_back(particle(i, pid, status));
}
void shower_graph_dot::add_edge(int from, int to) {
  graph->edges.push_back(edge(from,to));
}
void shower_graph_dot::add_jet(const char* name, const vector<int>& particles) {
  graph->jets[name] = particles;
}

typedef vector<particle>::iterator particle_iter;
typedef vector<edge>::iterator edge_iter;
typedef map<string, vector<int> >::iterator jet_iter;

void shower_graph_dot::save(const char* filename) const {
  ofstream f(filename);
  f << "digraph {" << endl;

  f << "ratio=\"0.6\"" << endl;

  for (particle_iter it=graph->particles.begin(), end=graph->particles.end();
       it!=end; ++it) f << (*it) << endl;

  for (jet_iter it=graph->jets.begin(), end=graph->jets.end();
       it!=end; ++it)
  {
    f << '\"'<< it->first << "\""
      "[color=red, style=filled, width=4, height=1.5, fontsize=60];"
      << endl;
  }

  for (edge_iter it=graph->edges.begin(), end=graph->edges.end();
       it!=end; ++it) f << (*it) << endl;

  for (jet_iter it=graph->jets.begin(), end=graph->jets.end();
       it!=end; ++it)
  {
    const vector<int>& p = it->second;
    for (size_t i=0, size=p.size(); i<size; ++i)
    {
      f << p[i] << "->\"" << it->first <<"\" [penwidth=4];"<< endl;
    }
  }

  f << "}" << endl;
  f.close();
}
