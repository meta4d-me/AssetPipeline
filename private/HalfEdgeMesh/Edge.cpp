#include "Edge.h"

namespace cd::hem
{

bool Edge::IsOnBoundary() const
{
	bool isOnBoundary = false;
	return isOnBoundary;
}

Point Edge::Center() const
{
	return Point::Zero();
}

float Edge::Length() const
{
	return 0.0f;
}

}