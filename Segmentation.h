/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <vector>
#include <list>
#include <map>

#include "project.h"
#include "cgalHeader.hpp"
#include "iglHeader.h"
#include "Skeletonization.h"

BEGIN_PROJECT_NAMESPACE

struct Constraint
{
private:
    MAT_3d V;
    MAT_2i E;
    enum : char
    {
        UpperConstraint = 'U',
        LowerConstraint = 'L',
        LocalConstraint = 'M',  // constraints in the middle
    };



public:

    inline void get_constraint(MAT_3d& V, MAT_2i& E)
    {
        V = this->V;
        E = this->E;
    }
};

/// <summary>
/// Segment is a subset of pmesh. 
/// One segment represents the set of pmesh faces where the vertices corresponding to a skel_chain 
/// are located.
/// </summary>
class Segment
{
private:
    size_t id; // begins with 1
    MAT_3d V;
    MAT_3i F;
    std::vector<size_t> node2v_map;
    std::map<size_t, size_t> vID_map; // <old_id, new_id> of V;

    std::map<size_t, size_t> nd_map; // <vID_skel_node, vID_seg>
    std::vector<Constraint>  constraints;
    //void sort_F(const MAT_3i& F_old, MAT_3i& F_new);

public:
    Segment();
    Segment(const size_t id, const MAT_3d& V, const MAT_3i& F) :id(id), V(V), F(F){ }
    Segment(
        const std::vector<SkelNode>& skel_nodes, 
        const SkelChain&             skel_chain, 
        const MAT_3d&                V, 
        const MAT_3i&                F,
        MAT_3i&                      V_ID,  // V_ID.row(old_id) = <new_id...0, seg_id...1, skel_nd_id...1>         // ...1 means index starts from 1
        MAT_4i&                      F_ID); // F_ID.row(old_id) = <ocupied 0/1, seg_id...1, seg_id...1, seg_id...1>

    static void set_VF_ID(
        const std::vector<SkelNode>&  skel_nodes,
        const std::vector<SkelChain>& skel_chains,
        const MAT_3d&                 V,
        const MAT_3i&                 F,
        MAT_3i&                       V_ID,   // V_ID.row(old_id) = <new_id...0, seg_id...1, skel_nd_id...1>     // ...1 means index starts from 1
        MAT_4i&                       F_ID);  // F_ID.row(old_id) = <ocupied 0/1, seg_id...1, seg_id...1, seg_id...1>
    
    static size_t count_V_ID(const size_t segment_id, const MAT_3i& V_ID);
    static size_t count_F_ID(const size_t segment_id, const MAT_4i& F_ID);

    void set_segment(
        const std::vector<SkelNode>& skel_nodes, 
        const SkelChain&             skel_chain, 
        const MAT_3d&                V, 
        const MAT_3i&                F, 
        MAT_3i&                      V_ID, 
        MAT_4i&                      F_ID);

    inline size_t        get_id() { return this->id; }
    inline void          get_segment(MAT_3d& V, MAT_3i& F)
    {
        V = this->V;
        F = this->F;
    }
    inline const MAT_3d& get_V()
    {
        return this->V;
    }
    inline const MAT_3i& get_F()
    {
        return this->F;
    }
    
    const MAT_2i get_edges_around_vertex(RVec_3d& V);
    const MAT_6i get_faces_around_edges(MAT_2i& E); // return (F11, F12, F13, F21, F22, F23)

};

class Segmentation
{
private:
    std::vector<Segment> segments;

public:
    Segmentation();
    Segmentation(SkelGraph& skel_graph, Polyhedron& pmesh);

    void set_segments(SkelGraph& skel_graph, Polyhedron& pmesh);
    void set_constraints(Settings& settings);

    inline std::vector<Segment>& get_segments()
    {
        return this->segments;
    }

};








//
//
//class Segments
//{
//private:
//    std::list<SkelChain> skel_segments;
//
//    std::list<SkelChain>::iterator& select_next_chain(
//        size_t                                       source_node, 
//        std::vector<std::list<SkelChain>::iterator&> it_branches);
//
//public:
//    Segments();
//    Segments(SkelGraph& skel_graph, Polyhedron& pmesh);
//
//    void set_id_breadth_first(SkelGraph& skel_graph);
//
//
//    void breadth_first_traverse(SkelGraph& skel_graph);
//    void depth_first_merge(SkelGraph& skel_graph);
//    void insert(std::vector<SkelChain>& segment);
//    void insert(SkelChain& skel_chain);
//
//};
//
//class Segmentation
//{
//private:
//    Polyhedron                  pmesh;
//    std::string                 out_path;
//
//    size_t                      number_of_segments = 0;
//
//
//public:
//    Segmentation();
//    Segmentation(std::string path);
//
//    void set_out_path(std::string path);
//
//	bool extract_from_skeleton(Skeleton& skeleton, Polyhedron& pmesh);
//    bool extract_from_map_length(SkelGraph& skel_graph, Polyhedron& pmesh);
//    bool extract_from_mesh(Triangle_mesh& tmesh);
//	bool output_segments_to_file();
//};

END_PROJECT_NAMESPACE