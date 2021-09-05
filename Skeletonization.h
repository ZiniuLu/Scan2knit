/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <algorithm>

#include "project.h"
#include "cgalHeader.hpp"
#include "Settings.h"
#include "MeshFile.h"


#include <CGAL/boost/graph/split_graph_into_polylines.h>

BEGIN_PROJECT_NAMESPACE

//enum connection_type
//{
//	PARENTS,
//	BROTHERS,
//	CHILDREN
//};

class Node
{
protected:
	std::pair<size_t, Point> node; // <node_number, point>

public:
	Node();
	Node(const size_t id, const Point& p);
	Node(const std::pair<size_t, Point>& nd);

	inline void			set(const size_t id, const Point& p) { this->node = std::make_pair(id, p); }
	inline void			set(const std::pair<size_t, Point>& nd) { this->node = nd; }
	inline size_t		get_id() const { return this->node.first; }	// node_number begins with 1 !!!
	inline const Point&	point() const { return this->node.second; }		// get point
};

class SkelNode : public Node
{
public:
	enum : char
	{
		NODE		 = 'N',
		INTERSECTION = 'I',
		ROOT		 = 'R',
		TOP			 = 'T',
		EXTENTION	 = 'E',
		RING_TOP	 = 't',
		RING_root	 = 'r',
		// currently does not support ring-structure
	};

private:
	char type = SkelNode::NODE;
	std::vector<std::pair<size_t, Point>> mapping_vertices;


public:
	SkelNode();
	SkelNode(const size_t nd_nr, const Point& p);
	SkelNode(const std::pair<size_t, Point>& nd);

	inline void set_type(const char nd_type) 
	{ 
		this->type = nd_type; 
	}
	inline char get_type() 
	{ 
		return this->type; 
	}
	inline void set_mapping_vertices(const std::vector<std::pair<size_t, Point>>& v_map) 
	{ 
		this->mapping_vertices = v_map; 
	}
	inline const std::vector<std::pair<size_t, Point>>& 
				get_mapping_vertices() const 
	{ 
		return this->mapping_vertices; 
	}
};

class SkelEdge
{
private:
	std::pair<size_t, size_t> edge;	// <source, target>

public:
	SkelEdge();
	SkelEdge(const size_t& source, const size_t& target);
	SkelEdge(const std::pair<size_t, size_t>& eg);

	inline size_t source() const { return this->edge.first; }
	inline size_t target() const { return this->edge.second; }
	
	// source & target: node_number, begin with 1 !!!
};

class SkelChain
{
private:
	size_t	id = 0; // begin with 1
	char	chain_type_source = SkelNode::NODE;
	char	chain_type_target = SkelNode::NODE;
	size_t	node_amount		  = 0;
	std::vector<size_t>	vertex_amount;	// vertex_amount.size() == node_amount
	std::vector<size_t> chain;

public:
	SkelChain();
	SkelChain(std::vector<size_t> skel_chain, char source_type, char target_type);

	inline void	set_id(size_t id) { this->id = id; }
	inline void	set_chain(std::vector<size_t>& skel_chain)
	{
		this->chain.clear();
		this->chain.insert(this->chain.end(), skel_chain.begin(), skel_chain.end());
	}
	inline void	set_type(char source_type, char target_type)
	{
		this->set_source_type(source_type);
		this->set_target_type(target_type);
	}
	inline void	set_source_type(char type) { this->chain_type_source = type; }
	inline void	set_target_type(char type) { this->chain_type_target = type; }
	inline void	set_node_amount(size_t nr) { this->node_amount = nr; }
	inline void	set_vertex_amount(std::vector<size_t> v_amount)
	{
		this->vertex_amount.clear();
		this->vertex_amount.insert(this->vertex_amount.end(), v_amount.begin(), v_amount.end());
	}
	inline void	push_back_vertex_amount(size_t nr) { this->vertex_amount.push_back(nr); }
	inline void	pop_back_vertex_amount() { this->vertex_amount.pop_back(); }
	inline void	reverse()
	{
		char type;
		type = this->chain_type_source;
		this->chain_type_source = this->chain_type_target;
		this->chain_type_target = type;

		std::reverse(this->chain.begin(), this->chain.end());
	}
	inline void	insert(const SkelChain& next_skel_chain)
	{
		if (this->chain.size() <= 0)
			this->chain_type_source = next_skel_chain.get_source_type();
		this->chain_type_target = next_skel_chain.get_target_type();
		this->node_amount += next_skel_chain.get_node_amount() - 1;

		const std::vector<size_t>& v_amount = next_skel_chain.get_vertex_amount_per_skel_node();
		this->vertex_amount.pop_back();
		this->vertex_amount.insert(this->vertex_amount.end(), v_amount.begin(), v_amount.end());

		const std::vector<size_t>& next_chain = next_skel_chain.get_chain();
		this->chain.insert(this->chain.end(), next_chain.begin(), next_chain.end());
	}
	inline void	clear()
	{
		this->id = 0;
		this->chain_type_source = SkelNode::NODE;
		this->chain_type_target = SkelNode::NODE;
		this->node_amount = 0;
		this->vertex_amount.clear();
		this->chain.clear();
	}

