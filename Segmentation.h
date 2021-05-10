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
	Segmentation(Skeleton& skeleton, Triangle_mesh& tmesh);
};

END_PROJECT_NAMESPACE