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
	std::vector<std::string>		 filePath; // fileDir + fileName + fileType
	std::ifstream					 input;
	bool							 isOpen = false;

public:
	MeshFile();
	MeshFile(const std::string filePath);

	int								 open(const std::string filePath);
	const bool						 is_open() const;
	const std::vector<std::string>&  get_file_path() const;
	std::ifstream&					 get_ifstream();
};

class Mesh
{
private:
	v_string	  filePath;	// path, filename, suffix
	bool		  is_tmesh = false;
	Triangle_mesh tmesh;

	int			  load_obj(MeshFile& meshFile);
	int			  load_off(MeshFile& meshFile);

public:
	Mesh();
	Mesh(const std::string filePath);
	Mesh(MeshFile& meshFile);

	void		   load_mesh_file(MeshFile& meshFile);
	bool		   is_triangle_mesh();
	Triangle_mesh& get_tmesh();
	v_string&	   get_file_path();
};

END_PROJECT_NAMESPACE
