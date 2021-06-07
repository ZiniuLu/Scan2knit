/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <math.h>

#include "Skeletonization.h"


BEGIN_PROJECT_NAMESPACE


// Node::
Node::Node() { }
Node::Node(const size_t nd_nr, const Point& p) { set(nd_nr, p); }
Node::Node(const std::pair<size_t, Point>& nd) { set(nd); }

void         Node::set(const size_t nd_nr, const Point& p) { this->node = std::make_pair(nd_nr, p); }
void         Node::set(const std::pair<size_t, Point>& nd) { this->node = nd; }

size_t       Node::node_number() const { return this->node.first; }

const Point& Node::point() const { return this->node.second; }




// SkelNode:: : public Node
SkelNode::SkelNode() { }
SkelNode::SkelNode(const size_t nd_nr, const Point& p) : Node(nd_nr, p) { }
SkelNode::SkelNode(const std::pair<size_t, Point>& nd) : Node(nd) { }

void                      SkelNode::set_type(const char nd_type) { this->type = nd_type; }
char                      SkelNode::get_type() { return this->type; }

void                      SkelNode::set_mapping_vertices(const std::vector<Point>& v_map)
{
    this->mapping_vertices = v_map;
}
const std::vector<Point>& SkelNode::get_mapping_vertices() const
{
    return this->mapping_vertices;
}




// SkelEdge::
SkelEdge::SkelEdge(){ }
SkelEdge::SkelEdge(const size_t& source, const size_t& target) 
{ 
    this->edge = std::make_pair(source, target); 
}
SkelEdge::SkelEdge(const std::pair<size_t, size_t>& eg) { this->edge = eg; }

size_t SkelEdge::source() const { return this->edge.first; }
size_t SkelEdge::target() const { return this->edge.second; }



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

void SkelChain::set_chain(std::vector<size_t> skel_chain)
{
    this->chain.clear();
    this->chain = skel_chain;
}
void SkelChain::set_type(char source_type, char target_type)
{
    this->chain_type_source = source_type;
    this->chain_type_target = target_type;
}

void SkelChain::get_chain(std::vector<size_t>& skel_chain)
{
    skel_chain = this->chain;
}
char SkelChain::get_source_type()
{
    return this->chain_type_source;
}
char SkelChain::get_target_type()
{
    return this->chain_type_target;;
}




// SkelExtn::
SkelExtn::SkelExtn(){ }
const SkelNode& SkelExtn::get_skel_node()  const { return this->skel_node; }
const Point&    SkelExtn::get_extn_point() const { return this->extn_point; }




// Skel::
Skel::Skel() { }
Skel::Skel(Mesh& mesh) { this->extract_to_end(mesh); }
Skel::Skel(Skeleton& skeleton)
{
    //this->filePath.push_back();
    this->skeleton = skeleton;
}

bool      Skel::extract_to_end(Mesh& mesh)
{
    if (!mesh.is_triangle_mesh()) { return false; }

    this->filePath = mesh.get_file_path();

    // 1. extract skeleton;
    Print("Extracting skeletons ... ");
    auto& tmesh = mesh.get_tmesh();
    CGAL::extract_mean_curvature_flow_skeleton(tmesh, this->skeleton); // caculate object skeleton
    Print("done.\n");

    // 3. caculate V & E numbers
    size_t numV_skel = boost::num_vertices(this->skeleton);
    size_t numE_skel = boost::num_edges(this->skeleton);
    
    Print("number of vertices: " + numV_skel);
    Print("number of edges: " + numE_skel);
}

v_string& Skel::get_file_path() { return this->filePath; }
Skeleton& Skel::get_skeleton() { return this->skeleton; }




// SkelGraph::
SkelGraph::SkelGraph() { }
SkelGraph::SkelGraph(Skel& skel, Mesh& mesh)
{
    // get file path
    this->filePath = skel.get_file_path();

    // build skel graph
    Skeleton& mcf_skel = skel.get_skeleton();
    Triangle_mesh& tmesh = mesh.get_tmesh();
    this->set_skel_graph(mcf_skel, tmesh);
}

