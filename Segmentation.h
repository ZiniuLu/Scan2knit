/* Created on: 26/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once
#include "project.h"
#include "Skeletonization.h"
#include "cgalHeader.h"

BEGIN_PROJECT_NAMESPACE

class Segment
{
public:
	int segment(Skeleton& skeleton, Polyhedron& tmesh);
};

END_PROJECT_NAMESPACE

// Property map associating a facet with an integer as id to an
// element in a vector stored internally
template<class ValueType>
struct Facet_with_id_pmap : public boost::put_get_helper<ValueType&, Facet_with_id_pmap<ValueType> >
{
    typedef face_descriptor key_type;
    typedef ValueType value_type;
    typedef value_type& reference;
    typedef boost::lvalue_property_map_tag category;
    Facet_with_id_pmap(std::vector<ValueType>& internal_vector) : internal_vector(internal_vector) { }
    reference operator[](key_type key) const
    {
        return internal_vector[key->id()];
    }
private:
    std::vector<ValueType>& internal_vector;
};