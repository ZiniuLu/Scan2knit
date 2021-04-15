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

void                      SkelNode::set_type(const node_type nd_type) { this->type = nd_type; }

node_type                 SkelNode::get_type() { return this->type; }

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

size_t                 SkelEdge::source() const { return this->edge.first; }

size_t                 SkelEdge::target() const { return this->edge.second; }




// Skel::
Skel::Skel() { }

Skel::Skel(Mesh& mesh) { this->extract_to_end(mesh); }

void            Skel::extract_to_end(Mesh& mesh)
{
    this->filePath = mesh.get_file_path();

    // 1. extract skeleton;
    std::cout << "\nextracting skeletons ... ";
    auto& tmesh = mesh.get_tmesh();
    CGAL::extract_mean_curvature_flow_skeleton(tmesh, this->skeleton); // caculate object skeleton
    std::cout << "done." << std::endl;

    // 3. caculate V & E numbers
    size_t numV_skel = boost::num_vertices(this->skeleton);
    size_t numE_skel = boost::num_edges(this->skeleton);
    std::cout << "number of vertices: " << numV_skel << std::endl;
    std::cout << "number of edges: " << numE_skel << std::endl;

    std::cout << std::endl;

    // 3. Output all the edges of the skeleton.
    //output_skel_File();
    //Eigen::MatrixXd V1;
    //Eigen::MatrixXi E1;
    //output_skel_File(V1, E1);

    // 4. Output skeleton points and the corresponding surface points
    //output_map_File();
    //size_t numV_tmesh = boost::num_vertices(tmesh);
    //size_t numV_map = numV_tmesh + numV_skel;
    //size_t numE_map = numV_tmesh;
    //Eigen::MatrixXd V2;
    //Eigen::MatrixXi E2;
    //output_map_File(V2, E2);

    //display_once(V1, E1, 5, 3.0f);
}

// to do, cannot display skeletons
/*
void                   Skel::extract_step_by_step(Mesh& mesh)
{
    //load file
    //load_file(meshFile);
    auto tmesh = mesh.get_tmesh();
    //Skeleton skeleton;
    Skeletonization mcs(tmesh);

    // 1. Contract the mesh by mean curvature flow.
    mcs.contract_geometry();
    // 2. Collapse short edges and split bad triangles.
    mcs.collapse_edges();
    mcs.split_faces();
    // 3. Fix degenerate vertices.
    mcs.detect_degeneracies();
    // Perform the above three steps in one iteration.
    mcs.contract();
    // Iteratively apply step 1 to 3 until convergence.
    mcs.contract_until_convergence();
    // Convert the contracted mesh into a curve skeleton and
    // get the correspondent surface points
    mcs.convert_to_skeleton(skeleton);

    this->numV_skel = boost::num_vertices(this->skeleton);
    this->numE_skel = boost::num_edges(this->skeleton);
    std::cout << "number of vertices of the skeleton: " << this->numV_skel << "\n";
    std::cout << "number of edges of the skeleton: " << this->numE_skel << "\n";
}*/
/*
void Skel::display_once(
    const Eigen::MatrixXd& V, const Eigen::MatrixXi& E,
    const float v_size,       const float e_size)
{
    int numE = E.rows();

    // Plot the mesh
    igl::opengl::glfw::Viewer viewer;

    viewer.data().point_size = v_size;
    viewer.data().line_width = e_size;
    viewer.data().add_points(V, Eigen::RowVector3d(0, 1, 0));
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0) - 1;
        int index2 = E(i, 1) - 1;
        viewer.data().add_edges(V.row(index1), V.row(index2), Eigen::RowVector3d(1, 0, 0));
    }
    viewer.core().camera_zoom = 0.005;
    viewer.launch();
}
*/

const v_string& Skel::get_file_path() const { return this->filePath; }

const Skeleton& Skel::get_skeleton() const { return this->skeleton; }




// SkelGraph::
SkelGraph::SkelGraph() { }

