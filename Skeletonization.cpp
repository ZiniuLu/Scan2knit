/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <math.h>
#include <numeric>

#include "Skeletonization.h"


BEGIN_PROJECT_NAMESPACE

// Node::
Node::Node() { }
Node::Node(const size_t id, const Point& p) { set(id, p); }
Node::Node(const std::pair<size_t, Point>& nd) { set(nd); }

// SkelNode:: : public Node
SkelNode::SkelNode() { }
SkelNode::SkelNode(const size_t nd_nr, const Point& p) : Node(nd_nr, p) { }
SkelNode::SkelNode(const std::pair<size_t, Point>& nd) : Node(nd) { }

// SkelEdge::
SkelEdge::SkelEdge(){ }
SkelEdge::SkelEdge(const size_t& source, const size_t& target) 
{ 
    this->edge = std::make_pair(source, target); 
}
SkelEdge::SkelEdge(const std::pair<size_t, size_t>& eg) { this->edge = eg; }

// SkelChain::
SkelChain::SkelChain()
{
    this->chain.clear();
}
SkelChain::SkelChain(std::vector<size_t> skel_chain, char source_type, char target_type)
{
    this->set_chain(skel_chain);
    this->set_type(source_type, target_type);
}

// SkelExtn::
SkelExtn::SkelExtn(){ }

// Skel::
Skel::Skel() { }
Skel::Skel(Mesh& mesh) { this->extract_to_end(mesh); }
Skel::Skel(Skeleton& skeleton)
{
    //this->filePath.push_back();
    this->skeleton = skeleton;
}




// SkelGraph::
SkelGraph::SkelGraph() { }
SkelGraph::SkelGraph(Skel& skel, Mesh& mesh)
{
    // get file path
    this->filePath = skel.get_file_path();

    // build skel graph
    Skeleton& mcf_skel = skel.get_skeleton();
    //Triangle_mesh& tmesh = mesh.get_tmesh();
    Polyhedron& pmesh = mesh.get_pmesh();
    this->set_skel_graph(mcf_skel, pmesh);
}