	inline const size_t	get_id() const { return this->id; }
	inline const std::vector<size_t>& get_chain() const { return this->chain; }
	inline const size_t	get_node_amount() const { return this->node_amount; }
	inline const size_t	get_vertex_amount() const 
	{ 
		// do not caculate intersection_node!
		auto it_begin = (this->chain_type_source != SkelNode::INTERSECTION) ? (this->chain.begin()) : (this->chain.begin() + 1);
		auto it_end = (this->chain_type_target != SkelNode::INTERSECTION) ? (this->chain.end()) : (this->chain.end() - 1);
			
		return std::accumulate(it_begin, it_end, 0); 
	}
	inline const std::vector<size_t>& 
						get_vertex_amount_per_skel_node() const { return this->vertex_amount; }
	inline const char	get_source_type() const { return this->chain_type_source; }
	inline const char	get_target_type() const { return this->chain_type_target; }
	inline const size_t	get_source_node() const { return this->chain.front(); }
	inline const size_t	get_target_node() const { return this->chain.back(); }
	inline const size_t get_node_next2source() const { return this->chain[1]; }
	inline const size_t get_node_next2target() const { return this->chain[this->node_amount - 2]; }
};

class SkelExtn	// skeleton extension
{
private:
	SkelNode skel_node;
	Point	 extn_point;

public:
	SkelExtn();
	SkelExtn(const SkelNode& skel_node, const Point& point) :skel_node(skel_node), extn_point(point){ }
	
	inline const SkelNode& get_skel_node() const { return this->skel_node; }
	inline const size_t get_skel_node_id() const { return this->skel_node.get_id(); }
	inline const Point&	get_extn_point() const { return this->extn_point; }
};

//only needed for the display of the skeleton as maximal polylines
//struct Output_polylines
//{
//	const Skeleton&	skeleton;
//	std::ofstream&	out;
//	int				polyline_size;
//	std::stringstream sstr;
//	
//	Output_polylines(const Skeleton& skeleton, std::ofstream& out) : skeleton(skeleton), out(out) {}
//	
//	inline void start_new_polyline()
//	{
//		polyline_size = 0;
//		sstr.str("");
//		sstr.clear();
//	}
//	inline void	add_node(Skeleton_vertex v) 
//	{
//		++polyline_size;
//		sstr << " " << skeleton[v].point;
//	}
//	inline void end_polyline()
//	{
//		out << polyline_size << sstr.str() << "\n";
//	}
//};


class Skel
{
private:
	v_string		filePath;
	Skeleton		skeleton;

public:
	Skel();
	Skel(Mesh& mesh);
	Skel(Skeleton& skeleton);

	inline bool		 extract_to_end(Mesh& mesh)
	{
		if (!mesh.is_triangle_mesh()) { return false; }

		this->filePath = mesh.get_file_path();

		// 1. extract skeleton;
		Print("\tExtracting mean curvature flow skeletons ... ");
		//auto& tmesh = mesh.get_tmesh();
		auto& pmesh = mesh.get_pmesh();
		CGAL::extract_mean_curvature_flow_skeleton(pmesh, this->skeleton); // caculate object skeleton

		// 3. caculate V & E numbers
		size_t numV_skel = boost::num_vertices(this->skeleton);
		size_t numE_skel = boost::num_edges(this->skeleton);

		std::ostringstream text;
		text << "\t\t\tnumber of vertices: \t" << numV_skel << "\n"
			<< "\t\t\tnumber of edges: \t" << numE_skel;
		Print(text.str());

		return true;
	}
	inline v_string& get_file_path() { return this->filePath; }
	inline Skeleton& get_skeleton() { return this->skeleton; }
};

