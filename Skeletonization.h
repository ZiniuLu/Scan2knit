/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

#include "project.h"
#include "MeshFile.h"
#include "cgalHeader.hpp"


BEGIN_PROJECT_NAMESPACE

//enum node_type
//{
//	// skeleton structure, parent / children
//	NODE,			// =1 parent && =1 child
//	INTERSECTION,	// =1 parent && >1 children
//	ROOT,			// =0 parent && >0 children
//	TOP,			// =1 parent && =0 children
//	END				// ROOT || TOP
//};
enum connection_type
{
	PARENTS,
	BROTHERS,
	CHILDREN
};

class Node
{
protected:
	std::pair<size_t, Point> node; // <node_number, point>

public:
	Node();
	Node(const size_t nd_nr, const Point& p);
	Node(const std::pair<size_t, Point>& nd);

	void		 set(const size_t nd_nr, const Point& p);
	void		 set(const std::pair<size_t, Point>& nd);
	size_t		 node_number() const;	// node_number begins with 1 !!!
	const Point& point() const;		// get point
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
	std::vector<Point> mapping_vertices;


public:
	SkelNode();
	SkelNode(const size_t nd_nr, const Point& p);
	SkelNode(const std::pair<size_t, Point>& nd);

	void					  set_type(const char nd_type);
	char					  get_type();
	void					  set_mapping_vertices(const std::vector<Point>& v_map);
	const std::vector<Point>& get_mapping_vertices() const;
};

class SkelEdge
{
private:
	std::pair<size_t, size_t> edge;	// <source, target>

public:
	SkelEdge();
	SkelEdge(const size_t& source, const size_t& target);
	SkelEdge(const std::pair<size_t, size_t>& eg);

	size_t source() const;
	size_t target() const;
	
	// source & target: node_number, begin with 1 !!!
};

class SkelChain
{
private:
	char chain_type_source = SkelNode::NODE;
	char chain_type_target = SkelNode::NODE;
	std::vector<size_t> chain;

public:
	SkelChain();
	SkelChain(std::vector<size_t> skel_chain, char source_type, char target_type);

	void set_chain(std::vector<size_t> skel_chain);
	void set_type(char source_type, char target_type);

	void get_chain(std::vector<size_t>& skel_chain);
	char get_source_type();
	char get_target_type();
};

class SkelExtn	// skeleton extension
{
private:
	SkelNode skel_node;
	Point	 extn_point;

public:
	SkelExtn();
	SkelExtn(SkelNode& skel_node, Point& point) :skel_node(skel_node), extn_point(point){ }
	
	const SkelNode& get_skel_node() const;
	const Point&	get_extn_point() const;
};


class Skel
{
private:
	v_string  filePath;
	Skeleton  skeleton;

public:
	Skel();
	Skel(Mesh& mesh);
	Skel(Skeleton& skeleton);

	bool	  extract_to_end(Mesh& mesh);
	v_string& get_file_path();
	Skeleton& get_skeleton();
};

class SkelGraph
{
private:
	v_string					   filePath;

	size_t						   node_size = 0;
	size_t						   edge_size = 0;

	size_t						   root_node_number = 0;
	std::vector<size_t>			   top_node_numbers;
	std::vector<size_t>			   intersection_node_numbers;
	std::vector<SkelExtn>		   skel_extensions;

	std::vector<SkelNode>		   skel_nodes;	// node number begins with 1 !!!
	std::vector<SkelEdge>		   skel_edges;	// only contain node numbers
	std::list<SkelChain>		   skel_chains; // source:root/top/intersection -> target:top/intersection

public:
	SkelGraph();
	SkelGraph(Skel& skel, Mesh& mesh);

	void						 set_skel_graph(const Skeleton& mcf_skel, const Triangle_mesh& tmesh);	// main function of SkelGraph
	
	const std::vector<SkelNode>& get_skel_nodes() const;
	const std::vector<SkelEdge>& get_skel_edges() const;
	const std::vector<SkelExtn>& get_skel_extns() const;
	
	const SkelNode&				 get_skel_node(const size_t node_number) const;
	std::vector<Point>&			 get_skel_map(const size_t node_number) const;

	size_t						 get_root_node_number();
	
	double						 get_segment_distance(const std::vector<size_t>& skel_segment) const;
	double						 get_edge_distance(const std::vector<SkelEdge>::iterator& it_eg) const;
	double						 get_node_distance(const size_t nd_nr_1, const size_t nd_nr_2) const;
	double						 get_node_distance(const SkelNode& nd1, const SkelNode& nd2) const;

	// output skel relevant info to files
	void						 output_skel_to_files();
	void						 output_skel_file();
	void						 output_map_file();
	void						 output_extension_file();

	// test
	void						 print_skel_nodes();
	void						 print_skel_edges();
	void						 print_skel_graph();

private:
	size_t contains_node(const Point& p);
	void   set_node_type(size_t nd_nr, char nd_type);
	void   analyse_skel_strucure();
	void   analyse_skel_node_type(
		std::set<size_t>& node_type_top, 
		std::set<size_t>& node_type_intersection);
	size_t analyse_skel_chains(
		const std::set<size_t>& nd_type_top,
		const std::set<size_t>& nd_type_intersection);
};

//class compairList   { public:	bool operator()(std::pair<size_t, size_t> pair1, std::pair<size_t, size_t> pair2); };
class equal_1		{ public:	bool operator()(size_t num); };
class larger_than_1 { public:	bool operator()(size_t num); };
class larger_than_2 { public:	bool operator()(size_t num); };


class SkelMap	// skel_point -> mesh_points
{
private:
	std::map<size_t, std::vector<size_t>> skel_map;	// <skel_node_nr, vector<surface_nodes_nr>>

public:
	SkelMap();
	SkelMap(const Skeleton& mcf_skel, const Triangle_mesh& tmesh);

private:
	void push_back(size_t skel_nr, size_t mesh_nr);
	void sort();

};

END_PROJECT_NAMESPACE