// public
void    SkelGraph::set_skel_graph(const Skeleton& mcf_skel, const Polyhedron& pmesh)
{
    // point in skeleton -> called "node"
    // point in mesh     -> called "vertex"

    // 1. get size of nodes and edges
    this->node_size = mcf_skel.m_vertices.size();
    this->edge_size = mcf_skel.m_edges.size();


    auto& skel_nds = this->skel_nodes;
    auto& skel_egs = this->skel_edges;

    // 2. insert nodes from mcf_skel
    auto set_skel_nodes = [&skel_nds](const Skeleton& skel)
    {
        auto& my_nodes = skel.m_vertices;  // skeleton nodes with associated mesh vertices

        auto it_nd_begin = my_nodes.begin();
        auto it_nd_end = my_nodes.end();
        auto it_nd = it_nd_begin;

        for (; it_nd != it_nd_end; it_nd++)
        {
            size_t my_node_nr = it_nd - it_nd_begin + 1; // in mcf_skel, node number starts from 0
            const Point& my_point = it_nd->m_property.point;

            skel_nds.push_back(SkelNode(my_node_nr, my_point));
            //Print("V " + my_node_nr + " :\t" + my_point);
        }
    };

    // 3. insert edges from mcf_skel
    auto set_skel_edges = [&skel_egs](const Skeleton& skel)
    {
        auto& my_edges = skel.m_edges;

        auto it_eg_begin = my_edges.begin();
        auto it_eg_end = my_edges.end();
        auto it_eg = it_eg_begin;

        for (; it_eg != it_eg_end; it_eg++)
        {
            size_t my_source = it_eg->m_source + 1;
            size_t my_target = it_eg->m_target + 1;

            skel_egs.push_back(SkelEdge(my_source, my_target));
            //size_t my_edge_nr = std::distance(it_eg_begin, it_eg) + 1;
            //Print("E " + my_edge_nr + " :\t" + my_source + "\t " + my_target);
        }
    };

    // 4. insert mapping lists of skeleton nodes and corresponding surface vertices
    auto set_skel_maps = [&skel_nds](const Skeleton& skel, const Polyhedron& pmesh)
    {
        auto& my_nodes = skel.m_vertices;

        auto it_nd_begin = my_nodes.begin();
        auto it_nd_end   = my_nodes.end();
        auto it_nd       = it_nd_begin;

        for (; it_nd != it_nd_end; it_nd++)
        {
            size_t offset = it_nd - it_nd_begin;

            const Point& p_skel = it_nd->m_property.point;    // point of current skel node
            SkelNode& nd = skel_nds[offset];          // current skel node
            
            std::vector<std::pair<size_t, Point>> mapping_v_set;

            for (const auto& vd : it_nd->m_property.vertices)
            {
                const Point& v_map_p = get(CGAL::vertex_point, pmesh, vd);
                size_t v_map_id = vd->id();
                mapping_v_set.push_back(std::make_pair(v_map_id, v_map_p));
            }

            nd.set_mapping_vertices(mapping_v_set);
        }
    };

    // 6. find nearst surface point of the extension cord of root/top node
    auto set_skel_extension = [this]
    {
        std::vector<size_t> nr_top;    // node number: root & top
        //std::vector<size_t> nr_next2top; // node connected with top node, in case that no vertex mapping to top node
        nr_top.push_back(this->root_node_number);
        for (auto& nd_nr : top_node_numbers)
        {
            nr_top.push_back(nd_nr);
        }
        //std::copy(this->top_node_numbers.begin(), this->top_node_numbers.end(), std::back_inserter(nr_top));

        // find extension cord
        for (size_t nr : nr_top)
        {
            SkelExtn my_extension;

            const SkelNode& nd_top = this->skel_nodes[nr - 1]; // root/top node of skeleton
            const Point* p_top;
            p_top = &(nd_top.point());

            //Print("TOP node " + nr + ": " + p_top);

            auto& v_maps = nd_top.get_mapping_vertices();

            std::vector<std::pair<size_t, Point>>::const_iterator it_v_begin;
            std::vector<std::pair<size_t, Point>>::const_iterator it_v_end;
            std::vector<std::pair<size_t, Point>>::const_iterator it_v;

            double p_x = 0;
            double p_y = 0;
            double p_z = 0;

            //long double zoom = 0;
            double len_max = 0;  // squared length

            if (v_maps.size() > 0) 
            { 
                it_v_begin = v_maps.begin();
                it_v_end = v_maps.end();
                it_v = it_v_begin;
            }
            else
            {
                // sometimes tope_node does not has mapping vertices
                size_t nr_next2top;
                for (auto skel_chain : this->skel_chains)
                {
                    const std::vector<size_t>& my_chain = skel_chain.get_chain();
                    size_t idx_max = my_chain.size() - 1;

                    if (nr == my_chain[0]) 
                    { 
                        nr_next2top = my_chain[1];
                        break;
                    }
                    else if (nr == my_chain[idx_max]) 
                    { 
                        nr_next2top = my_chain[idx_max - 1];
                        break;
                    }
                }
                const SkelNode& nd_next2top = this->skel_nodes[nr_next2top - 1]; // root/top node of skeleton
                //const Point& p_next2top = nd_next2top.point();
                p_top = &(nd_next2top.point());
                const auto& v_next_maps = nd_next2top.get_mapping_vertices();

                it_v_begin = v_next_maps.begin();
                it_v_end = v_next_maps.end();
                it_v = it_v_begin;
            }

            for (; it_v != it_v_end; it_v++)
            {
                
                const Point* p_map = &((*it_v).second);
                double _x = p_map->x() - p_top->x();
                double _y = p_map->y() - p_top->y();
                double _z = p_map->z() - p_top->z();

                double len = _x * _x + _y * _y + _z * _z;
                len_max = (len_max > len) ? len_max : len;

                p_x += _x;
                p_y += _y;
                p_z += _z;

                //Print("mapping to V\t" + p_map);
                //zoom++;
            }

            long double LEN = p_x * p_x + p_y * p_y + p_z * p_z;
            long double zoom = std::sqrt(LEN / len_max);

            p_x = p_x / zoom + p_top->x();
            p_y = p_y / zoom + p_top->y();
            p_z = p_z / zoom + p_top->z();

            //p_x = p_x / (zoom * 0.9) + p_top.x();
            //p_y = p_y / (zoom * 0.9) + p_top.y();
            //p_z = p_z / (zoom * 0.9) + p_top.z();
            //Print("p_x = " + p_x + "\tp_y = " + p_y + "\tp_z = " + p_z);
            
            const Point p_ext = Point(p_x, p_y, p_z);
            skel_extensions.push_back(SkelExtn(nd_top, p_ext));
        }
    };


    set_skel_nodes(mcf_skel);       // 2.
    set_skel_edges(mcf_skel);       // 3.
    set_skel_maps(mcf_skel, pmesh); // 4.

    // 5. find top_node and intersection_node in current skel
    analyse_skel_strucure();

    set_skel_extension();   // 6.
}

