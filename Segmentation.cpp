/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <fstream>
#include "Segmentation.h"
//#include "cgalHeader.hpp"

BEGIN_PROJECT_NAMESPACE

Segmentation::Segmentation() { }
Segmentation::Segmentation(Skeleton& skeleton, Triangle_mesh& tmesh)
{
    //// init the polyhedron simplex indices
    ////CGAL::set_halfedgeds_items_id(tmesh);

    //// for each input vertex compute its distance to the skeleton
    //std::vector<double> distances(num_vertices(tmesh));
    //for (Skeleton_vertex v : CGAL::make_range(vertices(skeleton)))
    //{
    //    const Point& skel_pt = skeleton[v].point;
    //    for (const vertex_descriptor& mesh_v : skeleton[v].vertices)
    //    {
    //        std::cout << mesh_v.idx()<<std::endl;
    //        const Point& mesh_pt = (skeleton[v]).vertices[mesh_v].idx;
    //        distances[*mesh_v] = std::sqrt(Vector_3(skel_pt, mesh_pt).squared_length());
    //    }
    //}

    //// create a property-map for sdf values
    //std::vector<double> sdf_values(num_faces(tmesh));
    //Facet_with_id_pmap<double> sdf_property_map(sdf_values);

    //// compute sdf values with skeleton
    //for (face_descriptor f : faces(tmesh))
    //{
    //    double dist = 0;
    //    for (halfedge_descriptor hd : halfedges_around_face(halfedge(f, tmesh), tmesh))
    //        dist += distances[target(hd, tmesh)->id()];
    //    sdf_property_map[f] = dist / 3.;
    //}
    //// post-process the sdf values
    //CGAL::sdf_values_postprocessing(tmesh, sdf_property_map);
    //// create a property-map for segment-ids (it is an adaptor for this case)
    //std::vector<std::size_t> segment_ids(num_faces(tmesh));
    //Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);
    //// segment the mesh using default parameters
    //std::cout << "number of segments: "
    //          << CGAL::segmentation_from_sdf_values(tmesh, sdf_property_map, segment_property_map) 
    //          << "\n";
    ////return EXIT_SUCCESS;
}
END_PROJECT_NAMESPACE