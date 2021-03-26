/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <iostream>
#include <fstream>
#include <string>

#include <igl/list_to_matrix.h>
#include <igl/opengl/glfw/Viewer.h>

#include "CurveSkeleton.h"



BEGIN_PROJECT_NAMESPACE

CurveSkeleton::CurveSkeleton(MeshFile& meshFile)
{
    this->filePath = meshFile.getFileDir() + meshFile.getFileName();

    // build triangle mesh
    meshFile.getInputStream() >> this->tmesh;
    this->isTriangleMesh = CGAL::is_triangle_mesh(this->tmesh);
    if (!this->isTriangleMesh)
    {
        std::cout << "Input geometry is not triangulated." << std::endl;
        return;
    }
    // print geometry info
    std::cout << "Number of vertices of the geometry: \t" << this->tmesh.num_vertices() << "\n";
    std::cout << "Number of edge of the geometry: \t" << this->tmesh.num_edges() << "\n";
}

void CurveSkeleton::extract_to_end(bool output)
{
    //extract skeleton;
    CGAL::extract_mean_curvature_flow_skeleton(this->tmesh, this->skeleton); // caculate object skeleton

    this->numV_skel = boost::num_vertices(this->skeleton);
    this->numE_skel = boost::num_edges(this->skeleton);

    std::cout << "Number of vertices of the skeleton: " << this->numV_skel << "\n";
    std::cout << "Number of edges of the skeleton: " << this->numE_skel << "\n";

    if (!output){ return; }

    // 3. Output all the edges of the skeleton.
    Eigen::MatrixXd V1;
    Eigen::MatrixXi E1;
    output_skel_File(V1, E1);

    // 4. Output skeleton points and the corresponding surface points
    const uint32_t numV_tmesh = boost::num_vertices(this->tmesh);
    numV_sm = numV_tmesh + numV_skel;
    numE_sm = numV_tmesh;
    Eigen::MatrixXd V2;
    Eigen::MatrixXi E2;
    output_sm_File(V2, E2);

    //display_once(V1, E1, 5, 3.0f);
}

// to do, can not display skeletons
void CurveSkeleton::extract_step_by_step(bool output)
{
    //Skeleton skeleton;
    Skeletonization mcs(tmesh);

    // 1. Contract the mesh by mean curvature flow.
    mcs.contract_geometry();
    // 2. Collapse short edges and split bad triangles.
    mcs.collapse_edges();
    mcs.split_faces();
    // 3. Fix degenerate vertices.
    mcs.detect_degeneracies();
    // Perform the above three steps in one iteration.
    mcs.contract();
    // Iteratively apply step 1 to 3 until convergence.
    mcs.contract_until_convergence();
    // Convert the contracted mesh into a curve skeleton and
    // get the correspondent surface points
    mcs.convert_to_skeleton(skeleton);

    this->numV_skel = boost::num_vertices(this->skeleton);
    this->numE_skel = boost::num_edges(this->skeleton);
    std::cout << "Number of vertices of the skeleton: " << this->numV_skel << "\n";
    std::cout << "Number of edges of the skeleton: " << this->numE_skel << "\n";

    if (!output) { return; }

    // 3. Output all the edges of the skeleton.
    Eigen::MatrixXd V1;
    Eigen::MatrixXi E1;
    output_skel_File(V1, E1);

    // 4. Output skeleton points and the corresponding surface points
    const uint32_t numV_tmesh = boost::num_vertices(this->tmesh);
    numV_sm = numV_tmesh + numV_skel;
    numE_sm = numV_tmesh;
    Eigen::MatrixXd V2;
    Eigen::MatrixXi E2;
    output_sm_File(V2, E2);
}


bool CurveSkeleton::is_triangle_mesh() { return this->isTriangleMesh; }
Triangle_mesh& CurveSkeleton::get_triangle_mesh() { return this->tmesh; }
Skeleton& CurveSkeleton::getSkeleton() { return this->skeleton; }


int CurveSkeleton::containsPoint(const Eigen::MatrixXd& V, const int vSize, const Point& p)
{
    for (int i = 0; i < vSize; i++)
    {
        if (V(i, 0) == p.x() && V(i, 1) == p.y() && V(i, 2) == p.z())
            return i;
    }

    return -1;
}

// Output all the edges of the skeleton.
void CurveSkeleton::output_skel_File(Eigen::MatrixXd& V, Eigen::MatrixXi E)
{
    V = Eigen::MatrixX3d(this->numV_skel, 3);
    E = Eigen::MatrixX2i(this->numE_skel, 2);
    std::ofstream output(this->filePath + "-skel.obj");
    int i = 0;
    int j = 0;

    for (const Skeleton_edge& e : CGAL::make_range(edges(this->skeleton)))
    {
        //std::cout << "e=" << e << std::endl;
        const Point& s = skeleton[source(e, skeleton)].point;
        const Point& t = skeleton[target(e, skeleton)].point;

        int edgeIndex1;
        int edgeIndex2;
        int sIndex = containsPoint(V, numV_skel, s);
        int tIndex = containsPoint(V, numV_skel, t);

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

    for (int l = 0; l < numE_skel; l++)
    {
        output << "l " << E(l, 0) << " " << E(l, 1) << "\n";
    }
    output.close();
}

// Output skeleton points and the corresponding surface points
void CurveSkeleton::output_sm_File(Eigen::MatrixXd& V, Eigen::MatrixXi E)
{
    V = Eigen::MatrixX3d(this->numV_sm, 3);
    E = Eigen::MatrixX2i(this->numE_sm, 2);
    std::ofstream output(this->filePath + "-sm.obj");


    int i = 0;
    int j = 0;
    for (const Skeleton_vertex& v : CGAL::make_range(vertices(skeleton)))
    {
        Point p1 = skeleton[v].point;

        int edgeIndex1;
        int edgeIndex2;
        int pointIndex1;
        int pointIndex2;

        pointIndex1 = containsPoint(V, this->numV_sm, skeleton[v].point);
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
            Point& p2 = get(CGAL::vertex_point, this->tmesh, vd);
            pointIndex2 = containsPoint(V, this->numV_sm, p2);
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

    for (int l = 0; l < numE_sm; l++)
    {
        output << "l " << E(l, 0) << " " << E(l, 1) << "\n";
    }
    output.close();
}

void CurveSkeleton::display_once(const Eigen::MatrixXd& V, const Eigen::MatrixXi& E, const float v_size, const float e_size)
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


END_PROJECT_NAMESPACE