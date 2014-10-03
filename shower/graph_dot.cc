#include "shower/graph_dot.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace std;

#define nlead 5ul // number of leading constituents

struct particle {
  int i, pid, status;
  float E, frac;
  vector<particle*> daughters;
  particle(int i, int pid, int status, float E)
  : i(i), pid(pid), status(status), E(E), frac(1.) { }
};

bool sort_by_E(particle* i, particle* j) {
  return ( i->E < j->E );
}

ofstream& operator<<(ofstream& f, const particle& v) {
  f << v.i << " [label=\"";
  switch (v.pid) {
    case     1: f << 'd';        break;
    case     2: f << 'u';        break;
    case     3: f << 's';        break;
    case     4: f << 'c';        break;
    case     5: f << 'b';        break;
    case     6: f << 't';        break;
    case    -1: f << "~d";       break;
    case    -2: f << "~u";       break;
    case    -3: f << "~s";       break;
    case    -4: f << "~c";       break;
    case    -5: f << "~b";       break;
    case    -6: f << "~t";       break;
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
    case -2212: f << "~p";       break;
    case  2112: f << 'n';        break;
    case -2112: f << "~n";       break;
    case  2224: f << "∆++";      break;
    case -2224: f << "~∆++";     break;
    case  2214: f << "∆+";       break;
    case -2214: f << "~∆+";      break;
    case  2114: f << "∆0";       break;
    case -2114: f << "~∆0";      break;
    case  1114: f << "∆-";       break;
    case -1114: f << "~∆-";      break;
    case  3122: f << "Λ";        break;
    case -3122: f << "~Λ";       break;
    case   113: f << "ρ0";       break;
    case   213: f << "ρ+";       break;
    case  -213: f << "ρ-";       break;
    case   221: f << "η";        break;
    case   223: f << "ω";        break;
    case   331: f << "η'";       break;
    case  -331: f << "~η'";      break;
    case   311: f << "K0";       break;
    case  -311: f << "~K0";      break;
    case   321: f << "K+";       break;
    case  -321: f << "K-";       break;
    case   130: f << "K0L";      break;
    case   310: f << "K0S";      break;
    case   313: f << "K*0";      break;
    case  -313: f << "~K*0";     break;
    case   323: f << "K*+";      break;
    case  -323: f << "K*-";      break;
    case   511: f << "B0";       break;
    case  -511: f << "~B0";      break;
    case   521: f << "B+";       break;
    case  -521: f << "B-";       break;
    case   513: f << "B*0";      break;
    case   523: f << "B*+";      break;
    case  -513: f << "~B*0";     break;
    case  -523: f << "~B*+";     break;
    case   421: f << "D0";       break;
    case  -421: f << "~D0";      break;
    case   411: f << "D+";       break;
    case  -411: f << "D-";       break;
    case   413: f << "D*+";      break;
    case  2203: f << "(uu)1";    break;
    case  2101: f << "(ud)0";    break;
    case  2103: f << "(ud)1";    break;
    case    92: f << "FRAG 92";  break;
    case    91: f << "FRAG 91";  break;
    default: f << v.pid;
  }

  f<< "\", color=";
  switch (v.status) {
    case  1: f << "green";  break; // final state
    case  2: f << "blue";   break; // intermediate
    case  3: f << "yellow"; break; // initial state
    default: f << "black";  break;
  }
  f << ", style=filled];";
  return f;
}

typedef map<int, particle*>::iterator particle_iter;
typedef vector<particle*>::iterator   daughter_iter;
typedef map<string, vector<particle*> >::iterator jet_iter;

struct _graph_impl {
  map<string, vector<particle*> > jets;
  map<int, particle*> particles;

  _graph_impl() { }
  virtual ~_graph_impl() {
    // deleted particles on the heap
    for (particle_iter it=particles.begin(), end=particles.end();
         it!=end; ++it) delete it->second;
  }

};

shower_graph_dot::shower_graph_dot()
: graph( new _graph_impl() ) { }

shower_graph_dot::~shower_graph_dot() {
  delete graph;
}

