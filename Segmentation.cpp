/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <fstream>
#include <vector>
#include "Segmentation.h"

//#include <CGAL/boost/graph/Face_filtered_graph.h>

BEGIN_PROJECT_NAMESPACE




// Segment::
Segment::Segment(){ }
Segment::Segment(
    const std::vector<SkelNode>& skel_nodes, 
    const SkelChain&             skel_chain, 
    const MAT_3d&                V, 
    const MAT_3i&                F,
    MAT_3i&                      V_ID,
    MAT_4i&                      F_ID)
{
    this->set_segment(skel_nodes, skel_chain, V, F, V_ID, F_ID);
}

// public
void Segment::set_VF_ID(
    const std::vector<SkelNode>& skel_nodes,
    const std::vector<SkelChain>& skel_chains,
    const MAT_3d& V,
    const MAT_3i& F,
    MAT_3i& V_ID,
    MAT_4i& F_ID)
{
    size_t numV = V.rows();
    size_t numF = F.rows();

    V_ID.resize(numV, 3);
    F_ID.resize(numF, 4);
    V_ID.setZero();
    F_ID.setZero();

    for (const auto& skel_chain : skel_chains)
    {
        size_t seg_id = skel_chain.get_id();
        auto source_type = skel_chain.get_source_type();
        auto target_type = skel_chain.get_target_type();
        const auto& my_chain = skel_chain.get_chain();
        size_t numNd = my_chain.size();

        for (size_t i = 0; i < numNd; i++)
        {
            size_t nd_id = my_chain[i];
            const auto& v_maps = skel_nodes[nd_id - 1].get_mapping_vertices();
            for (const auto& v_map : v_maps)
            {
                size_t v_id = v_map.first;
                if (V_ID.row(v_id)(1) != 0) { continue; }
                V_ID.row(v_id) << 0, seg_id, nd_id;
            }
        }
        for (size_t j = 0; j < numF; j++)
        {
            auto& f = F.row(j);
            F_ID.row(j) << 0, V_ID.row(f(0))(1), V_ID.row(f(1))(1), V_ID.row(f(2))(1);
        }
    }

    //std::cout << "\nV_ID = " << V_ID << std::endl;
    //
    //for (size_t i = 0; i < F_ID.rows(); i++)
    //{
    //    auto& f_id = F_ID.row(i);
    //    if(f_id(1) != f_id(2) || f_id(1) != f_id(3))
    //        std::cout << "\nF_ID_" << i << " = " << f_id << std::endl;
    //}
}

size_t Segment::count_V_ID(const size_t segment_id, const MAT_3i& V_ID)
{
    size_t count = 0;
    size_t numV = V_ID.rows();
    for (size_t i = 0; i < numV; i++)
    {
        if (V_ID.row(i)(1) == segment_id)
            count++;
    }
    return count;
}
size_t Segment::count_F_ID(const size_t segment_id, const MAT_4i& F_ID)
{
    size_t count = 0;
    size_t numF = F_ID.rows();
    for (size_t i = 0; i < numF; i++)
    {
        const auto& f_id = F_ID.row(i);

        bool cond0 = f_id(1) == segment_id && f_id(2) == segment_id && f_id(3) == segment_id;

        bool cond1 = f_id(1) == segment_id && f_id(2) == segment_id && f_id(3) != segment_id;
        bool cond2 = f_id(1) == segment_id && f_id(2) != segment_id && f_id(3) == segment_id;
        bool cond3 = f_id(1) != segment_id && f_id(2) == segment_id && f_id(3) == segment_id;
        
        if (cond0 || cond1 || cond2 || cond3)
            count++;
    }
    return count;
}

