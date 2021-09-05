/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include "project.h"
#include "cgalHeader.hpp"
#include "iglHeader.h"
#include <vector>

typedef std::vector<std::string> v_string;

BEGIN_PROJECT_NAMESPACE

class MeshFile
{
private:
	std::vector<std::string> filePath; // fileDir + fileName + fileType
	std::ifstream			 input1;
	std::ifstream			 input2;
	bool					 isOpen = false;

public:
	MeshFile();
	MeshFile(const std::string& filePath);

	bool open(const std::string& filePath);

	const bool						is_open() const;
	const std::vector<std::string>& get_file_path() const;
	std::ifstream&					get_ifstream1();
	std::ifstream&					get_ifstream2();

	static int isotropic_remeshing(std::string& in_path, std::string& out_path, double edge_length, size_t nb_iter);
};

class Mesh
{
private:
	v_string	  filePath;	// path, filename, suffix
	bool		  is_tmesh = false;
	Triangle_mesh tmesh;
	Polyhedron	  pmesh;

public:
	Mesh();
	Mesh(const std::string& filePath);
	Mesh(MeshFile& meshFile);

	inline bool load(const std::string& filePath)
	{
		MeshFile meshFile(filePath);
		return this->load_mesh_file(meshFile);
	}
	inline bool load_mesh_file(MeshFile& meshFile)
	{
		this->filePath = meshFile.get_file_path();

		// build triangle mesh
		const auto& suffix = this->filePath[2];
		Print("\t\tfile format: " + suffix);
		Print("\t\tAnalyzing triangle mesh ... ");

		bool loaded = false;
		if (suffix == ".off") { loaded = this->load_off(meshFile); }
		else if (suffix == ".obj") { loaded = this->load_obj(meshFile); }
		else
		{
			Print("[error] Currently does not support \"" + this->filePath[2] + "\"!");
			Print("[error] Please convert the file format to \".off\" or \".obj\" first!");
			loaded = false;
		}

		if (loaded) { Print("\t\tdone.\n"); }
		return loaded;
	}

	inline bool			is_triangle_mesh() { return this->is_tmesh; }
	inline v_string&	get_file_path() { return this->filePath; }

	inline Triangle_mesh&	get_tmesh() { return this->tmesh; }
	inline Polyhedron&		get_pmesh() { return this->pmesh; }

	static inline void		get_tmesh(Triangle_mesh& tmesh, double scaling)
	{
		PMP::transform(Transform(CGAL::SCALING, scaling), tmesh);
	}
	static inline void		get_pmesh(Polyhedron& pmesh, double scaling)
	{ 
		PMP::transform(Transform(CGAL::SCALING, scaling), pmesh);
	}

	static inline void		get_VF(MAT_3d& V, MAT_3i& F, Triangle_mesh& tmesh)
	{
		get_VF(V, F, tmesh, 1.0);
	}
	static inline void		get_VF(MAT_3d& V, MAT_3i& F, Polyhedron& pmesh)
	{
		get_VF(V, F, pmesh, 1.0);
	}

	static void				get_VF(MAT_3d& V, MAT_3i& F, Triangle_mesh& tmesh, double scaling);
	static void				get_VF(MAT_3d& V, MAT_3i& F, Polyhedron& pmesh, double scaling);
		
private:
	bool load_obj(MeshFile& meshFile);
	bool load_off(MeshFile& meshFile);
};

END_PROJECT_NAMESPACE


extern PROJECT_NAME::Mesh*	mesh;
extern Triangle_mesh*		tmesh;
extern Polyhedron*			pmesh;