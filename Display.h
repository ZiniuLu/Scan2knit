/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <string>

#include "project.h"
//#include "cgalHeader.h"
#include "iglHeader.h"
#include "Skeletonization.h"
#include "Segmentation.h"
#include "Stitch.h"


//#define V_SIZE 5
//#define E_SIZE 3.0f
//#define CAM_ZOOM 0.1;

BEGIN_PROJECT_NAMESPACE

class IglColor
{
	typedef unsigned char uchar;
public:
	static RVec_3d rgb(uchar r, uchar g, uchar b); // convert RGB value from (0~255) to (0~1)

	static RVec_3d black();
	static RVec_3d white();
	static RVec_3d red();
	static RVec_3d lime();
	static RVec_3d blue();
	static RVec_3d yellow();
	static RVec_3d cyan();
	static RVec_3d magenta();
	static RVec_3d silver();
	static RVec_3d gray();
	static RVec_3d maroon();
	static RVec_3d olive();
	static RVec_3d green();
	static RVec_3d purple();
	static RVec_3d teal();
	static RVec_3d navy();
};


struct BoundingBox
{
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;

	void set(MAT_3d& V);
};

class Display
{
public:
	bool	   show_menu = false;

private:
	MAT_3d	   V;
	MAT_2i	   E;	// index start from 0
	MAT_3i	   F;	// index start from 0
	IGL_Viewer viewer;
	IGL_Menu   menu;
	RVec_3d	   size; // object size in xyz


	void display_obj(std::string path);
	void display_off(std::string path);
	void display_stl(std::string path);

	void display_tmesh(Triangle_mesh& tmesh);
	void display_skeleton(Skeleton& skeleton);
	void display_skel(Skel& skel);
	void display_skel_graph(SkelGraph& skel_graph);
	void display_skel_graph(SkelGraph& skel_graph, std::string argv);	// display skel_map, skel_extention, skel_chains
	void display_segmentation(Segmentation& segmentation);

	void display_stitches(Stitches& stitches);
	void display_stitches(Stitches& stitches, size_t pos);
	void display_stitches(Stitches& stitches, size_t pos, size_t len);

	void set_parameter();
	void set_menu();


public:
	Display();
	Display(std::string path);
	Display(MAT_3d& V, MAT_2i& E, MAT_3i& F);
	Display(MAT_3d& V, MAT_3i& F);
	Display(MAT_3d& V, MAT_2i& E);

	Display(Triangle_mesh& tmesh);
	Display(Skeleton& skeleton);
	Display(Skel& skel);
	Display(SkelGraph& skel_graph);
	Display(SkelChain& skel_chain);
	Display(Stitches& stitches);

	void display(std::string path);
	void display(MAT_3d& V, MAT_2i& E, MAT_3i& F);
	void display(MAT_3d& V, MAT_3i& F);
	void display(MAT_3d& V, MAT_2i& E);
	void display(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F);
	void display(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F);
	void display(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E);

	void display(Triangle_mesh& tmesh);	// display triangle mesh
	void display(Skeleton& skeleton);	// display mcf-skeleton
	void display(Skel& skel);			// 
	void display(SkelGraph& skel_graph);
	void display(SkelGraph& skel_graph, char nd_type);

	void display(Stitches& stitches);
	void display(Stitches& stitches, size_t pos);
	void display(Stitches& stitches, size_t pos, size_t len);
};
END_PROJECT_NAMESPACE