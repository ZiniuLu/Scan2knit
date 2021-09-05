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
	inline static RVec_3d rgb(uchar r, uchar g, uchar b)
	{
		return RVec_3d(r / 255, g / 255, b / 255);
	} // convert RGB value from (0~255) to (0~1)

	inline static RVec_3d black() { return RVec_3d(0, 0, 0); }
	inline static RVec_3d white() { return RVec_3d(1, 1, 1); }
	inline static RVec_3d red() { return RVec_3d(1, 0, 0); }
	inline static RVec_3d lime() { return RVec_3d(0, 1, 0); }
	inline static RVec_3d blue() { return RVec_3d(0, 0, 1); }
	inline static RVec_3d yellow() { return RVec_3d(1, 1, 0); }
	inline static RVec_3d cyan() { return RVec_3d(0, 1, 1); }
	inline static RVec_3d magenta() { return RVec_3d(1, 0, 1); }
	inline static RVec_3d silver() { return RVec_3d(0.75, 0.75, 0.75); }
	inline static RVec_3d gray() { return RVec_3d(0.5, 0.5, 0.5); }
	inline static RVec_3d maroon() { return RVec_3d(0.5, 0, 0); }
	inline static RVec_3d olive() { return RVec_3d(0.5, 0.5, 0); }
	inline static RVec_3d green() { return RVec_3d(0, 0.5, 0); }
	inline static RVec_3d purple() { return RVec_3d(0.5, 0, 0.5); }
	inline static RVec_3d teal() { return RVec_3d(0, 0.5, 0.5); }
	inline static RVec_3d navy() { return RVec_3d(0, 0, 0.5); }
	inline static RVec_3d random() { return RVec_3d::Random().array().pow(2); }
	inline static MAT_3d random(size_t row)
	{
		MAT_3d color;
		color.resize(row, 3);
		for (size_t i = 0; i < row; i++)
		{
			color.row(i) << RVec_3d::Random().array().pow(2);
		}
		return color;
	}
};

struct IglGeometry
{
	std::string name;
	size_t id;
	bool show = true;

	MAT_3d V;
	MAT_2i E;
	MAT_3i F;

	MAT_3d color_V;
	MAT_3d color_E;
	MAT_3d color_F;

	IglGeometry() {}
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_2i E, MAT_3d color_V, MAT_3d color_E) :name(name), id(id), V(V), E(E), color_V(color_V), color_E(color_E) { }
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_3i F, MAT_3d color_V, MAT_3d color_F) :name(name), id(id), V(V), F(F), color_V(color_V), color_F(color_F) { }
	IglGeometry(std::string name, size_t id, MAT_3d V, MAT_2i E, MAT_3i F, MAT_3d color_V, MAT_3d color_E, MAT_3d color_F) :name(name), id(id), V(V), E(E), F(F), color_V(color_V), color_E(color_E), color_F(color_F) { }

	inline void get_colors_V(MAT_3d& colors_V)
	{
		size_t numV = this->V.rows();
		colors_V.resize(numV, 3);
		size_t i = 0;
		for (; i < numV; i++)
		{
			colors_V.row(i) << this->color_V;
		}
	}
	inline void get_colors_E(MAT_3d& colors_E)
	{
		size_t numE = this->E.rows();
		colors_E.resize(numE, 3);
		size_t i = 0;
		for (; i < numE; i++)
		{
			colors_E.row(i) << this->color_E;
		}
	}
	inline void get_colors_F(MAT_3d& colors_F)
	{
		size_t numF = this->F.rows();
		colors_F.resize(numF, 3);
		size_t i = 0;
		for (; i < numF; i++)
		{
			colors_F.row(i) << this->color_F;
		}
	}
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

	inline void	set(std::vector<IglGeometry>& geoms)
	{
		this->x_min = 0;
		this->x_max = 0;
		this->y_min = 0;
		this->y_max = 0;
		this->z_min = 0;
		this->z_max = 0;

		for (IglGeometry& geo : geoms)
		{
			MAT_3d& my_V = geo.V;
			size_t numV = my_V.rows();
			if (numV <= 0) { return; }

			RVec_3d m = my_V.colwise().minCoeff();
			RVec_3d M = my_V.colwise().maxCoeff();

			this->x_min = m(0);
			this->x_max = M(0);
			this->y_min = m(1);
			this->y_max = M(1);
			this->z_min = m(2);
			this->z_max = M(2);
		}
	}
	inline void	get_bounding_box_V(MAT_3d& V/*, MAT_3i& F*/)
	{
		V.resize(8, 3);
		V << this->x_min, this->y_min, this->z_min,
			this->x_max, this->y_min, this->z_min,
			this->x_max, this->y_max, this->z_min,
			this->x_min, this->y_max, this->z_min,
			this->x_min, this->y_min, this->z_max,
			this->x_max, this->y_min, this->z_max,
			this->x_max, this->y_max, this->z_max,
			this->x_min, this->y_max, this->z_max;

		//F <<0, 2, 1,
		//    0, 3, 2,
		//    0, 1, 5,
		//    0, 5, 4,
		//    0, 4, 7,
		//    0, 7, 3,
		//    6, 7, 4,
		//    6, 4, 5,
		//    6, 5, 1,
		//    6, 1, 2,
		//    6, 7, 3,
		//    6, 2, 3;
	}
	inline RVec_3d get_xyz_size()
	{
		return RVec_3d(
			this->x_max - this->x_min,
			this->y_max - this->y_min,
			this->z_max - this->z_min);
	}
};




