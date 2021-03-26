/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include "project.h"

#include <Eigen/Dense>

#define V_SIZE 5
#define E_SIZE 3.0f



BEGIN_PROJECT_NAMESPACE

class Display
{
private:
	size_t V_len = 0;
	size_t E_len = 0;
	Eigen::MatrixXd V_cache;
	Eigen::MatrixXi E_cache;
public:

	void display_once(
		const Eigen::MatrixXd& V,
		const Eigen::MatrixXi& E);
};

END_PROJECT_NAMESPACE