//double    SkelGraph::get_segment_length(const std::vector<size_t>& skel_segment) const
//{
//    double distance = 0;
//
//    auto it_begin = skel_segment.begin();
//    auto it_end   = skel_segment.end();
//    auto it       = it_begin;
//
//    for (; it + 1 != it_end; it++)
//    {
//        size_t s = *it;
//        size_t t = *(it + 1);
//        distance += get_node_length(s, t);
//    }
//
//    return distance;
//}
//double    SkelGraph::get_edge_length(const std::vector<SkelEdge>::iterator& it_eg) const
//{
//    size_t nd_nr_1 = (*it_eg).source();
//    size_t nd_nr_2 = (*it_eg).target();
//    return get_node_length(nd_nr_1, nd_nr_2);
//}
//double    SkelGraph::get_node_length(const size_t nd_nr_1, const size_t nd_nr_2) const
//{
//    const SkelNode& nd1 = get_skel_node(nd_nr_1);
//    const SkelNode& nd2 = get_skel_node(nd_nr_2);
//    return get_node_length(nd1, nd2);
//}
//double    SkelGraph::get_node_length(const SkelNode& nd1, const SkelNode& nd2) const
//{
//    Vector_3 v(nd1.point(), nd2.point());
//    return std::sqrt(v.squared_length());
//}