SkelGraph::SkelGraph(const Skel& skel)
{
    // get file path
    this->filePath = skel.get_file_path();

    // build skel graph
    const Skeleton& mcf_skel = skel.get_skeleton();
    //const Polyhedron& tmesh = mesh.get_tmesh();
    this->set_skel_graph(mcf_skel);
}

// create skel graph
void                   SkelGraph::set_skel_graph(const Skeleton& mcf_skel)
{
    // point in skeleton -> called "node"
    // point in mesh -> called "vertex"

    // 1. get size of nodes and edges
    this->node_size = mcf_skel.m_vertices.size();
    this->edge_size = mcf_skel.m_edges.size();
    
    // 2. insert nodes from mcf_skel
    set_skel_nodes(mcf_skel);

    // 3. insert edges from mcf_skel
    set_skel_edges(mcf_skel);

    // 4. insert mapping lists of skeleton nodes and corresponding surface vertices
    set_skel_maps(mcf_skel);

    // 5. analyse skel structure
    analyse_skel_structure();

    // 6. find nearst surface point of the extension cord of root/top node
    set_skel_extension();
}

void                   SkelGraph::set_skel_nodes(const Skeleton& mcf_skel)
{
    auto& my_nodes = mcf_skel.m_vertices;  // skeleton nodes with associated mesh vertices
    
    auto it_nd_begin = my_nodes.begin();
    auto it_nd_end = my_nodes.end();
    auto it_nd = it_nd_begin;
   
    for (; it_nd != it_nd_end; ++it_nd)
    {
        size_t my_node_nr = it_nd - it_nd_begin + 1; // in mcf_skel, node number starts from 0
        const Point& my_point = it_nd->m_property.point;

        this->skel_nodes.push_back(SkelNode(my_node_nr, my_point));
        //std::cout << "V " << my_node_nr << " :\t"<< my_point << std::endl;
    }
}

void                   SkelGraph::set_skel_edges(const Skeleton& mcf_skel)
{
    auto& my_edges = mcf_skel.m_edges;
    
    auto it_eg_begin = my_edges.begin();
    auto it_eg_end = my_edges.end();
    auto it_eg = it_eg_begin;
    
    for (; it_eg != it_eg_end; ++it_eg)
    {
        size_t my_source = it_eg->m_source + 1;
        size_t my_target = it_eg->m_target + 1;
        
        this->skel_edges.push_back(SkelEdge(my_source, my_target));
        //size_t my_edge_nr = std::distance(it_eg_begin, it_eg) + 1;
        //std::cout << "E " << my_edge_nr << " :\t" << my_source << "\t " << my_target << std::endl;
    }
}

void                   SkelGraph::set_skel_maps(const Skeleton& mcf_skel)
{
    auto& my_nodes = mcf_skel.m_vertices;
    
    auto it_nd_begin = my_nodes.begin();
    auto it_nd_end = my_nodes.end();
    auto it_nd = it_nd_begin;
    
    for (; it_nd != it_nd_end; ++it_nd)
    {
        size_t offset = it_nd - it_nd_begin;

        Point p_skel = (*it_nd).m_property.point;   // current skel node
        SkelNode& nd = this->skel_nodes[offset];     // current skel node
        std::vector<Point> mapping_v_set;

        if (p_skel == nd.point())
        {
            auto& vs = (*it_nd).m_property.vertices;

            for (auto v : vs)
            {
                mapping_v_set.push_back((*v).point());
            }
        }

        nd.set_mapping_vertices(mapping_v_set);
    }

}