struct GuiDisplay
{
	IGL_Viewer* viewer;
	IGL_Gui* gui;
	std::vector<IglGeometry>* geoms;

	GuiDisplay(IGL_Viewer* p_viewer, IGL_Gui* p_gui, std::vector<IglGeometry>* p_geoms);

	inline void Reset()
	{
		this->viewer = NULL;
		this->geoms = NULL;
	}
	void Draw(const char* title, bool* p_open);
	//void AddGeometry();

};

struct GuiControl
{
	GuiControl();

	inline void Reset(){ }
	void Draw(const char* title, bool* p_open);
};




class Display
{
private:
	/// <summary>
	/// geo_names[iterator_1][iterator_2]
	/// iterator_1: process_nr --- [0...5]	  --- reference "process_nr" in main.cpp
	/// iterator_2: geo_nr     --- [0...Inf.] --- geometry number in current process_nr
	/// </summary>
	//std::vector<std::vector<std::string>> geo_names;
	std::vector<IglGeometry> igl_geoms;	// index start from 0
	IGL_Viewer viewer;
	IGL_Gui    gui;
	Bound	   bound; // xyz-size of geometry; max of all bounding boxes

	inline void insert_geometry(const IglGeometry& geometry) { this->igl_geoms.push_back(geometry); }

	void insert_obj(std::string path);
	void insert_off(std::string path);
	void insert_stl(std::string path);

	void insert_skel_graph(SkelGraph& skel_graph);
	void insert_tmesh(Triangle_mesh& tmesh);
	void insert_pmesh(Polyhedron& pmesh);
	void insert_skeleton(Skeleton& skeleton);
	void insert_skel(Skel& skel);
	void insert_segment(Segment& segment);
	void insert_segments(std::vector<Segment>& segments);

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

	Display(Triangle_mesh& tmesh);
	Display(Skeleton& skeleton);
	Display(Skel& skel);
	Display(SkelGraph& skel_graph);
	Display(Segment& segment);
	Display(std::vector<Segment>& segments);
	Display(Stitches& stitches);

	Display(MAT_3d& V, MAT_2i& E, MAT_3i& F);
	Display(MAT_3d& V, MAT_3i& F);
	Display(MAT_3d& V, MAT_2i& E);
	Display(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F);
	Display(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F);
	Display(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E);

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

	inline void insert(Triangle_mesh& tmesh)// display triangle mesh
	{
		this->insert_tmesh(tmesh);
	}
	inline void insert(Polyhedron& pmesh)// display triangle mesh
	{
		this->insert_pmesh(pmesh);
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
	inline void insert(Segment& segment)
	{
		this->insert_segment(segment);
	}
	inline void insert(std::vector<Segment>& segments)
	{
		this->insert_segments(segments);
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
	inline void insert(MAT_3d& V, MAT_2i& E, MAT_3i& F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, E, F, IglColor::black(), IglColor::black(), IglColor::yellow());

		this->insert_geometry(my_geo);
	}
	inline void insert(MAT_3d& V, MAT_3i& F)
	{
		size_t id = this->igl_geoms.size() + 1;
		IglGeometry my_geo("geometry_" + id, id, V, F, IglColor::black(), IglColor::yellow());

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

	void erase(size_t& process_nr);

	inline void launch()
	{
		set_gui();
		set_events();

		viewer.launch(true, false, TO_STRING(PROJECT_NAME));
	}
	void update();
};

END_PROJECT_NAMESPACE