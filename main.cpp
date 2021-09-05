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

void init()
{
    // declared in project.h
    print_to     = 1;
    process_nr   = 0;
    //settings_loaded  = false;
    //is_triangle_mesh = false;
    gui_console  = new GuiConsole();

    // declared in Display.h
    display      = new Display();

    // declared in Settings.h
    settings     = new Settings();

    // declared in MeshFile.h
    mesh         = new Mesh();

    // declared in Skeletonization.h
    skel         = new Skel();
    skel_graph   = new SkelGraph();

    // declared in Segmentation.h
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