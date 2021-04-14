/* Created on: 31/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Surface_mesh.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/mesh_segmentation.h>

#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>


typedef CGAL::Simple_cartesian<double>									Kernel;
typedef Kernel::Point_3													Point;
//typedef CGAL::Surface_mesh<Point>										Triangle_mesh;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>	Polyhedron;

typedef CGAL::Mean_curvature_flow_skeletonization<Polyhedron>			Skeletonization;
typedef Skeletonization::Skeleton										Skeleton;
typedef Skeleton::vertex_descriptor										Skeleton_vertex;
typedef Skeleton::edge_descriptor										Skeleton_edge;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor				vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::halfedge_descriptor			halfedge_descriptor;
typedef boost::graph_traits<Polyhedron>::face_descriptor				face_descriptor;