void                   SkelGraph::set_skel_extension()
{
    std::vector<size_t> nr_curr;    // node numbers: root & top
    std::vector<size_t> nr_next;    // numbers of next nodes
    nr_curr.push_back(this->root_node_number);
    std::copy(this->top_node_numbers.begin(), this->top_node_numbers.end(), std::back_inserter(nr_curr));
 
    auto it_seg_begin = this->skel_segments.begin();
    auto it_seg_end = this->skel_segments.end();
    auto it_seg = it_seg_begin;

    for (size_t nd_nr : nr_curr)
    {
        for (it_seg = it_seg_begin; it_seg != it_seg_end; ++it_seg)
        {
            if ((*it_seg)[0] == nd_nr)
            {
                //++it_seg;
                nr_next.push_back((*it_seg)[1]);
                break;
            }
        }
    }

    // find extension cord
    for (size_t i = 0; i < nr_curr.size(); ++i)
    {
        const SkelNode& nd_curr = this->skel_nodes[nr_curr[i]-1]; // root/top skel node
        const SkelNode& nd_next = this->skel_nodes[nr_next[i]-1]; // skel node next to root/top
        const Point& p1 = nd_next.point();
        const Point& p2 = nd_curr.point();

        const auto& v_maps = nd_curr.get_mapping_vertices();

        auto it_v_begin = v_maps.begin();
        auto it_v_end = v_maps.end();
        auto it_v = it_v_begin;
        auto it_v_max = it_v_begin;
        double cosine_max = 0;

        for (; it_v != it_v_end; ++it_v)
        {
            const Point& p3 = (*it_v);
            double dot_product = (p2.x() - p1.x()) * (p3.x() - p2.x()) 
                               + (p2.y() - p1.y()) * (p3.y() - p2.y()) 
                               + (p2.z() - p1.z()) * (p3.z() - p2.z());
            if (dot_product < 0) { continue; }  // ignore obtuse angle
            
            double cosine = dot_product / std::sqrt(CGAL::squared_distance(p2, p3));    // ignore length of <p1,p2>
            if (cosine_max < cosine)
            {
                cosine_max = cosine;
                it_v_max = it_v;
            }
        }

        this->skel_extension.push_back(*it_v_max);
    }
}

void                   SkelGraph::output_skel_to_files()
{
    output_skel_file();
    output_map_file();
    output_extension_file();
}

void                   SkelGraph::output_skel_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-skel.obj";

    std::cout << "\nsaving all skeleton points and skeleton curves to \""
        << path << "\" ... ";

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

    std::cout << "done." << std::endl;
}

void                   SkelGraph::output_map_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-map.obj";
    std::cout << "saving the mapping list of skeleton points and surface points to \""
        << path << "\" ... ";
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

    std::cout << "done." << std::endl;

}

void                   SkelGraph::output_extension_file()
{
    std::string path = this->filePath[0] + this->filePath[1] + "-extension.obj";

    std::cout << "saving all skeleton extension points to \""
        << path << "\" ... ";

    std::ofstream output(path);

    
    std::vector<size_t> nr_curr;    // node numbers: root & top
    std::vector<size_t> nr_next;    // node numbers: next node
    nr_curr.push_back(this->root_node_number);
    std::copy(this->top_node_numbers.begin(), this->top_node_numbers.end(), std::back_inserter(nr_curr));

    auto it_seg_begin = this->skel_segments.begin();
    auto it_seg_end = this->skel_segments.end();
    auto it_seg = it_seg_begin;

    for (size_t nd_nr : nr_curr)
    {
        for (it_seg = it_seg_begin; it_seg != it_seg_end; ++it_seg)
        {
            if ((*it_seg)[0] == nd_nr)
            {
                nr_next.push_back((*it_seg)[1]);
                break;
            }
        }
    }

    // output vertices
    size_t max = this->skel_extension.size();
    for (size_t i = 0; i < max; ++i)
    {
        output << "v " << this->skel_nodes[nr_curr[i] - 1].point() << "\n";
        output << "v " << this->skel_nodes[nr_next[i] - 1].point() << "\n";
        output << "v " << this->skel_extension[i] << "\n";
    }

    for (size_t i = 0; i < max; ++i)
    {
        output << "l " << 3 * i + 1 << " " << 3 * i + 2 << "\n";
        output << "l " << 3 * i + 1 << " " << 3 * i + 3 << "\n";
    }

    output.close();

    std::cout << "done." << std::endl;
}

