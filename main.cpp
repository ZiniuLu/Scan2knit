/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "project.h"
#include "MeshFile.h"
#include "Skeletonization.h"
#include "Display.h"

using namespace PROJECT_NAMESPACE;

Settings settings;

int main(int argc, char *argv[])
{
    // 1. get parameters from Settings.csv
    settings.load();
    if (!settings.loaded)
    {
        std::cout << "Cannot find file name in Settings.csv !" << std::endl;
        return EXIT_SUCCESS;
    }

    // 2. open mesh file
    std::string file = settings.root_path + settings.File.folder_path + settings.File.name;
    Mesh mesh(file);
    if (!mesh.is_triangle_mesh()) 
    { 
        return EXIT_FAILURE; 
    }

    // 3. skeletonization
    Skel skel(mesh);
    //Skeleton& skeleton = skel.get_skeleton();
    //Triangle_mesh& tmesh = mesh.get_tmesh();
    SkelGraph skel_graph(skel, mesh);
    skel_graph.output_skel_to_files();

    // 4. segmentation
    //Segmentation segmentation(skeleton, tmesh);

    Display display;
    display.display(skel_graph);
    //display.display("../data/teddy.off"); 
    
    return EXIT_SUCCESS;
}