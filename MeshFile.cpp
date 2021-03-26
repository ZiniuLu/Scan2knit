/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include "MeshFile.h"
#include <string>
#include <iostream>
#include <fstream>

BEGIN_PROJECT_NAMESPACE

MeshFile::MeshFile(const char* filePath)
{
	// split file path
	std::string path = filePath;
	size_t len = path.length();
	size_t indexDot = path.rfind(".");
	size_t indexSlash = path.rfind("/"); // consider different situation: "/" and "\"

	this->fileDir = path.substr(0, size_t(indexSlash + 1));
	this->fileName = path.substr(size_t(indexSlash + 1), size_t(indexDot - indexSlash - 1));
	this->fileType = path.substr(indexDot, size_t(len - indexDot));

	// open file
	this->input = std::ifstream(path);
	this->isOpen = this->input.is_open();

	// build cgal triangle mesh
	if (this->isOpen)
	{
		std::cout << "Successfully loaded data from the file: " << this->fileName << this->fileType << " !"<< std::endl;
	}
}

bool MeshFile::is_open() { return this->isOpen; }
std::string& MeshFile::getFileDir() { return this->fileDir; }
std::string& MeshFile::getFileName() { return this->fileName; }
std::ifstream& MeshFile::getInputStream(){ return this->input; }

END_PROJECT_NAMESPACE
