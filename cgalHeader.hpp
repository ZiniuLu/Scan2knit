/* Created on: 31/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

// for skeletonization & segmentation
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/mesh_segmentation.h>

#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>

// for isotropic remeshing
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <boost/iterator/function_output_iterator.hpp>

// for skeletonization & segmentation
typedef CGAL::Simple_cartesian<double>									Kernel; 

typedef Kernel::Point_3											        Point;
typedef Kernel::Plane_3                                                 Plane_3;
typedef Kernel::Vector_3                                                Vector_3;

// structure of polygon mesh
typedef CGAL::Surface_mesh<Point>										Triangle_mesh;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>	Polyhedron;

// for skeletonization module
typedef CGAL::Mean_curvature_flow_skeletonization<Polyhedron>		    Skeletonization;
typedef Skeletonization::Skeleton										Skeleton;
typedef Skeleton::vertex_descriptor										Skeleton_vertex;
typedef Skeleton::edge_descriptor										Skeleton_edge;

// descriptor
typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor			t_vertex_descriptor;
typedef boost::graph_traits<Triangle_mesh>::halfedge_descriptor		    t_halfedge_descriptor;
typedef boost::graph_traits<Triangle_mesh>::face_descriptor			    t_face_descriptor;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor			    p_vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::halfedge_descriptor		    p_halfedge_descriptor;
typedef boost::graph_traits<Polyhedron>::face_descriptor			    p_face_descriptor;

// for isotropic remeshing
typedef CGAL::Exact_predicates_inexact_constructions_kernel             K;
typedef CGAL::Surface_mesh<K::Point_3>                                  r_Mesh;
typedef boost::graph_traits<r_Mesh>::halfedge_descriptor                r_halfedge_descriptor;
typedef boost::graph_traits<r_Mesh>::edge_descriptor                    r_edge_descriptor;




// Property map associating a facet with an integer as id to an
// element in a vector stored internally
template<class ValueType>
struct Facet_with_id_pmap : public boost::put_get_helper<ValueType&, Facet_with_id_pmap<ValueType> >
{
    typedef p_face_descriptor               key_type;
    typedef ValueType                       value_type;
    typedef value_type&                     reference;
    typedef boost::lvalue_property_map_tag  category;

    Facet_with_id_pmap(std::vector<ValueType>& internal_vector) : internal_vector(internal_vector) { }

    reference operator[](key_type key) const
    {
        return internal_vector[key->id()];
    }

private:
    std::vector<ValueType>& internal_vector;
};

struct halfedge2edge
{
    halfedge2edge(const r_Mesh& m, std::vector<r_edge_descriptor>& edges)
        : m_mesh(m), m_edges(edges)
    {}
    void operator()(const r_halfedge_descriptor& h) const
    {
        m_edges.push_back(edge(h, m_mesh));
    }
    const r_Mesh& m_mesh;
    std::vector<r_edge_descriptor>& m_edges;
};