// public
// create skel graph
void                         SkelGraph::set_skel_graph(const Skeleton& mcf_skel, const Triangle_mesh& tmesh)
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

        for (; it_nd != it_nd_end; ++it_nd)
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

        for (; it_eg != it_eg_end; ++it_eg)
        {
            size_t my_source = it_eg->m_source + 1;
            size_t my_target = it_eg->m_target + 1;

            skel_egs.push_back(SkelEdge(my_source, my_target));
            //size_t my_edge_nr = std::distance(it_eg_begin, it_eg) + 1;
            //Print("E " + my_edge_nr + " :\t" + my_source + "\t " + my_target);
        }
    };

    // 4. insert mapping lists of skeleton nodes and corresponding surface vertices
    auto set_skel_maps = [&skel_nds](const Skeleton& skel, const Triangle_mesh& tmesh)
    {
        auto& my_nodes = skel.m_vertices;

        auto it_nd_begin = my_nodes.begin();
        auto it_nd_end   = my_nodes.end();
        auto it_nd       = it_nd_begin;

        for (; it_nd != it_nd_end; ++it_nd)
        {
            size_t offset = it_nd - it_nd_begin;

            const Point& p_skel = it_nd->m_property.point;    // current skel node
            SkelNode& nd = skel_nds[offset];          // current skel node
            
            std::vector<Point> mapping_v_set;

            for (const auto& vd : it_nd->m_property.vertices)
            {
                const Point& v_map = get(CGAL::vertex_point, tmesh, vd);
                mapping_v_set.push_back(v_map);
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

            SkelNode& nd_top = this->skel_nodes[nr - 1]; // root/top node of skeleton
            const Point& p_top = nd_top.point();

            //Print("TOP node " + nr + ": " + p_top);

            auto& v_maps = nd_top.get_mapping_vertices();

            std::vector<Point>::const_iterator it_v_begin;
            std::vector<Point>::const_iterator it_v_end;
            std::vector<Point>::const_iterator it_v;

            double p_x = 0;
            double p_y = 0;
            double p_z = 0;

            long double zoom = 0;

            if (v_maps.size() > 0) 
            { 
                it_v_begin = v_maps.begin();
                it_v_end = v_maps.end();
                it_v = it_v_begin;
            }
            else
            {
                size_t nr_next2top;
                for (auto skel_chain : this->skel_chains)
                {
                    std::vector<size_t> my_chain;
                    skel_chain.get_chain(my_chain);
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
                const Point& p_next2top = nd_next2top.point();
                const auto& v_next_maps = nd_next2top.get_mapping_vertices();

                it_v_begin = v_next_maps.begin();
                it_v_end = v_next_maps.end();
                it_v = it_v_begin;
            }

            for (; it_v != it_v_end; ++it_v)
            {
                
                const Point& p_map = *it_v;
                p_x += p_map.x() - p_top.x();
                p_y += p_map.y() - p_top.y();
                p_z += p_map.z() - p_top.z();

                //Print("mapping to V\t" + p_map);

                ++zoom;

                //Print("\tp_x = " + p_x + "\tp_y = " + p_y + "\tp_z = " << p_z);
            }

            p_x = p_x / (zoom * 0.9) + p_top.x();
            p_y = p_y / (zoom * 0.9) + p_top.y();
            p_z = p_z / (zoom * 0.9) + p_top.z();
            //Print("p_x = " + p_x + "\tp_y = " + p_y + "\tp_z = " + p_z);
            
            skel_extensions.push_back(SkelExtn(nd_top, Point(p_x, p_y, p_z)));
        }
    };


    set_skel_nodes(mcf_skel);
    set_skel_edges(mcf_skel);
    set_skel_maps(mcf_skel, tmesh);

    // 5. find top_node and intersection_node in current skel
    analyse_skel_strucure();

    set_skel_extension();
}

const std::vector<SkelNode>& SkelGraph::get_skel_nodes() const { return this->skel_nodes; }
const std::vector<SkelEdge>& SkelGraph::get_skel_edges() const { return this->skel_edges; }
const std::vector<SkelExtn>& SkelGraph::get_skel_extns() const { return this->skel_extensions; }


const SkelNode&              SkelGraph::get_skel_node(const size_t node_number) const
{
    return this->skel_nodes[node_number - 1];
}
size_t                       SkelGraph::get_root_node_number()
{
    if (this->root_node_number == 0)
    {
        analyse_skel_strucure();
    }
    return this->root_node_number;
}

double                       SkelGraph::get_segment_distance(const std::vector<size_t>& skel_segment) const
{
    double distance = 0;

    auto it_begin = skel_segment.begin();
    auto it_end   = skel_segment.end();
    auto it       = it_begin;

    for (; it + 1 != it_end; ++it)
    {
        size_t s = *it;
        size_t t = *(it + 1);
        distance += get_node_distance(s, t);
    }

    return distance;
}
double                       SkelGraph::get_edge_distance(const std::vector<SkelEdge>::iterator& it_eg) const
{
    size_t nd_nr_1 = (*it_eg).source();
    size_t nd_nr_2 = (*it_eg).target();
    return get_node_distance(nd_nr_1, nd_nr_2);
}
double                       SkelGraph::get_node_distance(const size_t nd_nr_1, const size_t nd_nr_2) const
{
    const SkelNode& nd1 = get_skel_node(nd_nr_1);
    const SkelNode& nd2 = get_skel_node(nd_nr_2);
    return get_node_distance(nd1, nd2);
}
double                       SkelGraph::get_node_distance(const SkelNode& nd1, const SkelNode& nd2) const
{
    Vector_3 v(nd1.point(), nd2.point());
    return std::sqrt(v.squared_length());
}

// output skel relevant info to files
void                         SkelGraph::output_skel_to_files()
{
    Print("output skeleton graph to files ...");
    output_skel_file();
    output_map_file();
    output_extension_file();

    Print("output files done.\n");
}
void                         SkelGraph::output_skel_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-skel.obj";

    Print("\tsaving all skeleton points and skeleton curves to\t\t\t \"" + path + "\" ... ");

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

    Print("\tdone.\n");
}
void                         SkelGraph::output_map_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-map.obj";
    Print("\tsaving all mapping lists of skeleton points and surface points to\t \"" + path + "\" ... ");
    std::ofstream output(path);
    std::string eg_map;
    for (const SkelNode& nd : this->skel_nodes)
    {
        output << "v " << nd.point() << "\n";
    }

    // output mapping vertices & edges
    size_t mp_nr = this->node_size + 1;
    for (const SkelNode& nd : this->skel_nodes)
    {
        size_t nd_nr = nd.node_number();
        for (const Point& mp : nd.get_mapping_vertices())
        {
            output << "v " << mp << "\n";
            eg_map += "l " + std::to_string(nd_nr) + " " + std::to_string(mp_nr) + "\n";
            ++mp_nr;
        }
    }

    output << eg_map;
    output.close();

    Print("\tdone.\n");

}
void                         SkelGraph::output_extension_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-ext.obj";

    Print("\tsaving all skeleton extension points to\t\t\t\t\t \"" + path + "\" ... ");

    std::ofstream output(path);


    std::vector<size_t> nr_curr;    // node numbers: root/top
    std::vector<size_t> nr_next;    // node numbers: node next to root/top
    nr_curr.push_back(this->root_node_number);
    std::copy(this->top_node_numbers.begin(), this->top_node_numbers.end(), std::back_inserter(nr_curr));

    auto it_chains_begin = this->skel_chains.begin();
    auto it_chains_end = this->skel_chains.end();
    auto it_chains = it_chains_begin;

    for (size_t nd_nr : nr_curr)
    {
        for (it_chains = it_chains_begin; it_chains != it_chains_end; ++it_chains)
        {
            std::vector<size_t> chain_curr;
            (*it_chains).get_chain(chain_curr);
            if (chain_curr[0] == nd_nr)
            {
                nr_next.push_back(chain_curr[1]);
                break;
            }
        }
    }

    // output vertices
    size_t max = this->skel_extensions.size();
    for (size_t i = 0; i < max; ++i)
    {
        output << "v " << this->skel_nodes[nr_curr[i] - 1].point() << "\n";
        output << "v " << this->skel_nodes[nr_next[i] - 1].point() << "\n";
        output << "v " << this->skel_extensions[i].get_extn_point() << "\n";
    }

    for (size_t i = 0; i < max; ++i)
    {
        output << "l " << 3 * i + 1 << " " << 3 * i + 2 << "\n";
        output << "l " << 3 * i + 1 << " " << 3 * i + 3 << "\n";
    }

    output.close();

    Print("\tdone.\n");
}

