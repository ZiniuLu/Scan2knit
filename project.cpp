/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "project.h"

BEGIN_PROJECT_NAMESPACE

void Settings::load()
{
    this->loaded = false;
    this->root_path = "../";

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
        file.open(dir + fileName, std::ios::in);
        if (file) 
        {
            this->root_path = dir;
            break;
        }
    }

    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

        size_t len = line.length();
        if (len <= 2) continue; // _ , _ : at least 3 characters
        if (line[0] == '#') continue; // skip comment

        std::string key, value;

        size_t comma = line.find_first_of(",");
        if (comma > 0)
        {
            key = line.substr(0, comma);
            value = line.substr(comma + 1, len - comma - 1);    //_ _ , _ _ _ _
        }

        if      (key == "folder_path") { this->File.folder_path = value; }
        else if (key == "file_name")   { this->File.name = value; }
        
        else if (key == "obj_scale")     { this->Stitch.obj_scale = stod(value); }
        else if (key == "stitch_width")  { this->Stitch.stitch_width = stod(value); }
        else if (key == "stitch_height") { this->Stitch.stitch_height = stod(value); }
        else if (key == "layer0_offset") { this->Stitch.layer0_offset = stod(value); }
        
        else if (key == "viewer_point_size")  { this->Viewer.point_size = stod(value); }
        else if (key == "viewer_line_width")  { this->Viewer.line_width = stof(value); }
        else if (key == "viewer_camera_zoom") { this->Viewer.camera_zoom = stof(value); }

        if (this->File.name.length() > 0) { this->loaded = true; }
    }
}

void Settings::print() 
{
    std::cout << "print all settings ..." << std::endl;

    std::cout << "Settings:" << std::endl;
    std::cout << "loaded = " << this->loaded << std::endl;
    std::cout << "root_path = " << this->root_path << std::endl;

    std::cout << "\nFile:" << std::endl;
    std::cout << "folder_path = " << this->File.folder_path << std::endl;
    std::cout << "name = " << this->File.name << std::endl;

    std::cout << "\nStitch:" << std::endl;
    std::cout << "obj_scale = " << this->Stitch.obj_scale << std::endl;
    std::cout << "stitch_width = " << this->Stitch.stitch_width << std::endl;
    std::cout << "stitch_height = " << this->Stitch.stitch_height << std::endl;
    std::cout << "layer0_offset = " << this->Stitch.layer0_offset << std::endl;

    std::cout << "\nViewer:" << std::endl;
    std::cout << "obj_scale = " << this->Viewer.point_size << std::endl;
    std::cout << "line_width = " << this->Viewer.line_width << std::endl;
    std::cout << "camera_zoom = " << this->Viewer.camera_zoom << std::endl;
    std::cout << "done." << std::endl;
}


END_PROJECT_NAMESPACE
