/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include<string>

# define PROJECT_NAMESPACE Scan2knit
# define BEGIN_PROJECT_NAMESPACE namespace PROJECT_NAMESPACE {
# define END_PROJECT_NAMESPACE }

BEGIN_PROJECT_NAMESPACE

struct MESH_FILE
{
	std::string folder_path;
	std::string name;
};

struct STITCH
{
	double obj_scale;
	double stitch_width;
	double stitch_height;
	double layer0_offset;
};

struct VIEWER
{
	double point_size;
	double line_width;
	double camera_zoom;
};

struct Settings
{
	bool		loaded;
	std::string root_path;	// root directory of current solution

	MESH_FILE   File;	// file name of 3d mesh, read from Settings.csv
	STITCH		Stitch;
	VIEWER      Viewer;

	void load();
	void print();
};


END_PROJECT_NAMESPACE