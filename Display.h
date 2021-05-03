/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <string>

#include "project.h"
//#include "cgalHeader.h"
#include "iglHeader.h"
#include "Skeletonization.h"


#define V_SIZE 5
#define E_SIZE 3.0f
#define CAM_ZOOM 0.05;

BEGIN_PROJECT_NAMESPACE

class Display
{
private:
	MAT_3d	   V;
	MAT_2i	   E;
	MAT_3i	   F;
	MAT_3d	   N;	//face normal
	IGL_Viewer viewer;

	void display_tmesh(const Polyhedron& tmesh);
	void display_skeleton(const Skeleton& skeleton);
	void display_skel(const Skel& skel);
	void display_skel_graph(const SkelGraph& skel_graph);


public:
	Display();
	Display(const std::string path);
	Display(const MAT_3d& V, const MAT_3i& F);
	Display(const MAT_3d& V, const MAT_2i& E);
	Display(const Polyhedron& tmesh);
	Display(const Skeleton& skeleton);
	Display(const Skel& skel);
	Display(const SkelGraph& skel_graph);

	void display(const std::string path);
	void display(const Polyhedron& tmesh);// display triangle mesh
	void display(const Skeleton& skeleton);
	void display(const Skel& skel);
	void display(const SkelGraph& skel_graph);
	void display(const MAT_3d& V, const MAT_3i& F);
	void display(const MAT_3d& V, const MAT_2i& E);

	void display_obj(const std::string path);
	void display_off(const std::string path);
	void display_stl(const std::string path);
};

END_PROJECT_NAMESPACE