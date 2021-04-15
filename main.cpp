/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include "Skeletonization.h"
#include "MeshFile.h"

using namespace PROJECT_NAMESPACE;

void add_files(std::vector<std::string>& files)
{
    //files.push_back("../data/bunny.off");
    //files.push_back("../data/cat.off");
    //files.push_back("../data/duck.off");
    //files.push_back("../data/foot.off");
    //files.push_back("../data/fox.off");
    files.push_back("../data/hand.off");
    //files.push_back("../data/pikachu.off");
    //files.push_back("../data/teddy.off");
}

int main(int argc, char *argv[])
{
    std::vector<std::string> files;
    add_files(files);

    for (const auto& filePath : files)
    {
        //std::cout.flush();
        // 1. open file
        //std::string filePath = "../data/teddy.off";
        //filePath = (argc > 1) ? argv[1] : filePath;

        MeshFile meshFile;
        meshFile.open(filePath);
        if (!meshFile.is_open()) { return EXIT_FAILURE; }

        // 2. get mesh
        Mesh* p_mesh = NULL;
        p_mesh = new Mesh(meshFile);
        if (!p_mesh->is_triangle_mesh())
        {
            delete p_mesh;
            return EXIT_FAILURE;
        }

        // 3. extract skeleton
        Skel* p_skel = NULL;
        p_skel = new Skel(*p_mesh);

        // 4. set skel graph
        SkelGraph* p_skel_graph = NULL;
        p_skel_graph = new SkelGraph(*p_skel);
        p_skel_graph->output_skel_to_files();

        // 5. release pointers
        delete p_skel_graph;
        delete p_skel;
        delete p_mesh;
    }    

    return EXIT_SUCCESS;
}