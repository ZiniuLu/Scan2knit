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
	Print("\t\tSearching file: " + this->filePath[1] + this->filePath[2] + " ... ");
	this->input1 = std::ifstream(path);
	this->input2 = std::ifstream(path);
	this->isOpen = this->input1.is_open();

	if (!this->isOpen)
	{
		Print("\t\tfailed.");
		Print("\t\tCannot find file: " + this->filePath[1] + " !");
		return false;
	}

	Print("\t\tdone.");
	return true;

}
const bool		MeshFile::is_open() const { return this->isOpen; }

const v_string& MeshFile::get_file_path() const { return this->filePath; }
std::ifstream&	MeshFile::get_ifstream1() { return this->input1; }
std::ifstream&	MeshFile::get_ifstream2() { return this->input2; }


int MeshFile::isotropic_remeshing(std::string& in_path, std::string& out_path, double edge_length, size_t nb_iter)
{
	std::ifstream input(in_path);
	r_Mesh r_mesh;
	if (!input || !(input >> r_mesh) || !CGAL::is_triangle_mesh(r_mesh))
	{
		Print("[error] \"" + in_path + "\" is not a valid input file.");
		return 1;
	}

	Print("\t\tSplit border...");
	std::vector<r_edge_descriptor> border;

	PMP::border_halfedges(faces(r_mesh), r_mesh,
		boost::make_function_output_iterator(halfedge2edge(r_mesh, border)));
	PMP::split_long_edges(border, edge_length, r_mesh);

	Print("\t\tdone.");
	std::ostringstream text;
	text << "\t\tStart remeshing of " << in_path << " (" << num_faces(r_mesh) << " faces) ...";
	Print(text.str());
	
	PMP::isotropic_remeshing(faces(r_mesh), edge_length, r_mesh,
		PMP::parameters::number_of_iterations(nb_iter).protect_constraints(true));//i.e. protect border, here
	
	Print("\t\tdone.");
	Print("\t\tSaving new mesh to \"" + out_path + "\" ...");
	std::ofstream output(out_path);
	output << r_mesh;
	output.close();
	Print("\t\tdone.");

	return 0;
}


// Mesh::
Mesh::Mesh(){ }
Mesh::Mesh(const std::string& filePath) { this->load(filePath); }
Mesh::Mesh(MeshFile& meshFile) { this->load_mesh_file(meshFile); }

// public
void	Mesh::get_VF(MAT_3d& V, MAT_3i& F, Triangle_mesh& tmesh, double scaling)
{
	// (to do)
}
void	Mesh::get_VF(MAT_3d& V, MAT_3i& F, Polyhedron& pmesh, double scaling)
{
	get_pmesh(pmesh, scaling);

	int numV = pmesh.size_of_vertices();
	int numF = pmesh.size_of_facets();

	V.resize(numV, 3);
	F.resize(numF, 3);

	CGAL::set_halfedgeds_items_id(pmesh);

	// add vertices
	size_t v_i = 0;

	auto it_v_begin = pmesh.vertices_begin();
	auto it_v_end	= pmesh.vertices_end();
	auto it_v		= it_v_begin;

	for (; it_v != it_v_end; it_v++)
	{
		const Point& p = it_v->point();
		V.row(v_i) << p.x(), p.y(), p.z();
		v_i++;
	}

	// add facets
	size_t f_i = 0;
	for (p_face_descriptor fd : faces(pmesh))
	{
		size_t axis = 0;
		for (p_vertex_descriptor vd : CGAL::vertices_around_face(halfedge(fd, pmesh), pmesh))
		{
			F(f_i, axis) = vd->id();
			axis++;
		}
		//std::cout << "\nface: " << my_F.row(f_i) << "\t" << std::endl;
		f_i++;
	}
}

// private
bool Mesh::load_obj(MeshFile& meshFile)
{
	Print("(to do)");
	return true;
}//todo
bool Mesh::load_off(MeshFile& meshFile)
{
	meshFile.get_ifstream1() >> this->tmesh;
	meshFile.get_ifstream2() >> this->pmesh;
	this->is_tmesh = CGAL::is_triangle_mesh(this->tmesh);

	// check
	if (!this->is_tmesh)
	{
		Print("[error] Failed! Input geometry is not triangulated!");
		return false;
	}

	//Print("done.\n");

	// print geometry info
	std::ostringstream text;
	text << "\t\t\tnumber of vertices: \t" << boost::num_vertices(this->tmesh) << "\n"
		<< "\t\t\tnumber of edges: \t" << boost::num_edges(this->tmesh);
	Print(text.str());

	return true;
}

END_PROJECT_NAMESPACE
