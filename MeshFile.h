/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <fstream>

#include "project.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

typedef CGAL::Simple_cartesian<double>								Kernel;
typedef Kernel::Point_3												Point;
typedef CGAL::Surface_mesh<Point>									Triangle_mesh;

BEGIN_PROJECT_NAMESPACE

class MeshFile
{
private:
	std::string fileDir;
	std::string fileName;
	std::string fileType;

	std::ifstream input;
	bool isOpen = false;
	

public:
	MeshFile(const char* filePath);

	bool is_open();
	std::string& getFileDir();
	std::string& getFileName();
	std::ifstream& getInputStream();
};

END_PROJECT_NAMESPACE
