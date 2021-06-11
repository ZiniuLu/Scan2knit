/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include "project.h"
#include "Skeletonization.h"
#include "cgalHeader.hpp"

BEGIN_PROJECT_NAMESPACE

class Segmentation
{
public:
	Segmentation();
	Segmentation(Skeleton& skeleton, Polyhedron& pmesh);

	bool extract_to_end(Skeleton& skeleton, Polyhedron& pmesh);
};

END_PROJECT_NAMESPACE