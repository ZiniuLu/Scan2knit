/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <string>
#include <iostream>
#include <fstream>

#include "MeshFile.h"

using std::cout;
using std::cin;
using std::endl;

BEGIN_PROJECT_NAMESPACE

// MeshFile::
MeshFile::MeshFile(){ }
MeshFile::MeshFile(std::string filePath) { this->open(filePath); }

int				MeshFile::open(const std::string path)
{
	// split file path
	size_t len = path.length();
	size_t indexDot = path.rfind(".");
	size_t indexSlash = path.rfind("/"); // consider different situation: "/" and "\"

	this->filePath.push_back(path.substr(0, size_t(indexSlash + 1)));
	this->filePath.push_back(path.substr(size_t(indexSlash + 1), size_t(indexDot - indexSlash - 1)));
	this->filePath.push_back(path.substr(indexDot, size_t(len - indexDot)));

	// open file
	std::cout << "loading " << this->filePath[1] << this->filePath[2] << " ... ";
	this->input = std::ifstream(path);
	this->isOpen = this->input.is_open();

	if (!this->isOpen)
	{
		std::cout << "failed." << std::endl;
		std::cout << "Cannot open file: " << this->filePath[1] << " !" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "done." << std::endl;
	return EXIT_SUCCESS;

}

const bool		MeshFile::is_open() const { return this->isOpen; }

const v_string& MeshFile::get_file_path() const { return this->filePath; }

std::ifstream&	MeshFile::get_ifstream() { return this->input; }



// Mesh::
Mesh::Mesh(){ }
Mesh::Mesh(std::string filePath)
{
	MeshFile meshFile;
	meshFile.open(filePath);
	this->load_mesh_file(meshFile);
	this->filePath = meshFile.get_file_path();
}
Mesh::Mesh(MeshFile& meshFile)
{
	this->load_mesh_file(meshFile);	
	this->filePath = meshFile.get_file_path();
}

void			  Mesh::load_mesh_file(MeshFile& meshFile)
{
	auto path = meshFile.get_file_path();
	std::string filePath = path[0] + path[1];

	// build triangle mesh
	std::cout << "loading triangle mesh ... ";
	meshFile.get_ifstream() >> this->tmesh;
	this->is_tmesh = CGAL::is_triangle_mesh(this->tmesh);
	if (!this->is_tmesh)
	{
		std::cout << "\nInput geometry is not triangulated." << std::endl;
	}
	// print geometry info
	std::cout << "done" << std::endl;
	std::cout << "number of vertices: \t" << boost::num_vertices(this->tmesh) << std::endl;
	std::cout << "number of edges: \t" << boost::num_edges(this->tmesh) << std::endl;
}

bool			  Mesh::is_triangle_mesh() { return this->is_tmesh; }

const Polyhedron& Mesh::get_tmesh() const { return this->tmesh; }

const v_string&	  Mesh::get_file_path() const { return this->filePath; }


END_PROJECT_NAMESPACE
