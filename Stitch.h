/* Created on: 12/05/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#pragma once

#include <string>
#include <vector>

#include "project.h"
#include "iglHeader.h"

BEGIN_PROJECT_NAMESPACE

struct Stitch
{
	uint32_t index;
	uint32_t yarn;

	enum : char
	{
		Start = 's',
		End = 'e',
		Tuck = 't',
		Miss = 'm',
		Knit = 'k',
		Increase = 'i',
		Decrease = 'd',
	};

	enum : char
	{
		CW = 'c', Clockweise = CW,
		AC = 'a', Anticlockweise = AC, CCW = AC, Counterclockweise = AC,
	};

	char type = Knit;
	char direction = CW;

	uint32_t in_0 = -1U;
	uint32_t in_1 = -1U;
	uint32_t out_0 = -1U;
	uint32_t out_1 = -1U;

	MAT_3d vertex;
	RVec_3d color;

	bool check_type() const;
};

class Stitches
{
private:
	std::vector<Stitch> stitches;

public:
	const std::vector<Stitch>& get();
};

END_PROJECT_NAMESPACE