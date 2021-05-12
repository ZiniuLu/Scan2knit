/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <sstream>
#include <string>
#include "Display.h"
#include "Skeletonization.h"
#include <vector>
#include <algorithm>

BEGIN_PROJECT_NAMESPACE

// Display::
Display::Display() { }
Display::Display(const std::string path) { this->display(path); }
Display::Display(const MAT_3d& V, const MAT_2i& E, const MAT_3i& F) { this->display(V, E, F); }
Display::Display(const MAT_3d& V, const MAT_3i& F) { this->display(V, F); }
Display::Display(const MAT_3d& V, const MAT_2i& E) { this->display(V, E); }
Display::Display(const Triangle_mesh& tmesh) { this->display_tmesh(tmesh); }
Display::Display(const Skeleton& skeleton) { this->display_skeleton(skeleton); }
Display::Display(const Skel& skel) { this->display_skel(skel); }
Display::Display(const SkelGraph& skel_graph) { this->display_skel_graph(skel_graph); }

// public
void Display::display(const std::string path)
{
    size_t pos_max = path.length();
    if (path.find(".off") < pos_max)
    {
        this->display_off(path);
    }
    else if (path.find(".obj") < pos_max)
    {
        this->display_obj(path);
    }
    else if (path.find(".stl") < pos_max)
    {
        this->display_stl(path);
    }
}
void Display::display(const MAT_3d& V, const MAT_2i& E, const MAT_3i& F)
{
    int numV = V.rows();
    int numE = E.rows();
    int numF = F.rows();

    // set para for current viewer
    this->viewer.data().point_size = V_SIZE;
    this->viewer.data().line_width = E_SIZE;
    this->viewer.core().camera_zoom = CAM_ZOOM;

    // lauch current viewer
    this->viewer.data().set_mesh(V, F);

    // add edges
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0) - 1;   // source vertex number
        int index2 = E(i, 1) - 1;   // target vertex number

        auto& it_source = V.row(index1);
        auto& it_target = V.row(index2);
        auto color_eg = Eigen::RowVector3d(1, 0, 0);

        this->viewer.data().add_edges(it_source, it_target, color_eg);
        this->viewer.launch();
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
        int index1 = E(i, 0);   // source vertex number
        int index2 = E(i, 1);   // target vertex number

        auto& it_source = V.row(index1);
        auto& it_target = V.row(index2);
        auto color_eg = Eigen::RowVector3d(1, 0, 0);

        this->viewer.data().add_edges(it_source, it_target, color_eg);
    }
    this->viewer.core().camera_zoom = CAM_ZOOM;
    this->viewer.launch();
}


void Display::display(const Triangle_mesh& tmesh)
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
void Display::display_obj(const std::string path)
{
    std::ifstream input;
    input.open(path, std::ios::in);

    if (!input.is_open())
    {
        std::cout << "Cannot open file \"" << path << "\"!";
        return;
    }

    std::string line = "";
    std::vector<std::string> buf;
    std::vector<std::vector<double>> v;
    std::vector<std::vector<int>> e;
    std::vector<std::vector<int>> f;
    
    while (std::getline(input, line))
    {
        buf.clear();

        // divide elements in each line
        std::stringstream inputL(line);
        std::string str;
        while (inputL >> str)
        {
            buf.push_back(str);
        }

        if (buf.size() == 0) { continue; }
        
        auto it_buf_begin = buf.begin();
        std::string line_type = *it_buf_begin;

        if (line_type == "v")
        {
            std::vector<double> v_;
            v_.resize(3);
            v_[0] = stod(*(it_buf_begin + 1));
            v_[1] = stod(*(it_buf_begin + 2));
            v_[2] = stod(*(it_buf_begin + 3));
            v.push_back(v_);
        }
        else if (line_type == "l")
        {
            std::vector<int> e_;
            e_.resize(2);
            e_[0] = stoi(*(it_buf_begin + 1)) - 1;
            e_[1] = stoi(*(it_buf_begin + 2)) - 1;
            e.push_back(e_);
        }
        else if (line_type == "f")
        {
            std::vector<int> f_;
            f_.resize(3);
            f_[0] = stoi(*(it_buf_begin + 1)) - 1;
            f_[1] = stoi(*(it_buf_begin + 2)) - 1;
            f_[2] = stoi(*(it_buf_begin + 3)) - 1;
            f.push_back(f_);
        }
    }

    input.close();

    int numV = v.size();
    int numE = e.size();
    int numF = f.size();

    if (numV > 0)
    {
        this->V.resize(numV, 3);
        for (int i = 0; i < numV; i++)
        {
            (this->V)(i, 0) = v[i][0];
            (this->V)(i, 1) = v[i][1];
            (this->V)(i, 2) = v[i][2];
        }
    }
    if (numE > 0)
    {
        this->E.resize(numE, 2);
        for (int i = 0; i < numE; i++)
        {
            (this->E)(i, 0) = e[i][0];
            (this->E)(i, 1) = e[i][1];
        }
    }
    if (numF > 0)
    {
        this->F.resize(numF, 3);
        for (int i = 0; i < numF; i++)
        {
            (this->F)(i, 0) = f[i][0];
            (this->F)(i, 1) = f[i][1];
            (this->F)(i, 2) = f[i][2];
        }
    }

    if (numE == 0 && numF > 0)      
    { 
        display(this->V, this->F); 
    }
    else if (numE > 0 && numF == 0)
    { 
        display(this->V, this->E); 
    }
    else if (numE > 0 && numF > 0)  
    { 
        display(this->V, this->E, this->F); 
    }
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
    MAT_3d N;
    igl::readSTL(input, this->V, this->F, N);

    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();

}

void Display::display_tmesh(const Triangle_mesh& tmesh)
{
    int numV = tmesh.number_of_vertices();
    int numF = tmesh.number_of_faces();

    this->V.resize(numV, 3);
    this->F.resize(numF, 3);


    // add vertices
    size_t v_i = 0;
    for (const auto& v : tmesh.vertices())
    {
        const Point& p = tmesh.point(v);
        (this->V)(v_i, 0) = p.x();
        (this->V)(v_i, 1) = p.y();
        (this->V)(v_i, 2) = p.z();
        ++v_i;
    }

    // add facets
    size_t f_i = 0;
    for (const face_descriptor& fd : tmesh.faces())
    {
        size_t axis = 0;
        for (const vertex_descriptor& vd : CGAL::vertices_around_face(tmesh.halfedge(fd), tmesh))
        {
            (this->F)(f_i, axis) = vd;
            ++axis;
        }
        //std::cout << "face " << f_i << ": " 
        //          << (this->F)(f_i, 0) << "\t"
        //          << (this->F)(f_i, 1) << "\t"
        //          << (this->F)(f_i, 2) << "\t"
        //          << std::endl;
        ++f_i;
    }

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

    this->V.resize(numV, 3);
    this->E.resize(numE, 2);

    size_t v_i = 0;
    for (SkelNode nd : skel_nodes)
    {
        const Point& p = nd.point();
        (this->V)(v_i, 0) = p.x();
        (this->V)(v_i, 1) = p.y();
        (this->V)(v_i, 2) = p.z();
        ++v_i;
    }
    for (SkelEdge eg : skel_edges)
    {
        this->E << eg.source() - 1, eg.target() - 1;
    }

    this->display(V, E);
}

END_PROJECT_NAMESPACE

