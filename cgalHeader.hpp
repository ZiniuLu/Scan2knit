/* Created on: 31/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/mesh_segmentation.h>

//#include <CGAL/Polyhedron_3.h>
//#include <CGAL/Polyhedron_items_with_id_3.h>

//#include <CGAL/Nef_polyhedron_3.h>
//#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
//#include <CGAL/Gmpq.h>


// basic elements
typedef CGAL::Simple_cartesian<double>									Kernel;
//typedef CGAL::Exact_predicates_exact_constructions_kernel             Exact_Kernel;

typedef Kernel::Point_3											        Point;
typedef Kernel::Plane_3                                                 Plane_3;
typedef Kernel::Vector_3                                                Vector_3;
//typedef Exact_Kernel::Vector_3                                          Vector_3;
//typedef Exact_Kernel::Point_3                                           Point;
//typedef Exact_Kernel::Plane_3                                           Plane_3;

// structure of polygon mesh
//typedef CGAL::Surface_mesh<Exact_kernel::Point_3>                     Surface_mesh;
typedef CGAL::Surface_mesh<Point>										Triangle_mesh;
//typedef CGAL::Polyhedron_3<Kernel>                                      Polyhedron;
//typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>	Polyhedron;
//typedef CGAL::Nef_polyhedron_3<Exact_Kernel>                          Nef_polyhedron;

// for skeletonization module
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh>		Skeletonization;
typedef Skeletonization::Skeleton										Skeleton;
typedef Skeleton::vertex_descriptor										Skeleton_vertex;
typedef Skeleton::edge_descriptor										Skeleton_edge;

// descriptor & iterator
typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor			vertex_descriptor;
typedef boost::graph_traits<Triangle_mesh>::halfedge_descriptor			halfedge_descriptor;
typedef boost::graph_traits<Triangle_mesh>::face_descriptor				face_descriptor;
//typedef Triangle_mesh::Face_index                                       face_descriptor;
//typedef Triangle_mesh::Edge_index                                       edge_descriptor;
//typedef Triangle_mesh::Vertex_index                                     vertex_descriptor;

//typedef Polyhedron::Facet_iterator                                      Facet_iterator;
//typedef Polyhedron::Halfedge_around_facet_circulator                    Halfedge_facet_circulator;

//struct Plane_equation
//{
//	template<class Facet>
//	typename Facet::Plane_3 operator()(Facet& f)
//	{
//		typename Facet::Halfedge_handle h = f.halfedge();
//		typedef typename Facet::Plane_3 Plane;
//		return Plane(h->vertex()->point(),
//					 h->next()->vertex()->point(),
//					 h->next()->next()->vertex()->point());
//	}
//};


// Property map associating a facet with an integer as id to an
// element in a vector stored internally
 
template<class ValueType>
struct Facet_with_id_pmap : public boost::put_get_helper<ValueType&, Facet_with_id_pmap<ValueType> >
{
    typedef face_descriptor                 key_type;
    typedef ValueType                       value_type;
    typedef value_type& reference;
    typedef boost::lvalue_property_map_tag  category;

    Facet_with_id_pmap(std::vector<ValueType>& internal_vector) : internal_vector(internal_vector) { }

    reference operator[](key_type key) const
    {
        return internal_vector[key->id()];
    }

private:
    std::vector<ValueType>& internal_vector;
};


//double gmpq2double(CGAL::Gmpq r)
//{
//    return r.numerator().to_double() / r.denominator().to_double();
//}
