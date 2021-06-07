/* Created on: 04/06/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */
#pragma once

#include "project.h"

BEGIN_PROJECT_NAMESPACE

struct MESH_FILE
{
	std::string folder_path;
	std::string name;
};

struct STITCH
{
	double obj_scale = 1;
	double stitch_width = 0.96;
	double stitch_height = 0.42;
	double layer0_offset = 1;
};

struct VIEWER
{
	std::string viewer_name;

	float point_size_default = 1;
	float line_width_default = 1;

	float point_size_mesh = 1;
	float line_width_mesh = 1;

	float point_size_skel = 5;
	float line_width_skel = 5;

	float point_size_skel_map = 1;
	float line_width_skel_map = 1;

	float point_size_skel_ext = 5;
	float line_width_skel_ext = 5;

	float point_size_stitch = 5;
	float line_width_stitch = 5;
};

struct Settings
{
	std::string root_path;	// root directory of current solution

	MESH_FILE File;	// file name of 3d mesh, read from Settings.csv
	STITCH    Stitch;
	VIEWER    Viewer;

	bool load(size_t& output);
	void print(size_t& output);
};

END_PROJECT_NAMESPACE