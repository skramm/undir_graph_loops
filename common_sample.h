// Copyright Sebastien Kramm, 2016-2020
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
\file
\brief This file is used only to build the provided samples

Home page: https://github.com/skramm/udgcd
*/
#ifndef HG_COMMON_SAMPLE_H
#define HG_COMMON_SAMPLE_H

#include <boost/version.hpp>
#include "boost/graph/graphviz.hpp"
#include <boost/graph/connected_components.hpp>


#define UDGCD_DO_CYCLE_CHECKING
//#define UDGCD_LOG_FUNC
//#define UDGCD_PRINT_STEPS
//#define UDGCD_DEV_MODE

#include "udgcd.hpp"

#include <string>
#include <numeric>

#define SHOW_INFO \
	std::cout << "-START: " << __FILE__ \
		<< "\n-built with Boost " << BOOST_LIB_VERSION << '\n'

extern std::string prog_id; // allocated in samples files
int g_idx = 0;

//-------------------------------------------------------------------
/// Prints some details on graph and returns nb of expected cycles
/**
(assumes that no two vertices have two edges that join them !)

See https://en.wikipedia.org/wiki/Dense_graph
*/
template<typename graph_t>
size_t
printGraphInfo( const graph_t& g )
{
	auto v = boost::num_vertices(g);
	auto e = boost::num_edges(g);

	std::cout << "Graph info:"
		<< "\n -nb of vertices=" << v
		<< "\n -nb of edges=" << e
		<< "\n -density=" << 1.0 * e / v / (v-1);

	std::vector<size_t> component( v );
	auto nb_cc = boost::connected_components( g, &component[0] );
	auto nb_cycles = e -  v + nb_cc;
	std::cout  << "\n -nb graphs=" << nb_cc
		<< "\n  => nb cycles expected=" << nb_cycles << '\n';
	return nb_cycles;
}
//-------------------------------------------------------------------
#if 0
void
CallDot( std::string id_str )
{
	std::system(
		std::string(
//			"dot -Tsvg -Grankdir=LR -Nfontsize=24 "
			"dot -Tsvg -Nfontsize=24 out/"
			+ id_str
			+ ".dot > out/"
			+ id_str
			+ ".svg"
		).c_str()
	);
}
//-------------------------------------------------------------------
std::string
BuildDotFileName()
{
	return "out/plot_" + prog_id + "_" + std::to_string(g_idx);
}
#endif

#define THROW_ERROR( msg ) \
	{ \
		std::cerr << "udgcd: " << __FILE__ << '-' << __FUNCTION__ << ": " << msg << std::endl; \
		throw; \
	}

//-------------------------------------------------------------------
/// Generates a dot file from graph \c g and calls the renderer (dot/Graphviz) to produce a svg image of the graph
template<typename graph_t>
void
RenderGraph( const graph_t& g, const std::string id_str )
{
/*	std::string id_str;
	if( !name )
		id_str = BuildDotFileName();
	else
		id_str = *name;*/
	std::cout << "id_str=" << id_str << '\n';
	{
		std::string fname( "out/" + id_str + "_" + std::to_string(g_idx) + ".dot" );
		std::ofstream f ( fname );
		if( !f.is_open() )
			THROW_ERROR( "unable to open file" + fname );
		boost::write_graphviz( f, g );
	}
//	CallDot( id_str );
	g_idx++;
}