std::vector<SkelNode>& SkelGraph::get_skel_nodes() { return this->skel_nodes; }

std::vector<SkelEdge>& SkelGraph::get_skel_edges() { return this->skel_edges; }

const SkelNode&        SkelGraph::get_skel_node(const size_t node_number) const
{
    return this->skel_nodes[node_number - 1];
}

size_t                 SkelGraph::get_root_node_number()
{
    if (this->root_node_number == 0)
    {
        analyse_skel_structure();
    }
    return this->root_node_number;
}

double                 SkelGraph::get_segment_distance(const std::vector<size_t>& skel_segment) const
{
    double distance = 0;

    auto it_begin = skel_segment.begin();
    auto it_end = skel_segment.end();
    auto it = it_begin;

    for (; it + 1 != it_end; ++it)
    {
        size_t s = *it;
        size_t t = *(it + 1);
        distance += get_node_distance(s, t);
    }

    return distance;
}

double                 SkelGraph::get_edge_distance(const std::vector<SkelEdge>::iterator& it_eg) const
{
    size_t nd_nr_1 = (*it_eg).source();
    size_t nd_nr_2 = (*it_eg).target();
    return get_node_distance(nd_nr_1, nd_nr_2);
}

double                 SkelGraph::get_node_distance(const size_t nd_nr_1, const size_t nd_nr_2) const
{
    const SkelNode& nd1 = get_skel_node(nd_nr_1);
    const SkelNode& nd2 = get_skel_node(nd_nr_2);
    return get_node_distance(nd1, nd2);
}

double                 SkelGraph::get_node_distance(const SkelNode& nd1, const SkelNode& nd2) const
{
    return sqrt(CGAL::squared_distance(nd1.point(), nd2.point()));
}

// private
// if skel_nodes contains p, return node number, else return 0
size_t                 SkelGraph::contains_node(const Point& p)
{
    auto it_nd_begin = skel_nodes.begin();
    auto it_nd_end = skel_nodes.end();
    auto it_nd = it_nd_begin;
    for (;it_nd < it_nd_end; ++it_nd)
    {
        const Point& node = (*it_nd).point();

        if (node.x() != p.x()) { continue; }
        else if (node.y() != p.y()) { continue; }
        else if (node.z() != p.z()) { continue; }
        else { return it_nd - it_nd_begin + 1; }    // return node_number, begins with 1 !!!
    }

    return 0;
}

void                   SkelGraph::set_node_type(size_t nd_nr, node_type nd_type)
{
    auto it_nd = skel_nodes.begin();
    std::advance(it_nd, nd_nr - 1);
    (*it_nd).set_type(nd_type);
}

size_t                 SkelGraph::analyse_skel_structure()
{
    if (this->node_size == 0) { return 0; }

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
    // if node_frequency > 2 : intersection
    // if node_frequency = 2 : normal node
    // if node_frequency = 1 : end node -> root / top
    std::set<size_t> node_type_end;
    std::set<size_t> node_type_intersection;

    auto it_begin = node_frequency.begin();
    auto it_end = node_frequency.end();
    auto it_ = it_begin;

    auto it_equal_1 = std::find_if(it_begin, it_end, equal_1());                // = 1
    auto it_larger_than_2 = std::find_if(it_begin, it_end, larger_than_2());    // > 2

    while (it_ != it_end)
    {
        if (it_equal_1 < it_larger_than_2)
        {
            it_ = it_equal_1;
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_end.insert(node_nr);
            skel_nodes[node_nr - 1].set_type(END);
            ++it_;
            it_equal_1 = std::find_if(it_, it_end, equal_1());
        }
        else if(it_larger_than_2 < it_equal_1)
        {
            it_ = it_larger_than_2; 
            size_t node_nr = it_ - it_begin + 1;    // node_nr begins with 1
            node_type_intersection.insert(node_nr);
            skel_nodes[node_nr - 1].set_type(INTERSECTION);
            ++it_;
            it_larger_than_2 = std::find_if(it_, it_end, larger_than_2());
        }
        else
        {
            it_ = it_end;
        }
    }

    //std::cout << "\nfound " << node_type_end.size() << " nodes with type END:" << std::endl;
    //for (size_t n : node_type_end) { std::cout << n << " "; }
    //std::cout << "\nfound " << node_type_intersection.size() << " nodes with type INTERSECTION:" << std::endl;
    //for (size_t n : node_type_intersection) { std::cout << n << " "; }

    // 3. analyse skel tree structures
    return analyse_tree_segments(node_type_end, node_type_intersection);
}

