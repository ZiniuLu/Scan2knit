/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Skeletonization.h"
#include "MeshFile.h"
#include "Display.h"

using namespace PROJECT_NAMESPACE;

void get_settings(Settings& settings)
{
    std::string fileName = "settings/Settings.csv";
    std::string dirs[4] = {
        "",
        "../",
        "../../",
        "../../../",
    };

    std::fstream file;

    for (std::string dir : dirs)
    {
        file.open(dir+fileName, std::ios::in);
        if (file)
        {
            settings.ROOT_DIR = dir;
            break;
        }
    }

    std::string line;
    while (std::getline(file, line))
    {
        size_t len = line.length();
        if (len <= 2) continue; // _ , _ : at least 3 characters

        std::string key, value;

        size_t comma = line.find_first_of(",");
        if (comma > 0)
        {
            key = line.substr(0, comma);
            value = line.substr(comma + 1, len - comma - 1);    //_ _ , _ _ _ _
        }

        if (key == "FILE_NAME") { settings.FILE_NAME = value; }
        else if (key == "obj-scale") { settings.obj_scale = stod(value); }
        else if (key == "stitch-width") { settings.stitch_width = stod(value); }
        else if (key == "stitch-height") { settings.stitch_height = stod(value); }
        else if (key == "stitch-height") { settings.stitch_height = stod(value); }

        //std::cout << "ROOT_DIR = " << settings.ROOT_DIR << std::endl;
        //std::cout << "FILE_NAME = " << settings.FILE_NAME << std::endl;
        //std::cout << "obj_scale = " << settings.obj_scale << std::endl;
        //std::cout << "stitch_width = " << settings.stitch_width << std::endl;
        //std::cout << "stitch_height = " << settings.stitch_height << std::endl;
    }
}

int extract_skeletons(const std::vector<std::string> files)
{
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

        // 4. set skeleton graph
        SkelGraph* p_skel_graph = NULL;
        p_skel_graph = new SkelGraph(*p_skel);
        p_skel_graph->output_skel_to_files();

        // 5. segmentation

        // 6. display
        //Display display;
        //display.display(p_mesh->get_tmesh());


        // 7. release pointers
        delete p_skel_graph;
        delete p_skel;
        delete p_mesh;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    Settings settings;
    get_settings(settings);

    if (settings.FILE_NAME.length() == 0)
    {
        std::cout << "Cannot find file name in Settings.csv !" << std::endl;
        return EXIT_SUCCESS;
    }
    // 2. get mesh
    Mesh mesh("../data/teddy.off");
    if (!mesh.is_triangle_mesh()) { return EXIT_FAILURE; }

    Display display;
    display.display(mesh.get_tmesh());
    //display.display("../data/teddy.off"); 
    
    return EXIT_SUCCESS;
}