/// Renders graph in a .dot file but with edges part of cycle with some random color
template<typename graph_t,typename vertex_t>
void
RenderGraph2( const graph_t& g, const std::vector<std::vector<vertex_t>>& cycles, const std::string id_str )
{
	int nbColors = std::min(32, (int)cycles.size() );

	int bi = (int)std::ceil( std::log(nbColors) / std::log(2) );
	bi = std::max(2,bi);

// build color set
	std::vector<std::string> color_set( nbColors );
	for( size_t i=0; i<cycles.size(); i++ )
	{
		int r = 255 * (i%bi)    / (bi-1);
		int g = 255 * ((i/bi)%bi) / (bi-1);
		int b = std::max( 0, std::min(384-r-g,255) );
		std::ostringstream oss;
		oss << " [penwidth=\"2.0\";color=\"#" << std::setfill('0');
		oss << std::hex
			<< std::setw(2) << r
			<< std::setw(2) << g
			<< std::setw(2) << b
			<< "\"];\n";
		color_set[i%nbColors] = oss.str();
//		std::cout << "col "  << i << ": " << color_set[i%nbColors] << std::endl;
	}

	auto v_VPV = udgcd::priv::convertCycles2VVPV<vertex_t>( cycles );

	std::string fname( "out/" + id_str + "_" + std::to_string(g_idx) + ".dot" );

	std::ofstream f ( fname );
	if( !f.is_open() )
		THROW_ERROR( "unable to open file" + fname );
	f << "graph G {\n";
	for( auto p_it=boost::vertices(g); p_it.first != p_it.second; p_it.first++ )
		f << *p_it.first << ";\n";

// First, output all the edges part of a cycle, with a given color
//  and store them in a set, so that we can know they hav been drawned.
	std::set<udgcd::priv::VertexPair<vertex_t>> pairSet;
	for( size_t i=0; i<v_VPV.size(); i++ )     // for each cycle
	{
		for( const auto& pair: v_VPV[i] )    // for each pair
		{
			auto v1 = pair.v1;
			auto v2 = pair.v2;
			f << v1 << "--" << v2 << color_set[i%nbColors];
			pairSet.insert( pair );
		}
	}

// Second, add all the edges that were not part of a cycle


	for( auto p_it=boost::edges(g); p_it.first != p_it.second; p_it.first++ )
	{
		auto idx1 = boost::source( *p_it.first, g );
		auto idx2 = boost::target( *p_it.first, g );
		udgcd::priv::VertexPair<vertex_t> p( idx1, idx2 );
		if( pairSet.find(p) == pairSet.end() )
			f << p.v1 << "--" << p.v2 << ";\n";
	}

	f << "}\n";
//	CallDot( id_str );
	g_idx++;

}