void Segment::set_segment(
    const std::vector<SkelNode>& skel_nodes, 
    const SkelChain&             skel_chain, 
    const MAT_3d&                V, 
    const MAT_3i&                F,
    MAT_3i&                      V_ID,
    MAT_4i&                      F_ID)
{
    this->id = skel_chain.get_id();
    auto source_type = skel_chain.get_source_type();
    auto target_type = skel_chain.get_target_type();

    size_t numNode  = skel_chain.get_node_amount(); // skel_node
    size_t numV     = count_V_ID(this->id, V_ID);   // mesh_vertices
    size_t numF     = count_F_ID(this->id, F_ID);   // mesh_faces
    size_t v_id_new = 0;
    size_t f_id_new = 0;

    const auto& my_chain = skel_chain.get_chain();
    this->V.resize(numV, 3);
    this->F.resize(numF, 3);

    // add vertices associated from current segment
    for (size_t i = 0; i < V.rows(); i++)
    {
        if (V_ID.row(i)(1) == this->id)
        {
            this->V.row(v_id_new) << V.row(i);
            V_ID.row(i)(0) = v_id_new;
            v_id_new++;
        }
    }

    // add face, if its all 3 vertices are associated form current segment
    for (size_t j = 0; j < F.rows(); j++)
    {
        const auto& f = F.row(j);
        auto& f_id    = F_ID.row(j);
        if (f_id(0) == 0 && f_id(1) == this->id && f_id(2) == this->id && f_id(3) == this->id)
        {
            size_t f1 = V_ID.row(f(0))(0); // new id of vertex
            size_t f2 = V_ID.row(f(1))(0);
            size_t f3 = V_ID.row(f(2))(0);
            
            this->F.row(f_id_new) << f1, f2, f3;
            f_id(0) = 1; // to mark current face has been ocupied
            f_id_new++;
        }
    }

    // add face, if only 2 of its vertices are associate from current segment
    for (size_t j = 0; j < F.rows(); j++)
    {
        const auto& f = F.row(j);
        auto& f_id = F_ID.row(j);
        if (f_id(0) == 1) continue;
    
        bool cond1 = f_id(1) == this->id && f_id(2) == this->id && f_id(3) != this->id;
        bool cond2 = f_id(1) == this->id && f_id(2) != this->id && f_id(3) == this->id;
        bool cond3 = f_id(1) != this->id && f_id(2) == this->id && f_id(3) == this->id;
    
        if (cond1 || cond2 || cond3)
        {
            size_t f_i = 0;
            if      (cond1) f_i = 0;
            else if (cond2) f_i = 1;
            else if (cond3) f_i = 2;
    
            size_t v_i = F.row(j)(f_i); // index of V list
            auto& v_id = V_ID.row(v_i);
    
            if (this->V.rows() <= v_id_new)
            {
                this->V.conservativeResize(v_id_new + 1, 3);
            }
            
            this->V.row(v_id_new) << V.row(v_i);
            v_id(0) = v_id_new;
            v_id(1) = this->id;
    
            size_t f1, f2, f3;
            if (cond1)
            {
                f1 = v_id_new;
                f2 = V_ID.row(f(1))(0);
                f3 = V_ID.row(f(2))(0);
            }
            else if (cond2)
            {
                f1 = V_ID.row(f(0))(0);
                f2 = v_id_new;
                f3 = V_ID.row(f(2))(0);
            }
            else if (cond3)
            {
                f1 = V_ID.row(f(0))(0);
                f2 = V_ID.row(f(1))(0);
                f3 = v_id_new;
            }
    
            if (this->F.rows() <= f_id_new)
            {
                this->F.conservativeResize(f_id_new + 1, 3);
            }
    
            this->F.row(f_id_new) << f1, f2, f3;
            f_id(0) = 1;
            v_id_new++;
            f_id_new++;
        }
    }

    //for (size_t j = 0; j < F.rows(); j++)
    //{
    //    const auto& f = F.row(j);
    //    auto& f_id    = F_ID.row(j);
    //    if (f_id(0) == 1) continue;
    //
    //    bool cond1 = f_id(1) == this->id && f_id(2) != this->id && f_id(3) != this->id && f_id(2) != f_id(3);
    //    bool cond2 = f_id(1) != this->id && f_id(2) == this->id && f_id(3) != this->id && f_id(1) != f_id(3);
    //    bool cond3 = f_id(1) != this->id && f_id(2) != this->id && f_id(3) == this->id && f_id(1) != f_id(2);
    //    
    //    if (cond1 || cond2 || cond3)
    //    {
    //        size_t v1 = f(0);
    //        size_t v2 = f(1);
    //        size_t v3 = f(2);
    //
    //        auto& v_id1 = V_ID.row(v1);
    //        auto& v_id2 = V_ID.row(v2);
    //        auto& v_id3 = V_ID.row(v3);
    //
    //        if (this->V.rows() <= v_id_new)
    //        {
    //            this->V.conservativeResize(v_id_new + 2, 3);
    //        }
    //
    //        if (cond1)
    //        {
    //            this->V.row(v_id_new)     << V.row(v2);
    //            this->V.row(v_id_new + 1) << V.row(v3);
    //
    //            v_id2(0) = v_id_new;
    //            v_id3(0) = v_id_new + 1;
    //
    //            v_id2(1) = this->id;
    //            v_id3(1) = this->id;
    //        }
    //        else if (cond2)
    //        {
    //            this->V.row(v_id_new)     << V.row(v1);
    //            this->V.row(v_id_new + 1) << V.row(v3);
    //
    //            v_id1(0) = v_id_new;
    //            v_id3(0) = v_id_new + 1;
    //
    //            v_id1(1) = this->id;
    //            v_id3(1) = this->id;
    //        }
    //        else if (cond3)
    //        {
    //            this->V.row(v_id_new)     << V.row(v1);
    //            this->V.row(v_id_new + 1) << V.row(v2);
    //
    //            v_id1(0) = v_id_new;
    //            v_id2(0) = v_id_new + 1;
    //
    //            v_id1(1) = this->id;
    //            v_id2(1) = this->id;
    //        }
    //
    //        size_t f1 = v_id1(0);
    //        size_t f2 = v_id2(0);
    //        size_t f3 = v_id3(0);
    //
    //        if (this->F.rows() <= f_id_new)
    //        {
    //            this->F.conservativeResize(f_id_new + 1, 3);
    //        }
    //
    //        this->F.row(f_id_new) << f1, f2, f3;
    //        f_id(0) = 1;
    //        v_id_new += 2;
    //        f_id_new++;
    //    }
    //}

    //for (size_t i = 0; i < my_chain.size(); i++)
    //{
    //    if (i == my_chain.size() - 1 && target_type == SkelNode::INTERSECTION) { continue; }
    //    
    //    this->node2v_map.push_back(numV);
    //    const auto& v_maps = skel_nodes[my_chain[i] - 1].get_mapping_vertices();
    //
    //    numV += v_maps.size();
    //    this->V.conservativeResize(numV, 3);
    //
    //    for (const auto& v_map : v_maps)
    //    {
    //        size_t v_id_old = v_map.first;
    //        RVec_3d v_p = V.row(v_id_old);  // == v_p.second()
    //
    //        this->V.row(v_id_new) << v_p;
    //        this->vID_map.insert(std::make_pair(v_id_old, v_id_new));
    //        v_id_new++;
    //    }
    //}
    //
    //this->node2v_map.push_back(numV);
    //
    //for (size_t i = 0; i < F.rows(); i++)
    //{
    //    auto& f = F.row(i);
    //    size_t f0 = f(0);
    //    size_t f1 = f(1);
    //    size_t f2 = f(2);
    //
    //    bool find_f0 = (vID_map.find(f0) != vID_map.end()); // if found , find_f0 = true
    //    bool find_f1 = (vID_map.find(f1) != vID_map.end());
    //    bool find_f2 = (vID_map.find(f2) != vID_map.end());
    //
    //    if (find_f0 || find_f1 || find_f2)
    //    {
    //        if (!find_f0)
    //        {
    //            this->V.conservativeResize(this->V.rows() + 1, 3);
    //            RVec_3d v_p = V.row(f0);
    //            this->V.row(v_id_new) << v_p;
    //            this->vID_map.insert(std::make_pair(f0, v_id_new));
    //            v_id_new++;
    //        }
    //        if (!find_f1)
    //        {
    //            this->V.conservativeResize(this->V.rows() + 1, 3);
    //            this->V.row(v_id_new) << V.row(f1);
    //            this->vID_map.insert(std::make_pair(f1, v_id_new));
    //            v_id_new++;
    //        }
    //        if (!find_f2)
    //        {
    //            this->V.conservativeResize(this->V.rows() + 1, 3);
    //            this->V.row(v_id_new) << V.row(f2);
    //            this->vID_map.insert(std::make_pair(f2, v_id_new));
    //            v_id_new++;
    //        }
    //
    //        this->F.conservativeResize(numF + 1, 3);
    //        size_t f0_ = vID_map[f0];
    //        size_t f1_ = vID_map[f1];
    //        size_t f2_ = vID_map[f2];
    //        this->F.row(numF) << f0_, f1_, f2_;
    //        numF++;
    //    }
    //}

    std::cout << "\nV = \n" << this->V;
    std::cout << "\nF = \n" << this->F;
}

