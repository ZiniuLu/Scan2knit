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
#include "Segmentation.h"
#include "Display.h"

using namespace PROJECT_NAME;

/// <summary>
/// print_to = 
/// 0...output to console
/// 1...output to gui
/// </summary>
size_t          print_to = 1;      // 0...print to console, 1...output to gui
/// <summary>
/// process_nr = 
/// 0...Reset
/// 1...Load Settings (|| isotropic Remeshing)
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
Polyhedron*     pmesh = NULL;

Skel*           skel = NULL;
SkelGraph*      skel_graph = NULL;
Segmentation*   segmentation = NULL;

void init()
{
    display      = new Display();
    settings     = new Settings();
    gui_console  = new GuiConsole();

    mesh         = new Mesh();

    skel         = new Skel();
    skel_graph   = new SkelGraph();
    segmentation = new Segmentation();
    
}

void clear()
{
    delete display;
    delete settings;
    delete gui_console;

    delete mesh;
    delete skel_graph;
    delete skel;
    delete segmentation;

    display      = NULL;
    settings     = NULL;
    gui_console  = NULL;

    mesh         = NULL;
    tmesh        = NULL;
    pmesh        = NULL;
    skel         = NULL;
    skel_graph   = NULL;
    segmentation = NULL;
}

int main(int argc, char *argv[])
{
    init();

    display->launch();

    clear();
    
    return EXIT_SUCCESS;
}

