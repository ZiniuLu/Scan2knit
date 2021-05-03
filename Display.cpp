/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include "Display.h"
#include "Skeletonization.h"

BEGIN_PROJECT_NAMESPACE

// Display::
Display::Display() { }
Display::Display(const std::string path) { this->display(path); }
Display::Display(const MAT_3d& V, const MAT_3i& F) { this->display(V, F); }
Display::Display(const MAT_3d& V, const MAT_2i& E) { this->display(V, E); }
Display::Display(const Polyhedron& tmesh) { this->display_tmesh(tmesh); }
Display::Display(const Skeleton& skeleton) { this->display_skeleton(skeleton); }
Display::Display(const Skel& skel) { this->display_skel(skel); }
Display::Display(const SkelGraph& skel_graph) { this->display_skel_graph(skel_graph); }

// public
void Display::display(const std::string path)
{
    if (path.find(".off") > 0)
    {
        this->display_off(path);
    }
    else if (path.find(".obj") > 0)
    {
        this->display_obj(path);
    }
    else if (path.find(".stl") > 0)
    {
        this->display_stl(path);
    }
}
void Display::display(const MAT_3d& V, const MAT_3i& F)
{
    int numV = V.rows();
    int numF = F.rows();

    // set para for current viewer
    this->viewer.data().point_size = V_SIZE;
    this->viewer.data().line_width = E_SIZE;
    this->viewer.core().camera_zoom = CAM_ZOOM;

    // lauch current viewer
    this->viewer.data().set_mesh(V, F);
    this->viewer.launch();


}
void Display::display(const MAT_3d& V, const MAT_2i& E)
{
    int numV = V.rows();
    int numE = E.rows();

    // set para for current viewer
    this->viewer.data().point_size = V_SIZE;
    this->viewer.data().line_width = E_SIZE;

    // add points
    auto color_v = Eigen::RowVector3d(0, 1, 0);
    this->viewer.data().add_points(V, color_v);

    // add edges
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0) - 1;   // source vertex number
        int index2 = E(i, 1) - 1;   // target vertex number

        auto& source = V.row(index1);
        auto& target = V.row(index2);
        auto color_eg = Eigen::RowVector3d(1, 0, 0);

        this->viewer.data().add_edges(source, target, color_eg);
        this->viewer.launch();
    }
    this->viewer.core().camera_zoom = CAM_ZOOM;
    this->viewer.launch();
}
void Display::display(const Polyhedron& tmesh)
{
    this->display_tmesh(tmesh);
}
void Display::display(const Skeleton& skeleton)
{
    this->display_skeleton(skeleton);
}
void Display::display(const Skel& skel)
{
    this->display_skel(skel);
}
void Display::display(const SkelGraph& skel_graph)
{
    this->display_skel_graph(skel_graph);
}




// private

void Display::display_tmesh(const Polyhedron& tmesh) 
{
    auto it_v_begin = tmesh.vertices_begin();
    auto it_v_end = tmesh.vertices_end();
    auto it_v = it_v_begin;

    for (; it_v != it_v_end; ++it_v)
    {
        const Point& p = (*it_v).point();
        this->V << p.x(), p.y(), p.z();
    }

    auto it_f_begin = tmesh.facets_begin();
    auto it_f_end = tmesh.facets_end();
    auto it_f = it_f_begin;

    for (; it_f != it_f_end; ++it_f)
    {
        auto f_0 = it_f->facet_begin();
        auto f_1 = f_0.

        this->F << *it_f;
    }
}

void Display::display_obj(const std::string path)
{

}

void Display::display_off(const std::string path)
{
    igl::readOFF(path, this->V, this->F);
    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();
}
void Display::display_stl(const std::string path)
{
    std::ifstream input(path);
    igl::readSTL(input, this->V, this->F, this->N);

    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();

}

void Display::display_skeleton(const Skeleton& skeleton)
{
    Skel skel(skeleton);
    this->display_skel(skel);

}
void Display::display_skel(const Skel& skel)
{
    SkelGraph skel_graph(skel);
    this->display_skel_graph(skel_graph);
}
void Display::display_skel_graph(const SkelGraph& skel_graph)
{
    const std::vector<SkelNode>& skel_nodes = skel_graph.get_skel_nodes();
    const std::vector<SkelEdge>& skel_edges = skel_graph.get_skel_edges();

    size_t numV = skel_nodes.size();
    size_t numE = skel_edges.size();

    //this->V.resize(numV, 3);
    //this->E.resize(numE, 2);

    for (SkelNode nd : skel_nodes)
    {
        const Point& p = nd.point();
        this->V << p.x(), p.y(), p.z();
    }
    for (SkelEdge eg : skel_edges)
    {
        this->E << eg.source() - 1, eg.target() - 1;
    }

    this->display(V, E);
}

END_PROJECT_NAMESPACE