const MAT_2i Segment::get_edges_around_vertex(RVec_3d& V)
{
    MAT_2i my_E;

    size_t row = 0;
    

    return MAT_2i();
}
const MAT_6i Segment::get_faces_around_edges(MAT_2i& E)
{
    return MAT_6i();
}

// private

//void Segment::sort_F(const MAT_3i& F_old, MAT_3i& F_new)
//{
//    F_new = F_old;
//
//    for (size_t i = 0; i < F_new.rows(); i++)
//    {
//        auto& f = F_new.row(i);
//
//        if (f(1) < f(2) && f(1) < f(0))
//        {
//            size_t p = f(0);
//            f(0) = f(1);
//            f(1) = f(2);
//            f(2) = p;
//        }
//        else if (f(2) < f(1) && f(2) < f(0))
//        {
//            size_t p = f(2);
//            f(2) = f(1);
//            f(1) = f(0);
//            f(0) = p;
//        }
//    }
//
//    for (size_t j = 0; j < F_new.rows() - 1; j++)
//    {
//        for (size_t i = 0; i < F_new.rows() - 1; i++)
//        {
//            bool cond_1 = F_new.row(i)(0) > F_new.row(i + 1)(0);
//            bool cond_2 = F_new.row(i)(0) == F_new.row(i + 1)(0) && F_new.row(i)(1) > F_new.row(i + 1)(1);
//            bool cond_3 = F_new.row(i)(0) == F_new.row(i + 1)(0) && F_new.row(i)(1) == F_new.row(i + 1)(1) && F_new.row(i)(2) > F_new.row(i + 1)(2);
//            
//            if (cond_1 || cond_2 || cond_3)
//            {
//                auto f = F_new.row(i);
//                F_new.row(i) << F_new.row(i + 1);
//                F_new.row(i + 1) << f;
//            }
//        }
//    }
//}




