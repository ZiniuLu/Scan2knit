/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <string>
#include <iostream>
#include <fstream>

#include "MeshFile.h"
#include "Settings.h"

//using std::cout;
//using std::cin;
//using std::endl;

BEGIN_PROJECT_NAMESPACE

// MeshFile::
MeshFile::MeshFile(){ }
MeshFile::MeshFile(const std::string& filePath) { this->open(filePath); }

bool MeshFile::open(const std::string& path)
{
	// split file path
	size_t len = path.length();
	size_t indexDot = path.rfind(".");
	size_t indexSlash = path.rfind("/"); // todo: consider different situation: "/" and "\"

	this->filePath.push_back(path.substr(0, size_t(indexSlash + 1)));
	this->filePath.push_back(path.substr(size_t(indexSlash + 1), size_t(indexDot - indexSlash - 1)));
	this->filePath.push_back(path.substr(indexDot, size_t(len - indexDot)));

	// open file
	Print("Loading " + this->filePath[1] + this->filePath[2] + " ... ");
	this->input = std::ifstream(path);
	this->isOpen = this->input.is_open();

	if (!this->isOpen)
	{
		Print("failed.");
		Print("Cannot open file: " + this->filePath[1] + " !");
		return false;
	}

	Print("done.\n");
	return true;

}
const bool		MeshFile::is_open() const { return this->isOpen; }

const v_string& MeshFile::get_file_path() const { return this->filePath; }
std::ifstream&	MeshFile::get_ifstream() { return this->input; }



// Mesh::
Mesh::Mesh(){ }
Mesh::Mesh(const std::string& filePath) { this->load(filePath); }
Mesh::Mesh(MeshFile& meshFile) { this->load_mesh_file(meshFile); }

// public
bool Mesh::load(const std::string& filePath)
{
	MeshFile meshFile(filePath);
	return this->load_mesh_file(meshFile);
}
bool		   Mesh::load_mesh_file(MeshFile& meshFile)
{
	this->filePath = meshFile.get_file_path();

	// build triangle mesh
	Print("Loading triangle mesh ... ");
	const auto& suffix = this->filePath[2];
	if (suffix == ".off")
	{
		Print("\tfile format: " + suffix);
		return this->load_off(meshFile);
	}
	else if (suffix == ".obj")
	{
		Print("\tfile format: " + suffix);
		return this->load_obj(meshFile);
	}
	else
	{
		Print("\nCurrently does not support \"" + this->filePath[2] + "\"!");
		Print("Please convert the file format to \".off\" or \".obj\" first!");
		return false;
	}
}
bool		   Mesh::is_triangle_mesh() { return this->is_tmesh; }

Triangle_mesh& Mesh::get_tmesh() { return this->tmesh; }
v_string&	   Mesh::get_file_path() { return this->filePath; }

// private
bool Mesh::load_obj(MeshFile& meshFile)
{
	Print("(to do)");
	return true;
}//todo
bool Mesh::load_off(MeshFile& meshFile)
{
	meshFile.get_ifstream() >> this->tmesh;
	this->is_tmesh = CGAL::is_triangle_mesh(this->tmesh);

	// check
	if (!this->is_tmesh)
	{
		Print("failed\nInput geometry is not triangulated.");
		return false;
	}

	Print("done.\n");

	// print geometry info
	Print("\tnumber of vertices: \t" + boost::num_vertices(this->tmesh));
	Print("\tnumber of edges: \t" + boost::num_edges(this->tmesh));

	return true;
}

END_PROJECT_NAMESPACE
