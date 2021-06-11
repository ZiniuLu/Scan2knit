/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <fstream>
#include "Segmentation.h"

#include <CGAL/boost/graph/Face_filtered_graph.h>

BEGIN_PROJECT_NAMESPACE

Segmentation::Segmentation() { }
Segmentation::Segmentation(Skeleton& skeleton, Polyhedron& pmesh)
{
	this->extract_to_end(skeleton, pmesh);
}

bool Segmentation::extract_to_end(Skeleton& skeleton, Polyhedron& pmesh)
{
    // init the polyhedron simplex indices
    CGAL::set_halfedgeds_items_id(pmesh);
    //for each input vertex compute its distance to the skeleton
    std::vector<double> distances(num_vertices(pmesh));
    for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    {
        const Point& skel_pt = skeleton[v].point;
        for (p_vertex_descriptor mesh_v : skeleton[v].vertices)
        {
            const Point& mesh_pt = mesh_v->point();
            distances[mesh_v->id()] = std::sqrt(CGAL::squared_distance(skel_pt, mesh_pt));
        }
    }
    // create a property-map for sdf values
    std::vector<double> sdf_values(num_faces(pmesh));
    Facet_with_id_pmap<double> sdf_property_map(sdf_values);
    // compute sdf values with skeleton
    for (p_face_descriptor f : faces(pmesh))
    {
        double dist = 0;
        for (p_halfedge_descriptor hd : halfedges_around_face(halfedge(f, pmesh), pmesh))
            dist += distances[target(hd, pmesh)->id()];
        sdf_property_map[f] = dist / 3.;
    }
    // post-process the sdf values
    CGAL::sdf_values_postprocessing(pmesh, sdf_property_map);
    // create a property-map for segment-ids (it is an adaptor for this case)
    std::vector<std::size_t> segment_ids(num_faces(pmesh));
    Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);
    // segment the mesh using default parameters
    std::cout << "Number of segments: "
        << CGAL::segmentation_from_sdf_values(pmesh, sdf_property_map, segment_property_map) << "\n";
    return EXIT_SUCCESS;
}

END_PROJECT_NAMESPACE