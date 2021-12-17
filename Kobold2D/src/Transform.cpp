#include "Transform.h"
#include "MathUtils.h"

void Transform::SetHeading(float newHeading)
{
	heading = newHeading;
	if (heading < 0.f)
		heading += PI * 2.f;
	if (heading > PI * 2.f)
		heading -= PI * 2.f;

	headingCosine = cos(heading);
	headingSine = sin(heading);
}

void Transform::ChangeHeading(float change)
{
	SetHeading(heading + change);
}
