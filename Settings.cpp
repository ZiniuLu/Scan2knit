/* Created on: 04/06/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <direct.h>

#include "project.h"
#include "Settings.h"

BEGIN_PROJECT_NAMESPACE

bool Settings::load(size_t& output)
{
    // find root path
    std::fstream file;
    this->root_path = "../";

    std::string fileName = "settings/Settings.csv";
    std::string dirs[4] = {
        "",
        "../",
        "../../",
        "../../../",
    };

    for (std::string& dir : dirs)
    {
        file.open(dir + fileName, std::ios::in);
        if (file)
        {
            this->root_path = dir;
            break;
        }
    }

    Print("\tFound root path = \"" + this->root_path + "\"");
    Print("Loading parameters from: \"" + this->root_path + fileName + "\" ... ");

    // get input
    std::string line;

    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

        size_t len = line.length();
        if (len <= 2) continue; // _ , _ : at least 3 characters

        size_t idx_c = line.find('#');
        if (idx_c == 0) { continue; } // skip comment
        else if (idx_c > 0) { line = line.substr(0, idx_c); }

        std::string key;
        std::stringstream value;

        size_t comma = line.find_first_of(",");
        if (comma > 0)
        {
            key = line.substr(0, comma);
            value = std::stringstream(line.substr(comma + 1, len - comma - 1));    //_ _ , _ _ _ _
        }

        if (key == "folder_path") { value >> this->File.folder_path; }
        else if (key == "file_name") { value >> this->File.name; }

        else if (key == "obj_scale") { value >> this->Stitch.obj_scale; }
        else if (key == "stitch_width") { value >> this->Stitch.stitch_width; }
        else if (key == "stitch_height") { value >> this->Stitch.stitch_height; }
        else if (key == "layer0_offset") { value >> this->Stitch.layer0_offset; }

        else if (key == "viewer_name") { value >> this->Viewer.viewer_name; }
        else if (key == "viewer_point_size_default") { value >> this->Viewer.point_size_default; }
        else if (key == "viewer_line_width_default") { value >> this->Viewer.line_width_default; }
        else if (key == "viewer_point_size_mesh") { value >> this->Viewer.point_size_mesh; }
        else if (key == "viewer_line_width_mesh") { value >> this->Viewer.line_width_mesh; }
        else if (key == "viewer_point_size_skel") { value >> this->Viewer.point_size_skel; }
        else if (key == "viewer_line_width_skel") { value >> this->Viewer.line_width_skel; }
        else if (key == "viewer_point_size_skel_map") { value >> this->Viewer.point_size_skel_map; }
        else if (key == "viewer_line_width_skel_map") { value >> this->Viewer.line_width_skel_map; }
        else if (key == "viewer_point_size_skel_ext") { value >> this->Viewer.point_size_skel_ext; }
        else if (key == "viewer_line_width_skel_ext") { value >> this->Viewer.line_width_skel_ext; }
        else if (key == "viewer_point_size_stitch") { value >> this->Viewer.point_size_stitch; }
        else if (key == "viewer_line_width_stitch") { value >> this->Viewer.line_width_stitch; }
        else if (key == "viewer_line_width_stitch") { value >> this->Viewer.line_width_stitch; }
        else if (key == "output") { value >> output; } // output = 0...console, 1...gui
    } // while

    return (this->File.name.length() > 0);
} // load()

void Settings::print(size_t& output)
{
    Print("print all settings ...");
    Print(" ");

    Print("Settings:");
    Print("\troot_path = " + this->root_path);
    Print(" ");

    Print("File:");
    Print("\tfolder_path = " + this->File.folder_path);
    Print("\tname = " + this->File.name);
    Print(" ");

    Print("Stitch:");
    Print("\tobj_scale = " + std::to_string(this->Stitch.obj_scale));
    Print("\tstitch_width = " + std::to_string(this->Stitch.stitch_width));
    Print("\tstitch_height = " + std::to_string(this->Stitch.stitch_height));
    Print("\tlayer0_offset = " + std::to_string(this->Stitch.layer0_offset));
    Print(" ");

    Print("Viewer:");
    Print("\tviewer_name = " + this->Viewer.viewer_name);
    Print("\tpoint_size_default = " + std::to_string(this->Viewer.point_size_default));
    Print("\tline_width_default = " + std::to_string(this->Viewer.line_width_default));
    Print("\tpoint_size_mesh = " + std::to_string(this->Viewer.point_size_mesh));
    Print("\tline_width_mesh = " + std::to_string(this->Viewer.line_width_mesh));
    Print("\tpoint_size_skel = " + std::to_string(this->Viewer.point_size_skel));
    Print("\tline_width_skel = " + std::to_string(this->Viewer.line_width_skel));
    Print("\tpoint_size_skel_map = " + std::to_string(this->Viewer.point_size_skel_map));
    Print("\tline_width_skel_map = " + std::to_string(this->Viewer.line_width_skel_map));
    Print("\tpoint_size_skel_ext = " + std::to_string(this->Viewer.point_size_skel_ext));
    Print("\tline_width_skel_ext = " + std::to_string(this->Viewer.line_width_skel_ext));
    Print("\tpoint_size_stitch = " + std::to_string(this->Viewer.point_size_stitch));
    Print("\tline_width_stitch = " + std::to_string(this->Viewer.line_width_stitch));
    Print(" ");

    Print("Output:");
    Print("\toutput = " + output);

    Print("done.\n");
}

END_PROJECT_NAMESPACE