// Segmentation::
Segmentation::Segmentation(){ }
Segmentation::Segmentation(SkelGraph& skel_graph, Polyhedron& pmesh) { this->set_segments(skel_graph, pmesh); }

// public
void Segmentation::set_segments(SkelGraph& skel_graph, Polyhedron& pmesh)
{
    const std::vector<SkelChain>& skel_chains = skel_graph.get_skel_chains();
    const std::vector<SkelNode>& skel_nodes   = skel_graph.get_skel_nodes();
    
    size_t skel_root_nr = skel_graph.get_root_node_number();
    size_t numChain = skel_chains.size();

    if (numChain <= 0) { return; }

    MAT_3d V;
    MAT_3i F;
    Mesh::get_VF(V, F, pmesh);

    MAT_3i V_ID; // <seg_id>
    MAT_4i F_ID; // <seg_id, seg_id, seg_id>

    Segment::set_VF_ID(skel_nodes, skel_chains, V, F, V_ID, F_ID);

    for (const SkelChain& skel_chain : skel_chains)
    {
        Segment curr_seg(skel_nodes, skel_chain, V, F, V_ID, F_ID);
        this->segments.push_back(curr_seg);
    }
}

void Segmentation::set_constraints(Settings& settings)
{

}









//Segments::Segments(){ }
//Segments::Segments(SkelGraph& skel_graph, Polyhedron& pmesh) { }
//
//void Segments::breadth_first_traverse(SkelGraph& skel_graph)
//{
//    size_t               skel_root_nr = skel_graph.get_root_node_number();
//    std::vector<size_t>& skel_top_nrs = skel_graph.get_top_node_numbers();
//    std::vector<size_t>& skel_intersection_nrs = skel_graph.get_intersection_node_numbers();
//
//    const std::vector<SkelNode>& skel_nodes = skel_graph.get_skel_nodes();
//    const std::vector<SkelEdge>& skel_edges = skel_graph.get_skel_edges();
//    const std::vector<SkelExtn>& skel_extns = skel_graph.get_skel_extns();
//    const std::vector<SkelChain>& skel_chains = skel_graph.get_skel_chains();
//
//    if (skel_chains.size() <= 1) { return; }
//    if (skel_root_nr == 0) { return; }
//
//    size_t current_chain_source_node = skel_root_nr;
//    size_t current_chain_target_node;
//    size_t next_segment_source_node;
//    SkelChain current_segment;
//
//    auto it_chains_begin = skel_chains.begin();
//    auto it_chains_end = skel_chains.end();
//    auto it_chains = it_chains_begin;
//}
//
//void Segments::depth_first_merge(SkelGraph& skel_graph)
//{
//    size_t               skel_root_nr = skel_graph.get_root_node_number();
//    std::vector<size_t>& skel_top_nrs = skel_graph.get_top_node_numbers();
//    std::vector<size_t>& skel_intersection_nrs = skel_graph.get_intersection_node_numbers();
//
//    const std::vector<SkelNode>& skel_nodes = skel_graph.get_skel_nodes();
//    const std::vector<SkelEdge>& skel_edges = skel_graph.get_skel_edges();
//    const std::vector<SkelExtn>& skel_extns = skel_graph.get_skel_extns();
//    const std::vector<SkelChain>& skel_chains = skel_graph.get_skel_chains();
//
//    if (skel_chains.size() <= 1) { return; }
//    if (skel_root_nr == 0)       { return; }
//    
//    size_t current_chain_source_node = skel_root_nr;
//    size_t current_chain_target_node;
//    size_t next_segment_source_node;
//    SkelChain current_segment;
//
//    auto it_chains_begin = skel_chains.begin();
//    auto it_chains_end   = skel_chains.end();
//    auto it_chains       = it_chains_begin;
//
//    // 1. find skel_chain for root_node
//    for (; it_chains != it_chains_end; it_chains++)
//    {
//        if (it_chains->get_source_node() == current_chain_source_node) { break; }
//        //if (it_chains->get_target_node() == current_chain_source_node) { it_chains->reverse(); break; }
//    }
//    current_chain_target_node = it_chains->get_target_node();
//    next_segment_source_node  = current_chain_target_node;
//    current_segment.insert(*it_chains);
//    //skel_chains.erase(it_chains);
//
//    // 2. find next skel_chain combinding with current segment
//    while (true)
//    {
//        if (skel_chains.size() <= 0) { break; }
//
//        std::vector<std::list<SkelChain>::iterator&> it_current_branches;
//
//        while (true)
//        {
//            if (current_segment.get_target_type() == SkelNode::TOP) { break; }
//
//            it_chains_begin = skel_chains.begin();
//            it_chains_end   = skel_chains.end();
//            it_chains       = it_chains_begin;
//
//            // find all branches of current_chain_target_node
//            for (; it_chains != it_chains_end; it_chains++)
//            {
//                if (it_chains->get_source_node() == current_chain_target_node)
//                {
//                    //it_current_branches.push_back(it_chains);
//                }
//                else if (it_chains->get_target_node() == current_chain_target_node)
//                {
//                    //it_chains->reverse();
//                    //it_current_branches.push_back(it_chains);
//                }
//            }
//
//            // select the chain with maximal vertex_amount
//            std::list<SkelChain>::iterator& it_next_chain =
//                this->select_next_chain(current_chain_target_node, it_current_branches);
//
//            current_segment.insert(*it_next_chain);
//            next_segment_source_node = it_next_chain->get_target_type();
//
//            // clear cache
//            //skel_chains.erase(it_next_chain);
//            it_current_branches.clear();
//        }
//
//        this->skel_segments.push_back(current_segment);
//        current_segment.clear();
//
//        // find the first chain of next segment
//        if (skel_chains.size() >= 0)
//        {
//            it_chains_begin = skel_chains.begin();
//            it_chains_end   = skel_chains.end();
//            it_chains       = it_chains_begin;
//
//            for (; it_chains != it_chains_end; it_chains++)
//            {
//                if (it_chains->get_source_node() == current_chain_target_node)
//                {
//                    //it_current_branches.push_back(it_chains);
//                }
//                else if (it_chains->get_target_node() == current_chain_target_node)
//                {
//                    //it_chains->reverse();
//                    //it_current_branches.push_back(it_chains);
//                }
//            }
//            // (to do)
//        }
//        
//    }
//    
//}
//
//void Segments::insert(SkelChain& skel_chain)
//{
//    for (SkelChain& skel_segment : this->skel_segments)
//    {
//        if (skel_segment.get_target_node() == skel_chain.get_source_node())
//        {
//            std::vector<size_t> my_chain;
//            skel_segment.get_chain();
//            //skel_segment.get_chain
//            //(to do)
//            break;
//        }
//        else if (skel_segment.get_target_node() == skel_chain.get_target_node())
//        {
//            skel_chain.reverse();
//            //this->insert(skel_segment, skel_chain);
//            break;
//        }
//    }
//}
//
//Segmentation::Segmentation(){ }
//
//Segmentation::Segmentation(std::string path) { this->set_out_path(path); }
//
////Segmentation::~Segmentation()
////{
////    delete this->segment_property_map;
////    delete this->segment_mesh;
////
////    this->segment_property_map = NULL;
////    this->segment_mesh = NULL;
////}
//
//void Segmentation::set_out_path(std::string path)
//{
//    this->out_path = path.substr(0, path.rfind('.'));
//}
//
//bool Segmentation::extract_from_map_length(SkelGraph& skel_graph, Polyhedron& pmesh)
//{
//    
//
//
//    return true;
//}

