/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include<string>

# define PROJECT_NAMESPACE Scan2knit
# define BEGIN_PROJECT_NAMESPACE namespace PROJECT_NAMESPACE {
# define END_PROJECT_NAMESPACE }

BEGIN_PROJECT_NAMESPACE

class Settings
{
public:
	std::string ROOT_DIR = "../";	// root directory of current solution
	std::string FILE_NAME = "";	// file name of 3d mesh, read from Settings.csv
	double	   obj_scale = 1.0;
	double	   stitch_width = 0.96;
	double	   stitch_height = 0.42;
	double	   offset_percent = 0.1;
};



END_PROJECT_NAMESPACE