// output skel relevant info to files
void    SkelGraph::output_skel_graph_to_files(Settings* settings)
{
    Print("\t\toutput skeleton_graph ...");

    //std::string path = this->filePath[0];
    Print("\t\t\tgenerating files under \"" + this->filePath[0] + "\"");
    output_skel_to_file(settings);
    output_skel_map_to_file(settings);
    output_skel_ext_to_file(settings);

    Print("\t\tdone.");
}
void    SkelGraph::output_skel_to_file(Settings* settings)
{
    std::string path = this->filePath[0] 
                    + this->filePath[1]
                    + settings->Skel_out.out_suffix_skel 
                    + settings->Skel_out.out_file_suffix;//"_skel.obj";
    Print("\t\t\tsaving skel to \"" + path + "\" ... ");

    std::ofstream output(path);
    for (const SkelNode& nd : this->skel_nodes)
    {
        output << "v " << nd.point() << "\n";
    }
    for (const SkelEdge& eg : this->skel_edges)
    {
        output << "l " << eg.source() << " " << eg.target() << "\n";
    }
    output.close();

    //Print("\t\t\tdone.");
}
void    SkelGraph::output_skel_map_to_file(Settings* settings)
{
    std::string path = this->filePath[0] 
                    + this->filePath[1]
                    + settings->Skel_out.out_suffix_skel_map 
                    + settings->Skel_out.out_file_suffix;//"_map.obj";
    Print("\t\t\tsaving skel_map to \"" + path + "\" ... ");
    
    std::ofstream      output(path);
    std::ostringstream eg_map;

    // print all skel_nodes
    for (const SkelNode& nd : this->skel_nodes)
    {
        output << "v " << nd.point() << "\n";
    }

    // output mapping vertices & edges
    size_t mp_nr = this->node_size + 1;
    for (const SkelNode& nd : this->skel_nodes)
    {
        size_t nd_nr = nd.get_id(); // start from 1
        for (const auto& mp : nd.get_mapping_vertices())
        {
            output << "v " << mp.second << "\n";
            eg_map << "l " << nd_nr << " " << mp_nr << "\n";
            mp_nr++;
        }
    }

    output << eg_map.str();
    output.close();

    //Print("\t\t\tdone.");
}
void    SkelGraph::output_skel_ext_to_file(Settings* settings)
{
    std::string path = this->filePath[0]
                    + this->filePath[1]
                    + settings->Skel_out.out_suffix_skel_ext
                    + settings->Skel_out.out_file_suffix;//"_ext.obj";
    Print("\t\t\tsaving skel_ext to \"" + path + "\" ... ");

    std::ofstream output(path);
    std::vector<size_t> nds_top;    // node numbers: root/top
    std::vector<size_t> nds_n2top;    // node numbers: node next to root/top

    
    
    auto it_chains_begin = this->skel_chains.begin();
    auto it_chains_end   = this->skel_chains.end();
    auto it_chains       = it_chains_begin;

    size_t max = this->skel_extensions.size();
    for (size_t i = 0; i < max; i++)
    {
        nds_top.push_back(this->skel_extensions[i].get_skel_node_id());
    }

    for (size_t i = 0; i < nds_top.size(); i++)
    {
        for (it_chains = it_chains_begin; it_chains != it_chains_end; it_chains++)
        {
            if (nds_top[i] == it_chains->get_source_node())
            {
                nds_n2top.push_back(it_chains->get_node_next2source());
                break;
            }
            else if (nds_top[i] == it_chains->get_target_node())
            {
                nds_n2top.push_back(it_chains->get_node_next2target());
                break;
            }
        }
    } 
    // output vertices
    
    for (size_t i = 0; i < max; i++)
    {
        output << "v " << this->skel_nodes[nds_top[i] - 1].point() << "\n";
        output << "v " << this->skel_nodes[nds_n2top[i] - 1].point() << "\n";
        output << "v " << this->skel_extensions[i].get_extn_point() << "\n";
    }

    for (size_t i = 0; i < max; i++)
    {
        output << "l " << 3 * i + 1 << " " << 3 * i + 2 << "\n";
        output << "l " << 3 * i + 1 << " " << 3 * i + 3 << "\n";
    }

    output.close();

    //Print("\t\t\tdone.");
}

// test
void    SkelGraph::print_skel_nodes()
{
    Print("\tsize of skeleton nodes: " + this->node_size);
    for (SkelNode& n : this->skel_nodes)
    {
        Print("\tV " + std::to_string(n.get_id()) + ": \t"
                   + std::to_string(n.point().x()) + "   \t" 
                   + std::to_string(n.point().y()) + "   \t"
                   + std::to_string(n.point().z()) + "   \t"
                   + "Type: " + n.get_type());
    }

    Print("\tdone.\n");
}
void    SkelGraph::print_skel_edges()
{
    Print("\tsize of skeleton edges: " + this->edge_size);
    for (SkelEdge& e : this->skel_edges)
    {
        Print("\tE ("
                  + std::to_string(e.source()) + "\t, " 
                  + std::to_string(e.target()) + ")");
    }

    Print("\tdone.\n");

}
void    SkelGraph::print_skel_graph()
{
    Print("print skel graph ... ");
    this->print_skel_nodes();
    this->print_skel_edges();
    Print("print skel graph done.");
}

// private
size_t  SkelGraph::contains_node(const Point& p)
{
    auto it_nd_begin = skel_nodes.begin();
    auto it_nd_end   = skel_nodes.end();
    auto it_nd       = it_nd_begin;

    for (; it_nd < it_nd_end; it_nd++)
    {
        const Point& node = (*it_nd).point();

        if (node.x() != p.x()) { continue; }
        else if (node.y() != p.y()) { continue; }
        else if (node.z() != p.z()) { continue; }
        else { return it_nd - it_nd_begin + 1; }    // return node_number, begins with 1 !!!
    }

    return 0;
}

