#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>

// Use a DIMACS network flow file as stdin.
// boykov_kolmogorov-eg < max_flow.dat
//
// Sample output:
// c  The total flow:
// s 13
//
// c flow values:
// f 0 6 3
// f 0 1 6
// f 0 2 4
// f 1 5 1
// f 1 0 0
// f 1 3 5
// f 2 4 4
// f 2 3 0
// f 2 0 0
// f 3 7 5
// f 3 2 0
// f 3 1 0
// f 4 5 0
// f 4 6 4
// f 5 4 0
// f 5 7 1
// f 6 7 7
// f 6 4 0
// f 7 6 0
// f 7 5 0

int main()
{
  using namespace boost;

  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
  typedef adjacency_list < vecS, vecS, directedS,
    property < vertex_name_t, std::string,
    property < vertex_index_t, long,
    property < vertex_color_t, boost::default_color_type,
    property < vertex_distance_t, long,
    property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,

    property < edge_capacity_t, long,
    property < edge_residual_capacity_t, long,
    property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;

  Graph g;
  property_map < Graph, edge_capacity_t >::type
      capacity = get(edge_capacity, g);
  property_map < Graph, edge_residual_capacity_t >::type
      residual_capacity = get(edge_residual_capacity, g);
  property_map < Graph, edge_reverse_t >::type rev = get(edge_reverse, g);
  Traits::vertex_descriptor s, t;

  // read input in DIMACS max flow format
  // http://lpsolve.sourceforge.net/5.5/DIMACS_maxf.htm
  read_dimacs_max_flow(g, capacity, rev, s, t);

  std::vector<default_color_type> color(num_vertices(g));
  std::vector<long> distance(num_vertices(g));
  long flow = boykov_kolmogorov_max_flow(g ,s, t);

  std::cout << "c  The total flow:" << std::endl;
  std::cout << "s " << flow << std::endl << std::endl;

  std::cout << "c flow values:" << std::endl;
  graph_traits < Graph >::vertex_iterator u_iter, u_end;
  graph_traits < Graph >::out_edge_iterator ei, e_end;
  for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
    for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei)
      if (capacity[*ei] > 0)
        std::cout << "f " << *u_iter << " " << target(*ei, g) << " "
          << (capacity[*ei] - residual_capacity[*ei]) << std::endl;

  return EXIT_SUCCESS;
}
