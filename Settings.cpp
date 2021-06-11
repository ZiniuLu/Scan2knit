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

    Print("\troot path = \"" + this->root_path + "\"");
    Print("\tLoading settings from: \"" + this->root_path + fileName + "\" ... ");

    // get input
    std::string line;

    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

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

        // input
        if (key == "folder_path") { value >> this->File.folder_path; }
        else if (key == "file_name") { value >> this->File.name; }

        // remeshing
        else if (key == "remeshing_edge_length") { value >> this->Remeshing.remeshing_edge_length; }
        else if (key == "remeshing_nr_iter") { value >> this->Remeshing.remeshing_nr_iter; }
        else if (key == "remeshing_out_suffix") { value >> this->Remeshing.remeshing_out_suffix; }

        // stitch
        else if (key == "obj_scale") { value >> this->Stitch.obj_scale; }
        else if (key == "stitch_width") { value >> this->Stitch.stitch_width; }
        else if (key == "stitch_height") { value >> this->Stitch.stitch_height; }
        else if (key == "layer0_offset") { value >> this->Stitch.layer0_offset; }

        // viewer
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

        // output
        else if (key == "out_file_suffix") { value >> this->Skel_out.out_file_suffix; }
        else if (key == "out_suffix_skel") { value >> this->Skel_out.out_suffix_skel; }
        else if (key == "out_suffix_skel_map") { value >> this->Skel_out.out_suffix_skel_map; }
        else if (key == "out_suffix_skel_ext") { value >> this->Skel_out.out_suffix_skel_ext; }



        else if (key == "output") { value >> output; } // output = 0...console, 1...gui
    } // while

    return (this->File.name.length() > 0);
} // load()

void Settings::print(size_t& output)
{
    Print("\tprint all settings ...");

    Print("\tSettings:");
    Print("\t\troot_path = " + this->root_path);

    Print("\tFile:");
    Print("\t\tfolder_path = " + this->File.folder_path);
    Print("\t\tname = " + this->File.name);

    Print("\tStitch:");
    Print("\t\tobj_scale = " + std::to_string(this->Stitch.obj_scale));
    Print("\t\tstitch_width = " + std::to_string(this->Stitch.stitch_width));
    Print("\t\tstitch_height = " + std::to_string(this->Stitch.stitch_height));
    Print("\t\tlayer0_offset = " + std::to_string(this->Stitch.layer0_offset));

    Print("\tViewer:");
    Print("\t\tpoint_size_default = " + std::to_string(this->Viewer.point_size_default));
    Print("\t\tline_width_default = " + std::to_string(this->Viewer.line_width_default));
    Print("\t\tpoint_size_mesh = " + std::to_string(this->Viewer.point_size_mesh));
    Print("\t\tline_width_mesh = " + std::to_string(this->Viewer.line_width_mesh));
    Print("\t\tpoint_size_skel = " + std::to_string(this->Viewer.point_size_skel));
    Print("\t\tline_width_skel = " + std::to_string(this->Viewer.line_width_skel));
    Print("\t\tpoint_size_skel_map = " + std::to_string(this->Viewer.point_size_skel_map));
    Print("\t\tline_width_skel_map = " + std::to_string(this->Viewer.line_width_skel_map));
    Print("\t\tpoint_size_skel_ext = " + std::to_string(this->Viewer.point_size_skel_ext));
    Print("\t\tline_width_skel_ext = " + std::to_string(this->Viewer.line_width_skel_ext));
    Print("\t\tpoint_size_stitch = " + std::to_string(this->Viewer.point_size_stitch));
    Print("\t\tline_width_stitch = " + std::to_string(this->Viewer.line_width_stitch));

    Print("\tOut:");
    Print("\t\tout_file_suffix = " + this->Skel_out.out_file_suffix);
    Print("\t\tout_suffix_skel = " + this->Skel_out.out_suffix_skel);
    Print("\t\tout_suffix_skel_map = " + this->Skel_out.out_suffix_skel_map);
    Print("\t\tout_suffix_skel_ext = " + this->Skel_out.out_suffix_skel_ext);

    Print("\tOutput:");
    Print("\t\toutput = " + output);

    Print("\tdone.\n");
}

END_PROJECT_NAMESPACE