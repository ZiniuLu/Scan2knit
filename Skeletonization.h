/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include <string>
#include <fstream>

#include <Eigen/Dense>

#include "project.h"
#include "MeshFile.h"

#include <CGAL/extract_mean_curvature_flow_skeleton.h>

typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor		vertex_descriptor;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh>	Skeletonization;
typedef Skeletonization::Skeleton									Skeleton;


BEGIN_PROJECT_NAMESPACE

template<class ValueType>
struct Facet_with_id_pmap :public boost::put_get_helper<ValueType&, Facet_with_id_pmap<ValueType> >
{
	typedef face_descriptor key_type;
	typedef ValueType value_type;
	typedef value_type& reference;
	typedef boost::lvalue_property_map_tag category;

	Facet_with_id_pmap(std::vector<ValueType>& internal_vector) :internal_vector(internal_vector);

	reference operator[](key_type key);

private:
	std::vector<ValueType>& internal_vector;
};

class CurveSkeleton
{
private:
	std::string filePath;
	Triangle_mesh tmesh;
	bool isTriangleMesh = false;

	Skeleton skeleton;
	uint32_t numV_skel;
	uint32_t numE_skel;
	uint32_t numV_sm;
	uint32_t numE_sm;


public:
	CurveSkeleton(MeshFile& meshFile);

	bool is_triangle_mesh();
	void extract_to_end(bool output = true);
	void extract_step_by_step(bool output = true);

	Triangle_mesh& get_triangle_mesh();
	Skeleton& getSkeleton();

	
	//Eigen::MatrixXd& GetSkeletonPoints();
	//Eigen::MatrixXi& GetSkeletonPointNrs();

private:
	int containsPoint(
		const Eigen::MatrixXd& V, 
		const int vSize, 
		const Point& p);

	void output_skel_File(Eigen::MatrixXd& V, Eigen::MatrixXi E);

	void output_sm_File(Eigen::MatrixXd& V, Eigen::MatrixXi E);
	
	void display_once(
		const Eigen::MatrixXd& V, 
		const Eigen::MatrixXi& E, 
		const float v_size, 
		const float e_size);
};

END_PROJECT_NAMESPACE