//bool Segmentation::extract_from_skeleton(Skeleton& skeleton, Polyhedron& pmesh)
//{
//    this->pmesh = pmesh;
//    // init the polyhedron simplex indices
//    CGAL::set_halfedgeds_items_id(this->pmesh);
//    //for each input vertex compute its distance to the skeleton
//    std::vector<double> distances(num_vertices(this->pmesh));
//    for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
//    {
//        const Point& skel_pt = skeleton[v].point;
//        for (p_vertex_descriptor mesh_v : skeleton[v].vertices)
//        {
//            const Point& mesh_pt = mesh_v->point();
//            distances[mesh_v->id()] = std::sqrt(CGAL::squared_distance(skel_pt, mesh_pt));
//        }
//    }
//    // create a property-map for sdf values
//    std::vector<double> sdf_values(num_faces(this->pmesh));
//    Facet_with_id_pmap<double> sdf_property_map(sdf_values);
//    // compute sdf values with skeleton
//    for (p_face_descriptor f : faces(this->pmesh))
//    {
//        double dist = 0;
//        for (p_halfedge_descriptor hd : halfedges_around_face(halfedge(f, this->pmesh), this->pmesh))
//            dist += distances[target(hd, this->pmesh)->id()];
//        sdf_property_map[f] = dist / 3.;
//    }
//    // post-process the sdf values
//    CGAL::sdf_values_postprocessing(this->pmesh, sdf_property_map);
//
//    // create a property-map for segment-ids (it is an adaptor for this case)
//    std::vector<size_t> segment_ids(num_faces(this->pmesh));
//    this->segment_property_map = new Facet_with_id_pmap<size_t>(segment_ids);
//    // segment the mesh using default parameters
//    this->number_of_segments = CGAL::segmentation_from_sdf_values(this->pmesh, sdf_property_map, *segment_property_map);
//    std::ostringstream text;
//    text << "Number of segments: " << this->number_of_segments << "\n";
//    Print(text.str());
//
//    this->segment_mesh = new Filtered_graph(this->pmesh);
//    // print area of each segment and then put it in a Mesh and print it in an OFF file
//
//    this->output_segments_to_file();
//
//    return true;
//}