void    SkelGraph::set_node_type(size_t nd_nr, char nd_type)
{
    auto it_nd = skel_nodes.begin();
    std::advance(it_nd, nd_nr - 1);
    (*it_nd).set_type(nd_type);
}

void    SkelGraph::analyse_skel_strucure()
{
    std::set<size_t> node_type_top;
    std::set<size_t> node_type_intersection;

    // 1. find Top && Intersection node
    analyse_skel_node_type(node_type_top, node_type_intersection);
    // 2. analyse skel chains
    analyse_skel_chains(node_type_top, node_type_intersection);
}

void    SkelGraph::analyse_skel_node_type(std::set<size_t>& node_type_top, std::set<size_t>& node_type_intersection)
{
    if (this->node_size == 0) { return; }

    // 1. caculate the number of times each node appears in all edges
    std::vector<int> node_frequency(this->node_size);

    for (SkelEdge e : this->skel_edges)
    {
        size_t i_s = e.source() - 1;
        size_t i_t = e.target() - 1;
        node_frequency[i_s] += 1;
        node_frequency[i_t] += 1;
    }

    // 2. set node type
    //     if node_frequency > 2 : intersection node -> [I]
    //     if node_frequency = 2 : normal node       -> [N]
    //     if node_frequency = 1 : top node          -> [T] // contains root node [R]
    
    node_type_top.clear();
    node_type_intersection.clear();

    auto it_begin = node_frequency.begin();
    auto it_end   = node_frequency.end();
    auto it_      = it_begin;

    auto it_equal_1       = std::find_if(it_begin, it_end, equal_1);          // = 1
    auto it_larger_than_2 = std::find_if(it_begin, it_end, larger_than_2);    // > 2

    while (it_ != it_end)
    {
        if (it_equal_1 < it_larger_than_2)
        {
            it_ = it_equal_1;
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_top.insert(node_nr);
            this->skel_nodes[node_nr - 1].set_type(SkelNode::TOP);
            it_++;
            it_equal_1 = std::find_if(it_, it_end, equal_1);
        }
        else if (it_larger_than_2 < it_equal_1)
        {
            it_ = it_larger_than_2;
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_intersection.insert(node_nr);
            this->skel_nodes[node_nr - 1].set_type(SkelNode::INTERSECTION);
            it_++;
            it_larger_than_2 = std::find_if(it_, it_end, larger_than_2);
        }
        else
        {
            it_ = it_end;
        }
    }
}