//test
void                         SkelGraph::print_skel_nodes()
{
    Print("\tsize of skeleton nodes: " + this->node_size);
    for (SkelNode& n : this->skel_nodes)
    {
        Print("\tV " + std::to_string(n.node_number()) + ": \t"
                   + std::to_string(n.point().x()) + "   \t" 
                   + std::to_string(n.point().y()) + "   \t"
                   + std::to_string(n.point().z()) + "   \t"
                   + "Type: " + n.get_type());
    }

    Print("\tdone.\n");
}
void                         SkelGraph::print_skel_edges()
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
void                         SkelGraph::print_skel_graph()
{
    Print("print skel graph ... ");
    this->print_skel_nodes();
    this->print_skel_edges();
    Print("print skel graph done.");
}



// private
size_t SkelGraph::contains_node(const Point& p)
{
    auto it_nd_begin = skel_nodes.begin();
    auto it_nd_end   = skel_nodes.end();
    auto it_nd       = it_nd_begin;

    for (; it_nd < it_nd_end; ++it_nd)
    {
        const Point& node = (*it_nd).point();

        if (node.x() != p.x()) { continue; }
        else if (node.y() != p.y()) { continue; }
        else if (node.z() != p.z()) { continue; }
        else { return it_nd - it_nd_begin + 1; }    // return node_number, begins with 1 !!!
    }

    return 0;
}

