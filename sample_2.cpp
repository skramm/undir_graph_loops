// Copyright Sebastien Kramm 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
\file sample_2.cpp
\brief Sample code demonstrating when somes nodes are NOT connected

In some situations, we can have a loop but explore can fail if the starting node DOES NOT lead to that loop
*/

#include <iostream>
#include <string>
#include "udgld.hpp"
#include <boost/version.hpp>

#include "boost/graph/graphviz.hpp"

/// memory allocation
UDGLD_INIT;

/*
  test graph

  N0  N1-N2
       \ /
       N3
*/

/// the nodes
std::vector<std::string> name = { "N0", "N1", "N2", "N3" };

std::string dot_command( "dot -Tpng -Gsize=\"6,6\" -Grankdir=LR -Nfontsize=24 obj/sample1_" );

//-------------------------------------------------------------------------------------------
/// Some typedefs for readability... ;-)
typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::undirectedS,
	boost::no_property,
	boost::property<
		boost::edge_color_t,
		boost::default_color_type
		>
	> graph_t;

	typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
	typedef boost::graph_traits<graph_t>::edge_descriptor   edge_t;


//-------------------------------------------------------------------
/// Generates a dot file from graph \c g and calls the renderer (dot/Graphviz) to produce a png image of the graph
template<typename Graphtype>
void RenderGraph( const Graphtype& g, int idx )
{
	std::string idx_str( std::to_string(idx) );
	{
		std::ofstream f ( "obj/sample1_" + idx_str + ".dot" );
		assert( f.is_open() );
		boost::write_graphviz( f, g );
	}
	// the cast to void is there to avoid a warning about "unused return value".
	(void)std::system( std::string( dot_command + idx_str + ".dot > obj/sample1_" + idx_str + ".png").c_str() );
}

//-------------------------------------------------------------------
int main(int, char*[])
{

	std::cout << "-built with Boost " << BOOST_LIB_VERSION << '\n';
	graph_t g( name.size() );

	add_edge(2, 1, g);
	add_edge(2, 3, g);
	add_edge(3, 1, g);

	int i=0;
	RenderGraph( g, i++ );

	std::vector<std::vector<vertex_t>> loops = udgld::FindLoops<graph_t,vertex_t>( g );      // no cycles at first
	udgld::PrintPaths( std::cout, loops );

	return 0;
}
//-------------------------------------------------------------------

/**
\page sample_graph_1 Sample graph

This page shows what is done in sample_1.cpp

\image html sample1_0.png "Original graph, without loops"
\image html "sample1_1.png"
\image html "sample1_2.png"
\image html "sample1_3.png"

*/