size_t  SkelGraph::analyse_skel_chains(const std::set<size_t>& nd_type_top, const std::set<size_t>& nd_type_intersection)
{
    if (this->node_size == 0 || this->edge_size == 0) { return 0; }

    size_t nd_top_size          = nd_type_top.size();
    size_t nd_intersection_size = nd_type_intersection.size();

    //Print("\n");
    std::ostringstream text1;
    text1 << "\t\t\tnumber of [T]op node(s): " << nd_top_size << "\t\t\t\t\t - { ";
    //std::ostringstream text2;
    for (size_t nd : nd_type_top) { text1 << "[" << nd << "] "; }
    text1 << "}\n";
    Print(text1.str());
    text1.clear();

    std::ostringstream text2;
    text2 << "\t\t\tnumber of [I]ntersection node(s): " << nd_intersection_size << "\t - { ";
    for (size_t nd : nd_type_intersection) { text2 << "[" << nd << "] "; }
    text2 << "}\n";
    Print(text2.str());
    text2.clear();

    // 1. copy all edge sources & targets into a vector
    std::vector<size_t> edges_s_t;

    auto it_eg_begin = this->skel_edges.begin();
    auto it_eg_end = this->skel_edges.end();
    auto it_eg = it_eg_begin;

    for (; it_eg != it_eg_end; it_eg++)
    {
        edges_s_t.push_back((*it_eg).source());
        edges_s_t.push_back((*it_eg).target());
    }

    // 2. find all top-chains: chains from node_top to node_intersection
    std::vector<size_t> chain_current;
    char                chain_source_type;
    char                chain_target_type;
    size_t              p_current;
    int i = 1;

    Print("\n\t\tAnalyzing skel_chains ...");
    Print("\t\t\t[R] - root_node\t\t[T] - top_node\t\t[I] - intersection_node");
    Print("\t\t\tskel_chain: [T] -> [T] or [T] -> [I] ...");

    chain_current.clear();
    
    for (size_t p_top : nd_type_top)
    {
        // add first node
        p_current = p_top; 
        chain_current.push_back(p_current);   // the first node in current chain
        chain_source_type = SkelNode::TOP;

        std::ostringstream text3;
        text3 << "\t\t\tskel_chain " << i << ": \t[T]-[" << p_current << "] -> ";

        // add subsequent nodes
        while (true)
        {
            auto it_st_begin = edges_s_t.begin();
            auto it_st_end   = edges_s_t.end();
            auto it_st       = find(it_st_begin, it_st_end, p_current); // find an edge contains p_current

            // no connection
            if (it_st == it_st_end)
            {
                text3 << "no connection!";
                chain_current.clear();
                break;
            }

            if ((it_st - it_st_begin) % 2 == 1)             // p_current is a target
            {
                p_current = *(it_st - 1);
                chain_current.push_back(p_current);         // add the node into current chain
                edges_s_t.erase(it_st - 1, it_st + 1);      // remove current edge
            }
            else                                            
            {                                               // p_current is a source
                p_current = *(it_st + 1);
                chain_current.push_back(p_current);
                edges_s_t.erase(it_st, it_st + 2);          // remove current edge
            }

            // check if p_current is the last node
            //      if: p_current is an intersection node
            auto it_its = nd_type_intersection.find(p_current); // check if p_current is one of the intersection node
            if (it_its != nd_type_intersection.end())       
            {
                chain_target_type = SkelNode::INTERSECTION;
                text3 << "[" << p_current << "]-[I]";
                break;                                      // build connection between node_top and node_intersection
            }

            //      if: p_current is a top node
            auto it_top = nd_type_top.find(p_current);
            if (it_top != nd_type_top.end())
            {
                chain_target_type = SkelNode::TOP;
                text3 << "[" << p_current << "]-[T]";
                break;
            }

            text3 << p_current << " -> "; // p_current is a normal node
        } // end of while

        text3 << "\n";
        Print(text3.str());
        text3.clear();

        if (chain_current.size() > 0)
        {
            SkelChain skel_chain;
            skel_chain.set_chain(chain_current);
            skel_chain.set_type(chain_source_type,chain_target_type);
            this->skel_chains.push_back(skel_chain);
            // reset
            chain_current.clear();
            chain_source_type = SkelNode::NODE;
            chain_target_type = SkelNode::NODE;
        }
        i++;
    } // end of for

    // 3. find all chains of intersection_node: from intersection_node to intersection_node
    Print("\n\t\t\tskel_chain: [I]->[I] ...");

    chain_source_type = SkelNode::INTERSECTION;
    chain_target_type = SkelNode::INTERSECTION;

    // iterator of intersection node
    auto it_intersection_begin = nd_type_intersection.begin();
    auto it_intersection_end   = nd_type_intersection.end();
    auto it_intersection       = it_intersection_begin;   // current intersection node

    while(true)
    {
        if (it_intersection == it_intersection_end) { break; }

        chain_current.clear();
        p_current = *it_intersection;

        // iterator of the vector incl. all edge sources & targets
        auto it_st_begin = edges_s_t.begin();
        auto it_st_end   = edges_s_t.end();
        auto it_st       = find(it_st_begin, it_st_end, p_current);
        
        // if current intersection node does not exist in edges_s_t
        if (it_st == it_st_end)
        {
            it_intersection++;
            continue;
        }


        std::ostringstream text4;
        text4 << "\t\t\tskel_chain " << i << ": \t[I]-[" << p_current << "] -> ";
        chain_current.push_back(*it_intersection);   // the first intersection node

        while (true)
        {
            if ((it_st - it_st_begin) % 2 == 1)             // if p_current is a target
            {
                p_current = *(it_st - 1);
                chain_current.push_back(p_current);
                edges_s_t.erase(it_st - 1, it_st + 1);      // remove current edge
            }
            else                                            // if p_current is a source
            {
                p_current = *(it_st + 1);
                chain_current.push_back(p_current);
                edges_s_t.erase(it_st, it_st + 2);          // remove current edge
            }

            // if p_current is one of the intersection nodes: end of current chain
            if (nd_type_intersection.find(p_current) != nd_type_intersection.end())
            {
                text4 << "[" << p_current << "]-[I]";
                break;
            }

            text4 << p_current << " -> "; 

            it_st_begin = edges_s_t.begin();
            it_st_end   = edges_s_t.end();
            it_st       = find(it_st_begin, it_st_end, p_current);

            if (it_st == it_st_end)
            {
                text4 << "\nERROR: cannot find next connection of node" << p_current <<"\n";
                break;
            }
        } // end of while

        text4 << "\n";
        Print(text4.str());
        text4.clear();

        if (chain_current.size() > 0)
        {
            SkelChain skel_chain;
            skel_chain.set_chain(chain_current);
            skel_chain.set_type(chain_source_type, chain_target_type);
            this->skel_chains.push_back(skel_chain);
            // reset
            chain_current.clear();
        }

        i++;
    } // end of while

    // 4. caculate node amount and vertex amount in each top chain
    Print("\n\t\tAnalyzing size of each chain begins with [T]: ");
    Print("\t\t\t[T]:{ number of skel_node, number of associated mesh_vertex }");
    std::vector<size_t> chain_nd_amount;    // node_amount in each chain
    std::vector<size_t> chain_vtx_amount;   // mesh_vertex_amount in each chain

    auto it_chains_begin = this->skel_chains.begin();
    auto it_chains_end = this->skel_chains.end();
    auto it_chains       = it_chains_begin;
    
    std::ostringstream text5;
    for (; it_chains != it_chains_end; it_chains++)
    {
        //if (it_chains->get_source_type() != SkelNode::TOP) { continue; }

        const std::vector<size_t>& my_chain = it_chains->get_chain();

        // node amount
        size_t nd_amount = my_chain.size();
        chain_nd_amount.push_back(nd_amount);

        // vertex amount
        auto it_chain_nd_begin = my_chain.begin();
        auto it_chain_nd_end   = my_chain.end();
        auto it_chain_nd       = it_chain_nd_begin;
        
        std::vector<size_t> vtx_amount;

        for (; it_chain_nd != it_chain_nd_end; it_chain_nd++)
        {
            SkelNode& my_nd = (this->skel_nodes)[*it_chain_nd - 1];
            vtx_amount.push_back(my_nd.get_mapping_vertices().size());
        }

        if (it_chains->get_source_type() == SkelNode::TOP)
        {
            auto it_vtx_begin = vtx_amount.begin();
            auto it_vtx_end = vtx_amount.end();
            if (it_chains->get_source_type() == SkelNode::INTERSECTION) 
            { 
                it_vtx_end--; 
            }
            chain_vtx_amount.push_back(std::accumulate(it_vtx_begin, it_vtx_end, 0));
        }
            
        else
            chain_vtx_amount.push_back(0); // [I]-chain should not contain R-node

        // output
        it_chains->set_node_amount(nd_amount);
        it_chains->set_vertex_amount(vtx_amount);

        if (it_chains->get_source_type() == SkelNode::TOP)
            text5 << "\t\t\t[T]-[";
        else if(it_chains->get_source_type() == SkelNode::INTERSECTION)
            text5 << "\t\t\t[I]-[";

        text5 << *(my_chain.begin()) << "]:{ " 
            << it_chains->get_node_amount() << ", " 
            << it_chains->get_vertex_amount() << " }" << "\t";
    }

    text5 << "\n";
    Print(text5.str());
    text5.clear();

    // 5. set node type: find ROOT node from TOP node
    auto it_vtx_am_begin = chain_vtx_amount.begin();
    auto it_vtx_am_end   = chain_vtx_amount.end();
    auto offset = std::max_element(it_vtx_am_begin, it_vtx_am_begin) - it_vtx_am_begin;
    
    auto it_chain_root = it_chains_begin;
    std::advance(it_chain_root, offset);
    
    const std::vector<size_t>& chain_root = it_chain_root->get_chain();
    this->root_node_number = chain_root[0];

    set_node_type(root_node_number, SkelNode::ROOT);
    std::ostringstream text6;
    text6 << "\n\t\troot node: [R]-[" << root_node_number << "]";
    Print(text6.str());
    text6.clear();
    Print("\n");

    for (size_t t : nd_type_top)
        if (t != root_node_number)
            this->top_node_numbers.push_back(t);

    for (size_t its : nd_type_intersection)
        this->intersection_node_numbers.push_back(its);

    for (auto& my_chain : this->skel_chains)
    {
        if (my_chain.get_source_node() == this->root_node_number)
        {
            my_chain.set_source_type(SkelNode::ROOT);
            break;
        }
        else if (my_chain.get_target_node() == this->root_node_number)
        {
            my_chain.reverse();
            my_chain.set_source_type(SkelNode::ROOT);
            break;
        }
    }

    // 6. set id of each skel_chain
    std::vector<size_t>     source_nodes;
    std::vector<size_t>     target_nodes;
    std::vector<SkelChain>  sorted_chains;
    size_t n = 0;

    source_nodes.push_back(this->root_node_number);

    while (true)
    {
        if (sorted_chains.size() >= this->skel_chains.size()) 
            break;

        for (auto& my_chain : this->skel_chains)
        {
            if (my_chain.get_id() > 0) continue;
            
            auto it_find_target = std::find(source_nodes.begin(), source_nodes.end(), my_chain.get_target_node());
            if (it_find_target != source_nodes.end()) 
            { 
                my_chain.reverse(); 
            }
                
            auto it_find_source = std::find(source_nodes.begin(), source_nodes.end(), my_chain.get_source_node());
            if (it_find_source != source_nodes.end())
            {
                my_chain.set_id(1); // 1 is not the real id, here only to mark if current chain is sorted. This can avoid the situation of merged skel_tree_branches
                target_nodes.push_back(my_chain.get_target_node());
                sorted_chains.push_back(my_chain);
            }
        }

        std::sort(sorted_chains.begin() + n, sorted_chains.end(), sort_chain_increase);
        source_nodes.clear();
        std::swap(source_nodes, target_nodes);
        n = sorted_chains.size();
    }

    for (size_t id = 0; id < sorted_chains.size(); id++)
    {
        sorted_chains[id].set_id(id + 1);
    }

    std::swap(this->skel_chains, sorted_chains);

    Print("\n\t\tSorting skel_chains ...");
    Print("\t\t\t[R] - root_node\t\t[T] - top_node\t\t[I] - intersection_node");
    Print("\t\t\tskel_chain: [R] -> [T] or [R] -> [I] or [I] -> [I] or [I] -> [T]...");
    
    std::ostringstream text7;
    for (const auto& my_chain : this->skel_chains)
    {
        text7 << "\t\t\tskel_chain: id = " << my_chain.get_id() << "; "
            << "\tsource_node = [" << my_chain.get_source_type() << "]-[" << my_chain.get_source_node() << "];"
            << "\ttarget_node = [" << my_chain.get_target_type() << "]-[" << my_chain.get_target_node() << "];"
            << "\tnode_amount = " << my_chain.get_node_amount() << ";"
            << "\tvertex_amount = " << my_chain.get_vertex_amount() << "\n";
    }
    Print(text7.str());
    //std::cout << text7.str() << std::endl;
    text7.clear();

    return this->root_node_number;
}



// globale functions
bool    sort_chain_increase(const SkelChain& chain1, const SkelChain& chain2)
{
    if (chain1.get_source_node() == chain2.get_source_node())
    {
        if (chain1.get_target_type() == SkelNode::TOP && chain2.get_target_type() != SkelNode::TOP)
        {
            return true;
        }
        else if (chain1.get_target_type() != SkelNode::TOP && chain2.get_target_type() == SkelNode::TOP)
        {
            return false;
        }
        else
        {
            return chain1.get_vertex_amount() <= chain2.get_vertex_amount();
        }
    }
    else
    {
        return chain1.get_source_node() <= chain2.get_source_node();
    }
}

END_PROJECT_NAMESPACE