void shower_graph_dot::add_particle(int i, int pid, int status, float E)
{
  particle_iter this_it = graph->particles.find(i);
  if ( this_it != graph->particles.end() ) {
    cerr << "Warning: Overwriting particle " << i << endl;
    delete this_it->second;
  }

  graph->particles[i] = new particle(i, pid, status, E);
}

void shower_graph_dot::add_edge(int from, int to) {
  particle_iter from_it = graph->particles.find(from),
                to_it   = graph->particles.find(to),
                end_it  = graph->particles.end();

  if ( from_it==end_it || to_it==end_it ) {
    throw runtime_error("Edge connecting to undefined vertex");
  } else { // OK -- add a daughter
    from_it->second->daughters.push_back(to_it->second);
  }
}

void shower_graph_dot::add_jet(const char* name, const vector<int>& particles) {
  const size_t size = particles.size();

  vector<particle*> jet(size,NULL);
  for (size_t i=0; i<size; ++i) jet[i] = graph->particles[particles[i]];

  sort(jet.begin(), jet.end(), sort_by_E);

  graph->jets[name] = jet;
}

void shower_graph_dot::save(const char* filename) const {
  ofstream f(filename);
  f << "digraph {" << endl;

  f << "ratio=\"0.6\"" << endl;

  // Write particle nodes
  for (particle_iter it=graph->particles.begin(), end=graph->particles.end();
       it!=end; ++it) f << *it->second << endl;

  // Write jet nodes
  for (jet_iter it=graph->jets.begin(), end=graph->jets.end();
       it!=end; ++it)
  {
    f << '\"'<< it->first << "\""
      "[color=red, style=filled, width=4, height=1.5, fontsize=60];"
      << endl;
  }

  // Write particle edges
  for (particle_iter p=graph->particles.begin(), pend=graph->particles.end();
       p!=pend; ++p)
  {
    daughter_iter dbegin = p->second->daughters.begin();
    daughter_iter dend   = p->second->daughters.end();

    if (dend-dbegin==1) {
      float frac = p->second->frac;
      (*dbegin)->frac = frac;

      f << p->first << "->" << (*dbegin)->i << " [penwidth="
        << frac << "];" << endl;
      continue;
    }

    sort(dbegin, dend, sort_by_E);

    const size_t  dsize = p->second->daughters.size();
    daughter_iter dlead = ( dsize<nlead ? dbegin+dsize : dbegin+nlead );
    daughter_iter d = dbegin;

    // Energy of leading constituents
    float Elead = 0.;
    for (; d<dlead; ++d) Elead += (*d)->E;

    d = dbegin;

    for (; d<dlead; ++d) {
      float frac = (*d)->E*10./Elead;
      if (frac<1.) frac = 1.;
      (*d)->frac = frac;

      f << p->first << "->" << (*d)->i << " [penwidth="
        << frac << "];" << endl;
    }

    for (; d<dend; ++d) {
      f << p->first << "->" << (*d)->i << " [penwidth=1];" << endl;
    }
  }

  // Write jet edges
  for (jet_iter j=graph->jets.begin(), end=graph->jets.end();
       j!=end; ++j)
  {
    const size_t  size  = j->second.size();
    daughter_iter begin = j->second.begin();
    daughter_iter lead  = ( size<nlead ? begin+size : begin+nlead );
    daughter_iter pend  = j->second.end();
    daughter_iter p     = j->second.begin();

    // Energy of leading constituents
    float Elead = 0.;
    for (; p<lead; ++p) Elead += (*p)->E;

    p = begin;

    for (; p<lead; ++p) {
      float frac = (*p)->E*20./Elead;
      if (frac<1.) frac = 1.;

      f << (*p)->i << "->\"" << j->first << "\" [penwidth="
        << frac << ", color=darkorange];" << endl;
    }

    for (; p<pend; ++p) {
      f << (*p)->i << "->\"" << j->first << "\" [penwidth=1];" << endl;
    }
  }

  f << "}" << endl;
  f.close();
}
