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

struct REMESHING
{
	double		remeshing_edge_length = 1;
	size_t		remeshing_nr_iter = 3;
	std::string	remeshing_out_suffix = "_rem.off";
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

struct SKEL_OUT
{
	std::string out_file_suffix = ".obj";
	std::string out_suffix_skel = "_skel";
	std::string out_suffix_skel_map = "_skel_map";
	std::string out_suffix_skel_ext = "_skel_ext";
};

struct Settings
{
	std::string root_path;	// root directory of current solution

	MESH_FILE	File;	// file name of 3d mesh, read from Settings.csv
	REMESHING	Remeshing;
	STITCH		Stitch;
	VIEWER		Viewer;
	SKEL_OUT	Skel_out;

	bool load(size_t& output);
	void print(size_t& output);
};

END_PROJECT_NAMESPACE


extern PROJECT_NAME::Settings* settings;