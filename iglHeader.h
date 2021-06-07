/* Created on: 03/04/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <igl/readOFF.h>
#include <igl/list_to_matrix.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>

#include <Eigen/Dense>
#include <imgui/imgui.h>

typedef Eigen::Matrix<double, Eigen::Dynamic, 3> MAT_3d;
typedef Eigen::Matrix<int, Eigen::Dynamic, 3>	 MAT_3i;
typedef Eigen::Matrix<int, Eigen::Dynamic, 2>	 MAT_2i;

typedef Eigen::RowVector3d						 RVec_3d;
typedef Eigen::RowVector2i						 RVec_2i;
typedef Eigen::RowVector2i						 RVec_3i;

typedef igl::opengl::glfw::Viewer				 IGL_Viewer;
typedef igl::opengl::ViewerCore					 IGL_ViewerCore;
typedef igl::opengl::glfw::imgui::ImGuiMenu		 IGL_Gui;