void   SkelGraph::set_node_type(size_t nd_nr, char nd_type)
{
    auto it_nd = skel_nodes.begin();
    std::advance(it_nd, nd_nr - 1);
    (*it_nd).set_type(nd_type);
}

void   SkelGraph::analyse_skel_strucure()
{
    std::set<size_t> node_type_top;
    std::set<size_t> node_type_intersection;

    // 1. find Top && Intersection node
    analyse_skel_node_type(node_type_top, node_type_intersection);
    // 2. analyse skel chains
    analyse_skel_chains(node_type_top, node_type_intersection);
}

void   SkelGraph::analyse_skel_node_type(std::set<size_t>& node_type_top, std::set<size_t>& node_type_intersection)
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

    auto it_equal_1       = std::find_if(it_begin, it_end, equal_1());          // = 1
    auto it_larger_than_2 = std::find_if(it_begin, it_end, larger_than_2());    // > 2

    while (it_ != it_end)
    {
        if (it_equal_1 < it_larger_than_2)
        {
            it_ = it_equal_1;
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_top.insert(node_nr);
            this->skel_nodes[node_nr - 1].set_type(SkelNode::TOP);
            ++it_;
            it_equal_1 = std::find_if(it_, it_end, equal_1());
        }
        else if (it_larger_than_2 < it_equal_1)
        {
            it_ = it_larger_than_2;
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_intersection.insert(node_nr);
            this->skel_nodes[node_nr - 1].set_type(SkelNode::INTERSECTION);
            ++it_;
            it_larger_than_2 = std::find_if(it_, it_end, larger_than_2());
        }
        else
        {
            it_ = it_end;
        }
    }
}