size_t                 SkelGraph::analyse_tree_segments(
                                  const std::set<size_t>& nd_type_end,
                                  const std::set<size_t>& nd_type_intersection)
{
    if (this->node_size == 0 || this->edge_size == 0) { return 0; }

    std::cout << "\nThe numbers of all end nodes of the skeleton: \n";
    for (size_t nd : nd_type_end) { std::cout << nd << " "; }
    
    std::cout << "\nThe numbers of all branch nodes of the skeleton: \n";
    for (size_t nd : nd_type_intersection) { std::cout << nd << " "; }

    // 1. copy all sources & targets into a vector
    std::vector<size_t> edges_s_t;
    
    auto it_eg_begin = this->skel_edges.begin();
    auto it_eg_end = this->skel_edges.end();
    auto it_eg = it_eg_begin;
    
    for (; it_eg != it_eg_end; ++it_eg)
    {
        edges_s_t.push_back((*it_eg).source());
        edges_s_t.push_back((*it_eg).target());
    }

    // for step 5
    std::map<size_t, size_t> end_2_its; // node_end to node_intersection
    std::multimap<size_t, size_t> its_2_its;    // node_intersection to node_intersection

    // 2. find all chains of node_end: from node_end to node_intersection
    std::vector<size_t> segment_current;
    size_t p_current;
    std::set<size_t> intersection_founded;
    int i = 1;
    
    std::cout << "\n\nextracting node chains from node_end to node_intersection ...";
    for (size_t p_end : nd_type_end)
    {
        segment_current.push_back(p_end);   // the first node in a branch
        p_current = p_end;
        std::cout << "\nskel_segment " << i << ": \n[" << p_current << "] -> ";

        while (true)
        {
            auto it_st_begin = edges_s_t.begin();
            auto it_st_end = edges_s_t.end();
            auto it_st = find(it_st_begin, it_st_end, p_current);

            if (it_st == it_st_end)
            {
                std::cout << "no connection!" << std::endl;
                segment_current.clear();
                --i;
                break;
            }

            if ((it_st - it_st_begin) % 2 == 1) // if p_current is a target
            {
                p_current = *(it_st - 1);
                segment_current.push_back(p_current);
                edges_s_t.erase(it_st - 1, it_st + 1);     // remove current edge
            }
            else                                // if p_current is a source
            {
                p_current = *(it_st + 1);
                segment_current.push_back(p_current);
                edges_s_t.erase(it_st, it_st + 2);     // remove current edge
            }

            auto it_its = nd_type_intersection.find(p_current);
            if (it_its != nd_type_intersection.end())   // p_current is a intersection node
            {
                // for step 3
                intersection_founded.insert(*it_its);
                std::cout << "[" << p_current << "]" << std::endl;
                //for step 5
                end_2_its.insert(std::make_pair(p_end, p_current));
                break;   // build connection between node_end and node_intersection
            }
            std::cout << p_current << " -> ";
        }

        if (segment_current.size() > 0)
        {
            this->skel_segments.push_back(segment_current);
            segment_current.clear();
        }
        ++i;
    }

    // 3. find all chains of node_intersection: from node_intersection to node_intersection
    std::cout << "\n\nextracting node chains from node_intersection to node_intersection ...";
    for (size_t p_its : intersection_founded)
    {
        segment_current.push_back(p_its);   // the first node in a branch
        p_current = p_its;
        std::cout << "\nskel_segment " << i << ": \n[" << p_current << "] -> ";

        while (true)
        {
            auto it_st_begin = edges_s_t.begin();
            auto it_st_end = edges_s_t.end();
            auto it_st = find(it_st_begin, it_st_end, p_current);

            if(it_st == it_st_end)
            {
                std::cout << "no connection!" << std::endl;
                segment_current.clear();
                --i;
                break;
            }

            if ((it_st - it_st_begin) % 2 == 1) // if p_current is a target
            {
                p_current = *(it_st - 1);
                segment_current.push_back(p_current);
                edges_s_t.erase(it_st - 1, it_st + 1);     // remove current edge
            }
            else                                // if p_current is a source
            {
                p_current = *(it_st + 1);
                segment_current.push_back(p_current);
                edges_s_t.erase(it_st, it_st + 2);     // remove current edge
            }

            auto it_its = nd_type_intersection.find(p_current);
            if (it_its != nd_type_intersection.end())
            {
                intersection_founded.insert(*it_its);
                std::cout << "[" << p_current << "]" << std::endl;
                // for step 5
                its_2_its.insert(std::make_pair(p_its, p_current));
                break;   // built connection between node_end and node_intersection
            }
            std::cout << p_current << " -> ";
        }

        if (segment_current.size() > 0)
        {
            skel_segments.push_back(segment_current);
            segment_current.clear();
        }
        ++i;
    }

    // 4. caculate node amount in each segment
    std::cout << "\ncaculate node amount in each segment: " << std::endl;
    std::vector<size_t> seg_nd_nr;   // segment node number
    
    auto it_seg_begin = this->skel_segments.begin();
    auto it_seg_end = this->skel_segments.end();
    auto it_seg = it_seg_begin;
    
    for (; it_seg != it_seg_end; ++it_seg)
    {
        size_t nr = (*it_seg).size();
        seg_nd_nr.push_back(nr);
        std::cout << nr << "\t";
    }
    std::cout << std::endl;
    
    auto it_seg_nd_nr_max = std::max_element(seg_nd_nr.begin(), seg_nd_nr.end());
    auto it_seg_root = this->skel_segments.begin();
    std::advance(it_seg_root, it_seg_nd_nr_max - seg_nd_nr.begin());
    this->root_node_number = *(*it_seg_root).begin();
    
    // set node type: from END to ROOT / TOP
    set_node_type(root_node_number, ROOT);
    
    for (size_t t : nd_type_end)
    {
        if (t != root_node_number)
        {
            set_node_type(t, TOP);
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

//test
void                   SkelGraph::print_skel_nodes()
{
    std::cout << "\n\nsize of skeleton nodes: " << this->node_size << std::endl;
    for (SkelNode n : this->skel_nodes)
    {
        std::cout << "V " << n.node_number() << ":\t"
            << n.point().x() << "   \t" << n.point().y() << "   \t" << n.point().z() << "   \t"
            << "Type: " << n.get_type() << std::endl;
    }

    std::cout << "(done)\n" << std::endl;
}

void                   SkelGraph::print_skel_edges()
{
    std::cout << "size of skeleton edges: " << this->edge_size << std::endl;
    for (SkelEdge e : this->skel_edges)
    {
        std::cout << "E (" << e.source() << "\t, " << e.target() << ")" << std::endl;
    }

    std::cout << "(done)\n" << std::endl;

}

void                   SkelGraph::print_skel_graph()
{
    this->print_skel_nodes();
    this->print_skel_edges();
}




//compairList::
bool compairList::operator()(std::pair<size_t, size_t> pair1, std::pair<size_t, size_t> pair2)
{
    if (pair1.first == pair2.first) { return pair1.second < pair2.second; }
    else { return pair1.first < pair2.first; }
}

//equal_1::
bool equal_1::operator()(size_t num) { return num == 1; }

//larger_than_1::
bool larger_than_1::operator()(size_t num) { return num > 1; }

//larger_than_2::
bool larger_than_2::operator()(size_t num) { return num > 2; }

END_PROJECT_NAMESPACE