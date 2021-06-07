/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <string>
#include <vector>

#include "project.h"
#include "iglHeader.h"
#include "Skeletonization.h"
#include "Segmentation.h"
#include "Stitch.h"


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

struct IglGeometry
{
	std::string name;
	size_t id;
	bool show = true;

	MAT_3d V;
	MAT_2i E;
	MAT_3i F;

	RVec_3d color_V;
	RVec_3d color_E;
	RVec_3d color_F;

	IglGeometry() {}
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_2i E, RVec_3d color_V, RVec_3d color_E) :name(name), id(id), V(V), E(E), color_V(color_V), color_E(color_E) { }
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_3i F, RVec_3d color_V, RVec_3d color_F) :name(name), id(id), V(V), F(F), color_V(color_V), color_F(color_F) { }
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_2i E, MAT_3i F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F) :name(name), id(id), V(V), E(E), F(F), color_V(color_V), color_E(color_E), color_F(color_F) { }

	void get_colors_V(MAT_3d& colors_V);
	void get_colors_E(MAT_3d& colors_E);
	void get_colors_F(MAT_3d& colors_F);
	//static void get_colors_V(const std::vector<IglGeometry>& igl_geoms, MAT_3d& colors_V);
	//static void get_colors_E(const std::vector<IglGeometry>& igl_geoms, MAT_3d& colors_E);
	//static void get_colors_F(const std::vector<IglGeometry>& igl_geoms, MAT_3d& colors_F);
};

struct Bound
{
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;

	void	set(std::vector<IglGeometry>& geoms);
	void	get_bounding_box_V(MAT_3d& V/*, MAT_3i& F*/);
	RVec_3d get_xyz_size();
};




struct GuiDisplay
{
	IGL_Viewer* viewer;
	IGL_Gui* gui;
	std::vector<IglGeometry>* geoms;

	GuiDisplay(IGL_Viewer* p_viewer, IGL_Gui* p_gui, std::vector<IglGeometry>* p_geoms);

	void Reset();
	void Draw(const char* title, bool* p_open);
	void AddGeometry();

};

struct GuiControl
{
	GuiControl();

	void Reset();
	void Draw(const char* title, bool* p_open);
};




class Display
{
private:
	std::vector<IglGeometry> igl_geoms;	// index start from 0
	IGL_Viewer viewer;
	IGL_Gui    gui;
	Bound	   bound; // xyz-size of geometry; max of all bounding boxes

	inline void insert_geometry(IglGeometry& geometry) { this->igl_geoms.push_back(geometry); }

	void insert_obj(std::string path);
	void insert_off(std::string path);
	void insert_stl(std::string path);

	void insert_skel_graph(SkelGraph& skel_graph);
	void insert_tmesh(Triangle_mesh& tmesh);
	void insert_skeleton(Skeleton& skeleton);
	void insert_skel(Skel& skel);
	void insert_segmentation(Segmentation& segmentation);

	void insert_stitches(Stitches& stitches);
	void insert_stitches(Stitches& stitches, size_t pos);
	void insert_stitches(Stitches& stitches, size_t pos, size_t len);

	void set_parameter(const std::string& geom_name);
	void set_camera_zoom();
	void set_gui();
	void set_events();

	static bool key_down(IGL_Viewer& viewer, unsigned char key, int modifier);


public:
	Display();
	Display(std::string path);
	Display(IglGeometry geometry);
	Display(MAT_3d& V, MAT_2i& E, MAT_3i& F);
	Display(MAT_3d& V, MAT_3i& F);
	Display(MAT_3d& V, MAT_2i& E);
	Display(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F);
	Display(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F);
	Display(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E);

	Display(Triangle_mesh& tmesh);
	Display(Skeleton& skeleton);
	Display(Skel& skel);
	Display(SkelGraph& skel_graph);
	Display(SkelChain& skel_chain);
	Display(Stitches& stitches);

	inline void insert(std::string path)
	{
		size_t pos_max = path.length();
		if (path.find(".off") < pos_max)
		{
			this->insert_off(path);
		}
		else if (path.find(".obj") < pos_max)
		{
			this->insert_obj(path);
		}
		else if (path.find(".stl") < pos_max)
		{
			this->insert_stl(path);
		}
	}
	inline void insert(IglGeometry& geometry)
	{
		this->insert_geometry(geometry);
	}
	inline void insert(MAT_3d& V, MAT_2i& E, MAT_3i& F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, E, F, IglColor::black(), IglColor::black(), IglColor::yellow());

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_3i& F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_"+id, id, V, F, IglColor::black(), IglColor::yellow());

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_2i& E)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, E, IglColor::black(), IglColor::black());

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, E, F, color_V, color_E, color_F);

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, F, color_V, color_F);

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, E, color_V, color_E);

		this->insert_geometry(my_geo);
	}

	inline void insert(Triangle_mesh& tmesh)// display triangle mesh
	{
		this->insert_tmesh(tmesh);
	}
	inline void insert(Skel& skel)			// 
	{
		this->insert_skel(skel);
	}
	inline void insert(Skeleton& skeleton)	// display mcf-skeleton
	{
		this->insert_skeleton(skeleton);
	}
	inline void insert(SkelGraph& skel_graph)
	{
		this->insert_skel_graph(skel_graph);
	}

	inline void insert(Stitches& stitches)
	{
		this->insert_stitches(stitches);
	}
	inline void insert(Stitches& stitches, size_t pos)
	{
		insert_stitches(stitches, pos);
	}
	inline void insert(Stitches& stitches, size_t pos, size_t len)
	{
		insert_stitches(stitches, pos, len);
	}

	void erase(size_t& process_nr);

	void display_default();
	void display();
};

END_PROJECT_NAMESPACE