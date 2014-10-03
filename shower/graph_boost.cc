#include "shower/graph_boost.h"

#include <fstream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
//#include <boost/graph/graphml.hpp>

//using namespace std;
//using namespace boost;

typedef boost::adjacency_list< boost::listS, boost::vecS, boost::directedS > _graph_impl_base;

class shower_graph_boost::_graph_impl: public _graph_impl_base
{
public:
  _graph_impl(num_t n): _graph_impl_base(n) { }
  virtual ~_graph_impl() { }
};

shower_graph_boost::shower_graph_boost(num_t n) : graph(new _graph_impl(n)) { }

shower_graph_boost::~shower_graph_boost() { delete graph; }

void shower_graph_boost::add_edge(num_t from, num_t to) {
  boost::add_edge(from, to, *graph);
}
void shower_graph_boost::save_dot(const char* filename) const {
  std::ofstream out(filename);
  boost::write_graphviz(out, *graph);
  out.close();
}