size_t SkelGraph::analyse_skel_chains(
    const std::set<size_t>& nd_type_top,
    const std::set<size_t>& nd_type_intersection)
{
    if (this->node_size == 0 || this->edge_size == 0) { return 0; }

    size_t nd_top_size          = nd_type_top.size();
    size_t nd_intersection_size = nd_type_intersection.size();

    std::ostringstream text1;
    text1 << "current skeleton contains " << nd_top_size << " top node(s):\t\t\t\t";

    //std::ostringstream text2;
    for (size_t nd : nd_type_top) { text1 << std::to_string(nd) << " "; }
    text1 << "\n";
    Print(text1.str());
    text1.clear();

    std::ostringstream text2;
    text2 << "current skeleton contains " << nd_intersection_size << " intersection node(s):\t";

    for (size_t nd : nd_type_intersection) { text2 << nd << " "; }
    text2 << "\n";
    Print(text2.str());
    text2.clear();

    // 1. copy all edge sources & targets into a vector
    std::vector<size_t> edges_s_t;

    auto it_eg_begin = this->skel_edges.begin();
    auto it_eg_end = this->skel_edges.end();
    auto it_eg = it_eg_begin;

    for (; it_eg != it_eg_end; ++it_eg)
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

    Print("\nanalyzing node chains from top_node to top_node / intersection_node ...");

    chain_current.clear();
    
    
    for (size_t p_top : nd_type_top)
    {
        p_current = p_top; 
        chain_current.push_back(p_current);   // the first node in current chain
        chain_source_type = SkelNode::TOP;

        std::ostringstream text3;
        text3 << "skel_chain " << i << ": \t[T]-[" << p_current << "] -> ";

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

            // if: p_current is an intersection node
            auto it_its = nd_type_intersection.find(p_current); // check if p_current is one of the intersection node
            if (it_its != nd_type_intersection.end())       
            {
                chain_target_type = SkelNode::INTERSECTION;
                text3 << "[-" << p_current << "]-[I]";
                break;                                      // build connection between node_top and node_intersection
            }

            // if: p_current is a top node
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
        ++i;
    } // end of for

    // 3. find all chains of intersection_node: from intersection_node to intersection_node
    Print("\nextracting node chains from intersection_node to intersection_node ...");

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
            ++it_intersection;
            continue;
        }


        std::ostringstream text4;
        text4 << "skel_chain " << i << ": \t[I]-[" << p_current << "] -> ";
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
                text4 << "\nERROR: cannot find next connection of node" << p_current<<"\n";
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

        ++i;
    } // end of while

    // 4. find root node : caculate node amount and vertex amount in each top chain
    Print("\ncaculate node_amount { associated mesh_vertex_amount } in each chain: ");
    std::vector<size_t> chain_nd_amount;    // node_amount in each chain
    std::vector<size_t> chain_vtx_amount;   // mesh_vertex_amount in each chain

    auto it_chains_begin = this->skel_chains.begin();
    auto it_chains_end = this->skel_chains.end();
    auto it_chains       = it_chains_begin;
    
    std::ostringstream text5;
    for (; it_chains != it_chains_end; ++it_chains)
    {
        if (it_chains->get_source_type() != SkelNode::TOP) { continue; }

        std::vector<size_t> my_chain;
        it_chains->get_chain(my_chain);

        // node amount
        size_t nd_amount = my_chain.size();
        chain_nd_amount.push_back(nd_amount);

        // vertex amount
        auto it_chain_nd_begin = my_chain.begin();
        auto it_chain_nd_end   = my_chain.end();
        auto it_chain_nd       = it_chain_nd_begin;
        
        size_t vtx_amount = 0;

        for (; it_chain_nd != it_chain_nd_end; ++it_chain_nd)
        {
            SkelNode& my_nd = (this->skel_nodes)[*it_chain_nd - 1];
            vtx_amount += my_nd.get_mapping_vertices().size();
        }

        chain_vtx_amount.push_back(vtx_amount);

        // output
        text5 << nd_amount << "{" << vtx_amount << "}" << "\t";
    }

    text5 << "\n";

    Print(text5.str());
    text5.clear();

    auto it_vtx_am_begin = chain_vtx_amount.begin();
    auto it_vtx_am_end = chain_vtx_amount.end();
    auto offset = std::max_element(it_vtx_am_begin, it_vtx_am_begin) - it_vtx_am_begin;
    
    auto it_chain_root = it_chains_begin;
    std::advance(it_chain_root, offset);
    
    std::vector<size_t> chain_root;
    it_chain_root->get_chain(chain_root);
    this->root_node_number = chain_root[0];

    // set node type: find ROOT node from TOP node
    set_node_type(root_node_number, SkelNode::ROOT);
    std::ostringstream text6;
    text6 << "\nroot node number: " << root_node_number << "\n\n";
    Print(text6.str());
    text6.clear();

    for (size_t t : nd_type_top)
    {
        if (t != root_node_number)
        {
            //set_node_type(t, SkelNode::TOP);
            this->top_node_numbers.push_back(t);
        }
    }

    for (size_t its : nd_type_intersection)
    {
        this->intersection_node_numbers.push_back(its);
    }

    //5. merge different node segments

    // TODO
    // currently no need to merge all segments

    return this->root_node_number;
}




//compairList::
//bool compairList::operator()(std::pair<size_t, size_t> pair1, std::pair<size_t, size_t> pair2)
//{
//    if (pair1.first == pair2.first) { return pair1.second < pair2.second; }
//    else { return pair1.first < pair2.first; }
//}

//equal_1::
bool equal_1::operator()(size_t num) { return num == 1; }
//larger_than_1::
bool larger_than_1::operator()(size_t num) { return num > 1; }
//larger_than_2::
bool larger_than_2::operator()(size_t num) { return num > 2; }


END_PROJECT_NAMESPACE