//-------------------------------------------------------------------
#if 0
/// Generates a dot file from graph \c g and calls the renderer (dot/Graphviz) to produce a svg image of the graph,
/// with names of vertices in a provided external vector \c v_names ("external properties")
template<typename graph_t>
void
RenderGraph2( const graph_t& g, std::vector<std::string>& v_names )
{
	std::string id_str = BuildDotFileName();
	{
		std::ofstream f ( id_str + ".dot" );
		assert( f.is_open() );
		boost::write_graphviz( f, g, boost::make_label_writer( &v_names[0] ) );
	}
	CallDot( id_str );
	g_idx++;
}
//-------------------------------------------------------------------
/// Generates a dot file from graph \c g and calls the renderer (dot/Graphviz) to produce a svg image of the graph, with printing of color attributes
/**
Note: involving dynamic properties implies that the graph is modified at present, thus the argument cannot be const !
See http://stackoverflow.com/questions/34160290/
*/
template<typename graph_t, typename vertex_t>
void
RenderGraph3( graph_t& g )
{
	boost::dynamic_properties dp;
	dp.property( "color",   boost::get( &vertex_t::color_str, g ) );

//	dp.property( "node_id", boost::get( boost::vertex_index, g ) );
	dp.property( "node_id", boost::get( &vertex_t::node_name, g ) );

	std::string id_str = BuildDotFileName();
	{
		std::ofstream f ( id_str + ".dot" );
		assert( f.is_open() );
		boost::write_graphviz_dp( f, g, dp );
	}
	CallDot( id_str );
	g_idx++;
}
#endif
//-------------------------------------------------------------------
/// General string tokenizer, taken from http://stackoverflow.com/a/236803/193789
/**
- see also this one: http://stackoverflow.com/a/53878/193789
*/
inline
std::vector<std::string>
split_string( const std::string &s, char delim )
{
	std::vector<std::string> velems;
	std::stringstream ss( s );
    std::string item;
    while( std::getline( ss, item, delim ) )
        velems.push_back(item);
    return velems;
}
//-------------------------------------------------------------------
template<typename graph_t>
graph_t
LoadGraph( const char* fname )
{
	graph_t g;

	std::cout<< " - Reading file:" << fname << '\n';
	std::ifstream f( fname );
	if( !f.is_open() )
	{
		std::cerr << "Unable to open file\n";
		throw "Unable to open file";
	}

	size_t nb_lines     = 0;
	size_t nb_empty     = 0;
	size_t nb_comment   = 0;

	{                            // read nb vertices
		std::string temp;
		std::getline( f, temp );
		auto v_tok = split_string( temp, ':' );
		if( v_tok.size() < 2 )
			throw "error 1st line";
		size_t nb = std::atoi( v_tok[1].c_str() );
//		std::cout << "nb vertices=" << nb << '\n';
		for( size_t i=0; i<nb; i++ )
			boost::add_vertex(g);
	}

	do
	{
		std::string temp;
		std::getline( f, temp );
		nb_lines++;

		if( temp.empty() )          // if empty
			nb_empty++;
		else                        // if NOT empty
		{
			if( temp.at(0) == '#' )  // if comment
				nb_comment++;
			else                     // if NOT comment
			{
				auto v_tok = split_string( temp, '-' );
				if( v_tok.size() < 2 )
				{
					std::cerr << "not enough items on line " << nb_lines << ": -" << temp << "-\n";
					throw ( "Invalid data on line: " + std::to_string( nb_lines ) );
				}
				int v1 = std::atoi( v_tok[0].c_str() );
				int v2 = std::atoi( v_tok[1].c_str() );
				boost::add_edge( v1, v2, g );
			}
		}
	}
	while( !f.eof() );

	std::cout<< " - file info:"
		<< "\n  - nb lines=" << nb_lines
		<< "\n  - nb empty=" << nb_empty
		<< "\n  - nb comment=" << nb_comment << '\n';

	return g;
}
//-------------------------------------------------------------------
/// Process the graph \c g to find cycles
/// This function is called by the two apps: random_test.cpp and read_graph.cpp
/**
This function makes sure :
- that the correct number of cycles are found
- that the computed cycles are correct.

It will return a pair <int,cycles>
The first value is 0 in case of success, -1 if incorrect cycles were found.
If other value, it is the absolute number of differences between:
- the \b computed number of cycles
- and the \b expected number of cycles

*/
template<typename graph_t,typename vertex_t>
std::pair<int,std::vector<std::vector<vertex_t>>>
processGraph( graph_t& g )
{
//	using vertex_t = boost::graph_traits<graph_t>::vertex_descriptor;

	auto expected = printGraphInfo( g );

	udgcd::UdgcdInfo info;
	auto cycles = udgcd::findCycles<graph_t,vertex_t>( g, info );
	udgcd::printPaths( std::cout, cycles, "final" );
	if( expected != cycles.size() )
		std::cout << "ERROR: computed nb of cycles is not what expected (expected=" << expected << ")\n";

//	std::cout << "diff=" << (int)expected - (int)cycles.size() << "\n";

	udgcd::priv::printStatus( std::cout, cycles, __LINE__ );

	auto check = udgcd::priv::checkCycles( cycles, g );
	if( check.first != 0 )
	{
		std::cout << "ERROR: " << check.first << " incorrect cycles found\n";
		return std::make_pair(-1, cycles );
	}
	if( check.second != 0 )
	{
		std::cout << "Found: " << check.second << " non chordless cycles\n";
	}

	info.print( std::cout );
//	info.printCSV( std::cerr );

	auto diff = (int)cycles.size() - (int)expected;
	return std::make_pair(diff, cycles );
}
//-------------------------------------------------------------------

#endif // HG_COMMON_SAMPLE_H
