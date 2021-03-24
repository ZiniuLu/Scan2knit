#include <igl/opengl/glfw/Viewer.h>
#include <igl/list_to_matrix.h>
#include<Eigen/Dense>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>

#include <iostream>
#include <fstream>
#include <string>

typedef CGAL::Simple_cartesian<double>                        Kernel;
typedef Kernel::Point_3                                       Point;
typedef CGAL::Surface_mesh<Point>                             Triangle_mesh;
typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor    vertex_descriptor;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh> Skeletonization;
typedef Skeletonization::Skeleton                             Skeleton;
typedef Skeleton::vertex_descriptor                           Skeleton_vertex;
typedef Skeleton::edge_descriptor                             Skeleton_edge;

int containsPoint(const Eigen::MatrixXd& V, const int vSize, const Point& p)
{
    for (int i = 0; i < vSize; i++)
    {
        if (V(i, 0) == p.x() && V(i, 1) == p.y() && V(i, 2) == p.z())
            return i;
    }

    return -1;
}

void outputSKELFile(const std::string& fileName, const Skeleton& skeleton, Eigen::MatrixXd& V, Eigen::MatrixXi& E)
{
    const int numV = V.rows();
    const int numE = E.rows();
    std::ofstream output("../data/" + fileName + "-skel.obj");
    int i = 0;
    int j = 0;

    for (const Skeleton_edge& e : CGAL::make_range(edges(skeleton)))
    {
        //std::cout << "e=" << e << std::endl;
        const Point& s = skeleton[source(e, skeleton)].point;
        const Point& t = skeleton[target(e, skeleton)].point;

        int edgeIndex1;
        int edgeIndex2;
        int sIndex = containsPoint(V, numV, s);
        int tIndex = containsPoint(V, numV, t);

        if (sIndex < 0)
        {
            V(i, 0) = s.x();
            V(i, 1) = s.y();
            V(i, 2) = s.z();
            output << "v " << s << "\n";
            i++;
        }
        edgeIndex1 = (sIndex < 0) ? i : (sIndex + 1);

        if (tIndex < 0)
        {
            V(i, 0) = t.x();
            V(i, 1) = t.y();
            V(i, 2) = t.z();
            output << "v " << t << "\n";
            i++;
        }
        edgeIndex2 = (tIndex < 0) ? i : (tIndex + 1);

        E(j, 0) = edgeIndex1;
        E(j, 1) = edgeIndex2;
        j++;

    }

    for (int l = 0; l < numE; l++)
    {
        output << "l " << E(l, 0) << " " << E(l, 1) << "\n";
    }
    output.close();
}

void outputSMFile(const std::string& fileName, const Skeleton& skeleton,const Triangle_mesh& tmesh, Eigen::MatrixXd& V, Eigen::MatrixXi& E)
{
    int numV = V.rows();
    int numE = E.rows();
    std::ofstream output("../data/" + fileName + "-sm.obj");

    
    int i = 0;
    int j = 0;
    for (const Skeleton_vertex& v : CGAL::make_range(vertices(skeleton)))
    {
        Point p1 = skeleton[v].point;
        
        int edgeIndex1;
        int edgeIndex2;
        int pointIndex1;
        int pointIndex2;

        pointIndex1 = containsPoint(V, numV, skeleton[v].point);
        if (pointIndex1 < 0)
        {
            V(i, 0) = p1.x();
            V(i, 1) = p1.y();
            V(i, 2) = p1.z();
            output << "v " << p1 << "\n";
            i++;
        }
        edgeIndex1 = (pointIndex1 < 0) ? i : (pointIndex1 + 1);

        for (vertex_descriptor vd : skeleton[v].vertices)
        {
            Point& p2 = get(CGAL::vertex_point, tmesh, vd);
            pointIndex2 = containsPoint(V, numV, p2);
            if (pointIndex2 < 0)
            {
                V(i, 0) = p2.x();
                V(i, 1) = p2.y();
                V(i, 2) = p2.z();
                output << "v " << p2 << "\n";
                i++;
            }
            edgeIndex2 = (pointIndex2 < 0) ? i : (pointIndex2 + 1);

            E(j, 0) = edgeIndex1;
            E(j, 1) = edgeIndex2;
            j++;
        }
    }

    //std::cout << "output -sm file: \n" << std::endl;
    //std::cout << "numV = " << numV << "\ti = " << i << std::endl;
    //std::cout << "numE = " << numE << "\tj = " << j << std::endl;

    for (int l = 0; l < numE; l++)
    {
        output << "l " << E(l, 0) << " " << E(l, 1) << "\n";
    }
    output.close();
}

void setViewer(const Eigen::MatrixXd& V, const Eigen::MatrixXi& E, const float v_size, const float e_size)
{
    int numE = E.rows();

    // Plot the mesh
    igl::opengl::glfw::Viewer viewer;

    viewer.data().point_size = v_size;
    viewer.data().line_width = e_size;
    viewer.data().add_points(V, Eigen::RowVector3d(0, 1, 0));
    for (unsigned i = 0; i < numE; i++)
    {
        int index1 = E(i, 0) - 1;
        int index2 = E(i, 1) - 1;
        viewer.data().add_edges(V.row(index1), V.row(index2), Eigen::RowVector3d(1, 0, 0));
    }
    viewer.core().camera_zoom = 0.005;
    viewer.launch();
}




int main(int argc, char *argv[])
{
    //1. open file
    std::string fileDir = "../data/";
    std::string fileName = "hand";
    std::string fileType = ".off";

    std::ifstream input((argc > 1) ? argv[1] : fileDir + fileName + fileType);
    Triangle_mesh tmesh;
    input >> tmesh;
    if (!CGAL::is_triangle_mesh(tmesh))
    {
        std::cout << "Input geometry is not triangulated." << std::endl;
        return EXIT_FAILURE;
    }
    const int numV_tmesh = boost::num_vertices(tmesh);

    std::cout << "Number of vertices of the geometry: " << numV_tmesh << "\n";

    // 2. extract skeleton
    Skeleton skeleton;

    CGAL::extract_mean_curvature_flow_skeleton(tmesh, skeleton); // caculate object skeleton

    const int numV_skel = boost::num_vertices(skeleton);
    const int numE_skel = boost::num_edges(skeleton);

    std::cout << "Number of vertices of the skeleton: " << numV_skel << "\n";
    std::cout << "Number of edges of the skeleton: " << numE_skel << "\n";

    // 3. Output all the edges of the skeleton.
    Eigen::MatrixXd V1 = Eigen::MatrixX3d(numV_skel, 3);
    Eigen::MatrixXi E1 = Eigen::MatrixX2i(numE_skel, 2);
    outputSKELFile(fileName, skeleton, V1, E1);

    // 4. Output skeleton points and the corresponding surface points
    const int numV_sm = numV_tmesh + numV_skel;
    const int& numE_sm = numV_tmesh;
    Eigen::MatrixXd V2 = Eigen::MatrixX3d(numV_sm, 3);
    Eigen::MatrixXi E2 = Eigen::MatrixX2i(numE_sm, 2);
    outputSMFile(fileName, skeleton, tmesh, V2, E2);
    //std::cout << V1 << std::endl;
    //std::cout << E1 << std::endl;
    
    setViewer(V1, E1, 5, 3.0f);

    return EXIT_SUCCESS;
}