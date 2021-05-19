/* Created on: 12/05/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include "Stitch.h"

BEGIN_PROJECT_NAMESPACE

 // Stitch
bool Stitch::check_type() const
{
	if (type == Start)
	{
		return (in_0 == -1U && in_1 == -1U &&
			out_0 != -1U && out_1 == -1U);
	}
	else if (type == End)
	{
		return (in_0 != -1U && in_1 == -1U &&
			out_0 == -1U && out_1 == -1U);
	}
	else if (type == Tuck || type == Miss || type == Knit)
	{
		return (in_0 != -1U && in_1 == -1U &&
			out_0 != -1U && out_1 == -1U);
	}
	else if (type == Increase)
	{
		return (in_0 != -1U && in_1 == -1U &&
			out_0 != -1U && out_1 != -1U);
	}
	else if (type == Decrease)
	{
		return (in_0 != -1U && in_1 != -1U &&
			out_0 != -1U && out_1 == -1U);
	}
	else
	{
		return false;
	}
}



// Stitches
const std::vector<Stitch>& Stitches::get()
{
	return this->stitches;
}

END_PROJECT_NAMESPACE