class SkelGraph
{
private:
	v_string	filePath;

	size_t		node_size = 0;
	size_t		edge_size = 0;

	size_t					root_node_number = 0;
	std::vector<size_t>		top_node_numbers;	// doesn't contain root_node_number
	std::vector<size_t>		intersection_node_numbers;
	std::vector<SkelExtn>	skel_extensions;		// top_node -> extension_node

	std::vector<SkelNode>	skel_nodes;	// node number begins with 1 !!!
	std::vector<SkelEdge>	skel_edges;	// only contain node numbers
	std::vector<SkelChain>	skel_chains; // source:root/top/intersection -> target:top/intersection

public:
	SkelGraph();
	SkelGraph(Skel& skel, Mesh& mesh);

	void	set_skel_graph(const Skeleton& mcf_skel, const Polyhedron& pmesh);	// main function of SkelGraph
	
	inline const std::vector<SkelNode>&  get_skel_nodes() const { return this->skel_nodes; }
	inline const std::vector<SkelEdge>&  get_skel_edges() const { return this->skel_edges; }
	inline const std::vector<SkelExtn>&  get_skel_extns() const { return this->skel_extensions; }
	inline const std::vector<SkelChain>& get_skel_chains() const { return this->skel_chains; }
	
	inline const SkelNode&		get_skel_node(const size_t node_number) const
	{
		return this->skel_nodes[node_number - 1];
	}
	inline const std::vector<std::pair<size_t, Point>>& 
								get_skel_map(const size_t node_number) const
	{
		return this->skel_nodes[node_number - 1].get_mapping_vertices();
	}
	inline size_t				get_root_node_number()
	{
		if (this->root_node_number == 0)
		{
			this->analyse_skel_strucure();
		}
		return this->root_node_number;
	}
	inline std::vector<size_t>	get_top_node_numbers()
	{
		if (this->root_node_number == 0)
		{
			this->analyse_skel_strucure();
		}
		return this->top_node_numbers;
	}
	inline std::vector<size_t>	get_intersection_node_numbers()
	{
		if (this->root_node_number == 0)
		{
			this->analyse_skel_strucure();
		}
		return this->intersection_node_numbers;
	}
	
	//double	get_segment_length(const std::vector<size_t>& skel_segment) const;
	//double	get_edge_length(const std::vector<SkelEdge>::iterator& it_eg) const;
	//double	get_node_length(const size_t nd_nr_1, const size_t nd_nr_2) const;
	//double	get_node_length(const SkelNode& nd1, const SkelNode& nd2) const;

	// output skel relevant info to files
	void	output_skel_graph_to_files(Settings* settings);
	void	output_skel_to_file(Settings* settings);
	void	output_skel_map_to_file(Settings* settings);
	void	output_skel_ext_to_file(Settings* settings);

	// test
	void	print_skel_nodes();
	void	print_skel_edges();
	void	print_skel_graph();

private:
	size_t	contains_node(const Point& p);
	void	set_node_type(size_t nd_nr, char nd_type);
	void	analyse_skel_strucure();
	void	analyse_skel_node_type(
		std::set<size_t>& node_type_top, 
		std::set<size_t>& node_type_intersection);
	size_t	analyse_skel_chains(
		const std::set<size_t>& nd_type_top,
		const std::set<size_t>& nd_type_intersection);
};




// globale functions

//inline bool compairList(std::pair<size_t, size_t> pair1, std::pair<size_t, size_t> pair2)
//{
//	if (pair1.first == pair2.first) { return pair1.second < pair2.second; }
//	else { return pair1.first < pair2.first; }
//}
inline bool equal_1(size_t num) { return num == 1; }
inline bool larger_than_1(size_t num) { return num > 1; }
inline bool larger_than_2(size_t num) { return num > 2; }

bool sort_chain_increase(const SkelChain& chain1, const SkelChain& chain2);

END_PROJECT_NAMESPACE
