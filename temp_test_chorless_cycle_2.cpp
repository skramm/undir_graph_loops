// temp_test_chorless_cycle_2.cpp
/**
Attempt using a tree and a DFS
*/

#include <vector>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>

#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/undirected_dfs.hpp>
#include <boost/graph/depth_first_search.hpp>

/// Type of vertices in the tree. Hold an index on the source vertex in undirected graph
template<typename vertex_t>
struct tree_vertex_t
{
	vertex_t src_vertex;
	tree_vertex_t( vertex_t v ): src_vertex(v)
	{}
};

/// The tree definition
template<typename vertex_t>
using tree_t = boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::directedS,
		tree_vertex_t<vertex_t>
	>;

//--------------------------------------------------------
template<typename T>
void
printVector( std::ostream& f, const std::vector<T>& vec )
{
	f << "* vector #=" << vec.size() << ": { ";
	for( const auto& elem : vec )
		f << elem << "-";
	f << " }\n";
}

#define COUT for(int i=0; i<depth; i++ ) std::cout << "  "; std::cout
//--------------------------------------------------------
/// Convert undirected graph to tree
template<typename vertex_t, typename graph_t>
void
fill_tree(
	tree_t<vertex_t>&             tree,       ///< output tree
	const std::vector<vertex_t>&  cycle,      ///< the cycle we are investigating
	tree_vertex_t<vertex_t>       t_current,  ///< current vertex
	const graph_t&                gr,
	std::vector<vertex_t>         cvec
)
{
	static int depth;
	depth++;
	auto current = t_current.src_vertex;
	COUT << __FUNCTION__ << "(): current=" << current << "\n";

	if( depth == 10 )		std::exit(1);

	cvec.push_back( current );

	for(
		auto p_it = boost::out_edges( current, gr );
		p_it.first != p_it.second;
		p_it.first++
	)
	{
		COUT << "L1: depth=" << depth << " current=" << current << ": edge " << *p_it.first << "\n";
		vertex_t vs = boost::source( *p_it.first, gr );
		vertex_t next = boost::target( *p_it.first, gr );
		assert( vs == current );

		auto it = std::find( cvec.begin(), cvec.end(), next );
		if( it != cvec.end() )     // found in the current path
		{
			std::cout << "FOUND in PATH, stop\n";
		}
		else                       // NOT found in the current path
		{
			if( next == cycle.back() )   // if last element of cycle
			{
				COUT << "Found last: " << next << ", return\n";
				return;
			}
			else
			{
				COUT << "create edge " << current << "-" << next << "\n";
				tree_vertex_t<vertex_t> t_next(next);

				auto v = boost::add_vertex( t_next, tree );   // add edge in tree
				boost::add_edge( t_current, v, tree );   // add edge in tree
				fill_tree( tree, cycle, t_next, gr, cvec );
			}
		}
	}
	COUT << "END\n";
	depth--;
}

//--------------------------------------------------------
class myDFSVisitor : public boost::default_dfs_visitor
{
	public:
		template < typename Vertex, typename Graph >
  		void discover_vertex(Vertex u, const Graph & g) const
		{
			std::cout << "At " << u << std::endl;
		}

		template < typename Edge, typename Graph >
  		void examine_edge(Edge e, const Graph& g) const
 		{
			 std::cout << "Examining edges " << e << std::endl;
		}
};

//--------------------------------------------------------
template<typename vertex_t, typename graph_t>
std::vector<std::vector<vertex_t>>
extractChordlessCycles( const std::vector<vertex_t>& cycle, const graph_t& gr )
{
	std::vector<std::vector<vertex_t>> out;

// build tree (call to recursive function)
	tree_t<vertex_t> tree;
	std::vector<vertex_t> empty;
	tree_vertex_t<vertex_t> tfirst( cycle[0] );
	fill_tree( tree, cycle, tfirst, gr, empty );
//                            ^
//                         current

// DFS
/*	myDFSVisitor vis;
	auto indexmap = boost::get( boost::vertex_index, tree );
	auto colormap = boost::make_vector_property_map<boost::default_color_type>(indexmap);

	boost::depth_first_search( gr, vis, colormap, cycle[0] );
*/

	return out;
}

//--------------------------------------------------------
using  graph_t = boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		boost::undirectedS
	>;

//--------------------------------------------------------
int main()
{
	graph_t g;
	boost::add_edge( 0, 1, g );
	boost::add_edge( 1, 2, g ) ;

	boost::add_edge( 0, 7, g );   //  7--0--1--2---3--4---5---6
	boost::add_edge( 2, 3, g );   //     |  |\    /   |   |
	boost::add_edge( 3, 4, g );   //     |  | \--/    /   |
	boost::add_edge( 4, 5, g );   //     |   \-------/    |
	boost::add_edge( 5, 0, g );   //     \----------------/
	boost::add_edge( 1, 3, g );
	boost::add_edge( 1, 4, g );
	boost::add_edge( 5, 6, g );

	std::vector<size_t> vec{ 0,1,2,3,4,5 };
	auto res = extractChordlessCycles( vec, g );

}