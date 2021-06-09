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
#include "Settings.h"
#include "MeshFile.h"
#include "Skeletonization.h"
#include "Display.h"

using namespace PROJECT_NAME;

/// <summary>
/// output = 
/// 0...output to console
/// 1...output to gui
/// </summary>
size_t          print_to = 1;      // 0...print to console, 1...output to gui
/// <summary>
/// process_nr = 
/// 0...Reset
/// 1...Load Settings
/// 2...Load Mesh File
/// 3...extract skeleton
/// 4...segmentation
/// 5...slicing
/// </summary>
size_t          process_nr = 0;
bool            settings_loaded = false;
bool            is_triangle_mesh = false;

Display*        display = NULL;
Settings*       settings = NULL;
GuiConsole*     gui_console = NULL;

Mesh*           mesh = NULL;
Triangle_mesh*  tmesh = NULL;

Skel*           skel = NULL;
SkelGraph*      skel_graph = NULL;

void init()
{
    display = new Display();
    settings = new Settings();
    gui_console = new GuiConsole();

    mesh = new Mesh();

    skel = new Skel();
    skel_graph = new SkelGraph();
}

void clear()
{
    delete display;
    delete settings;
    delete gui_console;

    delete mesh;
    delete skel_graph;
    delete skel;

    display     = NULL;
    settings    = NULL;
    gui_console = NULL;

    mesh        = NULL;
    tmesh       = NULL;
    skel        = NULL;
    skel_graph  = NULL;
}

int main(int argc, char *argv[])
{
    init();

    display->launch();

    clear();
    
    return EXIT_SUCCESS;
}


//{
    // 1. get parameters from Settings.csv
    //settings.load();
    //if (!settings.loaded)
    //{
    //    std::cout << "Cannot find file name in Settings.csv !" << std::endl;
    //    return EXIT_SUCCESS;
    //}
    //
    //Display display;
    //display.display_default();
    //
    //// 2. open mesh file
    //std::string file = settings.root_path + settings.File.folder_path + settings.File.name;
    //Mesh mesh(file);
    //if (!mesh.is_triangle_mesh()) 
    //{ 
    //    return EXIT_FAILURE; 
    //}
    //
    //// 3. skeletonization
    //Skel skel(mesh);
    ////Skeleton& skeleton = skel.get_skeleton();
    ////Triangle_mesh& tmesh = mesh.get_tmesh();
    //SkelGraph skel_graph(skel, mesh);
    //skel_graph.output_skel_to_files();
    //
    //// 4. segmentation
    ////Segmentation segmentation(skeleton, tmesh);
    //
    ////Display display;
    //display.insert(mesh.get_tmesh());
    //display.insert(skel_graph);
    //display.display();
    ////display.display("../data/teddy.off"); 
//}