//bool Segmentation::extract_from_mesh(Triangle_mesh& tmesh)
//{
//    typedef boost::graph_traits<Triangle_mesh>::face_descriptor     t_face_descriptor;
//    typedef Triangle_mesh::Property_map<t_face_descriptor, double>  Facet_double_map;
//    
//    Facet_double_map sdf_property_map;
//    sdf_property_map = tmesh.add_property_map<t_face_descriptor, double>("f:sdf").first;
//    CGAL::sdf_values(tmesh, sdf_property_map);
//    // create a property-map for segment-ids
//    typedef Triangle_mesh::Property_map<t_face_descriptor, std::size_t> Facet_int_map;
//    Facet_int_map segment_property_map = tmesh.add_property_map<t_face_descriptor, std::size_t>("f:sid").first;;
//    // segment the mesh using default parameters for number of levels, and smoothing lambda
//    // Any other scalar values can be used instead of using SDF values computed using the CGAL function
//    this->number_of_segments = CGAL::segmentation_from_sdf_values(tmesh, sdf_property_map, segment_property_map);
//    typedef CGAL::Face_filtered_graph<Triangle_mesh> t_Filtered_graph;
//    //print area of each segment and then put it in a Mesh and print it in an OFF file
//    t_Filtered_graph* segment_tmesh;
//    segment_tmesh = new t_Filtered_graph(tmesh);
//    for (std::size_t id = 0; id < this->number_of_segments; id++)
//    {
//        segment_tmesh->set_selected_faces(id, segment_property_map);
//        std::cout << "Segment " << id << "'s area is : " << CGAL::Polygon_mesh_processing::area(*segment_tmesh) << std::endl;
//        Triangle_mesh out;
//        CGAL::copy_face_graph(*segment_tmesh, out);
//        std::ostringstream oss;
//        oss << this->out_path << "_seg_" << id << ".off";
//        std::ofstream os(oss.str().data());
//        os << out;
//    }
//    return true;
//}

//bool Segmentation::output_segments_to_file()
//{
//    for (size_t id = 0; id < this->number_of_segments; id++)
//    {
//        this->segment_mesh->set_selected_faces(id, *(this->segment_property_map));
//        std::ostringstream text;
//        text << "Segment " << id << "'s area is : " 
//            << CGAL::Polygon_mesh_processing::area(*(this->segment_mesh));
//        Polyhedron out;
//        CGAL::copy_face_graph(*(this->segment_mesh), out);
//        std::ostringstream oss;
//        oss << this->out_path << "_seg_" << id << ".off";
//        std::ofstream os(oss.str().data());
//        os << out;
//    }
//    return true;
//}

END_PROJECT_NAMESPACE