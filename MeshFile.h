/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include "project.h"
#include "cgalHeader.hpp"
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

	bool load(const std::string& filePath);
	bool load_mesh_file(MeshFile& meshFile);
	bool is_triangle_mesh();

	Triangle_mesh&	get_tmesh();
	Polyhedron&		get_pmesh();
	v_string&		get_file_path();

private:
	bool load_obj(MeshFile& meshFile);
	bool load_off(MeshFile& meshFile);
};

END_PROJECT_NAMESPACE
